/* 
 *  fork_test.c 
 *  version 1 
 *  Created on: 2019-9-16 
 *      Author: Zhc Guo
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <iostream>
using namespace std;

int main()
{
#if 1
    pid_t fpid; //fpid表示fork函数返回的值
    int count = 0;
    int ret = 0;
    fpid = fork();
    if (fpid < 0)
        printf("error in fork!\n");
    else if (fpid == 0)
    {
        printf("i am the child process, my process id is %d\n", getpid());
        printf("我是爹的儿子\n"); //对某些人来说中文看着更直白。
        count++;
        execlp("ls", "ls", "-a", NULL);
        exit(errno);
    }
    else
    {
        wait(&ret);
        cout << "子进程返回结果 ret =" << ret << endl;
        printf("i am the parent process, my process id is %d\n", getpid());
        printf("我是孩子他爹\n");
        count++;
    }
    printf("统计结果是: %d\n", count);
#else
    fork();
    fork() && fork() || fork();
    fork();
    printf("+\n");
#endif
    return 0;
}