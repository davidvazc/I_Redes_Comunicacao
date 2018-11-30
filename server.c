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
#define SERVER_PORT     9000
#define N_USERS   2

void process_client(int fd);
void erro(char *msg);

typedef struct{
	double call_duracao;
	double call_feitas;
	double call_perdidas;
	double call_recebidas;
	double sms_recebidas;
	double sms_enviadas;
}Grupo;


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
Grupo grupo;
Grupo ogrupo;

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

}



int main() {
    int fd, client;
    struct sockaddr_in addr, client_addr;
    int client_addr_size;
    
    bzero((void *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);
    
    if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        erro("na funcao socket");
    if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
        erro("na funcao bind");
    if( listen(fd, 5) < 0)
        erro("na funcao listen");
    
    while (1) {
        printf("\n-- A espera de mensagem --\n");
        
        while(waitpid(-1,NULL,WNOHANG)>0);
        
        client_addr_size = sizeof(client_addr);
        client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
        
        
        if (client > 0) {
            if (fork() == 0) {
                close(fd);
                process_client(client);
                close(client);
                exit(0);
            }
            
            
        }
    }
    close(fd);
    exit(0);
    return 0;
}

void process_client(int client_fd)
{
    struct sockaddr_in addr, client_addr2;
    int nread = 0;
    char id_client[BUF_SIZE];
    int noti_fd;
    char buffer[BUF_SIZE];
    write(client_fd,"Seja bem vindo ao servidor Isabela!",BUF_SIZE);
    write(client_fd,"Introduza o ID da isabela: ",BUF_SIZE);
    faz_client();
    cria_grupo();
    nread = read(client_fd, id_client, BUF_SIZE);
    client_addr_size2 = sizeof(client_addr2);
    noti = accept(noti_fd,(struct sockaddr *)&client_addr2,(socklen_t *)&client_addr_size2);
	
    if(noti>0){
	if(fork()==0){
		close(noti_fd);
		notificacoes(noti,id_client);
		exit(0);
	}
	close(noti);
    }
    
    
    while(!(strcmp(buffer,"SAIR")==0)){
        write(client_fd,"Introduza mensagem: ",BUF_SIZE);
        nread = read(client_fd, buffer, BUF_SIZE);
        buffer[nread] = '\0';
        printf("%s\n", buffer);
        
    }
    
    printf("Fechando conexão com o cliente...\n");
    close(client_fd);
    exit(0);
    
}

//envia dados do cliente pretendido
void envia_info(int client_fd,char id_client[]){
    
	int i=0;	
	char scalls_feitas[BUF_SIZE],scalls_duracao[BUF_SIZE],scalls_perdidas[BUF_SIZE],scalls_recebidas[BUF_SIZE];
    char ssms_recebidas[BUF_SIZE],ssms_enviadas[BUF_SIZE],sdepartamento[BUF_SIZE], slocalizacao[BUF_SIZE];
    char sAtividade[BUF_SIZE];
	while(strcmp(total_pessoas[i].id,id_client)!=0 && i<BUF_SIZE){
	i++;
	}
    
    sprintf(sAtividade,"Actividade:%s",total_pessoas[i].atividade);
    sprintf(slocalizacao,"Localização:%s",total_pessoas[i].localizacao);
    sprintf(sdepartamento,"Departamento:%s",total_pessoas[i].departamento);
    sprintf(scalls_feitas,"Chamadas feitas:%d",total_pessoas[i].call_feitas);
    sprintf(scalls_duracao,"Duração chamadas:%d",total_pessoas[i].call_duracao);
    sprintf(scalls_perdidas,"Chamadas perdidas: %d",total_pessoas[i].call_perdidas);
    sprintf(scalls_recebidas,"Chamadas recebidas: %d",total_pessoas[i].call_recebidas);
	sprintf(ssms_recebidas,"SMS recebidas: %d",total_pessoas[i].sms_recebidas);
	sprintf(ssms_enviadas,"SMS enviadas: %d",total_pessoas[i].sms_enviadas);
	
	write(client_fd,sAtividade,BUF_SIZE);
	write(client_fd,slocalizacao,BUF_SIZE);
	write(client_fd,sdepartamento,BUF_SIZE);
	write(client_fd,scalls_feitas,BUF_SIZE);
	write(client_fd,scalls_duracao,BUF_SIZE);
	write(client_fd,scalls_perdidas,BUF_SIZE);
	write(client_fd,scalls_recebidas,BUF_SIZE);
  	write(client_fd,ssms_recebidas,BUF_SIZE);
  	write(client_fd,ssms_enviadas,BUF_SIZE);

}

//Media do grupo
void cria_grupo(){
	double total_calls_duracao = 0;
  	double total_calls_feitas = 0;
 	double total_calls_perdidas = 0;
 	double total_calls_recebidas = 0;
 	double total_sms_recebidas = 0;
 	double total_sms_enviadas = 0;
	
	//somar
	for(int i=0;i<N_USERS;i++){
        	total_calls_duracao+=total_pessoas[i].call_duracao;
        	total_calls_feitas+=total_pessoas[i].call_feitas;
        	total_calls_perdidas+=total_pessoas[i].call_perdidas;
        	total_calls_recebidas+=total_pessoas[i].call_recebidas;
		total_sms_recebidas+=total_pessoas[i].sms_recebidas;
		total_sms_enviadas+=total_pessoas[i].sms_enviadas;
	}
	//Media
	double media_calls_duracao=total_calls_duracao/N_USERS;
	double media_calls_feitas=total_calls_feitas/N_USERS;
	double media_calls_perdidas=total_calls_perdidas/N_USERS;
	double media_calls_recebidas=total_calls_recebidas/N_USERS;
	double media_sms_recebidas=total_sms_recebidas/N_USERS;
	double media_sms_enviadas=total_sms_enviadas/N_USERS;
	
	//Guarda em grupo
	grupo.calls_recebidas=media_calls_recebidas;
	grupo.calls_feitas=media_calls_feitas;
	grupo.calls_perdidas=media_calls_perdidas;
	grupo.calls_duracao=media_calls_duracao;
	grupo.sms_recebidas=media_sms_recebidas;
	grupo.sms_enviadas=media_sms_enviadas;
	
}


//Enviar media do grupo
void media_grupo(int client_fd){
	//Passar para String
	char scalls_duracao[BUF_SIZE];
  	char scalls_feitas[BUF_SIZE];
  	char scalls_perdidas[BUF_SIZE];
  	char scalls_recebidas[BUF_SIZE];
  	char ssms_recebidas[BUF_SIZE];
  	char ssms_enviadas[BUF_SIZE];

  	sprintf(scalls_recebidas, "Media chamadas recebidas: %f", grupo.calls_recebidas);
  	sprintf(scalls_feitas, "Media chamadas feitas: %f", grupo.calls_feitas);
  	sprintf(scalls_perdidas, "Media chamadas perdidas: %f", grupo.calls_perdidas);
  	sprintf(scalls_duracao, "Duração media de chamada: %f", grupo.calls_duracao);
  	sprintf(ssms_enviadas, "SMS enviadas: %f", grupo.sms_enviadas);
  	sprintf(ssms_recebidas, "SMS recebidas: %f", grupo.sms_recebidas);
	
	//Envia para cliente
  	write(client_fd,scalls_recebidas, BUF_SIZE);
 	write(client_fd,scalls_feitas, BUF_SIZE);
  	write(client_fd,scalls_perdidas, BUF_SIZE);
  	write(client_fd,scalls_duracao, BUF_SIZE);
  	write(client_fd,ssms_enviadas, BUF_SIZE);
  	write(client_fd,ssms_recebidas, BUF_SIZE);

}


void subscricoes(int client_fd,char id_client[]){
    int i = 0;
	int nread=0;
	char buffer[BUF_SIZE];
    while(strcmp(total_pessoas[i].id,id_client)!=0 && i<BUF_SIZE){
		i++;
	}
	write(client_fd,"Escolha um dado que queira subscrever/cancelar:\n1.Chamadas recebidas.\n2.Chamadas feitas.\n3.Chamadas perdidas\n4.Duração media de chamada.\n5.SMS enviados.\n6.SMS recebidos.\n",BUF_SIZE);
	nread=read(client_fd,buffer,BUF_SIZE);
	buffer[nread]='\0';
	do{
	if(strcmp(buffer,"1")==0){
        if(total_pessoas[i].sub_call_recebidas==false){
            total_pessoas[i].sub_call_recebidas=true;
			write(client_fd,"Está agora subscrito.",BUF_SIZE);
			ogrupo.calls_recebidas=grupo.calls_recebidas;
		}
		else{
            total_pessoas[i].sub_call_recebidas=false;
			write(client_fd,"Deixou de estar subscrito.",BUF_SIZE);
		
		}
	}
	else if(strcmp(buffer,"2")==0){
        if(total_pessoas[i].sub_call_feitas==false){
            total_pessoas[i].sub_call_feitas=true;
			write(client_fd,"Está agora subscrito.",BUF_SIZE);
			ogrupo.calls_feitas=grupo.calls_feitas;
		}
		else{
            total_pessoas[i].sub_call_feitas=false;
			write(client_fd,"Deixou de estar subscrito.",BUF_SIZE);
		}
	}
	else if(strcmp(buffer,"3")==0){
        if(total_pessoas[i].sub_call_perdidas==false){
            total_pessoas[i].sub_call_perdidas=true;
			write(client_fd,"Está agora subscrito.",BUF_SIZE);
			ogrupo.calls_perdidas=grupo.calls_perdidas;
		}
		else{
            total_pessoas[i].sub_call_perdidas=false;
			write(client_fd,"Deixou de estar subscrito.",BUF_SIZE);
		}
		
	}
	else if(strcmp(buffer,"4")==0){
        if(total_pessoas[i].sub_call_duracao==false){
            total_pessoas[i].sub_call_duracao=true;
			write(client_fd,"Está agora subscrito.",BUF_SIZE);
			ogrupo.calls_duracao=grupo.calls_duracao;
		}
		else{
            total_pessoas[i].sub_call_duracao=false;
			write(client_fd,"Deixou de estar subscrito.",BUF_SIZE);
		}
		
	}
	else if(strcmp(buffer,"5")==0){
		if(total_pessoas[i].sub_sms_enviadas==false){
			total_pessoas[i].sub_sms_enviadas=true;
			write(client_fd,"Está agora subscrito.",BUF_SIZE);
			ogrupo.sms_enviadas=grupo.sms_enviadas;
		}
		else{
			total_pessoas[i].sub_sms_enviadas=false;
			write(client_fd,"Deixou de estar subscrito.",BUF_SIZE);
		}
		
	}
	else if(strcmp(buffer,"6")==0){
		if(total_pessoas[i].sub_sms_recebidas==false){
			total_pessoas[i].sub_sms_recebidas=true;
			write(client_fd,"Está agora subscrito.",BUF_SIZE);
			ogrupo.sms_recebidas=grupo.sms_recebidas;
		}
		else{
			total_pessoas[i].sub_sms_recebidas=false;
			write(client_fd,"Deixou de estar subscrito.",BUF_SIZE);
		}
		
	}
	else{
		write(client_fd,"Essa opção não existe.",BUF_SIZE);
	}
	}while(strcmp(buffer,"6")!=0 && strcmp(buffer,"5")!=0 && strcmp(buffer,"4")!=0 && strcmp(buffer,"3")!=0 && strcmp(buffer,"2")!=0 && strcmp(buffer,"1")!=0);
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
