# Quick help with GDB commands

## Attach to remote target

- target remote localhost:50250
- target extended-remote localhost:50250

## Running programm

- monitor reset init
- load
- continue
- monitor reset halt
- r

## Breakpoint

- b hello.c:main

## Programm stepping

- n - next - next source line
- ni - netxi - next instruction
- s - step - next source line, goes inside functions
- si - stepi - next instruction, but goes into CALL
- until - to line greater than the current, step LOOPS for example
- finish - to end of function / stack frame

## Info

- bt - backtrace
- up - one stack frame UP
- down - one stack frame DOWN
- info registers
- info locals
