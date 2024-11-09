

int : 1110 1000 1010 1010 1010 0100 . *2^0

1110 1000 1010 1010 1010 010 . 0 *2^1

1 . 110 1000 1010 1010 1010 0100 *2^23

double : 0 00(11个) 110 1000 1010 1010 1010 0100。。。

# 结论： double变回int 不会有精度损失 不会溢出 不会舍入舍出

>movsd $register1+$$register2 -> **$XMM寄存器** -> 64位
直接放 不改变机器数 改变真值

>cvtt $XMM -> int registers 
类型转换 不改变真值 改变机器数

>cmp registers & **imm**
if(=) congratulations
else oops