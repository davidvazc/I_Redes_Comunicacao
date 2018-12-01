/*******************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surjem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdbool.h>


#define BUF_SIZE    1024
#define SERVER_PORT     9001
#define N_USERS   2
#define NMEDIA  6



void process_client(int client_fd);
void erro(char *msg);
void media_grupo(int client_fd);
void subscricoes(int client_fd,int i);


typedef struct{
	char id[BUF_SIZE];
	char atividade[BUF_SIZE];
	char localizacao[BUF_SIZE];
	int call_duracao;
	bool sub_call_duracao;
	int call_feitas;
	bool sub_call_feitas;
	int call_perdidas;
	bool sub_call_perdidas;
	int call_recebidas;
	bool sub_call_recebidas;
	char departamento[BUF_SIZE];
	int sms_recebidas;
	bool sub_sms_recebidas;
	int sms_enviadas;
	bool sub_sms_enviadas;
}Client;

Client total_pessoas[BUF_SIZE];
int nr_clientes;
double media[NMEDIA];

void faz_client(){
    strcpy(total_pessoas[0].atividade, "Estudando");
    strcpy(total_pessoas[0].localizacao, "Portugal");
    strcpy(total_pessoas[0].departamento, "DEM");
	strcpy(total_pessoas[0].id , "teste1");
    total_pessoas[0].call_feitas = 7;
    total_pessoas[0].sub_call_feitas=false;
  	total_pessoas[0].call_duracao = 2;
    total_pessoas[0].sub_call_duracao=false;
  	total_pessoas[0].call_perdidas = 1;
	total_pessoas[0].sub_call_perdidas=false;
  	total_pessoas[0].call_recebidas = 5;
	total_pessoas[0].sub_call_recebidas=false;
 	total_pessoas[0].sms_recebidas = 6;
	total_pessoas[0].sub_sms_recebidas=false;
  	total_pessoas[0].sms_enviadas = 6;
	total_pessoas[0].sub_sms_enviadas=false;
    nr_clientes++;

  	strcpy(total_pessoas[1].atividade, "Bebendo");
  	strcpy(total_pessoas[1].localizacao, "Brazil");
  	strcpy(total_pessoas[1].departamento, "DEI");
  	strcpy(total_pessoas[1].id , "teste2");
    total_pessoas[1].call_feitas = 5;
    total_pessoas[1].sub_call_feitas=false;
    total_pessoas[1].call_duracao = 3;
    total_pessoas[1].sub_call_duracao=false;
    total_pessoas[1].call_perdidas = 3;
    total_pessoas[1].sub_call_perdidas=false;
    total_pessoas[1].call_recebidas = 10;
    total_pessoas[1].sub_call_recebidas=false;
  	total_pessoas[1].sms_recebidas = 4;
	total_pessoas[1].sub_sms_recebidas=false;
  	total_pessoas[1].sms_enviadas =5;
	total_pessoas[1].sub_sms_enviadas=false;
    nr_clientes++;
}

void calcula_media(){
    //somar
    for(int i=0;i<N_USERS;i++){
        media[0]+=total_pessoas[i].call_feitas;
        media[1]+=total_pessoas[i].call_duracao;
        media[2]+=total_pessoas[i].call_perdidas;
        media[3]+=total_pessoas[i].call_recebidas;
        media[4]+=total_pessoas[i].sms_recebidas;
        media[5]+=total_pessoas[i].sms_enviadas;
    }
    //Media
    media[0]=media[0]/N_USERS;
    media[1]=media[1]/N_USERS;
    media[2]=media[2]/N_USERS;
    media[3]=media[3]/N_USERS;
    media[4]=media[4]/N_USERS;
    media[5]=media[5]/N_USERS;
}

int main() {
    
    int fd, client;
    struct sockaddr_in addr, client_addr;
    int client_addr_size;
    
//    cria estrutura
    bzero((void *) &addr, sizeof(addr));
    
    // Configure settings of the server address struct
    // Address family = Internet
    addr.sin_family = AF_INET;
    //Set IP address to localhost
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Set port number, using htons function to use proper byte order
    addr.sin_port = htons(SERVER_PORT);
    
    //Create the socket.
    if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        erro("na funcao socket");
    //Bind the address struct to the socket
    if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
        erro("na funcao bind");
    //Listen on the socket, with 5 max connection requests queued
    if( listen(fd, 5) < 0)
        erro("na funcao listen");
    
    faz_client();
    
    while (1) {
        
        while(waitpid(-1,NULL,WNOHANG)>0);
        
        client_addr_size = sizeof(client_addr);
        client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
        
        
        if (client > 0) {
            if (fork() == 0) {
                close(fd);
                process_client(client);
                exit(0);
            }
            close(client);
        }
    }
    return 0;
}

void process_client(int client_fd){
    /*
//      CRIADO POR TIq
    struct sockaddr_in addr, client_addr2;
    long nreadSUB = 0;
    char id_client[BUF_SIZE];
    int client_addr_size2;
    int noti_fd;
    calcula_media();
    
    if ( (noti_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        erro("na funcao socket");
    if ( bind(noti_fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
        erro("na funcao bind");
    if( listen(noti_fd, 5) < 0)
        erro("na funcao listen");
    client_addr_size2 = sizeof(client_addr2);
//    falta declarar noti
    noti = accept(noti_fd,(struct sockaddr *)&client_addr2,(socklen_t *)&client_addr_size2);

    if(noti>0){
        if(fork()==0){
            close(noti_fd);
            notificacoes(noti,id_client);
            exit(0);
        }
    close(noti);
    }
    */
    int cliente=-1;
    long nread = 0;
    char buffer[BUF_SIZE],aux[BUF_SIZE];
    write(client_fd,"\n++++++++++++++++++++++++++++++++++++++++++\n+                                        +\n+   SEJA BEM VINDO AO SERVIDOR ISABELA   +\n+                                        +\n++++++++++++++++++++++++++++++++++++++++++\n\nIntoruza o seu id:",BUF_SIZE); //escreve mensagem de boas vindas e pede id
    
    nread = read(client_fd, buffer, BUF_SIZE);//le id
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    
    for(int i=0;i<nr_clientes;i++){
        if(strcmp(buffer,total_pessoas[i].id)==0){ //verifica se nome está presente
            cliente=i;
        }
    }
    if(cliente!=-1){
        write(client_fd,"Cliente existe",BUF_SIZE);//escreve resposta sobre id
    }
    else{
        write(client_fd,"\nO id introduzido não existe!\nIntroduza novamente:",BUF_SIZE);
        while(cliente==-1){
            nread = read(client_fd, buffer, BUF_SIZE);
            buffer[nread] = '\0';
            printf("%s\n", buffer);
            for(int i=0;i<nr_clientes;i++){
                if(strcmp(buffer,total_pessoas[i].id)==0){
                    cliente=i;
                    write(client_fd,"Cliente existe",BUF_SIZE);
                }
            }
            if(cliente==-1)
                write(client_fd,"\nO id introduzido não existe!\nIntroduza novamente:",BUF_SIZE);
        }
    }
    
    while(!(strcmp(buffer,"4")==0)){
        write(client_fd,"\n+++++++++++++++++++++++++++++++++\n+        Escolha a opção        +\n+    1-Informações pessoais     +\n+    2-Media de grupo           +\n+    3-Subscrição               +\n+    4-Sair                     +\n+++++++++++++++++++++++++++++++++\n",BUF_SIZE);
        nread = read(client_fd, buffer, BUF_SIZE);
        buffer[nread] = '\0';
        printf("%s\n", buffer);
        if(strcmp(buffer, "1")==0){
            while(!(strcmp(buffer,"10")==0)){
                write(client_fd,"\n+++++++++++++++++++++++++++\n+  1- Actividade          +\n+  2- Localização         +\n+  3- Departamento        +\n+  4- Chamadas feitas     +\n+  5- Duração chamadas    +\n+  6- Chamadas perdidas   +\n+  7- Chamadas recebidas  +\n+  8- SMS recebidas       +\n+  9- SMS enviadas        +\n+  10- Voltar             +\n+++++++++++++++++++++++++++\n",BUF_SIZE);
                nread = read(client_fd, buffer, BUF_SIZE);
                buffer[nread] = '\0';
                printf("%s\n", buffer);
                if(strcmp(buffer, "1")==0){
                    sprintf(aux,"\n\n\n>>>   Actividade: %s   <<<\n",total_pessoas[cliente].atividade);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "2")==0){
                    sprintf(aux,"\n\n\n>>>   Localização: %s   <<<\n",total_pessoas[cliente].localizacao);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "3")==0){
                    sprintf(aux,"\n\n\n>>>   Departamento: %s   <<<\n",total_pessoas[cliente].departamento);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "4")==0){
                    sprintf(aux,"\n\n\n>>>   Chamadas feitas: %d   <<<\n",total_pessoas[cliente].call_feitas);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "5")==0){
                    sprintf(aux,"\n\n\n>>>   Duração chamadas: %d   <<<\n",total_pessoas[cliente].call_duracao);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "6")==0){
                    sprintf(aux,"\n\n\n>>>   Chamadas perdidas: %d   <<<\n",total_pessoas[cliente].call_perdidas);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "7")==0){
                    sprintf(aux,"\n\n\n>>>   Chamadas recebidas: %d   <<<\n",total_pessoas[cliente].call_recebidas);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "8")==0){
                    sprintf(aux,"\n\n\n>>>   SMS recebidas: %d   <<<\n",total_pessoas[cliente].sms_recebidas);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "9")==0){
                    sprintf(aux,"\n\n\n>>>   SMS enviadas: %d   <<<\n",total_pessoas[cliente].sms_enviadas);
                    write(client_fd, aux, BUF_SIZE);
                } else if(strcmp(buffer, "10")==0){
                    write(client_fd,"\n", BUF_SIZE);
                } else{
                    write(client_fd,"\n\n\n>>>ERRO: Escolha 1,2,3,4,5,6,7,8,9 ou 10!\n",BUF_SIZE);
                }
            }
        } else if(strcmp(buffer,"2")==0){
            media_grupo(client_fd);
        } else if(strcmp(buffer,"3")==0){
            subscricoes(client_fd,cliente);
        } else if(strcmp(buffer,"4")==0){
            printf("\n");
        } else if(strcmp(buffer,"5")==0){
            total_pessoas[0].call_feitas = 20;
            sprintf(aux,"\n\n>>recebi a opçao %s e as chamdas feitas mudaram para %d   <<<\n",buffer,total_pessoas[0].call_feitas);
            write(client_fd, aux, BUF_SIZE);
        } else{
            write(client_fd,"\n\n>>ERRO: Escolha 1,2,3 ou 4!",BUF_SIZE);
        }
    }
    
    printf("Fechando conexão com o cliente...\n");
    close(client_fd);
    exit(0);
    
}

