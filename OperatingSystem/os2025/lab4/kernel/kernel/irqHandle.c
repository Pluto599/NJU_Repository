#include "x86.h"
#include "device.h"

#define SYS_WRITE 0
#define SYS_READ 1
#define SYS_FORK 2
#define SYS_EXEC 3
#define SYS_SLEEP 4
#define SYS_EXIT 5
#define SYS_GETPID 6
#define SYS_SEM 7
#define SYS_SHAREDVAR 8

#define STD_OUT 0
#define STD_IN 1

#define SEM_INIT 0
#define SEM_WAIT 1
#define SEM_POST 2
#define SEM_DESTROY 3

#define SHAREDVAR_CREATE 0
#define SHAREDVAR_DESTROY 1
#define SHAREDVAR_READ 2
#define SHAREDVAR_WRITE 3

extern TSS tss;

extern ProcessTable pcb[MAX_PCB_NUM];
extern int current;

extern Semaphore sem[MAX_SEM_NUM];
extern Device dev[MAX_DEV_NUM];
extern SharedVariable sharedVar[MAX_SHARED_VAR_NUM];

extern int displayRow;
extern int displayCol;

extern uint32_t keyBuffer[MAX_KEYBUFFER_SIZE];
extern int bufferHead;
extern int bufferTail;

void GProtectFaultHandle(struct StackFrame *sf);
void timerHandle(struct StackFrame *sf);
void keyboardHandle(struct StackFrame *sf);
void syscallHandle(struct StackFrame *sf);

void sysWrite(struct StackFrame *sf);
void sysRead(struct StackFrame *sf);
void sysFork(struct StackFrame *sf);
void sysExec(struct StackFrame *sf);
void sysSleep(struct StackFrame *sf);
void sysExit(struct StackFrame *sf);
void sysGetPid(struct StackFrame *sf);

void sysSem(struct StackFrame *sf);

void sysWriteStdOut(struct StackFrame *sf);

void sysReadStdIn(struct StackFrame *sf);

void sysSemInit(struct StackFrame *sf);
void sysSemWait(struct StackFrame *sf);
void sysSemPost(struct StackFrame *sf);
void sysSemDestroy(struct StackFrame *sf);

void sysSharedVar(struct StackFrame *sf);

void sysSVarCreate(struct StackFrame *sf);
void sysSVarDestroy(struct StackFrame *sf);
void sysSVarRead(struct StackFrame *sf);
void sysSVarWrite(struct StackFrame *sf);

void *memset(void *dest, uint8_t ch, size_t cnt)
{
	uint8_t *d = (uint8_t *)dest;
	while (cnt--)
	{
		*d++ = ch;
	}
	return dest;
}

void *memcpy(void *dest, const void *src, size_t cnt)
{
	uint8_t *d = (uint8_t *)dest;
	const uint8_t *s = (const uint8_t *)src;
	while (cnt--)
	{
		*d++ = *s++;
	}
	return dest;
}

uint32_t schedule()
{
	if (current != 0 &&
		pcb[current].state == STATE_RUNNING &&
		pcb[current].timeCount != MAX_TIME_COUNT)
		{
			return current;
		}
		
		int i;
		if (pcb[current].state == STATE_RUNNING)
		{
			pcb[current].state = STATE_RUNNABLE;
			pcb[current].timeCount = 0;
		}

		i = (current + 1) % MAX_PCB_NUM;
		while (i != current)
		{
			if (i != 0 && pcb[i].state == STATE_RUNNABLE)
			break;
			i = (i + 1) % MAX_PCB_NUM;
		}
		if (pcb[i].state != STATE_RUNNABLE)
		{
			i = 0;
		}
		
		return i;
}
	
void contextSwitch(int prev, int newpid)
{
	ProcessTable *cur_pt = pcb + newpid;
	cur_pt->state = STATE_RUNNING;
	current = newpid;
	uint32_t tmp = pcb[current].stackTop;
	pcb[current].stackTop = pcb[current].prevStackTop;
	// tss.esp0 = pcb[current].stackTop;
tss.esp0 = (uint32_t)&(pcb[current].stackTop);
asm volatile("movl %0, %%esp" ::"m"(tmp));
asm volatile("popl %%gs\n\t"
	"popl %%fs\n\t"
	"popl %%es\n\t"
	"popl %%ds\n\t"
	"popal\n\t"
	"addl $4, %%esp\n\t"
	"addl $4, %%esp\n\t"
	"iret" ::
	: "memory", "cc");
}

