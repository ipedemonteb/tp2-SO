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


section .text

%macro syscaller 1
    push rbp
    mov rbp, rsp

    mov rdi, %1
    int 80h

    mov rsp, rbp
    pop rbp 
    ret
%endmacro


readCaller:
    syscaller 0
writeCaller:
    syscaller 1
printRectangleCaller:
    syscaller 2
getScreenWidthCaller:
    syscaller 3
getScreenHeightCaller:
    syscaller 4
printCharCaller:
    syscaller 5
waitCaller:
    syscaller 6
keyboardStatusCaller:
    syscaller 7
keyboardKeyCaller:
    syscaller 8

beepSysCaller:
    syscaller 9

fontSizeUpCaller:
    syscaller 10

fontSizeDownCaller:
    syscaller 11
getTimeCaller:
    syscaller 12

