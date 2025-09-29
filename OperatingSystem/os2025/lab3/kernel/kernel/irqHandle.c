#include "x86.h"
#include "device.h"

extern TSS tss;
extern ProcessTable pcb[MAX_PCB_NUM];
extern int current;

extern int displayRow;
extern int displayCol;

void GProtectFaultHandle(struct StackFrame *sf);

void timerHandle(struct StackFrame *sf);

void syscallHandle(struct StackFrame *sf);

void sysWrite(struct StackFrame *sf);
void sysPrint(struct StackFrame *sf);

void sysFork(struct StackFrame *sf);
void sysExec(struct StackFrame *sf);
void sysSleep(struct StackFrame *sf);
void sysExit(struct StackFrame *sf);
void sysGetPid(struct StackFrame *sf);
void sysGetPpid(struct StackFrame *sf);
void sysWait(struct StackFrame *sf);

uint32_t schedule();
void contextSwitch(int nxt);
uint32_t loadUMain(uint32_t first_sector, uint32_t sector_count, uint32_t pid);

/*
kernel is loaded to location 0x100000, i.e., 1MB
size of kernel is not greater than 200*512 bytes, i.e., 100KB
user program is loaded to location 0x200000, i.e., 2MB
size of user program is not greater than 200*512 bytes, i.e., 100KB
*/
uint32_t loadUMain(uint32_t first_sector, uint32_t sector_count, uint32_t pid)
{
	putStr("into loadUMain\n");

	int i = 0;
	// int phoff = 0x34;					 // program header offset
	int offset = 0x1000;				 // .text section offset
	uint32_t elf = 0x100000 * (pid + 1); // physical memory addr to load
	uint32_t uMainEntry = 0x100000 * (pid + 1);

	for (i = 0; i < sector_count; i++)
	{
		readSect((void *)(elf + i * 512), first_sector + i);
	}

	uMainEntry = ((struct ELFHeader *)elf)->entry; // entry address of the program
	// phoff = ((struct ELFHeader *)elf)->phoff;
	// offset = ((struct ProgramHeader *)(elf + phoff))->off;

	for (i = 0; i < sector_count * 512; i++)
	{
		*(uint8_t *)(elf + i) = *(uint8_t *)(elf + i + offset);
	}

	return uMainEntry;
}

// TODO3: implement schedule
/*
 * Schedules the next process to run
 *
 * Returns:
 *   pid_t - The process ID of the scheduled process
 *           Returns -1 if no process is available for scheduling
 */
uint32_t schedule()
{
	// Select the next process and perform context switching
	int i = (current + 1) % MAX_PCB_NUM;
	for (; i != current; i = (i + 1) % MAX_PCB_NUM)
	{
		if (pcb[i].state == STATE_RUNNABLE)
		{
			break;
		}
	}

	current = i;

	// if (pcb[i].state != STATE_RUNNABLE)
	// {
		// if (pcb[0].state != STATE_RUNNABLE)
		// {
		// 	return -1;
		// }
		// else
		// {
		// 	i = 0;
		// }
	// 	i = 0;
	// }

	return current;
}

void contextSwitch( int cur)
{
	uint32_t tmpStackTop = pcb[cur].stackTop;
	pcb[cur].stackTop = pcb[cur].prevStackTop;
	// 设置TSS
	tss.esp0 = pcb[cur].stackTop;
	tss.ss0 = KSEL(SEG_KDATA);
	// 切换内核栈
	asm volatile("movl %0, %%esp" : : "m"(tmpStackTop));
	// 恢复寄存器状态
	asm volatile("popl %gs");
	asm volatile("popl %fs");
	asm volatile("popl %es");
	asm volatile("popl %ds");
	asm volatile("popal");
	asm volatile("addl $8, %esp");
	asm volatile("iret");
}

void irqHandle(struct StackFrame *sf)
{
	// pointer sf = esp
	/* Reassign segment register */
	asm volatile("movw %%ax, %%ds" ::"a"(KSEL(SEG_KDATA)));
	/*XXX Save esp to stackTop */
	uint32_t tmpStackTop = pcb[current].stackTop;
	pcb[current].prevStackTop = pcb[current].stackTop;
	pcb[current].stackTop = (uint32_t)sf;

	switch (sf->irq)
	{
	case -1:
		break;
	case 0xd:
		GProtectFaultHandle(sf);
		break;
	case 0x20:
		timerHandle(sf);
		break;
	case 0x80:
		syscallHandle(sf);
		break;
	default:
		assert(0);
	}
	/*XXX Recover stackTop */
	pcb[current].stackTop = tmpStackTop;
}

