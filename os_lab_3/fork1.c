#include <stdio.h>
#include <sys/types.h>
// #include <unistd.h>
#include <stdlib.h>

int value = 5;  // where? 全局变量，静态区

int main() {
    int i;  // where? 主函数中，栈区

    pid_t pid;

    for (i = 0; i < 2; i++) {  // How many new processes and printfs？2个

        pid = fork();

        if (pid == 0) {  // 在子进程中
            value += 15;
            printf("Child: value = %d\n", value);
        } else if (pid > 0) {
            wait(NULL);  // 等待子进程结束
            printf("PARNET: value = %d\n", value);
            exit(0);  // Notice：What will happen with or without this line?
        }
    }
}