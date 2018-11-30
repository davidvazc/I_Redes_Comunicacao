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


#define BUF_SIZE    1024
#define SERVER_PORT     9000
#define N_USERS   2

void process_client(int fd);
void erro(char *msg);


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

Client *total_pessoas[BUF_SIZE];

void faz_client(Client total_pessoas[]){
	Client cl1;
	Client cl2;

	strcpy(cl1.atividade, "Estudando");
	strcpy(cl1.localizacao, "Portugal");
	strcpy(cl1.departamento, "DEM");
	strcpy(cl1.id , "teste1");
	cl1.calls_feitas = 7;
	cl1.sub_calls_feitas=false;
  	cl1.call_duracao = 2;
	cl1.sub_calls_duracao=false;
  	cl1.call_perdidas = 1;
	cl1.sub_call_perdidas=false;
  	cl1.call_recebidas = 5;
	cl1.sub_call_recebidas=false;
 	cl1.sms_recebidas = 6;
	cl1.sub_sms_recebidas=false;
  	cl1.sms_enviadas = 6;
	cl1.sub_sms_enviadas=false;

  	strcpy(cl2.atividade, "Bebendo");
  	strcpy(cl2.localizacao, "Brazil");
  	strcpy(cl2.departamento, "DEI");
  	strcpy(cl2.id , "teste2");
  	cl2.calls_feitas = 5;
	cl2.sub_calls_feitas=false;
  	cl2.calls_duracao = 3;
	cl2.sub_calls_duracao=false;
  	cl2.calls_perdidas = 3;
	cl2.sub_calls_perdidas=false;
  	cl2.calls_recebidas = 10;
	cl2.sub_calls_recebidas=false;
  	cl2.sms_recebidas = 4;
	cl2.sub_sms_recebidas=false;
  	cl2.sms_enviadas =5;
	cl2.sub_sms_enviadas=false;

  	*total_pessoas[0] = cl1;
  	*total_pessoas[1] = cl2;

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
    int nread = 0;
    char buffer[BUF_SIZE];
    write(client_fd,"Seja bem vindo ao servidor Isabela!",BUF_SIZE);
    write(client_fd,"Introduza o ID da isabela: ",BUF_SIZE);
    faz_client(&total_pessoas[]);
    
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
	char scalls_feitas[BUF_SIZE];
	char scalls_duracao[BUF_SIZE];
	char scalls_perdidas[BUF_SIZE];
	char scalls_recebidas[BUF_SIZE];
	char ssms_recebidas[BUF_SIZE];
	char ssms_enviadas[BUF_SIZE];
	while(strcmp(*total_pessoas[i].id,id_client)!=0 && i<BUF_SIZE){
	i++;
	}
	sprintf(scalls_feitas,"%d",*total_pessoas[i].calls_feitas);
	sprintf(scalls_duracao,"%d",*total_pessoas[i].calls_duracao);
	sprintf(scalls_perdidas,"%d",*total_pessoas[i].calls_perdidas);
	sprintf(scalls_recebidas,"%d",*total_pessoas[i].calls_recebidas);
	sprintf(ssms_recebidas,"%d",*total_pessoas[i].sms_recebidas);
	sprintf(ssms_enviadas,"%d",*total_pessoas[i].sms_enviadas);
	write(client_fd,"Id: "+*total_pessoas[i].id,BUF_SIZE);
	write(client_fd,"Atividade: "+*total_pessoas[i].atividade,BUF_SIZE);
	write(client_fd,"Localização: "+*total_pessoas[i].localizacao,BUF_SIZE);
	write(client_fd,"Departamento: "+*total_pessoas[i].departamento,BUF_SIZE);
	write(client_fd,"Chamadas feitas: "+scalls_feitas,BUF_SIZE);
	write(client_fd,"Duração chamadas: "+scalls_duracao,BUF_SIZE);
	write(client_fd,"Chamadas perdidas: "+scalls_perdidas,BUF_SIZE);
	write(client_fd,"Chamadas recebidas: "+scalls_recebidas,BUF_SIZE);
  	write(client_fd,"SMS recebidas: "+ssms_recebidas,BUF_SIZE);
  	write(client_fd,"SMS enviadas: "+ssms_enviadas,BUF_SIZE);

}