void GProtectFaultHandle(struct StackFrame *sf)
{
	assert(0);
	return;
}

// TODO2: implement timerHandle
/*
进程切换的流程：
1. 遍历pcb，将状态为STATE_BLOCKED的进程的sleepTime减一，如果进程的sleepTime变为0，重新设为STATE_RUNNABLE
2. 递增当前进程的 timeCount。若时间片耗尽（timeCount == MAX_TIME_COUNT）且存在其他可运行进程（STATE_RUNNABLE），则触发进程切换；否则继续执行当前进程。
*/
void timerHandle(struct StackFrame *sf)
{
	// putStr("into timerHandle\n");

	for (int i = 0; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state == STATE_BLOCKED)
		{
			if (pcb[i].sleepTime > 0) // 对通过 sleep 阻塞的进程递减
			{
				pcb[i].sleepTime--;
				if (pcb[i].sleepTime == 0)
				{
					pcb[i].state = STATE_RUNNABLE;
				}
			}
			// 通过wait阻塞的进程不作处理
		}
	}

	pcb[current].timeCount++;

	// putStr("pid = ");
	// putNum(current);
	// putStr(", timeCount = ");
	// putNum(pcb[current].timeCount);
	// putStr("\n");

	if (pcb[current].timeCount > MAX_TIME_COUNT)
	{
		// putStr("pid = ");
		// putNum(current);
		// putStr(", timeCount > MAX_TIME_COUNT\n");
		// putChar('\n');

		pcb[current].timeCount = 0;
		pcb[current].state = STATE_RUNNABLE;

		int next = schedule();

		current = next;
		pcb[current].state = STATE_RUNNING;
	}

	contextSwitch(current);

	return;
}

// TODO4: implement syscallHandle
void syscallHandle(struct StackFrame *sf)
{
	switch (sf->eax)
	{ // syscall number
	case 0:
		sysWrite(sf);
		break; // for SYS_WRITE
	case 1:
		sysFork(sf);
		break; // for SYS_FORK
	case 2:
		sysExec(sf);
		break; // for SYS_EXEC
	case 3:
		sysSleep(sf);
		break; // for SYS_SLEEP
	case 4:
		sysExit(sf);
		break; // for SYS_EXIT
	case 5:
		sysGetPid(sf);
		break; // for SYS_GETPID
	case 6:
		sysGetPpid(sf);
		break; // for SYS_GETPPID
	case 7:
		sysWait(sf);
		break; // for SYS_WAIT
	default:
		break;
	}
}

void sysWrite(struct StackFrame *sf)
{
	switch (sf->ecx)
	{ // file descriptor
	case 0:
		sysPrint(sf);
		break; // for STD_OUT
	default:
		break;
	}
}

void sysPrint(struct StackFrame *sf)
{
	int sel = sf->ds; // segment selector for user data, need further modification
	char *str = (char *)sf->edx;
	int size = sf->ebx;
	int i = 0;
	int pos = 0;
	char character = 0;
	uint16_t data = 0;
	asm volatile("movw %0, %%es" ::"m"(sel));
	for (i = 0; i < size; i++)
	{
		asm volatile("movb %%es:(%1), %0" : "=r"(character) : "r"(str + i));
		if (character == '\n')
		{
			displayRow++;
			displayCol = 0;
			if (displayRow == 25)
			{
				displayRow = 24;
				displayCol = 0;
				scrollScreen();
			}
		}
		else
		{
			data = character | (0x0c << 8);
			pos = (80 * displayRow + displayCol) * 2;
			asm volatile("movw %0, (%1)" ::"r"(data), "r"(pos + 0xb8000));
			displayCol++;
			if (displayCol == 80)
			{
				displayRow++;
				displayCol = 0;
				if (displayRow == 25)
				{
					displayRow = 24;
					displayCol = 0;
					scrollScreen();
				}
			}
		}
		// asm volatile("int $0x20"); //XXX Testing irqTimer during syscall
		// asm volatile("int $0x20":::"memory"); //XXX Testing irqTimer during syscall
	}

	updateCursor(displayRow, displayCol);
	// take care of return value
	return;
}