//Media do grupo
void media_grupo(int client_fd){
	
	//Passar para String
	char buff[BUF_SIZE];
    calcula_media();

  	sprintf(buff, "\n++++++++++++++++++++++++++++++++++++\n+  Media chamadas recebidas: %.2f  +\n+  Media chamadas feitas: %.2f     +\n+  Media chamadas perdidas: %.2f   +\n+  Duração media de chamada: %.2f  +\n+  SMS enviadas: %.2f              +\n+  SMS recebidas: %.2f             +\n++++++++++++++++++++++++++++++++++++",media[0],media[1],media[2],media[3],media[4],media[5]);
	//Envia para cliente
  	write(client_fd,buff, BUF_SIZE);

}


void subscricoes(int client_fd,int i){
    char buffer[BUF_SIZE];
    long nread=0;
    do{
        write(client_fd,"++++++++++++++++++++++++++++++++++++++++++++++++\n+  Selecione a que deseja subscrever/cancelar: +\n+  1- Chamadas recebidas                       +\n+  2- Chamdas feitas                           +\n+  3- Chamadas perdidas                        +\n+  4- Duração media de chamada                 +\n+  5- SMS enviados                             +\n+  6- SMS recebidos                            +\n+  7- Voltar                                   +\n++++++++++++++++++++++++++++++++++++++++++++++++\n",BUF_SIZE);
    
        nread=read(client_fd,buffer,BUF_SIZE);
        buffer[nread]='\0';
        printf("%s\n", buffer);
	
        if(strcmp(buffer,"1")==0){
            if(total_pessoas[i].sub_call_recebidas==false){
                total_pessoas[i].sub_call_recebidas=true;
                write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
            } else{
                total_pessoas[i].sub_call_recebidas=false;
                write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
		
            }
        } else if(strcmp(buffer,"2")==0){
            if(total_pessoas[i].sub_call_feitas==false){
                total_pessoas[i].sub_call_feitas=true;
                write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
            } else{
                total_pessoas[i].sub_call_feitas=false;
                write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
            }
        } else if(strcmp(buffer,"3")==0){
            if(total_pessoas[i].sub_call_perdidas==false){
                total_pessoas[i].sub_call_perdidas=true;
                write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
            } else{
                total_pessoas[i].sub_call_perdidas=false;
                write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
            }
		} else if(strcmp(buffer,"4")==0){
            if(total_pessoas[i].sub_call_duracao==false){
                total_pessoas[i].sub_call_duracao=true;
                write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
            } else{
                total_pessoas[i].sub_call_duracao=false;
                write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
            }
        } else if(strcmp(buffer,"5")==0){
            if(total_pessoas[i].sub_sms_enviadas==false){
                total_pessoas[i].sub_sms_enviadas=true;
                write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
            } else{
                total_pessoas[i].sub_sms_enviadas=false;
                write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
            }
		} else if(strcmp(buffer,"6")==0){
            if(total_pessoas[i].sub_sms_recebidas==false){
                total_pessoas[i].sub_sms_recebidas=true;
                write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
            } else{
                total_pessoas[i].sub_sms_recebidas=false;
                write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
            }
		
        } else if(strcmp(buffer,"7")==0){
            write(client_fd,"\n", BUF_SIZE);
        } else{
            write(client_fd,"\n\n\n>>>ERRO: Escolha 1,2,3,4,5,6 ou 7!\n",BUF_SIZE);
        }
	}while(strcmp(buffer,"7")!=0);
}