// new added
void sleep(ListHead *list)
{
	pcb[current].state = STATE_BLOCKED;
	pcb[current].timeCount = 0;
	pcb[current].sleepTime = 0;

	pcb[current].blocked.next = list->next;
	pcb[current].blocked.prev = list;
	list->next = &(pcb[current].blocked);
	(pcb[current].blocked.next)->prev = &(pcb[current].blocked);

	// 切换进程
	uint32_t new_pid = schedule();
	if (current != new_pid)
	{
		contextSwitch(current, new_pid);
	}
}

void wakeup(ListHead *list)
{
	if(list->next == NULL)
	{
		putStr("wakeup list is empty\n");

		return;
	}

	ProcessTable *pt  = (ProcessTable *)((uint32_t)(list->prev) - (uint32_t)&(((ProcessTable *)0)->blocked));
	list->prev = (list->prev)->prev;
	(list->prev)->next = list;

	pt->blocked.next = NULL;
	pt->blocked.prev = NULL;

	pt->state = STATE_RUNNABLE;
	pt->timeCount = 0;
	pt->sleepTime = 0;

	putStr("wake up pcb[");
	putNum(pt->pid);
	putStr("]\n");
}
	
/*
kernel is loaded to location 0x100000, i.e., 1MB
size of kernel is not greater than 200*512 bytes, i.e., 100KB
user program is loaded to location 0x200000, i.e., 2MB
size of user program is not greater than 200*512 bytes, i.e., 100KB
*/
uint32_t loadUMain(uint32_t first_sector, uint32_t sector_count, uint32_t pid)
{
	int i = 0;
	// int phoff = 0x34; // program header offset
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

void irqHandle(struct StackFrame *sf)
{ // pointer sf = esp
	/* Reassign segment register */
	asm volatile("movw %%ax, %%ds" ::"a"(KSEL(SEG_KDATA)));
	/* Save esp to stackTop */
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
	case 0x21:
		keyboardHandle(sf);
		break;
	case 0x80:
		syscallHandle(sf);
		break;
	default:
		assert(0);
	}
	/* Recover stackTop */
	pcb[current].stackTop = tmpStackTop;
}

void GProtectFaultHandle(struct StackFrame *sf)
{
	assert(0);
	return;
}

void timerHandle(struct StackFrame *sf)
{
	int i;
	i = (current + 1) % MAX_PCB_NUM;
	while (i != current)
	{
		if (pcb[i].state == STATE_BLOCKED)
		{
			pcb[i].sleepTime--;
			if (pcb[i].sleepTime == 0)
			{
				putStr("timerHandle, pcb[");
				putNum(i);
				putStr("] wake up\n");

				pcb[i].state = STATE_RUNNABLE;
			}
		}
		i = (i + 1) % MAX_PCB_NUM;
	}

	if (pcb[current].state == STATE_RUNNING &&
		pcb[current].timeCount != MAX_TIME_COUNT)
	{
		pcb[current].timeCount++;
	}

	// trigger a schedule
	uint32_t new_pid = schedule();

	if (current != new_pid)
	{
		contextSwitch(current, new_pid);
	}
	return;
}

