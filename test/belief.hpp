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

/**
 * @file      belief.hpp<test>
 * @brief     Header of MockBelief
 * @date      Tue Oct 26 14:21:25 2021
 * @author    Robert Greener
 * @copyright GPL-3.0-or-later
 *
 * This module mocks Belief
 */

#ifndef BIBS_TESTING_BELIEF_H
#define BIBS_TESTING_BELIEF_H

#include "bibs/belief.hpp"
#include "gmock/gmock.h"

namespace BIBS::testing {
class MockBelief : public BIBS::IBelief {
public:
  using IBelief::IBelief;

  MOCK_METHOD(double, beliefRelationship, (const IBelief *b2),
              (const, override));

  MOCK_METHOD(void, setBeliefRelationship,
              (const IBelief *b2, const double value), (override));

  MOCK_METHOD(double, observedBehaviourRelationship, (const IBehaviour *beh),
              (const, override));

  MOCK_METHOD(void, setObservedBehaviourRelationship,
              (const IBehaviour *beh, const double value), (override));

  MOCK_METHOD(double, performingBehaviourRelationship, (const IBehaviour *beh),
              (const, override));

  MOCK_METHOD(void, setPerformingBehaviourRelationship,
              (const IBehaviour *beh, const double value), (override));
};
} // namespace BIBS::testing

#endif // BIBS_TESTING_BELIEF_H
