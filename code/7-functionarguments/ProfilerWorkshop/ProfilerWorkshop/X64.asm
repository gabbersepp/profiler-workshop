
extern EnterCallbackCpp:proc

_TEXT   SEGMENT

PUBLIC FnEnterCallback

FnEnterCallback PROC
  push rbp
  mov rbp, rsp
  sub RSP, 20h
  CALL EnterCallbackCpp
  mov rsp, rbp
  pop rbp
  ret
FnEnterCallback ENDP

PUBLIC FnLeaveCallback

FnLeaveCallback PROC
  ret
FnLeaveCallback ENDP

PUBLIC FnTailcallCallback

FnTailcallCallback PROC
  ret
FnTailcallCallback ENDP

_TEXT   ENDS

END