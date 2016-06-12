#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

//student LI ZIYI , ID = 2014220402002 , SISE , UESTC
//philosophers think and eat unless get two chopsticks.
/*KEY: pthread_t:The ID of a thread.
       pthread_muutex_trylock():Occupy if lock empty and return if full
       pthread_mutex_lock():Occupy
       pthread_mutex_unlock():release
*/
#define PHILO_NUM 5
#define THINKING 1
#define HUNGRY 2
#define EATING 3
#define TRUE 1

sem_t chopsticks[PHILO_NUM];
pthread_mutex_t mutex;

void *philosopher(void *arg){
    int id = (int) arg;

    int state = THINKING;
    int right = (id + 1) % PHILO_NUM;
    int left = (id + PHILO_NUM - 1) % PHILO_NUM;
    char ptrState[32];

    while(TRUE){
        switch(state){
        case THINKING:
            usleep(3000);
            state = HUNGRY;

            printf("Philosopher %d is thinking \n",id);
            break;
        case HUNGRY:

            if(sem_wait(&chopsticks[left]) == 0){//status is lock
                if(sem_trywait(&chopsticks[right]) == 0){//status is not lock

                    printf("Philosopher %d is eating.\n",id);
                    state = EATING;
                }else{
                    state = THINKING;

                    sem_post(&chopsticks[left]);//release the asking left chopstick
                    printf("Philosopher %d right chopsticks is busy,right=%d\n",id,right);
                }
            }else{
                printf("Philosopher %d left chopsticks is busy,left=%d\n",id,left);//?
            }
            break;
        case EATING:
            printf("Philosopher %d fetch left and right chopsticks: (%d,%d)\n",id,left,right);
            sem_post(&chopsticks[left]);
            sem_post(&chopsticks[right]);
            printf("Philosopher %d release left and right chopsticks: (%d,%d)\n",id,left,right);
            usleep(500);
            state = THINKING;

            break;
        }
        pthread_mutex_lock(&mutex);
        pthread_mutex_unlock(&mutex);
        usleep(1000);
    }

    pthread_exit((void*)0);//end the present thread
    sleep(5000);//make a interval for debug
    /*int round++;
    if（round == 30) break；*/
}

int main(){
    pthread_t tid[PHILO_NUM];//number of philosophers
    int i;
    pthread_mutex_init(&mutex,NULL);//initial the mutex lock

    for(i = 0 ; i < PHILO_NUM ; i ++){
        sem_init(&chopsticks[i],0,1);
    }
    for(i = 0 ; i < PHILO_NUM ; i ++){
        pthread_create(&tid[i],NULL,philosopher,(void*)i);//create a thread
    }
    for(i = 0 ; i < PHILO_NUM ; i ++){
        pthread_join(tid[i],NULL);//suspend the thread until the other thread ends
    }
    return 0;
}
