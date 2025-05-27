#include "smoker.h"

const char* material_names[] = {"烟草", "纸", "胶水"};

void supplier_process(int supplier_id) {
    printf("供应者 %d 启动\n", supplier_id);

    while (status->active) {
        sem_wait(supplier_sem);

        if (!status->active)
            break;

        sem_wait(mutex_sem);

        // 清空
        memset(table->materials, 0, sizeof(table->materials));

        // 供应者轮流提供不同的材料组合
        int combination = table->round % 3;

        switch (combination) {
            case 0:  // 提供纸和胶水，缺烟草
                table->materials[PAPER] = 1;
                table->materials[GLUE] = 1;
                printf("供应者 %d: 提供了 %s 和 %s (缺 %s)\n", supplier_id, material_names[PAPER], material_names[GLUE], material_names[TOBACCO]);
                sem_signal(tobacco_sem);  // 唤醒有烟草的抽烟者
                break;

            case 1:  // 提供烟草和胶水，缺纸
                table->materials[TOBACCO] = 1;
                table->materials[GLUE] = 1;
                printf("供应者 %d: 提供了 %s 和 %s (缺 %s)\n", supplier_id, material_names[TOBACCO], material_names[GLUE], material_names[PAPER]);
                sem_signal(paper_sem);  // 唤醒有纸的抽烟者
                break;

            case 2:  // 提供烟草和纸，缺胶水
                table->materials[TOBACCO] = 1;
                table->materials[PAPER] = 1;
                printf("供应者 %d: 提供了 %s 和 %s (缺 %s)\n", supplier_id, material_names[TOBACCO], material_names[PAPER], material_names[GLUE]);
                sem_signal(glue_sem);  // 唤醒有胶水的抽烟者
                break;
        }

        table->round++;

        sem_signal(mutex_sem);

        sleep(1);
    }

    printf("供应者 %d 结束\n", supplier_id);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "用法: %s <supplier_id>\n", argv[0]);
        exit(1);
    }

    int supplier_id = atoi(argv[1]);

    // 获取信号量
    tobacco_sem = create_semaphore(TOBACCO_SEM, 0);
    paper_sem = create_semaphore(PAPER_SEM, 0);
    glue_sem = create_semaphore(GLUE_SEM, 0);
    supplier_sem = create_semaphore(SUPPLIER_SEM, 1);  // 初始允许一个供应者工作
    mutex_sem = create_semaphore(MUTEX_SEM, 1);

    // 获取共享内存
    table = (Table*)create_shared_memory(TABLE_KEY, sizeof(Table));
    status = (Status*)create_shared_memory(STATUS_KEY, sizeof(Status));

    // 初始化状态
    status->active = 1;
    status->total_smokes = 0;
    memset(table->materials, 0, sizeof(table->materials));
    table->round = 0;
    supplier_process(supplier_id);

    return 0;
}