#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>


int main(){
  int pipe_fd[2];
  pid_t pid;
  char buffer[256];
  int status;

  //create a pipe
  if(pipe(pipe_fd)<0){
    printf(stderr, "Pipe error!\n");
    return;
  }

  //create 1st sub process
  if((pid=fork()) < 0){
    printf("Fork error!\n");
    return;
  }
  else if(pid == 0){
    //close(pipe_fd[0]);
    memset(buffer,0,sizeof(buffer));//clear space
    stpcpy(buffer,"Child process 1 is sending a message!\n");
    write(pipe_fd[1],buffer
          ,strlen("Child process 1 is sending a message!\n"));
    //printf("Present process id ",getpid());
  }
  else{
    //create 2nd sub process in parent process
    waitpid(pid,&status,0);
    if((pid=fork()) < 0){
      printf(stderr,"Fork error!\n");
      return;
    }
    else if(pid == 0){
      //close(pipe_fd[0]);
      memset(buffer,0,sizeof(buffer));//clear space
      stpcpy(buffer,"Child process 2 is sending a message!\n");
      write(pipe_fd[1],buffer
            ,strlen("Child process 2 is sending a message!\n"));
      //printf("Present process id ",getpid());
    }
    else{
      waitpid(pid,&status,0);
      read(pipe_fd[0],buffer,128);
      printf("Message got from pipe:\n%s",buffer);
    }
  }
   return 0;
}
