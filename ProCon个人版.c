#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 5

pthread_mutex_t mutex;
sem_t full;
sem_t empty;
char bufferpool[MAX][100];
int in = 0;
int out = 0;
//int flag = 0;//for transition

pthread_t producers[3];
pthread_t consumers[4];

void* produce(void* args);
void* consume(void* args);

FILE* file;
int t[4]={1,2,3,4};

int main(){
  //int thread_id[4],id=0;

  pthread_mutex_init(&mutex,NULL);
  int ret1=sem_init(&full,0,0);
  int ret2=sem_init(&empty,0,MAX);
  if(ret1!=ret2)
	printf("sem init failed\n");

  if((file=fopen("resource.txt","r"))==NULL){
	printf("File open failed...\n");
	return 1;
  }
  //initialize the thread_id array
  /* THE COMPILER GCC ON LINUX DOSEN’T SUPPORT INIT IN FOOR LOOP
    for(int i = 0; i<=3; i++){
      thread_id[i] = i+1;
  }*/

  //create 3 threads as producers
  int i;//THE COMPILER GCC ON LINUX DOSEN’T SUPPORT INIT IN FOOR LOOP
  for(i = 0; i <= 2; i++){
    //int* t = (thread_id + i);//array pointer
    int ret=pthread_create(&producers[i],NULL,produce,&t[i]);
    if(ret != 0)
	printf("Failed to create producer %d \n",t[i]);
  }

  //create 4 threads as consumers
  for(i = 0; i <= 3; i++){
    //int* t = (thread_id + i);//array pointer
    int ret=pthread_create(&consumers[i],NULL,consume,&t[i]);
    if(ret != 0)
	printf("Failed to create consumer %d \n",t[i]);
  }

  //end producers
  for(i = 0;i < 3;i++ ){
    pthread_join(producers[i],NULL);
  }

  //end consumers
  for(i = 0;i < 4;i++ ){
	pthread_join(consumers[i],NULL);
  }

  fclose(file);
  return 0;
}

void* produce(void* args){
  int id = *(int*) args;//forced conversion
  char product[50]={0};

  while(1){
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    //produce an item
    if(fgets(product,50,file)){
      strcpy(bufferpool[in],product);//put the item in the array or file
      in = (in + 1) % MAX;
      printf("A product has been product by producer%d\n",id);
    }
    else if(ferror(file)){//reading file error
      //fclose(file);
      //clearerr(file);
      printf("Error uccured when producer %d reading the file!\n",id);
      pthread_mutex_unlock(&mutex);
      pthread_exit(0);
    }
    else if(feof(file)){ //reading file end
      //fclose(file);
      //clearerr(file);
      printf("The producers finished producing.\n");
      pthread_mutex_unlock(&mutex);
      pthread_exit(0);
    }

    sleep(2);//wait
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
  return ((void*)NULL);
}

void* consume(void* args){
  int id = *(int*) args;//forced conversion
  char product[50]={0};

  //consume an item
  while(1){
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    //consume the item
    if(strcpy(product,bufferpool[out])){//take out the item
      printf(">***>Consumer %d has consumed an item, the item is::: %s\n",id,product);
      out = (out + 1) % MAX;//update the pointer
    }
    else{
      printf("The consumer %d is out of work.\n",id);
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
  return ((void*)NULL);
}
