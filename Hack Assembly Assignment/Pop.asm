@256
D=A
@SP
M=D
D=0
//The code above is for setting my stack pointer at 256
@42
D=A
@SP
A=M
M=D
@SP
M=M+1
//The code above is to create a stack and have the stack pointer correct
@SP
M=M-1
A=M
D=M
(END)
@END
0;JMP
//The code above is for Pop
