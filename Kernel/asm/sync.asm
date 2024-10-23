GLOBAL _xadd
GLOBAL _xchg
GLOBAL acquire
GLOBAL release

_xadd:
  mov rax, rdi
  lock xadd [rsi], eax
  ret

_xchg:
  mov rax, rsi
  xchg [rdi], eax
  ret

acquire:
  mov al, 0
.retry:
  xchg [rdi], al       ; en la clase en vez de rdi decia lock pero seguro es rdi
  test al, al
  jz .retry
  ret

release:
  mov byte [rdi], 1     ; idem
  ret
