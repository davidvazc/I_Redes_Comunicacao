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

#define BUF_SIZE  2048
#define SERVER_PORT 9000
#define SERVER_PORT2 9001
#define IP_ADRR "127.0.0.1"

void erro(char *msg);
int done=0;
char endServer[100];
struct hostent *hostPtr;

void* notifica(){ //Função que recebe as notificações
    sleep(3);
    int nread = 0;
    char buffer[BUF_SIZE];
    struct sockaddr_in addr2;
    int fd2;
    strcpy(endServer, IP_ADRR);
    if ((hostPtr = gethostbyname(endServer)) == 0)
        erro("Nao consegui obter endereço");
    //    cria estrutura
    bzero((void *) &addr2, sizeof(addr2));
    // Configure settings of the server address struct
    // Address family = Internet
    addr2.sin_family = AF_INET;
    //Set IP address to localhost
    addr2.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
    //Set port number, using htons function to use proper byte order
    addr2.sin_port = htons(SERVER_PORT2);
    //Create the socket.
    if((fd2 = socket(AF_INET,SOCK_STREAM,0)) == -1)
        erro("socket");
    //Connect
    if( connect(fd2,(struct sockaddr *)&addr2,sizeof (addr2)) < 0) //Conecta ao thread server
        erro("Connect");
    nread = read(fd2, buffer, BUF_SIZE); //Recebe mensagem a dizer que conectou
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    while(done==0){ //Fica a espera de mensagens do thread server(notificação) enquanto o cliente nao pedir para sair do server
        nread = read(fd2, buffer, BUF_SIZE);
        buffer[nread] = '\0';
        printf("%s\n", buffer);
    }

    close(fd2);
    pthread_exit(NULL);
    return NULL;


}

int main(int argc, char *argv[]) {

    int fd;
    struct sockaddr_in addr;
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

    int nread = 0;
    char buffer[BUF_SIZE],acao[BUF_SIZE];
    pthread_t my_thread;



    nread = read(fd, buffer, BUF_SIZE); //Le mensagem de boas vindas
    buffer[nread] = '\0';
    system("clear");
    printf("%s\n", buffer);

    scanf("%s", acao);
    write(fd, acao, BUF_SIZE);      //escreve o id

    nread = read(fd, buffer, BUF_SIZE);//le resposta sobre id
    buffer[nread] = '\0';

    if(strcmp(buffer,"Cliente existe")==0){ //leu que o cliente existe na base de dados
        printf("\n");
    }
    else{
        system("clear");
        printf("%s\n", buffer);
        while(strcmp(buffer,"Cliente existe")!=0){ //enquanto o id inserido não existir na base de dados
            scanf("%s", acao);
            write(fd, acao, BUF_SIZE);
            nread = read(fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            if(strcmp(buffer,"Cliente existe")!=0){ //id ainda não é o correto
                system("clear");
                printf("%s\n", buffer);
            }
            else //quando se insere o id correto
                printf("\n");
        }
    }
    system("clear");
    pthread_create(&my_thread,NULL,notifica,NULL); //Criar o thread que le as notificações
    do{
        nread = read(fd, buffer, BUF_SIZE); // le o menu
        buffer[nread] = '\0';
        printf("%s\n", buffer);//print do menu no cliente
        scanf("%s", acao); //scan da ação pretendida no menu
        write(fd, acao, BUF_SIZE); //envia a ação pretendido.
        if(strcmp(acao,"1")==0){ //informações pessoais
            system("clear");
            while(!(strcmp(acao,"11")==0)){ //enquanto nao pedir para sair do sub-menu das informaçoes do cliente
                nread = read(fd, buffer, BUF_SIZE); //le o sub-menu
                buffer[nread] = '\0';
                printf("%s\n", buffer); //print do sub-menu no cliente
                scanf("%s", acao); //scan da ação pretendida no sub-menu
                write(fd, acao, BUF_SIZE); //envia o que se pretende fazer
                nread = read(fd, buffer, BUF_SIZE); //le a informação pessoal pretendida
                buffer[nread] = '\0';
                system("clear");
                printf("%s\n", buffer); //print da informação no cliente
            }
        } else if(strcmp(acao,"2")==0){ //informações do grupo
            system("clear");
            while(!(strcmp(acao,"8")==0)){ //enquanto nao pedir para sair do sub-menu das informações do grupo
                nread = read(fd, buffer, BUF_SIZE); //le o sub-menu
                buffer[nread] = '\0';
                printf("%s\n", buffer); //print do sub-menu no cliente
                scanf("%s", acao); //scan da ação pretendida
                write(fd, acao, BUF_SIZE); //envia o que se pretende fazer
                nread = read(fd, buffer, BUF_SIZE); //le a informação do grupo pretendida
                buffer[nread] = '\0';
                system("clear");
                printf("%s\n", buffer); //print da informação no cliente
            }
        } else if(strcmp(acao,"3")==0){ //Subscrições
            system("clear");
            while(!(strcmp(acao,"8")==0)){ //enquanto nao pedir para sair do sub-menu das subscrições
                nread = read(fd, buffer, BUF_SIZE); // le o sub-menu
                buffer[nread] = '\0';
                printf("%s\n", buffer); //print do sub-menu no cliente
                scanf("%s", acao); //scan da informação a que se quer subscrever
                write(fd, acao, BUF_SIZE); //envia a que informação se quer subscrever
                nread = read(fd, buffer, BUF_SIZE); //le a resposta do servidor
                buffer[nread] = '\0';
                system("clear");
                printf("%s\n", buffer); //print se esta subscrito ou se deixou de estar subscrito
            }
        } else if(strcmp(acao,"4")==0){ //sai do servidor
            printf("\n");
        } else{ //nao foi enviada nenhuma das ações que estão no menu
            nread = read(fd, buffer, BUF_SIZE); //le que tem de enviar uma das opções do menu
            buffer[nread] = '\0';
            system("clear");
            printf("%s\n", buffer); //print dessa informação no cliente
        }
    }while((strcmp(acao, "4")!=0)); //enquanto nao pedir para sair
    system("clear");
    printf("\nFechando conexão com o servidor...\n");
    done=1; //para fechar o thread do cliente
    pthread_join(my_thread,NULL); //fica a espera que ele termine
    close(fd); //fecha o socket
    exit(0); // termina o programa
}

void erro(char *msg){
    printf("Erro: %s\n", msg);
    exit(-1);
}

