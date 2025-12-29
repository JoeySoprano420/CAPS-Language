#pragma once
#include <string>
#include <vector>
#include <cstdint>

// CAPS x86-64 Backend Header
// This backend implements direct x86-64 codegen for CAPS-Language,
// emphasizing deterministic processes, typed channels with ownership transfer,
// no shared memory, and compile-time guarantees.

// Key Concepts:
// - Deterministic Processes: FSMs compiled to fixed instruction sequences.
// - Typed Channels: Bounded buffers with type-safe, ownership-transferring ops.
// - Ownership Transfer: Send moves data; no shared mutable state.
// - No Shared Memory: All communication via channels; private process state.
// - Compile-Time Guarantees: Bounds, types, topology enforced statically.

namespace caps::x64 {

// Forward declarations for the full x86-64 backend implementation
// Implements Milestones 3-6: instruction encoding, timers, unwind, debug

struct X64Instruction {
  // Placeholder for x86-64 instruction representation
  std::string mnemonic;
  std::vector<std::string> operands;
  std::vector<uint8_t> bytes;
};

struct X64Function {
  std::string name;
  uint32_t text_begin;
  uint32_t text_end;
  std::vector<X64Instruction> instructions;
  // Add locals, scopes, inlines, etc. for debug
};

struct X64Module {
  std::vector<X64Function> functions;
  // .bss globals, relocs, etc.
};

// Emit .asm file
bool emit_asm(const X64Module& mod, const std::string& filename);

// Emit .obj file with COFF format, unwind, debug
bool emit_obj(const X64Module& mod, const std::string& filename);

} // namespace caps::x64