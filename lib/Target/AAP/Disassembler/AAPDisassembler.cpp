//===-- AAPDisassembler.cpp - Disassembler for AAP ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the AAP Disassembler.
//
//===----------------------------------------------------------------------===//

#include "AAPDisassembler.h"
#include "AAP.h"
#include "MCTargetDesc/AAPMCTargetDesc.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"

#define DEBUG_TYPE "AAP-disassembler"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

static MCDisassembler *createAAPDisassembler(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             MCContext &Ctx) {
  return new AAPDisassembler(STI, Ctx);
}

extern "C" void LLVMInitializeAAPDisassembler() {
  // Register the disassembler
  TargetRegistry::RegisterMCDisassembler(getTheAAPTarget(),
                                         createAAPDisassembler);
}

// Forward declare because the autogenerated code will reference this.
// Definition is further down.
DecodeStatus DecodeGR8RegisterClass(MCInst &Inst, unsigned RegNo,
                                    uint64_t Address, const void *Decoder);
DecodeStatus DecodeGR64RegisterClass(MCInst &Inst, unsigned RegNo,
                                     uint64_t Address, const void *Decoder);

DecodeStatus decodeMemSrc3Operand(MCInst &Inst, unsigned Operand,
                                  uint64_t Address, const void *Decoder);
DecodeStatus decodeMemSrc10Operand(MCInst &Inst, unsigned Operand,
                                   uint64_t Address, const void *Decoder);

DecodeStatus decodeOff3Operand(MCInst &Inst, unsigned Operand,
                               uint64_t Address, const void *Decoder);
DecodeStatus decodeOff10Operand(MCInst &Inst, unsigned Operand,
                                uint64_t Address, const void *Decoder);

DecodeStatus decodeShiftOperand(MCInst &Inst, unsigned RegNo, uint64_t Address,
                                const void *Decoder);

#include "AAPGenDisassemblerTables.inc"

DecodeStatus AAPDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                             ArrayRef<uint8_t> Bytes,
                                             uint64_t Address, raw_ostream &os,
                                             raw_ostream &cs) const {
  uint32_t Insn;
  DecodeStatus Result;

  // First try a 16-bit instruction
  if (Bytes.size() < 2) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  Insn = (Bytes[1] << 8) | (Bytes[0] << 0);

  // Next try generic 16-bit instructions
  Result = decodeInstruction(DecoderTable16, MI, Insn, Address, this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 2;
    return Result;
  }

  // Finally try a 32-bit instruction
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  Insn = (Bytes[3] << 24) | (Bytes[2] << 16) | (Bytes[1] << 8) |
         (Bytes[0] << 0);

  // Call auto-generated decoder function
  Result = decodeInstruction(DecoderTable32, MI, Insn, Address, this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  // In case of invalid decode, assume it was a short instruction
  Size = 2;
  return MCDisassembler::Fail;
}

static const unsigned AAPRegs8[] = {AAP::R0, AAP::R1, AAP::R2, AAP::R3,
                                    AAP::R4, AAP::R5, AAP::R6, AAP::R7};
static const unsigned AAPRegs64[] = {
    AAP::R0,  AAP::R1,  AAP::R2,  AAP::R3,  AAP::R4,  AAP::R5,  AAP::R6,
    AAP::R7,  AAP::R8,  AAP::R9,  AAP::R10, AAP::R11, AAP::R12, AAP::R13,
    AAP::R14, AAP::R15, AAP::R16, AAP::R17, AAP::R18, AAP::R19, AAP::R20,
    AAP::R21, AAP::R22, AAP::R23, AAP::R24, AAP::R25, AAP::R26, AAP::R27,
    AAP::R28, AAP::R29, AAP::R30, AAP::R31, AAP::R32, AAP::R33, AAP::R34,
    AAP::R35, AAP::R36, AAP::R37, AAP::R38, AAP::R39, AAP::R40, AAP::R41,
    AAP::R42, AAP::R43, AAP::R44, AAP::R45, AAP::R46, AAP::R47, AAP::R48,
    AAP::R49, AAP::R50, AAP::R51, AAP::R52, AAP::R53, AAP::R54, AAP::R55,
    AAP::R56, AAP::R57, AAP::R58, AAP::R59, AAP::R60, AAP::R61, AAP::R62,
    AAP::R63};

template <std::size_t N>
static DecodeStatus decodeRegisterClass(MCInst &Inst, uint64_t RegNo,
                                        const unsigned (&Regs)[N]) {
  if (RegNo >= N)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(Regs[RegNo]));
  return MCDisassembler::Success;
}

DecodeStatus DecodeGR8RegisterClass(MCInst &Inst, unsigned RegNo,
                                    uint64_t Address, const void *Decoder) {
  return decodeRegisterClass(Inst, RegNo, AAPRegs8);
}
DecodeStatus DecodeGR64RegisterClass(MCInst &Inst, unsigned RegNo,
                                     uint64_t Address, const void *Decoder) {
  return decodeRegisterClass(Inst, RegNo, AAPRegs64);
}

DecodeStatus decodeMemSrc3Operand(MCInst &Inst, unsigned Operand,
                                  uint64_t Address, const void *Decoder) {
  unsigned Reg = (Operand >> 16) & 0x7;
  int32_t Offset = SignExtend32<3>(Operand & 0xffff);

  if (decodeRegisterClass(Inst, Reg, AAPRegs8) == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  Inst.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

DecodeStatus decodeMemSrc10Operand(MCInst &Inst, unsigned Operand,
                                   uint64_t Address, const void *Decoder) {
  unsigned Reg = (Operand >> 16) & 0x3f;
  int32_t Offset = SignExtend32<10>(Operand & 0xffff);

  if (decodeRegisterClass(Inst, Reg, AAPRegs64) == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  Inst.addOperand(MCOperand::createImm(Offset));
  return MCDisassembler::Success;
}

DecodeStatus decodeOff3Operand(MCInst &Inst, unsigned Operand,
                               uint64_t Address, const void *Decoder) {
  Inst.addOperand(MCOperand::createImm(SignExtend32<3>(Operand)));
  return MCDisassembler::Success;
}

DecodeStatus decodeOff10Operand(MCInst &Inst, unsigned Operand,
                                uint64_t Address, const void *Decoder) {
  Inst.addOperand(MCOperand::createImm(SignExtend32<10>(Operand)));
  return MCDisassembler::Success;
}

DecodeStatus decodeShiftOperand(MCInst &Inst, unsigned Operand,
                                uint64_t Address, const void *Decoder) {
  Inst.addOperand(MCOperand::createImm(Operand + 1));
  return MCDisassembler::Success;
}
