; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=x86_64-apple-darwin -mcpu=x86-64 -mattr=+avx2 < %s | FileCheck %s --check-prefix=AVX2
; RUN: llc -mtriple=x86_64-apple-darwin -mcpu=x86-64 -mattr=+avx512f,+avx512vl,+avx512bw < %s | FileCheck %s --check-prefix=AVX512

define i4 @v4i64(<4 x i64> %a, <4 x i64> %b, <4 x i64> %c, <4 x i64> %d) {
; AVX2-LABEL: v4i64:
; AVX2:       ## BB#0:
; AVX2-NEXT:    vpcmpgtq %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    vextracti128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpcmpgtq %ymm3, %ymm2, %ymm1
; AVX2-NEXT:    vextracti128 $1, %ymm1, %xmm2
; AVX2-NEXT:    vpacksswb %xmm2, %xmm1, %xmm1
; AVX2-NEXT:    vpand %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpextrd $3, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrd $2, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrd $1, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vmovd %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    movb -{{[0-9]+}}(%rsp), %al
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512-LABEL: v4i64:
; AVX512:       ## BB#0:
; AVX512-NEXT:    vpcmpgtq %ymm1, %ymm0, %k1
; AVX512-NEXT:    vpcmpgtq %ymm3, %ymm2, %k0 {%k1}
; AVX512-NEXT:    kmovd %k0, %eax
; AVX512-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX512-NEXT:    movb -{{[0-9]+}}(%rsp), %al
; AVX512-NEXT:    vzeroupper
; AVX512-NEXT:    retq
  %x0 = icmp sgt <4 x i64> %a, %b
  %x1 = icmp sgt <4 x i64> %c, %d
  %y = and <4 x i1> %x0, %x1
  %res = bitcast <4 x i1> %y to i4
  ret i4 %res
}

define i4 @v4f64(<4 x double> %a, <4 x double> %b, <4 x double> %c, <4 x double> %d) {
; AVX2-LABEL: v4f64:
; AVX2:       ## BB#0:
; AVX2-NEXT:    vcmpltpd %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    vextractf128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vcmpltpd %ymm2, %ymm3, %ymm1
; AVX2-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX2-NEXT:    vpacksswb %xmm2, %xmm1, %xmm1
; AVX2-NEXT:    vpand %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpextrd $3, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrd $2, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrd $1, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vmovd %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    movb -{{[0-9]+}}(%rsp), %al
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512-LABEL: v4f64:
; AVX512:       ## BB#0:
; AVX512-NEXT:    vcmpltpd %ymm0, %ymm1, %k1
; AVX512-NEXT:    vcmpltpd %ymm2, %ymm3, %k0 {%k1}
; AVX512-NEXT:    kmovd %k0, %eax
; AVX512-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX512-NEXT:    movb -{{[0-9]+}}(%rsp), %al
; AVX512-NEXT:    vzeroupper
; AVX512-NEXT:    retq
  %x0 = fcmp ogt <4 x double> %a, %b
  %x1 = fcmp ogt <4 x double> %c, %d
  %y = and <4 x i1> %x0, %x1
  %res = bitcast <4 x i1> %y to i4
  ret i4 %res
}

define i16 @v16i16(<16 x i16> %a, <16 x i16> %b, <16 x i16> %c, <16 x i16> %d) {
; AVX2-LABEL: v16i16:
; AVX2:       ## BB#0:
; AVX2-NEXT:    vpcmpgtw %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    vextracti128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpcmpgtw %ymm3, %ymm2, %ymm1
; AVX2-NEXT:    vextracti128 $1, %ymm1, %xmm2
; AVX2-NEXT:    vpacksswb %xmm2, %xmm1, %xmm1
; AVX2-NEXT:    vpand %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpextrb $15, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $14, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $13, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $12, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $11, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $10, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $9, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $8, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $7, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $6, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $5, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $4, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $3, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $2, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $1, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrb $0, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    movzwl -{{[0-9]+}}(%rsp), %eax
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512-LABEL: v16i16:
; AVX512:       ## BB#0:
; AVX512-NEXT:    vpcmpgtw %ymm1, %ymm0, %k1
; AVX512-NEXT:    vpcmpgtw %ymm3, %ymm2, %k0 {%k1}
; AVX512-NEXT:    kmovd %k0, %eax
; AVX512-NEXT:    ## kill: %AX<def> %AX<kill> %EAX<kill>
; AVX512-NEXT:    vzeroupper
; AVX512-NEXT:    retq
  %x0 = icmp sgt <16 x i16> %a, %b
  %x1 = icmp sgt <16 x i16> %c, %d
  %y = and <16 x i1> %x0, %x1
  %res = bitcast <16 x i1> %y to i16
  ret i16 %res
}

