#include "smoker.h"

void signal_handler(int sig) {
    printf("\n收到信号 %d，正在清理资源...\n", sig);
    if (status) {
        status->active = 0;
    }
    cleanup_resources();
    exit(0);
}

int main() {
    pid_t pids[5];  // 3个抽烟者 + 2个供应者
    int i;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    tobacco_sem = create_semaphore(TOBACCO_SEM, 0);
    paper_sem = create_semaphore(PAPER_SEM, 0);
    glue_sem = create_semaphore(GLUE_SEM, 0);
    supplier_sem = create_semaphore(SUPPLIER_SEM, 1);
    mutex_sem = create_semaphore(MUTEX_SEM, 1);

    table = (Table*)create_shared_memory(TABLE_KEY, sizeof(Table));
    status = (Status*)create_shared_memory(STATUS_KEY, sizeof(Status));

    status->active = 1;
    status->total_smokes = 0;
    memset(table->materials, 0, sizeof(table->materials));
    table->round = 0;

    for (i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // 抽烟者
            char smoker_type[10];
            sprintf(smoker_type, "%d", i);
            execl("./smoker", "smoker", smoker_type, NULL);
            perror("execl smoker failed");
            exit(1);
        } else if (pids[i] == -1) {
            perror("fork failed");
            exit(1);
        }
    }

    // 创建2个供应者进程
    for (i = 3; i < 5; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            char supplier_id[10];
            sprintf(supplier_id, "%d", i - 2);
            execl("./supplier", "supplier", supplier_id, NULL);
            perror("execl supplier failed");
            exit(1);
        } else if (pids[i] == -1) {
            perror("fork failed");
            exit(1);
        }
    }

    int active_processes = 5;
    while (active_processes > 0) {
        int status_code;
        pid_t finished_pid = wait(&status_code);

        if (finished_pid > 0) {
            active_processes--;
        }

        if (!status->active) {
            for (i = 0; i < 5; i++) {
                if (pids[i] > 0) {
                    kill(pids[i], SIGTERM);
                }
            }
        }
    }

    cleanup_resources();

    return 0;
}