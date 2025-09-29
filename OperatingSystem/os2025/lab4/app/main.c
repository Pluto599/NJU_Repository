#include "lib.h"
#include "types.h"

#define test4

#define N 4
int item[N] = {};
sem_t full;
sem_t empty;
sem_t mutex;
int in = 0;
int out = 0;
int id;

void produce();
void consume();

int uEntry(void)
{
	#ifndef test4
	int ret = 0;
	#endif
	
	#ifdef test1
	// For lab4.1
	// Test 'scanf'
	int dec = 0;
	int hex = 0;
	char str[6];
	char cha = 0;

	while (1)
	{
		printf("Input:\" Test %%c Test %%6s %%d %%x\"\n");
		ret = scanf(" Test %c Test %6s %d %x", &cha, str, &dec, &hex);
		printf("Ret: %d; %c, %s, %d, %x.\n", ret, cha, str, dec, hex);
		if (ret == 4)
			break;
	}
	#endif

	#ifdef test2
	// For lab4.2
	// Test 'Semaphore'
	int i = 4;

	sem_t sem;
	printf("Parent Process: Semaphore Initializing.\n");
	ret = sem_init(&sem, 2);
	if (ret == -1)
	{
		printf("Parent Process: Semaphore Initializing Failed.\n");
		exit();
	}

	ret = fork();
	if (ret == 0) //child process
	{
		while (i != 0)
		{
			i--;
			printf("Child Process: Semaphore Waiting.\n");
			sem_wait(&sem);
			// printf(".. Wait Over: ret = %d\n", ret);
			printf("Child Process: In Critical Area.\n");
		}
		printf("Child Process: Semaphore Destroying.\n");
		ret = sem_destroy(&sem);
		// printf(".. Destroy ret = %d\n", ret);
		exit();
	}
	else if (ret != -1) // parent process
	{
		while (i != 0)
		{
			i--;
			printf("Parent Process: Sleeping.\n");
			sleep(128);
			printf("Parent Process: Semaphore Posting.\n");
			ret = sem_post(&sem);
			// printf(".. Post ret = %d\n", ret);
		}
		printf("Parent Process: Semaphore Destroying.\n");
		sem_destroy(&sem);
		exit();
	}
	#endif

	#ifdef test3
	// For lab4.3
	// Test 'Shared Variable'
	printf("==============TEST SHAREDVARIABLE=============\n");
	int number = 114514;

	sharedvar_t svar;
	ret = createSharedVariable(&svar, number);
	printf("Parent Process: create Shared Variable: %d  with value: %d\n", svar, number);
	if (ret == -1)
	exit();

	ret = fork();
	if (ret == 0) // child process
	{
		number = readSharedVariable(&svar);
		printf("Child Process: readShared Variable: %d get value: %d\n", svar, number);
		sleep(128);

		number = readSharedVariable(&svar);
		printf("Child Process: readShared Variable: %d get value: %d\n", svar, number);
		number = 2333;
		writeSharedVariable(&svar, number);
		printf("Child Process: writeShared Variable: %d with value: %d\n", svar, number);

		exit();
	}
	else if (ret != -1) // parent process
	{
		number = -5678;
		sleep(64);

		writeSharedVariable(&svar, number);
		printf("Parent Process: writeShared Variable: %d with value: %d\n", svar, number);
		sleep(128);

		number = readSharedVariable(&svar);
		printf("Parent Process: readShared Variable: %d get value: %d\n", svar, number);
		sleep(128);

		destroySharedVariable(&svar);
		printf("Parent Process: destroyShared Variable: %d\n", svar);
		exit();
	}
	#endif

	#ifdef test4
	// TODO 4.4
	/*
	生产者-消费者问题：
	1. 4个生产者，1个消费者同时运行
	2. 生产者生产，printf("Producer %d: produce\n", id);
	3. 消费者消费，printf("Consumer : consume\n");
	4. 任意P、V及生产、消费动作之间添加sleep(128);
	*/
	sem_init(&full, 0);
	sem_init(&empty, N);
	sem_init(&mutex, 1);

	// 创建4个生产者进程
	int child_pid;
	for (int i = 0; i < 4; i++)
	{
		child_pid = fork();
		if (child_pid == 0)
		{
			break;
		}
	}

	id = getpid(); 
	// printf("pid = %d\n", id);
	switch (id)
	{
		case 1:
		// consumer: 1
			for(int i = 0; i < 8; i++)
			{
				sleep(128);
				consume();
			}
			sem_destroy(&full);
			sem_destroy(&empty);
			sem_destroy(&mutex);
			exit();
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		// producer: 2, 3, 4, 5
			for (int i = 0; i < 2; i++)
			{
				sleep(128);
				produce();
			}
			exit();
			break;
		default:
			printf("Error: Unknown process id %d\n", id);
			exit();
		}

#endif
	
	return 0;
}

void produce()
{
	sem_wait(&empty);
	sem_wait(&mutex);

	item[in] = 1;
	in = (in + 1) % N;
	printf("Producer %d: produce\n", id);
	sleep(128);

	sem_post(&mutex);
	sem_post(&full);
}

void consume()
{
	sem_wait(&full);
	sem_wait(&mutex);

	item[out] = 0;
	out = (out + 1) % N;
	printf("Consumer: consume\n");
	sleep(128);

	sem_post(&mutex);
	sem_post(&empty);
}
