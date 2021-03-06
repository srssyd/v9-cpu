# Lab1实验报告

在lab1.c中，主要包含以下功能：
- 处理时钟中断
- 进行内核线程的切换
- 在不同的线程输出不同的内容


### 时钟中断的处理
通过```stmr(5000)```这条指令，控制每5000个始终周期进入一次时间终端，并由```ivec(alltraps)```这条指令设置由```alltraps```这个函数进行中断处理。

### 内核线程的切换

维护两个在内核上的栈，分别为task0_stack与task1_stack。

在最开始，在task1_stack上分别push了 task1的地址，中断的错误码，a,b,c三个寄存器的值以及返回地址。

之后，task0执行，遇到时间中断后，先保存现有的寄存器，并与task1的栈互换。此时，由于返回地址被修改，```trap```函数会返回到```trapret```这个函数中。然后在```trapret```中，pop初始的a,b,c三个寄存器的值，并由```RTI```指令，pop中断的错误码,并把task1的地址给予pc寄存器。从此之后，线程切换到task1。

在这之后，每次时间中断，都会进行栈与寄存器及返回地址的切换。只是此时不再进入```trapret```，而在```alltraps```中处理剩下的逻辑。

### 输出的处理

通过向1号端口输出数据，来实现向屏幕中打印字符串。
