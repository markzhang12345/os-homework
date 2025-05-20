#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int calculate_fx(int x) {  // f(x)
    if (x == 1)
        return 1;
    return calculate_fx(x - 1) * x;
}

int calculate_fy(int y) {  // f(y)
    if (y == 1 || y == 2)
        return 1;
    return calculate_fy(y - 1) + calculate_fy(y - 2);
}

int main(int argc, char* argv[]) {
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    int pipe_fx[2];  // f(x)进程向主进程传值
    int pipe_fy[2];  // f(y)进程向主进程传值

    // 创建管道
    if (pipe(pipe_fx) < 0 || pipe(pipe_fy) < 0) {
        perror("pipe error");
        exit(1);
    }

    pid_t pid_fx = fork();

    if (pid_fx < 0) {
        perror("fork error");
        exit(1);
    } else if (pid_fx == 0) {
        // 子进程计算f(x)
        close(pipe_fx[0]);  // 0读1写
        close(pipe_fy[0]);
        close(pipe_fy[1]);

        int result_x = calculate_fx(x);
        printf("进程ID %d 计算 f(%d) = %d\n", getpid(), x, result_x);

        write(pipe_fx[1], &result_x, sizeof(result_x));
        close(pipe_fx[1]);

        exit(0);
    }

    pid_t pid_fy = fork();

    if (pid_fy < 0) {
        perror("fork error");
        exit(1);
    } else if (pid_fy == 0) {
        // 子进程计算f(y)
        close(pipe_fy[0]);
        close(pipe_fx[0]);
        close(pipe_fx[1]);

        int result_y = calculate_fy(y);
        printf("进程ID %d 计算 f(%d) = %d\n", getpid(), y, result_y);

        // 将结果写入管道
        write(pipe_fy[1], &result_y, sizeof(result_y));
        close(pipe_fy[1]);

        exit(0);
    }

    close(pipe_fx[1]);
    close(pipe_fy[1]);

    int result_x, result_y;

    read(pipe_fx[0], &result_x, sizeof(result_x));

    read(pipe_fy[0], &result_y, sizeof(result_y));

    close(pipe_fx[0]);
    close(pipe_fy[0]);

    int result_xy = result_x + result_y;

    // 等待子进程结束，前面没资源的自己会阻塞
    waitpid(pid_fx, NULL, 0);
    waitpid(pid_fy, NULL, 0);

    printf("进程ID %d 计算 f(%d,%d) = f(%d) + f(%d) = %d + %d = %d\n", getpid(), x, y, x, y, result_x, result_y, result_xy);

    return 0;
}