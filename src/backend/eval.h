#pragma once
#include "backend/runtime.h"

namespace caps {

Value eval_expr(const Runtime& rt, const ProcessInstance* self, const IRExpr& e);

} // namespace caps