// TODO 4.1.2: keyboard
/*
	1. 将读取到的keyCode放入到keyBuffer中
	2. 唤醒阻塞在dev[STD_IN]上的一个进程
*/
void keyboardHandle(struct StackFrame *sf)
{
	uint32_t keyCode = getKeyCode();
	if (keyCode == 0) // illegal keyCode
		return;

	keyBuffer[bufferTail] = keyCode;
	bufferTail = (bufferTail + 1) % MAX_KEYBUFFER_SIZE;

	// 打印输入的字符到屏幕上
	char ch = getChar(keyCode);
	if (ch != 0)
	{
		if (ch == '\n')
		{
			displayRow++;
			displayCol = 0;
			if (displayRow == MAX_ROW)
			{
				displayRow = MAX_ROW - 1;
				displayCol = 0;
				scrollScreen();
			}
			
			dev[STD_IN].value++;

			// 唤醒阻塞在STD_IN上的进程
			if (dev[STD_IN].value <= 0)
			{ // with process blocked
				wakeup(&dev[STD_IN].pcb);
			}
		}
		else
		{
			uint16_t data = ch | (0x0c << 8);
			int pos = (MAX_COL * displayRow + displayCol) * 2;
			asm volatile("movw %0, (%1)" ::"r"(data), "r"(pos + 0xb8000));
			displayCol++;
			if (displayCol == MAX_COL)
			{
				displayRow++;
				displayCol = 0;
				if (displayRow == MAX_ROW)
				{
					displayRow = MAX_ROW - 1;
					displayCol = 0;
					scrollScreen();
				}
			}
		}
		updateCursor(displayRow, displayCol);
	}

	return;
}

void syscallHandle(struct StackFrame *sf)
{
	switch (sf->eax)
	{ // syscall number
	case SYS_WRITE:
		sysWrite(sf);
		break; // for SYS_WRITE
	case SYS_READ:
		sysRead(sf);
		break; // for SYS_READ
	case SYS_FORK:
		sysFork(sf);
		break; // for SYS_FORK
	case SYS_EXEC:
		sysExec(sf);
		break; // for SYS_EXEC
	case SYS_SLEEP:
		sysSleep(sf);
		break; // for SYS_SLEEP
	case SYS_EXIT:
		sysExit(sf);
		break; // for SYS_EXIT
	case SYS_GETPID:
		sysGetPid(sf);
		break; // for SYS_GETPID
	case SYS_SEM:
		sysSem(sf);
		break; // for SYS_SEM
	case SYS_SHAREDVAR:
		sysSharedVar(sf);
		break; // for SYS_SHAREDVAR
	default:
		break;
	}
}

void sysWrite(struct StackFrame *sf)
{
	switch (sf->ecx)
	{ // file descriptor
	case STD_OUT:
		if (dev[STD_OUT].state == 1)
			sysWriteStdOut(sf);
		break; // for STD_OUT
	default:
		break;
	}
}

void sysWriteStdOut(struct StackFrame *sf)
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
			if (displayRow == MAX_ROW)
			{
				displayRow = MAX_ROW - 1;
				displayCol = 0;
				scrollScreen();
			}
		}
		else
		{
			data = character | (0x0c << 8);
			pos = (MAX_COL * displayRow + displayCol) * 2;
			asm volatile("movw %0, (%1)" ::"r"(data), "r"(pos + 0xb8000));
			displayCol++;
			if (displayCol == MAX_COL)
			{
				displayRow++;
				displayCol = 0;
				if (displayRow == MAX_ROW)
				{
					displayRow = MAX_ROW - 1;
					displayCol = 0;
					scrollScreen();
				}
			}
		}
	}

	updateCursor(displayRow, displayCol);
	return;
}

void sysRead(struct StackFrame *sf)
{
	switch (sf->ecx)
	{
	case STD_IN:
		if (dev[STD_IN].state == 1)
		{
			putStr("sysRead STD_IN\n");
			sysReadStdIn(sf);
		}
		break; // for STD_IN
	default:
		break;
	}
}

