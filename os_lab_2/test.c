#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

int sum = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void* producer(void* arg) {
    for (int i = 1; i <= 10; i++) {
        sem_wait(&empty);

        pthread_mutex_lock(&mutex);

        sum++;
        printf("生产者: 生产数据, sum = %d\n", sum);

        pthread_mutex_unlock(&mutex);

        sem_post(&full);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 1; i <= 10; i++) {
        sem_wait(&full);

        pthread_mutex_lock(&mutex);

        int old_sum = sum;
        sum = 0;
        printf("消费者: 消费数据, sum = %d\n", sum);

        pthread_mutex_unlock(&mutex);

        sem_post(&empty);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);

    printf("开始...\n");

    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("结束\n");
    return 0;
}