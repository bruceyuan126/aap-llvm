//===-- AAPSubtarget.h - Define Subtarget for the AAP -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the AAP specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef AAP_SUBTARGET_H
#define AAP_SUBTARGET_H

#include "AAPFrameLowering.h"
#include "AAPISelLowering.h"
#include "AAPInstrInfo.h"
#include "AAPSelectionDAGInfo.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "AAPGenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class AAPSubtarget : public AAPGenSubtargetInfo {
  virtual void anchor();
  AAPInstrInfo InstrInfo;
  AAPFrameLowering FrameLowering;
  AAPTargetLowering TLInfo;
  AAPSelectionDAGInfo TSInfo;

public:
  /// This constructor initializes the data members to match that
  /// of the specified triple.
  ///
  AAPSubtarget(const Triple &TT, const std::string &CPU, const std::string &FS,
               const TargetMachine &TM);

  /// ParseSubtargetFeatures - Parses features string setting specified
  /// subtarget options.  Definition of function is auto generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

  const AAPInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const AAPFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const AAPTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const AAPSelectionDAGInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }
  const TargetRegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }
};
} // End llvm namespace

#endif
