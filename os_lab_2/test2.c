#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t barber_ready;
sem_t customer_ready;
sem_t mutex;

int waiting_customers = 0;
int chairs = 3;

void* barber_function(void* arg) {
    while (1) {
        printf("理发师: 睡觉中...\n");
        sem_wait(&customer_ready);

        sem_wait(&mutex);
        waiting_customers--;
        sem_post(&mutex);

        printf("理发师: 为顾客理发中...\n");
        sleep(2);

        printf("理发师: 理发完成\n");
        sem_post(&barber_ready);
    }
    return NULL;
}

void* customer_function(void* arg) {
    int id = *((int*)arg);
    free(arg);

    sleep(rand() % 5);

    printf("顾客 %d: 到达理发店\n", id);

    sem_wait(&mutex);
    if (waiting_customers < chairs) {
        waiting_customers++;
        printf("顾客 %d: 等待理发，当前等待人数: %d\n", id, waiting_customers);
        sem_post(&mutex);

        sem_post(&customer_ready);

        sem_wait(&barber_ready);
        printf("顾客 %d: 理发完成，离开理发店\n", id);
    } else {
        printf("顾客 %d: 没有空位，离开理发店\n", id);
        sem_post(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t barber_thread;
    pthread_t customer_threads[10];

    sem_init(&barber_ready, 0, 0);
    sem_init(&customer_ready, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&barber_thread, NULL, barber_function, NULL);
    for (int i = 0; i < 10; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_function, id);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    printf("所有顾客已处理完毕，程序结束\n");
    return 0;
}