void notificacoes(int noti_fd,char id_client[]){
	int i=0;
	while(strcmp(total_pessoas[i].id,id_client)!=0 && i<N_USERS){
		i++;
	}
	while(1){
		if(ogrupo.calls_recebidas!=grupo.calls_recebidas){
			ogrupo.calls_recebidas=grupo.calls_recebidas;
			if(total_pessoas[i].sub_calls_recebidas==true){
				write(noti_fd,"O valor das chamadas recebidas foi alterado!",BUF_SIZE);
			}
		}
		else if(ogrupo.calls_feitas!=grupo.calls_feitas){
			ogrupo.calls_feitas=grupo.calls_feitas;
			if(total_pessoas[i].sub_calls_feitas==true){
				write(noti_fd,"O valor das chamadas feitas foi alterado!",BUF_SIZE);
			}
		}
		else if(ogrupo.calls_duracao!=grupo.calls_duracao){
			ogrupo.calls_duracao=grupo.calls_duracao;
			if(total_pessoas[i].sub_calls_duracao==true){
				write(noti_fd,"O valor da duração das chamadas foi alterado!",BUF_SIZE);
			}
		}
		else if(ogrupo.calls_perdidas!=grupo.calls_perdidas){
			ogrupo.calls_perdidas=grupo.calls_perdidas;
			if(total_pessoas[i].sub_calls_perdidas==true){
				write(noti_fd,"O valor das chamadas perdidas foi alterado!",BUF_SIZE);
			}
		}
		else if(ogrupo.sms_recebidas!=grupo.sms_recebidas){
			ogrupo.sms_recebidas=grupo.sms_recebidas;
			if(total_pessoas[i].sub_sms_recebidas==true){
				write(noti_fd,"O valor das SMS recebidas foi alterado!",BUF_SIZE);
			}
		}
		else if(ogrupo.sms_enviadas!=grupo.sms_enviadas){
			ogrupo.sms_enviadas=grupo.sms_enviadas;
			if(total_pessoas[i].sub_sms_enviadas==true){
				write(noti_fd,"O valor das SMS enviadas foi alterado!",BUF_SIZE);
			}
		}
	}
	
	
}

void erro(char *msg)
{
    printf("Erro: %s\n", msg);
    exit(-1);
}
