global _f1
global _f2
global _f3
global _df1 
global _df2
global _df3

global _f4
global _f5
global _f6
global _df4
global _df5
global _df6


section .data
    const1 dq 2.0
    const2 dq 5.0
    const3 dq -1.0
    const4 dq 3.0

section .text
_f1: ; 2^x+1
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    ; integer part
    fld qword[ebp + 8] ; x
    frndint ; [x]
    fld1
    fscale ; st0 = st0 * 2^st1 = 2^[x]
    
    ; fraction part
    fld qword[ebp + 8]
    fld st0 ; duplicate x
    frndint ; [x] in st0
    fsubp st1, st0 ; st0 = x-[x] = {x}
    f2xm1 ; st0 = 2^{x} - 1
    fld1
    faddp ; st0 = 2^{x}
    
    fmulp ; st0 = 2^[x] * 2^{x} = 2^x
    fld1
    faddp ; 2^x+1
    
    leave
    ret  
    
_f2: ; x^5
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld qword[ebp + 8]
    fmul qword[ebp + 8]
    fmul qword[ebp + 8]
    fmul qword[ebp + 8]
    fmul qword[ebp + 8]
    
    leave 
    ret
       
_f3: ; (1-x)/3
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld1
    fld qword[ebp + 8]
    fsubp
    fld qword[const4]
    fdivp
    
    add esp, 4
    leave
    ret
   
_df1: ; (2^x + 1)' = 2^x * ln2
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    ; integer part
    fld qword[ebp + 8] ; x
    frndint ; [x]
    fld1
    fscale ; st0 = st0 * 2^st1 = 2^[x]
    
    ; fraction part
    fld qword[ebp + 8]
    fld st0 ; duplicate x
    frndint ; [x] in st0
    fsubp st1, st0 ; st0 = x-[x] = {x}
    f2xm1 ; st0 = 2^{x} - 1
    fld1
    faddp ; st0 = 2^{x}
    
    fmulp ; st0 = 2^[x] * 2^{x} = 2^x
    fldln2
    fmulp ; 2^x * ln2
    
    leave
    ret

_df2: ; (x^5)' = 5 * x^4
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld qword[const2]
    fld qword[ebp + 8]
    fld qword[ebp + 8]
    fld qword[ebp + 8]
    fld qword[ebp + 8]
    fmulp
    fmulp
    fmulp
    fmulp
    
    leave
    ret

_df3: ; ((1 - x)/3)' = -1/3
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld qword[const3]
    fld qword[const4]
    fdivp
    
    leave
    ret
    
; ----- test functions ----- ;

_f4: ; 1 + x
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld1
    fld qword[ebp + 8]
    faddp
    
    leave
    ret
    
_f5: ; 1 - x
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld1
    fld qword[ebp + 8]
    fsubp
    
    leave
    ret
    
_f6: ; 0
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fldz
    
    leave
    ret
    
_df4: ; (1+x)' = 1
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld1
    
    leave
    ret
    
_df5: ; (1-x)' = -1
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fld qword[const3]
    
    leave
    ret
    
_df6: ; (0)' = 0
    push ebp
    mov ebp, esp
    sub esp, 8
    finit
    
    fldz
    
    leave
    ret