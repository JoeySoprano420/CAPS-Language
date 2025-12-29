#include <gtest/gtest.h>
#include "aot/aot_codegen.h"
#include "x64/x64_codegen.h"

// Backend tests

TEST(BackendTests, AOTGeneration) {
  GroupDecl group;
  // Setup group
  std::string code = emit_cpp(group, true);
  EXPECT_FALSE(code.empty());
}

TEST(BackendTests, X64Generation) {
  IRProgram ir;
  // Setup IR
  X64Module mod = emit_x64(ir);
  EXPECT_FALSE(mod.functions.empty());
}