//Media do grupo
void media_grupo(int client_fd){
	Cliente media;
	double total_calls_duracao = 0;
  	double total_calls_feitas = 0;
 	double total_calls_perdidas = 0;
 	double total_calls_recebidas = 0;
 	double total_sms_recebidas = 0;
 	double total_sms_enviadas = 0;
	
	//somar
	for(int i=0;i<N_USERS;i++){
		total_calls_duracao+=*total_pessoas[i].calls_duracao;
		total_calls_feitas+=*total_pessoas[i].calls_feitas;
		total_calls_perdidas+=*total_pessoas[i].calls_perdidas;
		total_calls_recebidas+=*total_pessoas[i].calls_recebidas;
		total_sms_recebidas+=*total_pessoas[i].sms_recebidas;
		total_sms_enviadas+=*total_pessoas[i].sms_enviadas;
	}
	//Media
	double media_calls_duracao=total_calls_duracao/N_USERS;
	double media_calls_feitas=total_calls_feitas/N_USERS;
	double media_calls_perdidas=total_calls_perdidas/N_USERS;
	double media_calls_recebidas=total_calls_recebidas/N_USERS;
	double media_sms_recebidas=total_sms_recebidas/N_USERS;
	double media_sms_enviadas=total_sms_enviadas/N_USERS;
	
	//Passar para String
	char scalls_duracao[BUF_SIZE];
  	char scalls_feitas[BUF_SIZE];
  	char scalls_perdidas[BUF_SIZE];
  	char scalls_recebidas[BUF_SIZE];
  	char ssms_recebidas[BUF_SIZE];
  	char ssms_enviadas[BUF_SIZE];

  	sprintf(scalls_recebidas, "%f", media_calls_recebidas);
  	sprintf(scalls_feitas, "%f", media_calls_feitas);
  	sprintf(scalls_perdidas, "%f", media_calls_perdidas);
  	sprintf(scalls_duracao, "%f", media_calls_duracao);
  	sprintf(ssms_enviadas, "%f", media_sms_enviadas);
  	sprintf(ssms_recebidas, "%f", media_sms_recebidas);
	
	//Envia para cliente
  	write(client_fd, "Media chamadas recebidas: "+scalls_recebidas, BUF_SIZE);
 	write(client_fd, "Media chamadas feitas: "+scalls_feitas, BUF_SIZE);
  	write(client_fd, "Media chamadas perdidas: "+scalls_perdidas, BUF_SIZE);
  	write(client_fd, "Duração media de chamada: "+scalls_duracao, BUF_SIZE);
  	write(client_fd, "SMS enviadas: "+ssms_enviadas, BUF_SIZE);
  	write(client_fd, "SMS recebidas: "+ssms_recebidas, BUF_SIZE);

}


void subscricoes(int client_fd,char id_client[]){
	int i;
	int nread=0;
	char buffer[BUF_SIZE];
	while(strcmp(*total_pessoas[i].id,id_client)!=0 && i<BUF_SIZE){
		i++;
	}
	write(client_fd,"Escolha um dado que queira subscrever/cancelar:\n1.Chamadas recebidas.\n2.Chamadas feitas.\n3.Chamadas perdidas\n4.Duração media de chamada.\n5.SMS enviados.\n6.SMS recebidos.\n",BUF_SIZE);
	nread=read(client_fd,buffer,BUF_SIZE);
	buffer[nread]='\0';
	do{
	if(strcmp(buffer,'1')==0){
		if(*total_pessoas[i].sub_calls_recebidas==false){
			*total_pessoas[i].sub_calls_recebidas=true;
		}
		else{
			*total_pessoas[i].sub_calls_recebidas=false;
		
		}
	}
	else if(strcmp(buffer,'2')==0){
		if(*total_pessoas[i].sub_calls_feitas==false){
			*total_pessoas[i].sub_calls_feitas=true;
		}
		else{
			*total_pessoas[i].sub_calls_feitas=false
		
		}
	}
	else if(strcmp(buffer,'3')==0){
		if(*total_pessoas[i].sub_calls_perdidas==false){
			*total_pessoas[i].sub_calls_perdidas=true;
		}
		else{
			*total_pessoas[i].sub_calls_perdidas=false;
		
		}
		
	}
	else if(strcmp(buffer,'4')==0){
		if(*total_pessoas[i].sub_calls_duracao==false){
			*total_pessoas[i].sub_calls_duracao=true;
			
		}
		else{
			*total_pessoas[i].sub_calls_duracao=false;
		
		}
		
	}
	else if(strcmp(buffer,'5')==0){
		if(*total_pessoas[i].sub_sms_enviadas==false){
			*total_pessoas[i].sub_sms_enviadas=true;
		}
		else{
			*total_pessoas[i].sub_sms_enviadas=false;
		
		}
		
	}
	else if(strcmp(buffer,'6')==0){
		if(*total_pessoas[i].sub_sms_recebidas==false){
			*total_pessoas[i].sub_sms_recebidas=true;
		}
		else{
			*total_pessoas[i].sub_sms_recebidas=false;
		
		}
		
	}
	else{
		write(client_fd,"Essa opção não existe.",BUF_SIZE);
	}
	}while(strcmp(buffer,'6')!=0 && strcmp(buffer,'5')!=0 && strcmp(buffer,'4')!=0 && strcmp(buffer,'3')!=0 && strcmp(buffer,'2')!=0 && strcmp(buffer,'1')!=0);
}

void erro(char *msg)
{
    printf("Erro: %s\n", msg);
    exit(-1);
}