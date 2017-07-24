#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

int sock;

struct motors {
  unsigned char pin;
  unsigned char value;
};

int clean();
  
static void *server(){
  sock = socket(AF_UNIX,SOCK_STREAM,0);
  if (sock == -1) {
    perror("could not open socket");
    return ;
  }
  struct sockaddr_un sa = {0};
  sa.sun_family = AF_UNIX;
  strcpy(sa.sun_path,"/tmp/flypi-test-socket");
  remove(sa.sun_path);

  if (bind(sock,(const struct sockaddr*) &sa,sizeof(struct sockaddr_un))==-1) {
    perror("socket bind error");
    clean();
    return ;
  }
  if (listen(sock, 128) == -1){
    perror("listen error");
    clean();
    return ;
  }
  while(1){
    
  puts("Waiting for client...");
  int cli=accept(sock,NULL,NULL);
  if (cli<0) {
    perror("failed to accept");
    clean();
    return;
  }
  puts("Accepted new client");

  struct motors mBuf[2]={{6,0},{19,255}};
  int sendCnt=send(cli,&mBuf,sizeof(struct motors)*2,0);
  while(1){
    char* b;
    int msgLen=recv(cli,&b,sizeof(char),0);
    if(msgLen<1){
      perror("disconnected or error");
      break;
    }
    write(1,&b,msgLen);
    sleep(1);
  }

  }
  clean();
  return ;
}

int clean(){
  close(sock);
}

static void* control(){
  while(1){
    puts("kimamani loop");
    sleep(1);
  }
}

int main(){
  pthread_t comThread;
  pthread_t ctrlThread;
  if(pthread_create(&comThread,NULL,server,NULL)<0
   &&pthread_create(&ctrlThread,NULL,control,NULL)<0){
    perror("failed to create thread");
    return 1;
  }
  while(1){

  }
}