define i8 @v8i32(<8 x i32> %a, <8 x i32> %b, <8 x i32> %c, <8 x i32> %d) {
; AVX2-LABEL: v8i32:
; AVX2:       ## BB#0:
; AVX2-NEXT:    vpcmpgtd %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    vextracti128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpcmpgtd %ymm3, %ymm2, %ymm1
; AVX2-NEXT:    vextracti128 $1, %ymm1, %xmm2
; AVX2-NEXT:    vpacksswb %xmm2, %xmm1, %xmm1
; AVX2-NEXT:    vpand %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpextrw $7, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $6, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $5, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $4, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $3, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $2, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $1, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vmovd %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    movb -{{[0-9]+}}(%rsp), %al
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512-LABEL: v8i32:
; AVX512:       ## BB#0:
; AVX512-NEXT:    vpcmpgtd %ymm1, %ymm0, %k1
; AVX512-NEXT:    vpcmpgtd %ymm3, %ymm2, %k0 {%k1}
; AVX512-NEXT:    kmovd %k0, %eax
; AVX512-NEXT:    ## kill: %AL<def> %AL<kill> %EAX<kill>
; AVX512-NEXT:    vzeroupper
; AVX512-NEXT:    retq
  %x0 = icmp sgt <8 x i32> %a, %b
  %x1 = icmp sgt <8 x i32> %c, %d
  %y = and <8 x i1> %x0, %x1
  %res = bitcast <8 x i1> %y to i8
  ret i8 %res
}

define i8 @v8f32(<8 x float> %a, <8 x float> %b, <8 x float> %c, <8 x float> %d) {
; AVX2-LABEL: v8f32:
; AVX2:       ## BB#0:
; AVX2-NEXT:    vcmpltps %ymm0, %ymm1, %ymm0
; AVX2-NEXT:    vextractf128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpacksswb %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vcmpltps %ymm2, %ymm3, %ymm1
; AVX2-NEXT:    vextractf128 $1, %ymm1, %xmm2
; AVX2-NEXT:    vpacksswb %xmm2, %xmm1, %xmm1
; AVX2-NEXT:    vpand %xmm1, %xmm0, %xmm0
; AVX2-NEXT:    vpextrw $7, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $6, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $5, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $4, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $3, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $2, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vpextrw $1, %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    vmovd %xmm0, %eax
; AVX2-NEXT:    andl $1, %eax
; AVX2-NEXT:    movb %al, -{{[0-9]+}}(%rsp)
; AVX2-NEXT:    movb -{{[0-9]+}}(%rsp), %al
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512-LABEL: v8f32:
; AVX512:       ## BB#0:
; AVX512-NEXT:    vcmpltps %ymm0, %ymm1, %k1
; AVX512-NEXT:    vcmpltps %ymm2, %ymm3, %k0 {%k1}
; AVX512-NEXT:    kmovd %k0, %eax
; AVX512-NEXT:    ## kill: %AL<def> %AL<kill> %EAX<kill>
; AVX512-NEXT:    vzeroupper
; AVX512-NEXT:    retq
  %x0 = fcmp ogt <8 x float> %a, %b
  %x1 = fcmp ogt <8 x float> %c, %d
  %y = and <8 x i1> %x0, %x1
  %res = bitcast <8 x i1> %y to i8
  ret i8 %res
}

define i32 @v32i8(<32 x i8> %a, <32 x i8> %b, <32 x i8> %c, <32 x i8> %d) {
; AVX2-LABEL: v32i8:
; AVX2:       ## BB#0:
; AVX2-NEXT:    pushq %rbp
; AVX2-NEXT:  Lcfi0:
; AVX2-NEXT:    .cfi_def_cfa_offset 16
; AVX2-NEXT:  Lcfi1:
; AVX2-NEXT:    .cfi_offset %rbp, -16
; AVX2-NEXT:    movq %rsp, %rbp
; AVX2-NEXT:  Lcfi2:
; AVX2-NEXT:    .cfi_def_cfa_register %rbp
; AVX2-NEXT:    andq $-32, %rsp
; AVX2-NEXT:    subq $32, %rsp
; AVX2-NEXT:    vpcmpgtb %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    vpcmpgtb %ymm3, %ymm2, %ymm1
; AVX2-NEXT:    vpand %ymm1, %ymm0, %ymm0
; AVX2-NEXT:    vextracti128 $1, %ymm0, %xmm1
; AVX2-NEXT:    vpextrb $15, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $14, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $13, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $12, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $11, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $10, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $9, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $8, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $7, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $6, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $5, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $4, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $3, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $2, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $1, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $0, %xmm1, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $15, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $14, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $13, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $12, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $11, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $10, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $9, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $8, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $7, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $6, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $5, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $4, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $3, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $2, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $1, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    vpextrb $0, %xmm0, %eax
; AVX2-NEXT:    andb $1, %al
; AVX2-NEXT:    movb %al, (%rsp)
; AVX2-NEXT:    movl (%rsp), %eax
; AVX2-NEXT:    movq %rbp, %rsp
; AVX2-NEXT:    popq %rbp
; AVX2-NEXT:    vzeroupper
; AVX2-NEXT:    retq
;
; AVX512-LABEL: v32i8:
; AVX512:       ## BB#0:
; AVX512-NEXT:    vpcmpgtb %ymm1, %ymm0, %k1
; AVX512-NEXT:    vpcmpgtb %ymm3, %ymm2, %k0 {%k1}
; AVX512-NEXT:    kmovd %k0, %eax
; AVX512-NEXT:    vzeroupper
; AVX512-NEXT:    retq
  %x0 = icmp sgt <32 x i8> %a, %b
  %x1 = icmp sgt <32 x i8> %c, %d
  %y = and <32 x i1> %x0, %x1
  %res = bitcast <32 x i1> %y to i32
  ret i32 %res
}
