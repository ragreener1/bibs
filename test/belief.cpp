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

#include "bibs/belief.hpp"

#include "behaviour.hpp"
#include "belief.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>

TEST(MockBelief, NameOnlyConstructor) {
  std::string name1 = "B1";
  auto b1 = BIBS::testing::MockBelief(std::move(name1));

  std::string name2 = "B2";
  auto b2 = BIBS::testing::MockBelief(std::move(name2));

  EXPECT_NE(b1.uuid, b2.uuid);
  EXPECT_EQ(b1.name, "B1");
  EXPECT_EQ(b2.name, "B2");
}

TEST(MockBelief, NameAndUUIDConstructor) {
  std::string name = "B1";
  auto uuid = boost::uuids::random_generator_mt19937()();
  auto b = BIBS::testing::MockBelief(std::move(name), uuid);

  EXPECT_EQ(b.name, "B1");
  EXPECT_EQ(b.uuid, uuid);
}

TEST(Belief, NameOnlyConstructor) {
  std::string name1 = "B1";
  auto b1 = BIBS::Belief(std::move(name1));

  std::string name2 = "B2";
  auto b2 = BIBS::Belief(std::move(name2));

  EXPECT_NE(b1.uuid, b2.uuid);
  EXPECT_EQ(b1.name, "B1");
  EXPECT_EQ(b2.name, "B2");
}

TEST(Belief, NameAndUUIDConstructor) {
  std::string name = "B1";
  auto uuid = boost::uuids::random_generator_mt19937()();
  auto b = BIBS::Belief(std::move(name), uuid);

  EXPECT_EQ(b.name, "B1");
  EXPECT_EQ(b.uuid, uuid);
}

TEST(Belief, SetAndGetBeliefRelationshipWhenExists) {
  auto b1 = BIBS::Belief("b1");
  auto b2 = std::make_unique<BIBS::testing::MockBelief>("b2");

  b1.setBeliefRelationship(b2.get(), 5.0);

  EXPECT_EQ(b1.beliefRelationship(b2.get()), 5.0);
}

TEST(Belief, beliefRelationshipWhenNotExists) {
  auto b1 = BIBS::Belief("b1");
  auto b2 = std::make_unique<BIBS::testing::MockBelief>("b2");

  EXPECT_THROW(b1.beliefRelationship(b2.get()), std::out_of_range);
}

TEST(Belief, setBeliefRelationshipUpdating) {
  auto b1 = BIBS::Belief("b1");
  auto b2 = std::make_unique<BIBS::testing::MockBelief>("b2");

  b1.setBeliefRelationship(b2.get(), 5.0);

  EXPECT_EQ(b1.beliefRelationship(b2.get()), 5.0);

  b1.setBeliefRelationship(b2.get(), 10.0);

  EXPECT_EQ(b1.beliefRelationship(b2.get()), 10.0);
}

TEST(Belief, observedBehaviourRelationshipWhenNotExists) {
  auto beh = std::make_unique<BIBS::testing::MockBehaviour>("beh");
  auto b = BIBS::Belief("b1");

  EXPECT_THROW(b.observedBehaviourRelationship(beh.get()), std::out_of_range);
}

TEST(
    Belief,
    observedBehaviourRelationshipWhenExistsAndSetObservedBehaviourRelationship) {
  auto beh = std::make_unique<BIBS::testing::MockBehaviour>("beh");
  auto b = BIBS::Belief("b1");

  b.setObservedBehaviourRelationship(beh.get(), 10.0);
  EXPECT_EQ(b.observedBehaviourRelationship(beh.get()), 10.0);
}

TEST(Belief, setObservedBehaviourRelationshipUpdate) {
  auto beh = std::make_unique<BIBS::testing::MockBehaviour>("beh");
  auto b = BIBS::Belief("b1");

  b.setObservedBehaviourRelationship(beh.get(), 5.0);
  EXPECT_EQ(b.observedBehaviourRelationship(beh.get()), 5.0);

  b.setObservedBehaviourRelationship(beh.get(), 2.0);
  EXPECT_EQ(b.observedBehaviourRelationship(beh.get()), 2.0);
}

TEST(Belief, performingBehaviourRelationshipWhenNotExists) {
  auto beh = std::make_unique<BIBS::testing::MockBehaviour>("beh");
  auto b = BIBS::Belief("b1");

  EXPECT_THROW(b.performingBehaviourRelationship(beh.get()), std::out_of_range);
}

TEST(
    Belief,
    performingBehaviourRelationshipWhenExistsAndSetObservedBehaviourRelationship) {
  auto beh = std::make_unique<BIBS::testing::MockBehaviour>("beh");
  auto b = BIBS::Belief("b1");

  b.setPerformingBehaviourRelationship(beh.get(), 10.0);
  EXPECT_EQ(b.performingBehaviourRelationship(beh.get()), 10.0);
}

TEST(Belief, setPerformingBehaviourRelationshipUpdate) {
  auto beh = std::make_unique<BIBS::testing::MockBehaviour>("beh");
  auto b = BIBS::Belief("b1");

  b.setPerformingBehaviourRelationship(beh.get(), 5.0);
  EXPECT_EQ(b.performingBehaviourRelationship(beh.get()), 5.0);

  b.setPerformingBehaviourRelationship(beh.get(), 2.0);
  EXPECT_EQ(b.performingBehaviourRelationship(beh.get()), 2.0);
}
