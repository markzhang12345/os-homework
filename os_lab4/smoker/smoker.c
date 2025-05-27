#include "smoker.h"

const char* smoker_names[] = {"有烟草的抽烟者", "有纸的抽烟者", "有胶水的抽烟者"};
const char* material_names[] = {"烟草", "纸", "胶水"};

void smoker_process(int smoker_type) {
    int my_sem;
    const char* my_name = smoker_names[smoker_type];
    const char* my_material = material_names[smoker_type];

    switch (smoker_type) {
        case TOBACCO:
            my_sem = tobacco_sem;
            break;
        case PAPER:
            my_sem = paper_sem;
            break;
        case GLUE:
            my_sem = glue_sem;
            break;
        default:
            fprintf(stderr, "未知的抽烟者类型: %d\n", smoker_type);
            exit(1);
    }

    printf("%s 启动 (拥有: %s)\n", my_name, my_material);

    while (status->active) {
        printf("%s: 等待材料...\n", my_name);
        sem_wait(my_sem);

        if (!status->active)
            break;

        sem_wait(mutex_sem);

        // 检查材料
        printf("%s: 发现有材料，开始卷烟...\n", my_name);

        // 取走材料
        memset(table->materials, 0, sizeof(table->materials));

        // 更新总抽烟次数
        status->total_smokes++;
        int current_smokes = status->total_smokes;

        sem_signal(mutex_sem);

        printf("%s: 正在卷烟... (第 %d 支烟)\n", my_name, current_smokes);
        sleep(2);

        printf("%s: 正在抽烟... \n", my_name);
        sleep(2);

        printf("%s: 抽完了！通知供应者继续工作\n", my_name);

        if (current_smokes >= 10) {
            status->active = 0;

            sem_signal(tobacco_sem);
            sem_signal(paper_sem);
            sem_signal(glue_sem);
            sem_signal(supplier_sem);
            break;
        }

        sem_signal(supplier_sem);

        sleep(1);  // 短暂休息
    }

    printf("%s 结束工作\n", my_name);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        exit(1);
    }

    int smoker_type = atoi(argv[1]);
    if (smoker_type < 0 || smoker_type > 2) {
        exit(1);
    }

    // 获取信号量
    tobacco_sem = create_semaphore(TOBACCO_SEM, 0);
    paper_sem = create_semaphore(PAPER_SEM, 0);
    glue_sem = create_semaphore(GLUE_SEM, 0);
    supplier_sem = create_semaphore(SUPPLIER_SEM, 1);
    mutex_sem = create_semaphore(MUTEX_SEM, 1);

    // 获取共享内存
    table = (Table*)create_shared_memory(TABLE_KEY, sizeof(Table));
    status = (Status*)create_shared_memory(STATUS_KEY, sizeof(Status));

    smoker_process(smoker_type);

    return 0;
}