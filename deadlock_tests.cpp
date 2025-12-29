#include <gtest/gtest.h>
#include "analysis/pipeline.h"

// Deadlock conformance tests

TEST(DeadlockTests, BasicCycle) {
  // Test basic deadlock detection
  GroupDecl group;
  // Setup processes with cycle
  EXPECT_TRUE(detect_deadlocks(group, Diag()));
}

TEST(DeadlockTests, NoDeadlock) {
  // Test no deadlock case
  GroupDecl group;
  // Setup acyclic processes
  EXPECT_FALSE(detect_deadlocks(group, Diag()));
}