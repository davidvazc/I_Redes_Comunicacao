/*************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 *************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

#define BUF_SIZE  1024
#define SERVER_PORT 9001
#define SERVER_PORT2 9000
#define IP_ADRR "127.0.0.1"

void erro(char *msg);
void* notifica(void* idp);
int done=0;

int main(int argc, char *argv[]) {
    char endServer[100];
    int fd;
    struct sockaddr_in addr;
    struct hostent *hostPtr;
    strcpy(endServer, IP_ADRR);
    
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Nao consegui obter endereço");
    
    bzero((void *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr.sin_port = htons(SERVER_PORT);
    
    if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
        erro("socket");
    
    if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
        erro("Connect");
    
    long nread = 0;
    char buffer[BUF_SIZE],acao[BUF_SIZE];
    pthread_t my_thread;
    struct sockaddr_in addr2;
    int fd2;
    strcpy(endServer, IP_ADRR);
    
    
    nread = read(fd, buffer, BUF_SIZE); //Le mensagem de boas vindas
    buffer[nread] = '\0';
    system("clear");
    printf("%s\n", buffer);
    
    scanf("%s", acao);
    write(fd, acao, BUF_SIZE);      //escreve o id
    
    nread = read(fd, buffer, BUF_SIZE);//le resposta sobre id
    buffer[nread] = '\0';
    
    if(strcmp(buffer,"Cliente existe")==0){
        printf("\n");
    }
    else{
        system("clear");
        printf("%s\n", buffer);
        while(strcmp(buffer,"Cliente existe")!=0){
            scanf("%s", acao);
            write(fd, acao, BUF_SIZE);
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            if(strcmp(buffer,"Cliente existe")!=0){
                system("clear");
                printf("%s\n", buffer);
            }
            else
              printf("\n");
        }
    }
    system("clear");
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Nao consegui obter endereço");
    
    bzero((void *) &addr2, sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    addr2.sin_port = htons(SERVER_PORT2);
    
    if((fd2 = socket(AF_INET,SOCK_STREAM,0)) == -1)
        erro("socket");
    
    if( connect(fd2,(struct sockaddr *)&addr2,sizeof (addr2)) < 0)
        erro("Connect");
    pthread_create(&my_thread,NULL,notifica,&fd2);
    while(!(strcmp(acao, "4")==0)){
        nread = read(fd, buffer, BUF_SIZE);
        buffer[nread] = '\0';
        printf("%s\n", buffer);
        scanf("%s", acao);
        write(fd, acao, BUF_SIZE);
        if(strcmp(acao,"1")==0){
            system("clear");
            while(!(strcmp(acao,"10")==0)){
                nread = read(fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                printf("%s\n", buffer);
                scanf("%s", acao);
                write(fd, acao, BUF_SIZE);
                nread = read(fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                system("clear");
                printf("%s\n", buffer);
            }
        } else if(strcmp(acao,"2")==0){
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            system("clear");
            printf("%s\n", buffer);
        } else if(strcmp(acao,"3")==0){
            system("clear");
            while(!(strcmp(acao,"7")==0)){
                nread = read(fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                printf("%s\n", buffer);
                scanf("%s", acao);
                write(fd, acao, BUF_SIZE);
                nread = read(fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                system("clear");
                printf("%s\n", buffer);
            }
        } else if(strcmp(acao,"4")==0){
            printf("\n");
        } else if(strcmp(acao,"5")==0){
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            system("clear");
            printf("%s\n", buffer);
        } else{
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            system("clear");
            printf("%s\n", buffer);
        }
    }
    system("clear");
    printf("\nFechando conexão com o servidor...\n");
    done=1;
    pthread_join(my_thread,NULL);
    close(fd);
    exit(0);
}

void* notifica(void* idp){
    int fd2=*((int*) idp);
    long nread = 0;
    char buffer[BUF_SIZE];
	nread = read(fd2, buffer, BUF_SIZE);
        buffer[nread] = '\0';
        printf("%s\n", buffer);
	while(done==0){
		nread = read(fd2, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                printf("%s\n", buffer);

	}

	close(fd2);
      pthread_exit(NULL);
      return NULL;


}



void erro(char *msg){
    printf("Erro: %s\n", msg);
    exit(-1);
}
