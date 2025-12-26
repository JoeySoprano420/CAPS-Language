#pragma once
#include "ast/ast.h"
#include "ir/ir.h"
#include <ostream>

void print_ir(std::ostream& os, const IRGroup& g);