// TODO 4.1.1: stdin
/*
	1. 如果dev[STD_IN].value == 0，将当前进程阻塞在dev[STD_IN]上
	2. 进程被唤醒，读keyBuffer中的所有数据
*/
void sysReadStdIn(struct StackFrame *sf)
{
	putStr("sysReadStdIn, dev[STD_IN].value = ");
	putNum(dev[STD_IN].value);
	putStr("\n");

	if (bufferHead == bufferTail)
	{
		dev[STD_IN].value--;

		if (dev[STD_IN].value < 0)
		{
			sleep(&dev[STD_IN].pcb);

			return; // 立即返回，不执行后面的代码
		}

		// 已经有进程阻塞，或者其他异常情况
		sf->eax = 0; // 未读取到数据
		return;
	}
	else
	{
		// 进程被唤醒
		putStr("sysReadStdIn, pcb wake up\n");
		putStr("bufferHead = ");
		putNum(bufferHead);
		putStr(", bufferTail = ");
		putNum(bufferTail);
		putStr("\n");

		char *str = (char *)sf->edx;
		int size = sf->ebx;
		int sel = sf->ds;
		int count = 0; // 已读取的字符数

		asm volatile("movw %0, %%es" ::"m"(sel));

		// 循环读取键盘缓冲区的数据
		while (count < size && bufferHead != bufferTail)
		{
			uint32_t code = keyBuffer[bufferHead];
			bufferHead = (bufferHead + 1) % MAX_KEYBUFFER_SIZE;

			char ch = getChar(code);
			if (ch != 0)
			{
				// 有效字符写入到用户提供的缓冲区
				asm volatile("movb %0, %%es:(%1)" ::"r"(ch), "r"(str + count));
				count++;
			}

		}

		// 返回实际读取的字符数
		sf->eax = count;
	}
}

void sysFork(struct StackFrame *sf)
{
	ProcessTable *cur_pt = pcb + current;
	ProcessTable *fork_pt = NULL;
	int i;
	// find new process id
	for (i = 1; i < MAX_PCB_NUM; i++)
	{
		if (pcb[i].state == STATE_DEAD)
		{
			fork_pt = pcb + i;
			break;
		}
	}
	if (fork_pt == NULL)
	{
		// fork failed : no available place of a new process
		sf->eax = -1;
		return;
	}

	// pcb[i] available with its segements
	sf->eax = i;

	// fork work:

	// 1.copy the code and data/stack content
	memcpy((void *)((i + 1) * 0x100000), (void *)((current + 1) * 0x100000), 0x100000);

	// 2.copy the kernel stack (sf) for the USER process
	uint32_t origin_bias_ = (uint32_t)&(cur_pt->stackTop) - cur_pt->stackTop;
	assert(origin_bias_ == sizeof(struct StackFrame));
	memcpy(fork_pt, cur_pt, sizeof(ProcessTable));
	// NO need to do special with esp and eip, because :
	//		we have modified the base and offset of gdt
	// 		now for user process, its eip and esp will only between the 0x100000 and 0x200000
	// 		actually value of code and data : (0x100000 * i + 0x100000) ~ (0x100000 * i + 0x200000)
	// 		but unsafe for segment protection

	// 3. copy other state
	fork_pt->sleepTime = cur_pt->sleepTime;
	fork_pt->timeCount = cur_pt->timeCount;

	// 4. the part that should not be the same with the father process
	fork_pt->pid = i;
	fork_pt->regs.ss = USEL(2 * i + 2);
	fork_pt->regs.cs = USEL(2 * i + 1);
	fork_pt->regs.ds = USEL(2 * i + 2);
	fork_pt->regs.es = USEL(2 * i + 2);
	fork_pt->regs.fs = USEL(2 * i + 2);
	fork_pt->regs.gs = USEL(2 * i + 2);
	fork_pt->regs.eax = 0;
	fork_pt->stackTop = (uint32_t)&fork_pt->regs;
	fork_pt->prevStackTop = (uint32_t)&(fork_pt->stackTop);
	fork_pt->state = STATE_RUNNABLE;

	// trigger a schedule
	pcb[current].state = STATE_RUNNABLE;
	i = schedule();
	if (current != i)
	{
		contextSwitch(current, i);
	}
	return;
}

void sysExec(struct StackFrame *sf)
{
	// the pid of process to exec the user program
	int pid = current;
	// 1.load user program, and get Entry
	uint32_t first_sector, sector_count;
	first_sector = sf->ecx;
	sector_count = sf->edx;
	uint32_t uMainEntry = loadUMain(first_sector, sector_count, pcb[current].pid);
	// 2.reset process states
	assert(pcb[pid].pid == pid);
	pcb[pid].stackTop = (uint32_t)&(pcb[pid].stackTop);
	pcb[pid].prevStackTop = (uint32_t)&(pcb[pid].stackTop);
	pcb[pid].timeCount = MAX_TIME_COUNT;
	pcb[pid].sleepTime = 0;
	pcb[pid].regs.esp = 0x200000;
	pcb[pid].regs.eip = uMainEntry;
	pcb[pid].regs.eflags = 0x202;
	pcb[pid].regs.eax = 0;
}

