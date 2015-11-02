//===-- AAPSimulator.cpp - AAP Simulator  ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides an implementation of a simulated AAP
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/TargetRegistry.h"
#include "AAPSimulator.h"
#include <cstring>

#define GET_INSTRINFO_ENUM
#include "AAPGenInstrInfo.inc"

#define GET_REGINFO_ENUM
#include "AAPGenRegisterInfo.inc"

using namespace llvm;
using namespace AAPSim;

// Should the simulator call llvm_unreachable when executing unknown
#define UNKNOWN_SHOULD_UNREACHABLE 0

AAPSimulator::AAPSimulator() {
  std::string Error;
  TheTarget = TargetRegistry::lookupTarget("aap-none-none", Error);
  if (!TheTarget) {
    errs() << "aap-run: " << Error << "\n";
  }

  // Set up all MC/Target components needed by the disassembler
  MRI = TheTarget->createMCRegInfo("aap-none-none");
  if (!MRI) {
    errs() << "error: no register info\n";
    return;
  }

  AsmInfo = TheTarget->createMCAsmInfo(*MRI, "aap-none-none");
  if (!AsmInfo) {
    errs() << "error: no asminfo\n";
    return;
  }

  STI = TheTarget->createMCSubtargetInfo("aap-none-none", "", "");
  if (!STI) {
    errs() << "error: no subtarget info\n";
    return;
  }

  MII = TheTarget->createMCInstrInfo();
  if (!MII) {
    errs() << "error: no instruction info\n";
    return;
  }

  const MCObjectFileInfo *MOFI = new MCObjectFileInfo();
  MCContext Ctx(AsmInfo, MRI, MOFI);

  DisAsm = TheTarget->createMCDisassembler(*STI, Ctx);
  if (!DisAsm) {
    errs() << "error: no disassembler\n";
    return;
  }

  int AsmPrinterVariant = AsmInfo->getAssemblerDialect();
  IP = TheTarget->createMCInstPrinter(
      Triple("aap-none-none"), AsmPrinterVariant, *AsmInfo, *MII, *MRI);
  if (!IP) {
    errs() << "error: no instruction printer\n";
    return;
  }
}

void AAPSimulator::WriteCodeSection(llvm::StringRef Bytes, uint32_t address) {
  for (size_t i = 0; i < Bytes.size(); i++) {
    State.setCodeMem(address + i, Bytes[i]);
  }
}

void AAPSimulator::WriteDataSection(llvm::StringRef Bytes, uint32_t address) {
  for (size_t i = 0; i < Bytes.size(); i++) {
    State.setDataMem(address + i, Bytes[i]);
  }
}

static int getLLVMReg(unsigned Reg) {
  switch (Reg) {
    default: llvm_unreachable("Invalid register");
#define REG(x) case AAP::R##x: return x;
    REG(0)  REG(1)  REG(2)  REG(3)  REG(4)  REG(5)  REG(6)  REG(7)
    REG(8)  REG(9)  REG(10) REG(11) REG(12) REG(13) REG(14) REG(15)
    REG(16) REG(17) REG(18) REG(19) REG(20) REG(21) REG(22) REG(23)
    REG(24) REG(25) REG(26) REG(27) REG(28) REG(29) REG(30) REG(31)
    REG(32) REG(33) REG(34) REG(35) REG(36) REG(37) REG(38) REG(39)
    REG(40) REG(41) REG(42) REG(43) REG(44) REG(45) REG(46) REG(47)
    REG(48) REG(49) REG(50) REG(51) REG(52) REG(53) REG(54) REG(55)
    REG(56) REG(57) REG(58) REG(59) REG(60) REG(61) REG(62) REG(63)
#undef REG
  }
}

// Function to sign extend a uint16_t register for detecting overflow
static uint32_t signExtend16(uint16_t val) {
  uint32_t newval = static_cast<uint16_t>(val);
  uint32_t sign = newval & 0x8000;
  if (sign)
    newval |= 0xffff0000;
  return newval;
}

// Sign extend branch cc target (long, 10 bits)
static int16_t signExtendBranchCC(uint16_t val) {
  if (val & 0x0200)
    val |= 0xfe00;
  return static_cast<int16_t>(val);
}

// Sign extend branch target (long, 18 bits)
static int32_t signExtendBranch(uint32_t val) {
  if (val & 0x00020000)
    val |= 0xfffe0000;
  return static_cast<int32_t>(val);
}

