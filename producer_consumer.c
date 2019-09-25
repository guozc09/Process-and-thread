#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h> /* 提供了信号量的相关操作 */
#include <sys/mman.h>  /* 提供了共享内存的相关操作 */
#include <fcntl.h>
#include <sys/stat.h>
#include "error_plp.h" /*这是我自定义的一个出错处理函数 */

#define BUFFER_SIZE 10                                                   /* 公用环形级沖区的大小 */
#define RWRWRW S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH /*设定创建的文件的访问权限为:用户、组用户和其他用户都可读可写 */

int head, tail;              /* 即out和in指针，分别用来指示当前第一个可读和写的区的下标 */
int value_read, value_write; /* 分别用来保存读到的值和要写的值 */
int *shared_buffer;          /* 公用环形缓冲区指计，为了公用,这个指计将指向一个共享内存的数组 */
sem_t *full, *empty;         /* 分别指向full 和 empty 这两个信号量的指计，同样地，为了公用，这两个指针指向的信号量在共亭内存中实现 */
void producer(void);         /* 生产者所执行的代码 */
void consumer(void);         /* 消费者所执行的代码 */

int main(void)
{
    int fd;
    pid_t pid;
    void *ptr;
    int length;
    /* 初始化 */
    head = 0;
    tail = 0;
    value_read = 0;
    value_write = 0;
    /* 计算共享内存的长度 */
    length = 2 * sizeof(sem_t) + BUFFER_SIZE * sizeof(int);
    /* shm_open是一个POSIX函数，用来打开或创建一个与“/shm”关联的共享内存区 */
    if ((fd = shm_open("/shm", O_RDWR | O_CREAT, RWRWRW)) == -1)
    {
        err_exit("shm_open error"); /* 出错提示，可用简单的 printf或fprintf代替 */
    }
    if (ftruncate(fd, length) == -1) /* 截短共享内存的长度到我们所需要的长度 */
    {
        err_exit("ftruncate error");
    }
    if ((ptr = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) /* 将共享内存映射到进程地址空间 */
    {
        err_exit("mmap error");
    }
    /* 共享内存的交量布局依次为: full, empty, shared_buffer */
    full = (sem_t *)ptr;
    empty = ((sem_t *)ptr) + 1;
    shared_buffer = (int *)(((sem_t *)ptr) + 2);
    sem_init(full, 1, 0);            /* 初始化full为0, 且进程间共享 */
    sem_init(empty, 1, BUFFER_SIZE); /* 初始化empty为 BUFFER_SIZE，且进程间共享 */
    switch (pid = fork())            /* 生成子进程, 因为子进程继承了父进程的地址间所以共享内存在父子进程间都可见(这部分地址空间都映射到一个内核区域) */
    {
    case -1: /* 生成子进程失败 */
        err_exit("fork error");
        break;
    case 0:         /* 子进程 */
        producer(); /* 子进程是生产者 */
        exit(0);
    default:
        consumer(); /* 父进程是消费者 */
        break;
    }
    shm_unlink("/shm"); /* 删除共享内存区, 程序中基本上保证了子进程先退出, 因此父进程中无wait操作且这步操作放在父进程这里 */
}
/* 生产者5次后退出 */
void producer(void)
{
    while (value_write < 4) /* 退出条件判定 */
    {
        sem_wait(empty); /* 是否有空缓冲区，有则占有，无则被挂起，是原子操作 */
        sleep(3);        /* 休眠2s,测试时休眠时间可修改或用随机数代替 */
        value_write++;
        shared_buffer[tail] = value_write;
        printf("write %5d to position %5d\n", value_write, tail);
        tail = (tail + 1) % BUFFER_SIZE; /* 移动写指针 */
        sem_post(full);                  /* 写完一个缓冲区，释放信号量 full (值加1) */
    }
}
/* 消费者读 5 次后退出 */
void consumer(void)
{
    while (value_read < 4)
    {
        sem_wait(full); /* 获取信号量 */
        sleep(1);       /* 休眠 1s，测试时休眠时间可修改或者随机数代替 */
        value_read = shared_buffer[head];
        printf("read %5d from position %5d\n", value_read, head);
        head = (head + 1) % BUFFER_SIZE; /* 移动读指针 */
        sem_post(empty);                 /* 读完一个缓冲区，释放信号量 empty（值加1） */
    }
}