void sysSleep(struct StackFrame *sf)
{
	if (sf->ecx == 0)
	{
		return;
	}

	pcb[current].state = STATE_BLOCKED;
	pcb[current].sleepTime = sf->ecx;
	uint32_t new_pid = schedule();
	if (current != new_pid)
	{
		contextSwitch(current, new_pid);
	}

	return;
}

void sysExit(struct StackFrame *sf)
{
	pcb[current].state = STATE_DEAD;
	// trigger a schedule
	uint32_t new_pid = schedule();
	if (current != new_pid)
	{
		contextSwitch(current, new_pid);
	}
	return;
}

void sysGetPid(struct StackFrame *sf)
{
	sf->eax = pcb[current].pid;
	return;
}

void sysSem(struct StackFrame *sf)
{
	switch (sf->ecx)
	{
	case SEM_INIT:
		sysSemInit(sf);
		break;
	case SEM_WAIT:
		sysSemWait(sf);
		break;
	case SEM_POST:
		sysSemPost(sf);
		break;
	case SEM_DESTROY:
		sysSemDestroy(sf);
		break;
	default:
		break;
	}
}

// TODO 4.2: semaphore
/*
int sem_init(sem_t *sem, uint32_t value)
{
	*sem = syscall(SYS_SEM, SEM_INIT, value, 0, 0, 0);
	if (*sem != -1)
		return 0;
	else
		return -1;
}
*/
void sysSemInit(struct StackFrame *sf)
{
	int i;
	for (i = 0; i < MAX_SEM_NUM; i++)
	{
		if (sem[i].state == 0)
		{
			sem[i].state=1;
			sem[i].value = sf->edx;
			
			sem[i].pcb.next = &(sem[i].pcb);
			sem[i].pcb.prev = &(sem[i].pcb);

			sf->eax = i; // 创建成功，返回信号量ID
			return;
		}
	}

	if (i == MAX_SEM_NUM)
	{
		sf->eax = -1; // 创建失败，返回-1
		return;
	}
}

/*
int sem_wait(sem_t *sem)
{
	return syscall(SYS_SEM, SEM_WAIT, *sem, 0, 0, 0);
}
*/
// P()
void sysSemWait(struct StackFrame *sf)
{
	int i = (int)sf->edx;
	if (i < 0 || i >= MAX_SEM_NUM)
	{
		pcb[current].regs.eax = -1;
		return;
	}

	sf->eax = 0;
	
	sem[i].value--;
	if(sem[i].value<0)
	{
		sleep(&sem[i].pcb);
	}
}

/*
int sem_post(sem_t *sem)
{
	return syscall(SYS_SEM, SEM_POST, *sem, 0, 0, 0);
}
*/
// V()
void sysSemPost(struct StackFrame *sf)
{
	int i = (int)sf->edx;
	// ProcessTable *pt = NULL; // for compiling
	if (i < 0 || i >= MAX_SEM_NUM)
	{
		pcb[current].regs.eax = -1;
		return;
	}

	sf->eax = 0;

	sem[i].value++;
	if (sem[i].value <= 0)
	{
		wakeup(&sem[i].pcb);
	}
	
}

/*
int sem_destroy(sem_t *sem)
{
	return syscall(SYS_SEM, SEM_DESTROY, *sem, 0, 0, 0);
}
*/
void sysSemDestroy(struct StackFrame *sf)
{
	int i = (int)sf->edx;
	if (i < 0 || i >= MAX_SEM_NUM)
	{
		pcb[current].regs.eax = -1;
		return;
	}

	if (sem[i].state == 0)
	{
		putStr("sem_destroy: sem is not in use\n");
		pcb[current].regs.eax = -1;
		return;
	}

	if (sem[i].value < 0)
	{
		putStr("sem_destroy: sem is in use\n");
		pcb[current].regs.eax = -1;
		return;
	}

	sem[i].state = 0;
	sem[i].value = 0;
	sem[i].pcb.next = &(sem[i].pcb);
	sem[i].pcb.prev = &(sem[i].pcb);

	pcb[current].regs.eax = 0;
	return;
}

