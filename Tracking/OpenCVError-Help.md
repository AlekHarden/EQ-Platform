### Find that pesky OpenCV Error!

You can use gdb to debug the program and find which line is erroring.

1. Compile with -g flag
2. `gdb <executable>`
3. `set new-console on`
4. `run`
5. ctrl-c in gdb window to stop frozen program
6. `bt`

*or*

6. `info thread`
7. `thread 1` or `thread <#>`
8. `bt`