// TODO5: implement sysFork, sysExec, sysSleep, sysExit, sysGetPid
/*
sysFork要做的是在寻找一个空闲的pcb做为子进程的进程控制块，将父进程的资源复制给子进程。如果没有空闲pcb，则fork失败，父进程返回-1，成功则子进程返回0，父进程返回子进程pid

在处理fork时有以下几点注意事项：
	代码段和数据段可以按照前文的说明进行完全拷贝
	pcb的复制时，需要考虑哪些内容可以直接复制，哪些内容通过计算得到，哪些内容和父进程无关
	返回值放在哪

Tip
	initIdle()中有初始化pcb[0]的经验可供参考
*/
void sysFork(struct StackFrame *sf)
{
	putStr("into sysFork\n");
	putStr("pid = ");
	putNum(current);
	putStr("\n");

	// 查找空闲 pcb
	int i;
	for (i = 1; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state == STATE_DEAD)
			break;
	}
	if (i == MAX_PCB_NUM)
	{
		pcb[current].regs.eax = -1; // 没有空闲 pcb
		return;
	}

	// 复制用户空间
	for (int j = 0; j < 0x100000; j++)
	{
		*(uint8_t *)(j + (i + 1) * 0x100000) = *(uint8_t *)(j + (current + 1) * 0x100000);
	}

	// 复制PCB
	for (int j = 0; j < sizeof(ProcessTable); j++)
	{
		*((uint8_t *)(&pcb[i]) + j) = *((uint8_t *)(&pcb[current]) + j);
	}

	// 计算堆栈指针偏移量
	pcb[i].stackTop = (uint32_t)&(pcb[i].regs);
	pcb[i].prevStackTop = (uint32_t)&(pcb[i].stackTop);
	// uint32_t offset = (uint32_t)pcb[current].stackTop - (uint32_t)&pcb[current].stack[0];
	// pcb[i].stackTop = offset + (uint32_t)&pcb[i].stack[0];
	// uint32_t prev_offset = (uint32_t)pcb[current].prevStackTop - (uint32_t)&pcb[current].stack[0];
	// pcb[i].prevStackTop = prev_offset + (uint32_t)&pcb[i].stack[0];

	// 设置子进程状态
	pcb[i].state = STATE_RUNNABLE;
	pcb[i].timeCount = 0;
	pcb[i].sleepTime = 0;
	pcb[i].pid = i;

	// 子进程的父进程ID设置为当前进程的ID
	pcb[i].ppid = pcb[current].pid;
	// 增加父进程的子进程计数
	pcb[current].childCount++;

	// 设置段寄存器
	pcb[i].regs.ss = USEL(2 + 2 * i);
	pcb[i].regs.cs = USEL(1 + 2 * i);
	pcb[i].regs.ds = USEL(2 + 2 * i);
	pcb[i].regs.es = USEL(2 + 2 * i);
	pcb[i].regs.fs = USEL(2 + 2 * i);
	pcb[i].regs.gs = USEL(2 + 2 * i);

	// 设置返回值
	pcb[current].regs.eax = i;
	pcb[i].regs.eax = 0;

	pcb[current].state = STATE_RUNNABLE;

	putStr("fork success, parent pid = ");
	putNum(current);
	putStr(", child pid = ");
	putNum(i);
	putStr("\n");

	return;
}

/*
sysExec是操作系统内核中实现程序替换功能的关键系统调用，它负责将当前进程的内存映像替换为新的程序映像。下面我们将详细解析这个系统调用的实现原理和功能：

sysExec主要完成以下任务：
	替换当前进程的地址空间
	初始化新程序的执行环境
	开始执行新程序
*/

void sysExec(struct StackFrame *sf)
{
	putStr("into sysExec\n");
	putStr("pid = ");
	putNum(current);
	putStr("\n");

	// 加载用户程序
	uint32_t entry = loadUMain(sf->ecx, sf->edx, current);

	// 设置入口点和堆栈指针
	pcb[current].regs.eip = entry;
	pcb[current].regs.esp = 0x200000;

	// 设置段寄存器
	pcb[current].regs.cs = USEL(1 + current * 2);
	pcb[current].regs.ds = USEL(2 + current * 2);
	pcb[current].regs.es = USEL(2 + current * 2);
	pcb[current].regs.fs = USEL(2 + current * 2);
	pcb[current].regs.gs = USEL(2 + current * 2);
	pcb[current].regs.ss = USEL(2 + current * 2);

	// 重置通用寄存器
	pcb[current].regs.eax = 0;
	pcb[current].regs.ebx = 0;
	pcb[current].regs.ecx = 0;
	pcb[current].regs.edx = 0;
	pcb[current].regs.esi = 0;
	pcb[current].regs.edi = 0;
	pcb[current].regs.ebp = 0;

	// 设置标志寄存器，启用中断
	pcb[current].regs.eflags = 0x202; // IF=1

	return;
}

