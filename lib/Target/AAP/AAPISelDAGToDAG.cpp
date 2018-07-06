//===-- AAPISelDAGToDAG.cpp - A dag to dag inst selector for AAP ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the AAP target.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "AAP-isel"
#include "AAPMachineFunctionInfo.h"
#include "AAPRegisterInfo.h"
#include "AAPSubtarget.h"
#include "AAPTargetMachine.h"
#include "MCTargetDesc/AAPMCTargetDesc.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
using namespace llvm;

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// AAPDAGToDAGISel - AAP specific code to select AAP machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//
namespace {

class AAPDAGToDAGISel : public SelectionDAGISel {

  /// TM - Keep a reference to AAPTargetMachine.
  AAPTargetMachine &TM;

public:
  AAPDAGToDAGISel(AAPTargetMachine &tm, CodeGenOpt::Level OptLevel)
      : SelectionDAGISel(tm), TM(tm) {}

  // Pass Name
  virtual StringRef getPassName() const override {
    return "AAP DAG->DAG Pattern Instruction Selection";
  }

  bool SelectInlineAsmMemoryOperand(const SDValue &Op, unsigned ConstraintID,
                                    std::vector<SDValue> &OutOps) override;

// Include the pieces autogenerated from the target description.
#include "AAPGenDAGISel.inc"

private:
  /// getTargetMachine - Return a reference to the TargetMachine, casted
  /// to the target-specific type.
  const AAPTargetMachine &getTargetMachine() {
    return static_cast<const AAPTargetMachine &>(TM);
  }

  void Select(SDNode *N) override;

  // Complex Pattern for address selection.
  bool SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset);
  bool SelectAddr_MO3(SDValue Addr, SDValue &Base, SDValue &Offset);
  bool SelectAddr_MO10(SDValue Addr, SDValue &Base, SDValue &Offset);
};
} // end anonymous namespace

bool AAPDAGToDAGISel::SelectInlineAsmMemoryOperand(
    const SDValue &Op, unsigned ConstraintID, std::vector<SDValue> &OutOps) {
  switch (ConstraintID) {
  default:
    return true;
  case InlineAsm::Constraint_m:
    SDLoc DL(Op);
    SDValue RC =
        CurDAG->getTargetConstant(AAP::GR64RegClass.getID(), DL, MVT::i16);
    SDNode *N = CurDAG->getMachineNode(TargetOpcode::COPY_TO_REGCLASS, DL,
                                       Op.getValueType(), Op, RC);
    SDValue Zero = CurDAG->getTargetConstant(0, DL, MVT::i16);
    OutOps.push_back(SDValue(N, 0));
    OutOps.push_back(Zero);
    break;
  }
  return false;
}

/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
void AAPDAGToDAGISel::Select(SDNode *Node) {
  unsigned Opcode = Node->getOpcode();
  SDLoc dl(Node);

  // Dump information about the Node being selected
  LLVM_DEBUG(errs() << "Selecting: "; Node->dump(CurDAG); errs() << "\n");

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    return;
  }

  ///
  // Instruction Selection not handled by the auto-generated
  // tablegen selection should be handled here.
  ///
  switch (Opcode) {
  case ISD::FrameIndex: {
    assert(Node->getValueType(0) == MVT::i16);

    int FI = cast<FrameIndexSDNode>(Node)->getIndex();
    SDValue TFI = CurDAG->getTargetFrameIndex(FI, MVT::i16);

    // Handle single use
    SDNode *N =
        CurDAG->getMachineNode(AAP::LEA, dl, MVT::i16, TFI,
                               CurDAG->getTargetConstant(0, dl, MVT::i16));
    ReplaceNode(Node, N);
    return;
  }
  default:
    break;
  }

  // Select the default instruction
  SelectCode(Node);
}

bool AAPDAGToDAGISel::SelectAddr(SDValue Addr, SDValue &Base, SDValue &Offset) {
  // if Address is FI, get the TargetFrameIndex
  if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
    SDLoc dl(FIN);
    Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i16);
    Offset = CurDAG->getTargetConstant(0, dl, MVT::i16);
    return true;
  }

  if ((Addr.getOpcode() == ISD::TargetExternalSymbol ||
       Addr.getOpcode() == ISD::TargetGlobalAddress)) {
    return false;
  }

  bool isConstantOffset = CurDAG->isBaseWithConstantOffset(Addr);
  bool isSubOffset = Addr.getOpcode() == ISD::SUB;

  // Addresses of the form Addr+const, Addr-const or Addr|const
  if ((isConstantOffset || isSubOffset) &&
      isa<ConstantSDNode>(Addr.getOperand(1))) {
    ConstantSDNode *CN = dyn_cast<ConstantSDNode>(Addr.getOperand(1));
    SDLoc dl(CN);

    if (isInt<16>(CN->getSExtValue())) {
      // If the first operand is a FI, get the TargetFI Node
      if (FrameIndexSDNode *FIN =
              dyn_cast<FrameIndexSDNode>(Addr.getOperand(0))) {
        Base = CurDAG->getTargetFrameIndex(FIN->getIndex(), MVT::i16);
      } else {
        Base = Addr.getOperand(0);
      }

      int64_t off = CN->getSExtValue();
      if (isConstantOffset) {
        Offset = CurDAG->getTargetConstant(off, dl, MVT::i16);
      } else {
        assert(isSubOffset);
        Offset = CurDAG->getTargetConstant(-off, dl, MVT::i16);
      }
      return true;
    }
  }
  return false;
}

bool AAPDAGToDAGISel::SelectAddr_MO3(SDValue Addr, SDValue &Base,
                                     SDValue &Offset) {
  SDValue B, O;
  bool ret = SelectAddr(Addr, B, O);
  if (ret) {
    int64_t c = dyn_cast<ConstantSDNode>(O)->getSExtValue();
    if (AAP::isOff3(c)) {
      Base = B;
      Offset = O;
      return true;
    }
  }
  return false;
}
bool AAPDAGToDAGISel::SelectAddr_MO10(SDValue Addr, SDValue &Base,
                                      SDValue &Offset) {
  SDValue B, O;
  bool ret = SelectAddr(Addr, B, O);
  if (ret) {
    int64_t c = dyn_cast<ConstantSDNode>(O)->getSExtValue();
    if (AAP::isOff10(c)) {
      Base = B;
      Offset = O;
      return true;
    }
  }
  return false;
}

/// createAAPISelDag - This pass converts a legalized DAG into a
/// AAP-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createAAPISelDag(AAPTargetMachine &TM,
                                     CodeGenOpt::Level OptLevel) {
  return new AAPDAGToDAGISel(TM, OptLevel);
}