void sysSharedVar(struct StackFrame *sf)
{
	switch (sf->ecx)
	{
		case SHAREDVAR_CREATE:
		sysSVarCreate(sf);
		break;
		case SHAREDVAR_DESTROY:
		sysSVarDestroy(sf);
		break;
		case SHAREDVAR_READ:
		sysSVarRead(sf);
		break;
		case SHAREDVAR_WRITE:
		sysSVarWrite(sf);
		break;
		default:
		break;
	}
}

// TODO 4.3.1: shared variable in kernel space
/*
int createSharedVariable(sharedvar_t *svar, int value)
{
	*svar = syscall(SYS_SHAREDVAR, SHAREDVAR_CREATE, value, 0, 0, 0);
	if (*svar != -1)
		return 0;
	else
		return -1;
}
*/
// 成功创建时返回对应的共享变量描述符
void sysSVarCreate(struct StackFrame *sf)
{
	int i;
	for (i = 0; i < MAX_SHARED_VAR_NUM; i++)
	{
		if(sharedVar[i].state==0)
		{
			sharedVar[i].state=1;
			sharedVar[i].value=sf->edx;
			sf->eax=i; // 返回共享变量描述符

			putStr("sysSVarCreate, sharedVar[");
			putNum(i);
			putStr("].value = ");
			putNum(sharedVar[i].value);
			putStr("\n");

			return;
		}
	}

	if(i == MAX_SHARED_VAR_NUM)
	{
		sf->eax=-1; // 创建失败，返回-1
		return;
	}
}

/*
int destroySharedVariable(sharedvar_t *svar)
{
	return syscall(SYS_SHAREDVAR, SHAREDVAR_DESTROY, *svar, 0, 0, 0);
}
*/
// 销毁共享变量描述符对应的共享变量
void sysSVarDestroy(struct StackFrame *sf)
{
	int i = (int)sf->edx;
	if (i < 0 || i >= MAX_SHARED_VAR_NUM)
	{
		pcb[current].regs.eax = -1;
		return;
	}

	if (sharedVar[i].state == 0)
	{
		putStr("sysSVarDestroy: shared variable is not in use\n");
		pcb[current].regs.eax = -1;
		return;
	}

	if (sharedVar[i].value < 0)
	{
		putStr("sysSVarDestroy: shared variable is in use\n");
		pcb[current].regs.eax = -1;
		return;
	}

	sharedVar[i].state = 0;
	sharedVar[i].value = 0;

	pcb[current].regs.eax = 0;
	return;
}

/*
int readSharedVariable(sharedvar_t *svar)
{
	return syscall(SYS_SHAREDVAR, SHAREDVAR_READ, *svar, 0, 0, 0);
}
*/
// 返回共享变量的值
void sysSVarRead(struct StackFrame *sf)
{
	int i = (int)sf->edx;
	if (i < 0 || i >= MAX_SHARED_VAR_NUM)
	{
		pcb[current].regs.eax = -1;
		return;
	}

	putStr("sysSVarRead, sharedVar[");
	putNum(i);
	putStr("].value = ");
	putNum(sharedVar[i].value);
	putStr("\n");

	sf->eax = sharedVar[i].value;
	return;
}

/*
int writeSharedVariable(sharedvar_t *svar, int value)
{
	return syscall(SYS_SHAREDVAR, SHAREDVAR_WRITE, *svar, value, 0, 0);
}
*/
// 修改共享变量的值
void sysSVarWrite(struct StackFrame *sf)
{
	int i = (int)sf->edx;
	if (i < 0 || i >= MAX_SHARED_VAR_NUM)
	{
		pcb[current].regs.eax = -1;
		return;
	}

	sharedVar[i].value = sf->ebx;
	sf->eax = 0; // 成功

	putStr("sysVarWrite, sharedVar[");
	putNum(i);
	putStr("].value = ");
	putNum(sharedVar[i].value);
	putStr("\n");
	
	return;
}

// TODO optional: XCHG