/*
将当前的进程的sleepTime设置为传入的参数，将当前进程的状态设置为STATE_BLOCKED，然后再调用schedule()，选择下一个进程并进行切换

TIPS
	在schedule()中执行进程切换，调度
	建议将进程切换的操作，放在中断返回iret前

需要注意的是判断中断嵌套
*/
void sysSleep(struct StackFrame *sf)
{
	putStr("into sysSleep\n");
	putStr("pid = ");
	putNum(current);
	putStr("\n");

	// 设置当前进程为阻塞状态
	pcb[current].state = STATE_BLOCKED;
	pcb[current].sleepTime = sf->ecx;

	// 选择下一个可运行进程并进行切换
	int next = schedule();
	// if (next == -1)
	// {
	// 	// 没有可运行进程，恢复当前进程
	// 	pcb[current].state = STATE_RUNNING;
	// 	return;
	// }

	// 设置新进程状态
	current = next;
	pcb[current].state = STATE_RUNNING;

	contextSwitch(current);
}

/*
将当前进程的状态设置为STATE_DEAD，然后模拟时钟中断进行进程切换
*/
void sysExit(struct StackFrame *sf)
{
	putStr("into sysExit\n");
	putStr("pid = ");
	putNum(current);
	putStr("\n");

	// 设置当前进程为死亡状态
	pcb[current].state = STATE_DEAD;

	int next = -1;

	if (pcb[current].ppid != -1)
	{
		// 减少父进程的子进程计数
		pcb[pcb[current].ppid].childCount--;

		putStr("parent state = ");
		putNum(pcb[pcb[current].ppid].state);
		putStr(", parent sleepTime = ");
		putNum(pcb[pcb[current].ppid].sleepTime);
		putStr("\n");

		// 如果父进程正在等待子进程，则唤醒父进程
		if (pcb[pcb[current].ppid].state == STATE_BLOCKED && pcb[pcb[current].ppid].sleepTime <= 0)
		{
			putStr("wake up parent process\n");

			pcb[pcb[current].ppid].state = STATE_RUNNABLE;
			// 父进程返回自己的pid
			pcb[pcb[current].ppid].regs.eax = pcb[current].ppid;
			// 选择父进程作为下一个运行的进程
			next = pcb[current].ppid;
		}
	}

	// 处理子进程
	for (int i = 0; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state != STATE_DEAD && pcb[i].ppid == current)
		{
			pcb[i].ppid = 0;
		}
	}

	// 选择下一个可运行进程并进行切换
	if (next == -1)
	{
		next = schedule();
	}
	// if (next == -1)
	// {
	// 	// 没有可运行进程
	// 	// putStr("No runnable process!\n");
	// 	while (1)
	// 		;
	// }

	putStr("next pid = ");
	putNum(next);
	putStr("\n");
	// 设置新进程状态
	current = next;
	pcb[current].state = STATE_RUNNING;

	contextSwitch(current);
}

/*
获取当前进程的pid
*/
void sysGetPid(struct StackFrame *sf)
{
	sf->eax = current;
	return;
}

// TODO optional 3: add sysHandle wait
void sysGetPpid(struct StackFrame *sf)
{
	sf->eax = pcb[current].ppid;
	return;
}

/*
有无已终止的子进程
	有：直接返回0
	无：检查是否有正在运行的子进程
		有：阻塞当前进程，调用schedule()选择下一个进程
		无：返回-1
*/
void sysWait(struct StackFrame *sf)
{
	putStr("into sysWait\n");
	putStr("pid = ");
	putNum(current);
	putStr("\n");

	int i;
	for (i = 1; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state == STATE_DEAD && pcb[i].ppid == current)
		{
			putStr("DEAD child pid = ");
			putNum(i);
			putStr("\n");
			break;
		}
	}
	// 如果有已终止的子进程，直接返回
	if (i != MAX_PCB_NUM)
	{
		putStr("child dead, return 0\n");
		sf->eax = 0; // 返回0表示子进程已终止
		return;
	}

	// 检查是否有正在运行的子进程
	int has_children = 0;
	for (i = 1; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state != STATE_DEAD && pcb[i].ppid == current)
		{
			has_children = 1;
			break;
		}
	}
	putStr("has_child = ");
	putNum(has_children);
	putStr("\n");
	// 如果没有任何子进程，返回-1
	if (!has_children)
	{
		putStr("no childs, return -1\n");
		sf->eax = -1;
		return;
	}

	// 如果有正在运行的子进程，但没有已终止的子进程，则阻塞当前进程
	pcb[current].state = STATE_BLOCKED;
	pcb[current].sleepTime = 0;

	int next = schedule();
	current = next;
	pcb[current].state = STATE_RUNNING;

	contextSwitch(current);
}