#include "smoker.h"

// 全局变量定义
int tobacco_sem, paper_sem, glue_sem, supplier_sem, mutex_sem;
Table* table;
Status* status;

int create_semaphore(key_t key, int init_val) {
    int sem_id;
    union semun sem_arg;

    sem_id = semget(key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget failed");
        exit(1);
    }

    sem_arg.val = init_val;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {
        perror("semctl SETVAL failed");
        exit(1);
    }

    return sem_id;
}

// 创建共享内存
void* create_shared_memory(key_t key, size_t size) {
    int shm_id;
    void* shm_ptr;

    shm_id = shmget(key, size, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("shmat failed");
        exit(1);
    }

    return shm_ptr;
}

// P操作
void sem_wait(int sem_id) {
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("sem_wait failed");
        exit(1);
    }
}

// V操作
void sem_signal(int sem_id) {
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 1;
    sem_op.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sem_op, 1) == -1) {
        perror("sem_signal failed");
        exit(1);
    }
}

void cleanup_resources() {
    semctl(tobacco_sem, 0, IPC_RMID);
    semctl(paper_sem, 0, IPC_RMID);
    semctl(glue_sem, 0, IPC_RMID);
    semctl(supplier_sem, 0, IPC_RMID);
    semctl(mutex_sem, 0, IPC_RMID);

    shmdt(table);
    shmdt(status);

    int table_id = shmget(TABLE_KEY, 0, 0);
    int status_id = shmget(STATUS_KEY, 0, 0);
    shmctl(table_id, IPC_RMID, NULL);
    shmctl(status_id, IPC_RMID, NULL);
}