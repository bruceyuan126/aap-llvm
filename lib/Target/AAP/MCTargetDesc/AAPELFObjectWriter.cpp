//===-- AAPELFObjectWriter.cpp - AAP Target Descriptions ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AAPMCTargetDesc.h"
#include "AAPFixupKinds.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

using namespace llvm;

namespace {
class AAPELFObjectWriter : public MCELFObjectTargetWriter {
  StringRef CPU;

public:
  AAPELFObjectWriter(uint8_t OSABI, StringRef C);

  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // namespace

AAPELFObjectWriter::AAPELFObjectWriter(uint8_t OSABI, StringRef C)
    : MCELFObjectTargetWriter(/*Is64bit*/ false, OSABI, ELF::EM_AAP,
                              /*HasRelocationAddend*/ true),
      CPU(C) {}

unsigned AAPELFObjectWriter::getRelocType(MCContext & /*Ctx*/,
                                          const MCValue & /*Target*/,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
  llvm::MCFixupKind Kind = Fixup.getKind();

  switch ((unsigned)Kind) {
  case AAP::fixup_AAP_NONE:   return ELF::R_AAP_NONE;
  case AAP::fixup_AAP_BR32:   return ELF::R_AAP_BR32;
  case AAP::fixup_AAP_BRCC32: return ELF::R_AAP_BRCC32;
  case AAP::fixup_AAP_BAL32:  return ELF::R_AAP_BAL32;

  case AAP::fixup_AAP_ABS6:   return ELF::R_AAP_ABS6;
  case AAP::fixup_AAP_ABS9:   return ELF::R_AAP_ABS9;
  case AAP::fixup_AAP_ABS10:  return ELF::R_AAP_ABS10;
  case AAP::fixup_AAP_ABS12:  return ELF::R_AAP_ABS12;
  case AAP::fixup_AAP_ABS16:  return ELF::R_AAP_ABS16;

  case AAP::fixup_AAP_SHIFT6: return ELF::R_AAP_SHIFT6;
  case AAP::fixup_AAP_OFF10:  return ELF::R_AAP_OFF10;

  case FK_Data_1:             return ELF::R_AAP_8;
  case FK_Data_2:             return ELF::R_AAP_16;
  case FK_Data_4:             return ELF::R_AAP_32;
  case FK_Data_8:             return ELF::R_AAP_64;

  // Instrs with these fixups should never be generated or parsed, so for now
  // we should not be emitting relocations for them.
  case AAP::fixup_AAP_BR16:
  case AAP::fixup_AAP_BRCC16:
  case AAP::fixup_AAP_BAL16:
    llvm_unreachable("Cannot emit relocations for short instruction fixups!");
  default:
    llvm_unreachable("Unimplemented fixup kind!");
  }
  return ELF::R_AAP_NONE;
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createAAPELFObjectWriter(uint8_t OSABI, StringRef CPU) {
  return llvm::make_unique<AAPELFObjectWriter>(OSABI, CPU);
}
