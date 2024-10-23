GLOBAL readCaller
GLOBAL writeCaller
GLOBAL printRectangleCaller
GLOBAL getScreenWidthCaller
GLOBAL getScreenHeightCaller
GLOBAL printCharCaller
GLOBAL waitCaller
GLOBAL keyboardStatusCaller
GLOBAL keyboardKeyCaller
GLOBAL beepSysCaller
GLOBAL fontSizeUpCaller
GLOBAL fontSizeDownCaller
GLOBAL getTimeCaller
GLOBAL syscaller
GLOBAL create_process_caller
section .text

%macro syscaller_macro 1
    push rbp
    mov rbp, rsp

    mov rdi, %1
    int 80h

    mov rsp, rbp
    pop rbp 
    ret
%endmacro

syscaller: 
    push rbp
    mov rbp, rsp

    int 80h

    mov rsp, rbp
    pop rbp 
    ret

readCaller:
    syscaller_macro 0

writeCaller:
    syscaller_macro 1

printRectangleCaller:
    syscaller_macro 2

getScreenWidthCaller:
    syscaller_macro 3

getScreenHeightCaller:
    syscaller_macro 4

printCharCaller:
    syscaller_macro 5

waitCaller:
    syscaller_macro 6

keyboardStatusCaller:
    syscaller_macro 7

keyboardKeyCaller:
    syscaller_macro 8

beepSysCaller:
    syscaller_macro 9

fontSizeUpCaller:
    syscaller_macro 10

fontSizeDownCaller:
    syscaller_macro 11

getTimeCaller:
    syscaller_macro 12

create_process_caller:
    syscaller_macro 15