// Sign extend branch target (short, 6 bits)
static int16_t signExtendBranchS(uint16_t val) {
  if (val & 0x200)
    val |= 0xfc00;
  return static_cast<int16_t>(val);
}

SimStatus AAPSimulator::exec(MCInst &Inst, uint32_t pc_w, uint32_t &newpc_w) {
  switch (Inst.getOpcode()) {
    // Unknown instruction
    default:
#if UNKNOWN_SHOULD_UNREACHABLE
      llvm_unreachable("No simulator support for this instruction");
#else
      newpc_w = pc_w;
      return SimStatus::SIM_TRAP;
#endif
      break;

    // NOP Handling
    // 0: Breakpoint
    // 1: NOP
    // 2: Exit with retcode in Rd
    // 3: Write char Rd to stdout
    // 4: Write char Rd to stderr
    case AAP::NOP:
    case AAP::NOP_short: {
      int Reg = getLLVMReg(Inst.getOperand(0).getReg());
      uint16_t Command = Inst.getOperand(1).getImm();
      // Load register value and char for NOPs that require it
      uint16_t RegVal = State.getReg(Reg);
      char c = static_cast<char>(RegVal);
      switch (Command) {
        case 0: return SimStatus::SIM_BREAKPOINT;
        default:  // Treat unknown values as NOP
        case 1: break;
        case 2:
          State.setExitCode(RegVal);
          return SimStatus::SIM_QUIT;
        case 3:
          outs() << c;
          break;
        case 4:
          errs() << c;
          break;
      }
      break;
    }

    // Move Instructions
    case AAP::MOV_r:
    case AAP::MOV_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrc = getLLVMReg(Inst.getOperand(1).getReg());
      State.setReg(RegDst, State.getReg(RegSrc));
      break;
    }
    case AAP::MOVI_i16:
    case AAP::MOVI_i6_short: {
      int Reg = getLLVMReg(Inst.getOperand(0).getReg());
      uint16_t Val = Inst.getOperand(1).getImm() & 0xffff;
      State.setReg(Reg, Val);
      break;
    }

    // ADD
    case AAP::ADD_r:
    case AAP::ADD_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint32_t ValA = signExtend16(State.getReg(RegSrcA));
      uint32_t ValB = signExtend16(State.getReg(RegSrcB));
      uint32_t Res = ValA + ValB;
      State.setReg(RegDst, static_cast<uint16_t>(Res));
      // Test for overflow
      int32_t Res_s = static_cast<int32_t>(Res);
      State.setOverflow(static_cast<int16_t>(Res_s) != Res_s ? 1 : 0);
      break;
    }

    // ADDC
    case AAP::ADDC_r: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint32_t ValA = signExtend16(State.getReg(RegSrcA));
      uint32_t ValB = signExtend16(State.getReg(RegSrcB));
      uint32_t Res = ValA + ValB + State.getOverflow();
      State.setReg(RegDst, static_cast<uint16_t>(Res));
      // Test for overflow
      int32_t Res_s = static_cast<int32_t>(Res);
      State.setOverflow(static_cast<int16_t>(Res_s) != Res_s ? 1 : 0);
      break;
    }

    // ADDI
    case AAP::ADDI_i10:
    case AAP::ADDI_i3_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint32_t ValA = signExtend16(State.getReg(RegSrcA));
      uint32_t ValB = Inst.getOperand(2).getImm();
      uint32_t Res = ValA + ValB;
      State.setReg(RegDst, static_cast<uint16_t>(Res));
      // Test for overflow
      int32_t Res_s = static_cast<int32_t>(Res);
      State.setOverflow(static_cast<int16_t>(Res_s) != Res_s ? 1 : 0);
      break;
    }

    // SUB
    case AAP::SUB_r:
    case AAP::SUB_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint32_t ValA = signExtend16(State.getReg(RegSrcA));
      uint32_t ValB = signExtend16(State.getReg(RegSrcB));
      uint32_t Res = ValA - ValB;
      State.setReg(RegDst, static_cast<uint16_t>(Res));
      // Test for overflow
      int32_t Res_s = static_cast<int32_t>(Res);
      State.setOverflow(static_cast<int16_t>(Res_s) != Res_s ? 1 : 0);
      break;
    }

    // SUBC
    case AAP::SUBC_r: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint32_t ValA = signExtend16(State.getReg(RegSrcA));
      uint32_t ValB = signExtend16(State.getReg(RegSrcB));
      uint32_t Res = ValA - ValB - State.getOverflow();
      State.setReg(RegDst, static_cast<uint16_t>(Res));
      // Test for overflow
      int32_t Res_s = static_cast<int32_t>(Res);
      State.setOverflow(static_cast<int16_t>(Res_s) != Res_s ? 1 : 0);
      break;
    }

    // SUBI
    case AAP::SUBI_i10:
    case AAP::SUBI_i3_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint32_t ValA = signExtend16(State.getReg(RegSrcA));
      uint32_t ValB = Inst.getOperand(2).getImm();
      uint32_t Res = ValA - ValB;
      State.setReg(RegDst, static_cast<uint16_t>(Res));
      // Test for overflow
      int32_t Res_s = static_cast<int32_t>(Res);
      State.setOverflow(static_cast<int16_t>(Res_s) != Res_s ? 1 : 0);
      break;
    }

    // AND
    case AAP::AND_r:
    case AAP::AND_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = State.getReg(RegSrcB);
      uint16_t Res = ValA & ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // ANDI
    case AAP::ANDI_i9: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = Inst.getOperand(2).getImm();
      uint16_t Res = ValA & ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // OR
    case AAP::OR_r:
    case AAP::OR_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = State.getReg(RegSrcB);
      uint16_t Res = ValA | ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // ORI
    case AAP::ORI_i9: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = Inst.getOperand(2).getImm();
      uint16_t Res = ValA | ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // XOR
    case AAP::XOR_r:
    case AAP::XOR_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = State.getReg(RegSrcB);
      uint16_t Res = ValA ^ ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // XORI
    case AAP::XORI_i9: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = Inst.getOperand(2).getImm();
      uint16_t Res = ValA ^ ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // ASR
    case AAP::ASR_r:
    case AAP::ASR_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      int16_t ValA = static_cast<int16_t>(State.getReg(RegSrcA));
      int16_t ValB = static_cast<int16_t>(State.getReg(RegSrcB) & 0xf);
      uint16_t Res = static_cast<uint16_t>(ValA >> ValB);
      State.setReg(RegDst, Res);
      break;
    }

    // ASRI
    case AAP::ASRI_i6:
    case AAP::ASRI_i3_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int16_t ValA = static_cast<int16_t>(State.getReg(RegSrcA));
      int16_t ValB = static_cast<int16_t>(Inst.getOperand(2).getImm() & 0xf);
      uint16_t Res = static_cast<uint16_t>(ValA >> ValB);
      State.setReg(RegDst, Res);
      break;
    }

    // LSL
    case AAP::LSL_r:
    case AAP::LSL_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = State.getReg(RegSrcB) & 0xf;
      uint16_t Res = ValA << ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // LSLI
    case AAP::LSLI_i6:
    case AAP::LSLI_i3_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = Inst.getOperand(2).getImm() & 0xf;
      uint16_t Res = ValA << ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // LSR
    case AAP::LSR_r:
    case AAP::LSR_r_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      int RegSrcB = getLLVMReg(Inst.getOperand(2).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = State.getReg(RegSrcB) & 0xf;
      uint16_t Res = ValA >> ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // LSRI
    case AAP::LSRI_i6:
    case AAP::LSRI_i3_short: {
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegSrcA = getLLVMReg(Inst.getOperand(1).getReg());
      uint16_t ValA = State.getReg(RegSrcA);
      uint16_t ValB = Inst.getOperand(2).getImm() & 0xf;
      uint16_t Res = ValA >> ValB;
      State.setReg(RegDst, Res);
      break;
    }

    // Load
    case AAP::LDB:
    case AAP::LDB_short:
    case AAP::LDW:
    case AAP::LDW_short:
    case AAP::LDB_postinc:
    case AAP::LDB_postinc_short:
    case AAP::LDW_postinc:
    case AAP::LDW_postinc_short:
    case AAP::LDB_predec:
    case AAP::LDB_predec_short:
    case AAP::LDW_predec:
    case AAP::LDW_predec_short: {
      bool postinc = (Inst.getOpcode() == AAP::LDB_postinc ||
                      Inst.getOpcode() == AAP::LDB_postinc_short ||
                      Inst.getOpcode() == AAP::LDW_postinc ||
                      Inst.getOpcode() == AAP::LDW_postinc_short) ? true : false;
      bool predec  = (Inst.getOpcode() == AAP::LDB_predec ||
                      Inst.getOpcode() == AAP::LDB_predec_short ||
                      Inst.getOpcode() == AAP::LDW_predec ||
                      Inst.getOpcode() == AAP::LDW_predec_short) ? true : false;
      bool word = (Inst.getOpcode() == AAP::LDW ||
                   Inst.getOpcode() == AAP::LDW_short ||
                   Inst.getOpcode() == AAP::LDW_postinc ||
                   Inst.getOpcode() == AAP::LDW_postinc_short ||
                   Inst.getOpcode() == AAP::LDW_predec ||
                   Inst.getOpcode() == AAP::LDW_predec_short) ? true : false;
      // Initial register values
      int RegDst = getLLVMReg(Inst.getOperand(0).getReg());
      int RegMem = getLLVMReg(Inst.getOperand(1).getReg());
      int Offset = Inst.getOperand(2).getImm();
      uint16_t BaseAddress = State.getReg(RegMem);
      // Handle pre-dec
      if (predec) {
        BaseAddress -= word ? 2 : 1;
        State.setReg(RegMem, BaseAddress);
      }
      State.setReg(RegMem, BaseAddress);
      // Load
      uint16_t Address = BaseAddress + Offset;
      uint16_t Val = State.getDataMem(Address);
      if (word)
        Val |= State.getDataMem(Address + 1) << 8;
      State.setReg(RegDst, Val);
      // Handle post-inc
      if (postinc) {
        BaseAddress += word ? 2 : 1;
        State.setReg(RegMem, BaseAddress);
      }
      break;
    }

    // Store
    case AAP::STB:
    case AAP::STB_short:
    case AAP::STW:
    case AAP::STW_short:
    case AAP::STB_postinc:
    case AAP::STB_postinc_short:
    case AAP::STW_postinc:
    case AAP::STW_postinc_short:
    case AAP::STB_predec:
    case AAP::STB_predec_short:
    case AAP::STW_predec:
    case AAP::STW_predec_short: {
      bool postinc = (Inst.getOpcode() == AAP::STB_postinc ||
                      Inst.getOpcode() == AAP::STB_postinc_short ||
                      Inst.getOpcode() == AAP::STW_postinc ||
                      Inst.getOpcode() == AAP::STW_postinc_short) ? true : false;
      bool predec  = (Inst.getOpcode() == AAP::STB_predec ||
                      Inst.getOpcode() == AAP::STB_predec_short ||
                      Inst.getOpcode() == AAP::STW_predec ||
                      Inst.getOpcode() == AAP::STW_predec_short) ? true : false;
      bool word = (Inst.getOpcode() == AAP::STW ||
                   Inst.getOpcode() == AAP::STW_short ||
                   Inst.getOpcode() == AAP::STW_postinc ||
                   Inst.getOpcode() == AAP::STW_postinc_short ||
                   Inst.getOpcode() == AAP::STW_predec ||
                   Inst.getOpcode() == AAP::STW_predec_short) ? true : false;
      // Initial register values
      int RegMem = getLLVMReg(Inst.getOperand(0).getReg());
      int Offset = Inst.getOperand(1).getImm();
      int RegSrc = getLLVMReg(Inst.getOperand(0).getReg());
      uint16_t BaseAddress = State.getReg(RegMem);
      uint16_t Val = State.getReg(RegSrc);
      // Handle pre-dec
      if (predec) {
        BaseAddress -= word ? 2 : 1;
        State.setReg(RegMem, BaseAddress);
      }
      State.setReg(RegMem, BaseAddress);
      // Store
      uint16_t Address = BaseAddress + Offset;
      State.setDataMem(Address, Val & 0xff);
      if (word)
        State.setDataMem(Address + 1, Val >> 8);
      // Handle post-inc
      if (postinc) {
        BaseAddress += word ? 2 : 1;
        State.setReg(RegMem, BaseAddress);
      }
      break;
    }

    // Branch and Link, Jump and Link
    case AAP::BAL:
    case AAP::BAL_short:
    case AAP::JAL:
    case AAP::JAL_short: {
      int Reg = getLLVMReg(Inst.getOperand(1).getReg());
      State.setReg(Reg, newpc_w);
      uint16_t Imm = Inst.getOperand(0).getImm();
      int16_t SImm =
          (Inst.getOpcode() == AAP::BAL) ? static_cast<int16_t>(Imm) :
                                           signExtendBranchS(Imm);
      if (Inst.getOpcode() == AAP::BAL || Inst.getOpcode() == AAP::BAL_short)
        newpc_w = pc_w + SImm;
      else
        newpc_w = Imm;
      break;
    }

    // Conditional branches
    case AAP::BEQ_:
    case AAP::BEQ_short:
    case AAP::BNE_:
    case AAP::BNE_short:
    case AAP::BLTS_:
    case AAP::BLTS_short:
    case AAP::BGTS_:
    case AAP::BGTS_short:
    case AAP::BLTU_:
    case AAP::BLTU_short:
    case AAP::BGTU_:
    case AAP::BGTU_short: {
      uint16_t Imm = Inst.getOperand(0).getImm();
      uint16_t ValA = State.getReg(getLLVMReg(Inst.getOperand(1).getReg()));
      int16_t SValA = static_cast<int16_t>(ValA);
      uint16_t ValB = State.getReg(getLLVMReg(Inst.getOperand(2).getReg()));
      int16_t SValB = static_cast<int16_t>(ValB);
      bool longbr = (Inst.getOpcode() == AAP::BEQ_ ||
                     Inst.getOpcode() == AAP::BNE_ ||
                     Inst.getOpcode() == AAP::BLTS_ ||
                     Inst.getOpcode() == AAP::BGTS_ ||
                     Inst.getOpcode() == AAP::BLTU_ ||
                     Inst.getOpcode() == AAP::BGTU_) ? true : false;
      // FIXME: Long should use signExtendBranch
      int16_t SImm = longbr ? signExtendBranchCC(Imm) : signExtendBranchS(Imm);
      bool branch = false;
      // Decide whether to branch based on instruction type
      if (Inst.getOpcode() == AAP::BEQ_ || Inst.getOpcode() == AAP::BEQ_short)
        branch = (ValA == ValB) ? true : false;
      if (Inst.getOpcode() == AAP::BNE_ || Inst.getOpcode() == AAP::BNE_short)
        branch = (ValA != ValB) ? true : false;
      if (Inst.getOpcode() == AAP::BLTS_ || Inst.getOpcode() == AAP::BLTS_short)
        branch = (SValA < SValB) ? true : false;
      if (Inst.getOpcode() == AAP::BGTS_ || Inst.getOpcode() == AAP::BGTS_short)
        branch = (SValA > SValB) ? true : false;
      if (Inst.getOpcode() == AAP::BLTU_ || Inst.getOpcode() == AAP::BLTU_short)
        branch = (ValA < ValB) ? true : false;
      if (Inst.getOpcode() == AAP::BGTU_ || Inst.getOpcode() == AAP::BGTU_short)
        branch = (ValA > ValB) ? true : false;
      // Branch if needed
      if (branch)
        newpc_w = pc_w + SImm;
      break;
    }

    // Branch
    case AAP::BRA:
    case AAP::BRA_short: {
      int32_t Offset = Inst.getOperand(0).getImm();
      newpc_w = pc_w + Offset;
      break;
    }

    // Jump
    case AAP::JMP:
    case AAP::JMP_short: {
      int Reg = getLLVMReg(Inst.getOperand(0).getReg());
      newpc_w = State.getReg(Reg);
      break;
    }
  } // end opcode switch

  // By default, we exected the instruction
  return SimStatus::SIM_OK;
}

SimStatus AAPSimulator::step() {
  MCInst Inst;
  uint64_t Size;
  uint32_t pc_w = State.getPC();
  ArrayRef<uint8_t> *Bytes = State.getCodeArray();

  if (DisAsm->getInstruction(Inst, Size, Bytes->slice(pc_w << 1), (pc_w << 1),
                             nulls(), nulls())) {
    // Instruction decoded, execute it and write back our PC
    dbgs() << format("%06" PRIx64 ":", pc_w);
    IP->printInst(&Inst, dbgs(), "", *STI);
    dbgs() << "\n";

    uint32_t newpc_w = pc_w + (Size >> 1);
    SimStatus status;
    status = exec(Inst, pc_w, newpc_w);
    State.setPC(newpc_w);

    return status;
  }
  else {
    return SimStatus::SIM_INVALID_INSN;
  }
}