/* BIBS, the belief-induced behaviour simulation
 * Copyright (C) 2021 Robert Greener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "bibs/agent.hpp"
#include "bibs/behaviour.hpp"
#include "bibs/belief.hpp"
#include "bibs/bibs.hpp"

#include <boost/uuid/uuid_generators.hpp>
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

BIBS::IAgent::IAgent(const boost::uuids::uuid uuid) : uuid(uuid) {}

void BIBS::IAgent::tick(const sim_time_t t,
                        const std::vector<const IBehaviour *> &behs,
                        const std::vector<const IBelief *> &bels) {
  for (const auto &bel : bels) {
    updateActivation(t, bel);
  }

  perform(t, behs);
}

BIBS::Agent::Agent()
    : BIBS::Agent(boost::uuids::random_generator_mt19937()()) {}

BIBS::Agent::Agent(const boost::uuids::uuid uuid) : BIBS::IAgent(uuid) {}

BIBS::Agent::Agent(
    const std::map<sim_time_t, std::map<const IBelief *, double>> activationMap)
    : BIBS::IAgent(boost::uuids::random_generator_mt19937()()),
      activationMap(activationMap) {}

BIBS::Agent::Agent(
    const boost::uuids::uuid uuid,
    std::map<sim_time_t, std::map<const IBelief *, double>> activationMap)
    : BIBS::IAgent(uuid), activationMap(activationMap) {}

double BIBS::Agent::activation(const sim_time_t t, const IBelief *b) const {
  return this->activationMap.at(t).at(b);
}

const BIBS::IBehaviour *BIBS::Agent::performed(const sim_time_t t) const {
  return performedMap.at(t);
}

void BIBS::Agent::_addPerformed(const sim_time_t t, const IBehaviour *b) {
  performedMap.insert_or_assign(t, b);
}

double BIBS::Agent::observed(const IBelief *b, const sim_time_t t) const {
  double ret_value = 0.0;
  for (auto const &[a, w] : friends) {
    ret_value += w * b->observedBehaviourRelationship(a->performed(t));
  }

  return ret_value;
}

double BIBS::Agent::friendWeight(const IAgent *a) const {
  return friends.at(a);
}

void BIBS::Agent::setFriendWeight(const IAgent *a, double w) {
  friends.insert_or_assign(a, w);
}

double BIBS::Agent::contextualise(const IBelief *b, const sim_time_t t) const {
  double value_to_exp = 0.0;

  std::vector<const IBelief *> bs = heldBeliefs(t);

  for (const auto &b2 : bs) {
    value_to_exp += activation(t, b2) * b->beliefRelationship(b2);
  }

  return std::exp(value_to_exp);
}

double BIBS::Agent::contextualObserved(const IBelief *b,
                                       const sim_time_t t) const {
  return contextualise(b, t) * observed(b, t);
}

double BIBS::Agent::timeDelta(const IBelief *b) const {
  return timeDeltaMap.at(b);
}

void BIBS::Agent::setTimeDelta(const IBelief *b, const double td) {
  timeDeltaMap.insert_or_assign(b, td);
}

void BIBS::Agent::updateActivation(const sim_time_t t, const IBelief *b) {
  double newActivation =
      timeDelta(b) * activation(t - 1, b) + contextualObserved(b, t - 1);
  try {
    activationMap.at(t).emplace(b, newActivation);
  } catch (const std::out_of_range &) {
    std::map<const IBelief *, double> m;
    activationMap.emplace(t, m);
    activationMap.at(t).emplace(b, newActivation);
  }
}

double BIBS::Agent::beliefBehaviour(const IBelief *bel, const IBehaviour *beh,
                                    const sim_time_t t) const {
  return bel->performingBehaviourRelationship(beh) * activation(t, bel);
}

double BIBS::Agent::contextualBeliefBehaviour(const IBelief *bel,
                                              const IBehaviour *beh,
                                              const sim_time_t t) const {
  return contextualise(bel, t) * beliefBehaviour(bel, beh, t);
}

double BIBS::Agent::contextualBehaviour(const IBehaviour *b,
                                        const sim_time_t t) const {
  double returnVal = 0.0;

  for (const auto &[belief, _] : activationMap.at(t)) {
    returnVal += contextualBeliefBehaviour(belief, b, t);
  }

  return returnVal;
}

std::vector<const BIBS::IBelief *>
BIBS::Agent::heldBeliefs(const sim_time_t t) const {
  auto nBeliefs = activationMap.at(t).size();

  std::vector<const BIBS::IBelief *> returnVal(nBeliefs);

  size_t i = 0;
  for (const auto &[belief, _v] : activationMap.at(t)) {
    returnVal[i] = belief;
    ++i;
  }

  return returnVal;
}

double BIBS::Agent::environment(const IBehaviour *b, const sim_time_t t) const {
  return 0.0;
}

double BIBS::Agent::utility(const IBehaviour *b, const sim_time_t t) const {
  return contextualBehaviour(b, t) + environment(b, t);
}

void BIBS::Agent::perform(const sim_time_t t,
                          const std::vector<const IBehaviour *> &bs) {
  double maxUtility = std::numeric_limits<double>::lowest();
  const IBehaviour *maxBehaviour = nullptr;

  std::vector<const IBehaviour *> positiveBehaviours;
  std::vector<double> positiveUtilities;

  for (const auto &b : bs) {
    auto ut = utility(b, t);
    if (ut > maxUtility) {
      maxUtility = ut;
      maxBehaviour = b;
    }
    if (ut > 0) {
      positiveBehaviours.push_back(b);
      positiveUtilities.push_back(ut);
    }
  }

  if (positiveUtilities.size() <= 1) {
    performedMap[t] = maxBehaviour;
  } else {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::discrete_distribution<size_t> dist(positiveUtilities.begin(),
                                            positiveUtilities.end());

    performedMap[t] = positiveBehaviours[dist(eng)];
  }
}
