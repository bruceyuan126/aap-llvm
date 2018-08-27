; RUN: llc -asm-show-inst -march=aap < %s | FileCheck %s


; Check the correctness of custom selectcc codegen


define i16 @foo(i16 %a, i16 *%b) {
; CHECK-LABEL: foo
; CHECK:         ldw $[[REG1:r[0-9]+]], [$r3, 0]
; CHECK:         beq .[[TEST2:LBB0_[0-9]+]], $r2, $[[REG1]]
; CHECK:         mov $[[RES1:r[0-9]+]], $[[REG1]]
  %val1 = load volatile i16, i16* %b
  %tst1 = icmp eq i16 %a, %val1
  %val2 = select i1 %tst1, i16 %a, i16 %val1

; CHECK:       .[[TEST2]]
; CHECK:         ldw $[[REG2:r[0-9]+]], [$r3, 0]
; CHECK:         bne .[[TEST3:LBB0_[0-9]+]], $[[RES1]], $[[REG2]]
; CHECK:         mov $[[RES2:r[0-9]+]], $[[REG2]]
  %val3 = load volatile i16, i16* %b
  %tst2 = icmp ne i16 %val2, %val3
  %val4 = select i1 %tst2, i16 %val2, i16 %val3

; CHECK:       .[[TEST3]]
; CHECK:         ldw $[[REG3:r[0-9]+]], [$r3, 0]
; CHECK:         bltu .[[TEST4:LBB0_[0-9]+]], $[[RES2]], $[[REG3]]
; CHECK:         mov $[[RES3:r[0-9]+]], $[[REG3]]
  %val5 = load volatile i16, i16* %b
  %tst3 = icmp ult i16 %val4, %val5
  %val6 = select i1 %tst3, i16 %val4, i16 %val5

; CHECK:       .[[TEST4]]
; CHECK:         ldw $[[REG4:r[0-9]+]], [$r3, 0]
; CHECK:         bleu .[[TEST5:LBB0_[0-9]+]], $[[RES3]], $[[REG4]]
; CHECK:         mov $[[RES4:r[0-9]+]], $[[REG4]]
  %val7 = load volatile i16, i16* %b
  %tst4 = icmp ule i16 %val6, %val7
  %val8 = select i1 %tst4, i16 %val6, i16 %val7

; CHECK:       .[[TEST5]]
; CHECK:         ldw $[[REG5:r[0-9]+]], [$r3, 0]
; CHECK:         blts .[[TEST6:LBB0_[0-9]+]], $[[RES4]], $[[REG5]]
; CHECK:         mov $[[RES5:r[0-9]+]], $[[REG5]]
  %val9 = load volatile i16, i16* %b
  %tst5 = icmp slt i16 %val8, %val9
  %val10 = select i1 %tst5, i16 %val8, i16 %val9

; CHECK:       .[[TEST6]]
; CHECK:         ldw $[[REG6:r[0-9]+]], [$r3, 0]
; CHECK:         bles .[[TEST7:LBB0_[0-9]+]], $[[RES5]], $[[REG6]]
; CHECK:         mov $[[RES6:r[0-9]+]], $[[REG6]]
  %val11 = load volatile i16, i16* %b
  %tst6 = icmp sle i16 %val10, %val11
  %val12 = select i1 %tst6, i16 %val10, i16 %val11

; Unsupported condition codes are not tested as there are multiple ways
; for codegen to create the select.

  ret i16 %val12
}
