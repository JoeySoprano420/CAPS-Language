// RISC-V backend for CAPS-Language

namespace caps::riscv {

struct RISCModule {
  // RISC-V module
};

class RegisterAllocatorRISC {
  // RISC-V allocator
};

void select_instructions_riscv(const IRFunction& ir, Encoder& enc, RegisterAllocatorRISC& ra) {
  // RISC-V instruction selection
}

RISCModule emit_riscv(const IRProgram& ir) {
  // Emit RISC-V code
  return {};
}

} // namespace caps::riscv