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

#define BUF_SIZE  1024
#define SERVER_PORT 9000
#define IP_ADRR "127.0.0.1"

void erro(char *msg);

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
    
    nread = read(fd, buffer, BUF_SIZE); //Le mensagem de boas vindas
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    
    scanf("%s", acao);
    write(fd, acao, BUF_SIZE);      //escreve o id
    
    nread = read(fd, buffer, BUF_SIZE);//le resposta sobre id
    buffer[nread] = '\0';
    
    if(strcmp(buffer,"Cliente existe")==0){
        printf("\n");
    }
    else{
        
        printf("%s\n", buffer);
        while(strcmp(buffer,"Cliente existe")!=0){
            scanf("%s", acao);
            write(fd, acao, BUF_SIZE);
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            if(strcmp(buffer,"Cliente existe")!=0)
                printf("%s\n", buffer);
            else
              printf("\n");
        }
    }
    
    while(!(strcmp(acao, "4")==0)){
        nread = read(fd, buffer, BUF_SIZE);
        buffer[nread] = '\0';
        printf("%s\n", buffer);

        scanf("%s", acao);
        write(fd, acao, BUF_SIZE);
        if(strcmp(acao,"1")==0){
            while(!(strcmp(acao,"10")==0)){
                nread = read(fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                printf("%s\n", buffer);
                scanf("%s", acao);
                write(fd, acao, BUF_SIZE);
                nread = read(fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                printf("%s\n", buffer);
            }
        } else if(strcmp(acao,"2")==0){
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            printf("%s\n", buffer);
        } else if(strcmp(acao,"3")==0){
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            printf("%s\n", buffer);
            //            subscrito(client_fd, id);
        } else if(strcmp(acao,"4")==0){
            printf("\n");
        } else{
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            printf("%s\n", buffer);
        }
    }
    printf("\nFechando conexão com o servidor...\n");
    close(fd);
    exit(0);
}

void erro(char *msg){
    printf("Erro: %s\n", msg);
    exit(-1);
}
