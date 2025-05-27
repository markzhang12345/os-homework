#ifndef SMOKER_H
#define SMOKER_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TABLE_KEY 1001   // 桌子上的材料
#define STATUS_KEY 1002  // 系统状态

#define TOBACCO_SEM 2001   // 有烟草的抽烟者
#define PAPER_SEM 2002     // 有纸的抽烟者
#define GLUE_SEM 2003      // 有胶水的抽烟者
#define SUPPLIER_SEM 2004  // 供应者信号量
#define MUTEX_SEM 2005     // 互斥信号量

#define TOBACCO 0
#define PAPER 1
#define GLUE 2

typedef struct {
    int materials[3];  // 桌子上的材料 [烟草, 纸, 胶水]
    int round;         // 当前轮次
} Table;

typedef struct {
    int active;        // 系统是否活跃
    int total_smokes;  // 总抽烟次数
} Status;

// 信号量
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

extern int tobacco_sem, paper_sem, glue_sem, supplier_sem, mutex_sem;
extern Table* table;
extern Status* status;

int create_semaphore(key_t key, int init_val);
void* create_shared_memory(key_t key, size_t size);
void sem_wait(int sem_id);
void sem_signal(int sem_id);
void cleanup_resources();

#endif