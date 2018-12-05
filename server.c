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
#include <pthread.h>
#include <signal.h>

//includes do enunciado
#include <json-c/json.h>
#include <curl/curl.h>


#define BUF_SIZE   2048 //tamanho da mensagem
#define SERVER_PORT 9000 //porto do server principal
#define SERVER_PORT2 9001       //porto do server secundário





void process_client(int client_fd);
void erro(char *msg);
void media_grupo(int client_fd);
void subscricoes(int client_fd,int i);
void* notificacoes(void* idp);
void informacoes(int cliente,int client_fd,int i);
void medias(int client_fd,int i);


//struct para as estaticas de grupo
typedef struct{
    double calls_duracao;
    double calls_feitas;
    double calls_perdidas;
    double calls_recebidas;
    double sms_recebidas;
    double sms_enviadas;
}Grupo;


//struct para as informações pessoais
typedef struct{
    char id[BUF_SIZE];
    char atividade[BUF_SIZE];
    char tipo[BUF_SIZE];
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


Client total_pessoas[BUF_SIZE];  //informações pessoais
int nr_clientes;                //nr clientes
int done=0;                     //variavel para manter a thread

Grupo grupo;        //estatisticas de crupo
Grupo ogrupo;       //estatisticas de crupo auxiliar




//CODIGO DA ISABELA------------------------------------------------------------------
struct string {
    char *ptr;
    size_t len;
};

//Write function to write the payload response in the string structure
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

//Initilize the payload string
void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

//Get the Data from the API and return a JSON Object
struct json_object *get_student_data(){
    struct string s;
    struct json_object *jobj;

    //Intialize the CURL request
    CURL *hnd = curl_easy_init();

    //Initilize the char array (string)
    init_string(&s);

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    //To run on department network uncomment this request and comment the other
    //curl_easy_setopt(hnd, CURLOPT_URL, "http://10.3.4.75:9014/v2/entities?options=keyValues&type=student&attrs=activity,calls_duration,calls_made,calls_missed,calls_received,department,location,sms_received,sms_sent&limit=1000");
    //To run from outside
    curl_easy_setopt(hnd, CURLOPT_URL, "http://socialiteorion2.dei.uc.pt:9014/v2/entities?options=keyValues&type=student&attrs=activity,calls_duration,calls_made,calls_missed,calls_received,department,location,sms_received,sms_sent&limit=1000");

    //Add headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "cache-control: no-cache");
    headers = curl_slist_append(headers, "fiware-servicepath: /");
    headers = curl_slist_append(headers, "fiware-service: socialite");

    //Set some options
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc); //Give the write function here
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &s); //Give the char array address here

    //Perform the request
    CURLcode ret = curl_easy_perform(hnd);
    if (ret != CURLE_OK){
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));

        /*jobj will return empty object*/
        jobj = json_tokener_parse(s.ptr);

        /* always cleanup */
        curl_easy_cleanup(hnd);
        return jobj;

    }
    else if (CURLE_OK == ret) {
        jobj = json_tokener_parse(s.ptr);
        free(s.ptr);

        /* always cleanup */
        curl_easy_cleanup(hnd);
        return jobj;
    }
}

//FIM DO CODIGO DA ISABELA------------------------------------------------------------------


//funçao de testes usada inicialmente antes de ter sido implementada a Isabela
void faz_client(){
    strcpy(total_pessoas[0].atividade, "Estudando");
    strcpy(total_pessoas[0].tipo,"Estudante");
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
    strcpy(total_pessoas[0].tipo,"Estudante");
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

//cria a função de frupo auxiliar
void faz_ogrupo(){
    for(int i=0;i<nr_clientes;i++){
        ogrupo.calls_duracao+=total_pessoas[i].call_feitas;
        ogrupo.calls_feitas+=total_pessoas[i].call_duracao;
        ogrupo.calls_perdidas+=total_pessoas[i].call_perdidas;
        ogrupo.calls_recebidas+=total_pessoas[i].call_recebidas;
        ogrupo.sms_enviadas+=total_pessoas[i].sms_recebidas;
        ogrupo.sms_recebidas+=total_pessoas[i].sms_enviadas;
    }
    //Media
    ogrupo.calls_duracao=ogrupo.calls_duracao/nr_clientes;
    ogrupo.calls_feitas=ogrupo.calls_feitas/nr_clientes;
    ogrupo.calls_perdidas=ogrupo.calls_perdidas/nr_clientes;
    ogrupo.calls_recebidas=ogrupo.calls_recebidas/nr_clientes;
    ogrupo.sms_enviadas=ogrupo.sms_enviadas/nr_clientes;
    ogrupo.sms_recebidas=ogrupo.sms_recebidas/nr_clientes;
}


//calcula a media
void calcula_media(){
    int i;
    //somar
    for (i=0; i<nr_clientes; i++) {
        grupo.calls_duracao=0;
        grupo.calls_feitas=0;
        grupo.calls_perdidas=0;
        grupo.calls_recebidas=0;
        grupo.sms_enviadas=0;
        grupo.sms_recebidas=0;
    }
    for(i=0;i<nr_clientes;i++){
        grupo.calls_duracao+=total_pessoas[i].call_feitas;
        grupo.calls_feitas+=total_pessoas[i].call_duracao;
        grupo.calls_perdidas+=total_pessoas[i].call_perdidas;
        grupo.calls_recebidas+=total_pessoas[i].call_recebidas;
        grupo.sms_enviadas+=total_pessoas[i].sms_recebidas;
        grupo.sms_recebidas+=total_pessoas[i].sms_enviadas;
    }
    //Media
    grupo.calls_duracao=grupo.calls_duracao/nr_clientes;
    grupo.calls_feitas=grupo.calls_feitas/nr_clientes;
    grupo.calls_perdidas=grupo.calls_perdidas/nr_clientes;
    grupo.calls_recebidas=grupo.calls_recebidas/nr_clientes;
    grupo.sms_enviadas=grupo.sms_enviadas/nr_clientes;
    grupo.sms_recebidas=grupo.sms_recebidas/nr_clientes;
}

//CODIGO DA ISABELA ADPTADO AO NOSSO PROBLEMA-----------------------------------------------------------
void update_data(){
    //JSON obect
    struct json_object *jobj_array, *jobj_obj;
    struct json_object *jobj_object_id, *jobj_object_type, *jobj_object_activity, *jobj_object_location, *jobj_object_callsduration,
    *jobj_object_callsmade, *jobj_object_callsmissed, *jobj_object_callsreceived, *jobj_object_department, *jobj_object_smsreceived, *jobj_object_smssent;
    //enum json_type type = 0;
    int arraylen = 0;
    int i;

    //Get the student data
    jobj_array = get_student_data();

    //Get array length
    arraylen = json_object_array_length(jobj_array);


    //Example of howto retrieve the data
    for (i = 0; i < arraylen; i++) {
        //get the i-th object in jobj_array
        jobj_obj = json_object_array_get_idx(jobj_array, i);

        //get the name attribute in the i-th object
        jobj_object_id = json_object_object_get(jobj_obj, "id");
        jobj_object_type = json_object_object_get(jobj_obj, "type");
        jobj_object_activity = json_object_object_get(jobj_obj, "activity");
        jobj_object_location = json_object_object_get(jobj_obj, "location");
        jobj_object_callsduration = json_object_object_get(jobj_obj, "calls_duration");
        jobj_object_callsmade = json_object_object_get(jobj_obj, "calls_made");
        jobj_object_callsmissed = json_object_object_get(jobj_obj, "calls_missed");
        jobj_object_callsreceived= json_object_object_get(jobj_obj, "calls_received");
        jobj_object_department = json_object_object_get(jobj_obj, "department");
        jobj_object_smsreceived = json_object_object_get(jobj_obj, "sms_received");
        jobj_object_smssent = json_object_object_get(jobj_obj, "sms_sent");



        //total_pessoas[i].id = i+1;  // id mais simples, para testar mais facilmente
        if(jobj_object_callsduration != NULL && jobj_object_callsmade != NULL && jobj_object_callsmissed != NULL && jobj_object_callsreceived != NULL && jobj_object_smsreceived != NULL && jobj_object_smssent != NULL){
            strcpy(total_pessoas[i].id, json_object_get_string(jobj_object_id));
            strcpy(total_pessoas[i].atividade, json_object_get_string(jobj_object_activity));
            strcpy(total_pessoas[i].tipo, json_object_get_string(jobj_object_type));
            strcpy(total_pessoas[i].localizacao, json_object_get_string(jobj_object_location));
            strcpy(total_pessoas[i].departamento, json_object_get_string(jobj_object_department));
            total_pessoas[i].call_duracao = atoi(json_object_get_string(jobj_object_callsduration));
            total_pessoas[i].call_feitas = atoi(json_object_get_string(jobj_object_callsmade));
            total_pessoas[i].call_perdidas = atoi(json_object_get_string(jobj_object_callsmissed));
            total_pessoas[i].call_recebidas = atoi(json_object_get_string(jobj_object_callsreceived));
            total_pessoas[i].sms_recebidas = atoi(json_object_get_string(jobj_object_smsreceived));
            total_pessoas[i].sms_enviadas = atoi(json_object_get_string(jobj_object_smssent));
        }
        else{
            strcpy(total_pessoas[i].id, "-");
            strcpy(total_pessoas[i].tipo, "-");
            strcpy(total_pessoas[i].atividade, "-");
            strcpy(total_pessoas[i].localizacao, "-");
            strcpy(total_pessoas[i].departamento, "-");
            total_pessoas[i].call_duracao = 0;
            total_pessoas[i].call_feitas = 0;
            total_pessoas[i].call_perdidas = 0;
            total_pessoas[i].call_recebidas = 0;
            total_pessoas[i].sms_recebidas = 0;
            total_pessoas[i].sms_enviadas = 0;
        }
        nr_clientes++;
    }
}

//FIM DO CODIGO DA ISABELA------------------------------------------------------------------


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

    //    faz_client();

    //importa os dados dos clientes
    update_data();
    //ciclo para manter o processo a espera de clientes
    while (1) {

        while(waitpid(-1,NULL,WNOHANG)>0);

        client_addr_size = sizeof(client_addr);
        //se receber cliente retorna valor
        client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);

        //recebe valor e faz fork para o cliente
        if (client > 0) {
            if (fork() == 0) {
                close(fd);
                process_client(client);
                exit(0);
            }
            close(client);
        }
    }
    close(fd);
    return 0;
}


//processo para gerir o cliente
void process_client(int client_fd){
    int cliente=-1;
    long nread = 0;
    int i;
    char buffer[BUF_SIZE],aux[BUF_SIZE*4];
    write(client_fd,"\n++++++++++++++++++++++++++++++++++++++++++\n+                                        +\n+   SEJA BEM VINDO AO SERVIDOR ISABELA   +\n+                                        +\n++++++++++++++++++++++++++++++++++++++++++\n\nIntoruza o seu id:",BUF_SIZE); //escreve mensagem de boas vindas e pede id
    nread = read(client_fd, buffer, BUF_SIZE);//le id
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    for(i=0;i<nr_clientes;i++){
        if(strcmp(buffer,total_pessoas[i].id)==0){ //verifica se nome está presente
            cliente=i;
        }
    }
    if(cliente!=-1){
        write(client_fd,"Cliente existe",BUF_SIZE);//escreve resposta sobre id
    }
   else{
        write(client_fd,"\nO id introduzido não existe!\nIntroduza novamente:",BUF_SIZE);
        while(cliente==-1){ //enquanto o id nao existir na base de dados
            nread = read(client_fd, buffer, BUF_SIZE); //le id
            buffer[nread] = '\0';
            printf("%s\n", buffer);
            for(i=0;i<nr_clientes;i++){
                if(strcmp(buffer,total_pessoas[i].id)==0){ //se o id esta presente
                    cliente=i; //posição i do cliente na base de dados
                    write(client_fd,"Cliente existe",BUF_SIZE);
                }
            }
            if(cliente==-1)
                write(client_fd,"\nO id introduzido não existe!\nIntroduza novamente:",BUF_SIZE);
        }
    }
    pthread_t my_thread;
    int id=cliente;
    calcula_media();//Cria a informação do grupo
    faz_ogrupo(); //Cria a cópia que irá ser comparada no thread
    pthread_create(&my_thread,NULL,notificacoes,&id); //cria thread para enviar as notificações
    do{
        //envia menu
        write(client_fd,"\n+++++++++++++++++++++++++++++++++\n+        Escolha a opção        +\n+    1-Informações pessoais     +\n+    2-Media de grupo           +\n+    3-Subscrição               +\n+    4-Sair                     +\n+++++++++++++++++++++++++++++++++\n",BUF_SIZE);
        nread = read(client_fd, buffer, BUF_SIZE); //le o que o cliente pretende fazer
        buffer[nread] = '\0';
        printf("%s\n", buffer);
        if(strcmp(buffer, "1")==0){ // se quiser informações pessoais
            informacoes(cliente,client_fd,cliente);
        } else if(strcmp(buffer,"2")==0){ // se quiser informações do grupo
            medias(client_fd,cliente);
        } else if(strcmp(buffer,"3")==0){ // se quiser efetuar/alterar subscrições
            subscricoes(client_fd,cliente);
        } else if(strcmp(buffer,"4")==0){ // se quiser sair
            printf("\n");
        }else{ //caso escolha uma opção que não está no menu
            write(client_fd,"\n\n>>ERRO: Deveria ter escolhido 1,2,3 ou 4!",BUF_SIZE);
        }
    }while((strcmp(buffer,"4")!=0)); //enquanto o cliente não pedir para sair

    printf("Fechando conexão com o cliente...\n");
    done=1; //Fechar o thread
    pthread_join(my_thread,NULL); //Esperar que ele termine
    exit(0); // sair do processo que trata do cliente


}

//obtem informações
void informacoes(int cliente,int client_fd,int i){
    char buffer[BUF_SIZE],aux[BUF_SIZE*4];
    long nread=0;

    write(client_fd,"\n+++++++++++++++++++++++++++\n+  1- Actividade          +\n+  2- Localização         +\n+  3- Departamento        +\n+  4- Chamadas feitas     +\n+  5- Duração chamadas    +\n+  6- Chamadas perdidas   +\n+  7- Chamadas recebidas  +\n+  8- SMS recebidas       +\n+  9- SMS enviadas        +\n+  10- Mostrar todas      +\n+  11- Voltar             +\n+++++++++++++++++++++++++++\n",BUF_SIZE);

    nread = read(client_fd, buffer, BUF_SIZE);// le a açãp pretendida pelo cliente
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    update_data();
    if(strcmp(buffer, "1")==0){ //Se quiser saber a sua atividade
        sprintf(aux,"\n\n\n>>>   Actividade: %s   <<<\n",total_pessoas[cliente].atividade);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "2")==0){ //Se quiser saber a sua localização
            sprintf(aux,"\n\n\n>>>   Localização: %s   <<<\n",total_pessoas[cliente].localizacao);
            write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "3")==0){ //Se quiser saber o seu departamento
        sprintf(aux,"\n\n\n>>>   Departamento: %s   <<<\n",total_pessoas[cliente].departamento);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "4")==0){ //Se quiser saber as suas chamadas feitas
        sprintf(aux,"\n\n\n>>>   Chamadas feitas: %d   <<<\n",total_pessoas[cliente].call_feitas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "5")==0){ //Se quiser saber a duração das suas chamadas
        sprintf(aux,"\n\n\n>>>   Duração chamadas: %d   <<<\n",total_pessoas[cliente].call_duracao);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "6")==0){ //Se quiser saber as suas chamadas perdidas
        sprintf(aux,"\n\n\n>>>   Chamadas perdidas: %d   <<<\n",total_pessoas[cliente].call_perdidas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "7")==0){ //Se quiser saber as suas chamadas recebidas
        sprintf(aux,"\n\n\n>>>   Chamadas recebidas: %d   <<<\n",total_pessoas[cliente].call_recebidas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "8")==0){ //Se quiser saber os seus SMS recebidos
        sprintf(aux,"\n\n\n>>>   SMS recebidas: %d   <<<\n",total_pessoas[cliente].sms_recebidas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "9")==0){ //Se quiser saber os seus SMS enviados
        sprintf(aux,"\n\n\n>>>   SMS enviadas: %d   <<<\n",total_pessoas[cliente].sms_enviadas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "10")==0){ //Se quiser saber as informações todas
        sprintf(aux,"\n+++++++++++++++++++++++++++++++++++++++++\n+  Actividade: %s           \n+  Localização: %s          \n+  Departamento: %s         \n+  Chamadas feitas: %d      \n+  Duração de chamas: %d    \n+  Chamadas perdidas: %d    \n+  Chamadas recebidas: %d   \n+  SMS recebidas: %d        \n+  SMS envidas: %d          \n+++++++++++++++++++++++++++++++++++++++++",total_pessoas[cliente].atividade,total_pessoas[cliente].localizacao,total_pessoas[cliente].departamento,total_pessoas[cliente].call_feitas,total_pessoas[cliente].call_duracao,total_pessoas[cliente].call_perdidas,total_pessoas[cliente].call_recebidas,total_pessoas[cliente].sms_recebidas,total_pessoas[cliente].sms_enviadas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "11")==0){ //Se pedir para sair
        write(client_fd,"\n", BUF_SIZE);
    } else{ //Se não escolher nenhuma das opções no sub menu
        write(client_fd,"\n\n\n>>>ERRO: Escolha 1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ou 11!\n",BUF_SIZE);
    }
}

//faz as medias
void medias(int client_fd,int i){
    char buffer[BUF_SIZE],aux[BUF_SIZE*4];
    long nread=0;
    write(client_fd,"\n++++++++++++++++++++++++++++++++++++++++++++++++\n+  Selecioe a media que deseja ver:            +\n+  1- Chamadas recebidas                       +\n+  2- Chamdas feitas                           +\n+  3- Chamadas perdidas                        +\n+  4- Duração media de chamada                 +\n+  5- SMS enviados                             +\n+  6- SMS recebidos                            +\n+  7- Ver todas as opções                      +\n+  8- Voltar                                   +\n++++++++++++++++++++++++++++++++++++++++++++++++\n",BUF_SIZE);
    nread = read(client_fd, buffer, BUF_SIZE); //le a ação pretendida pelo cliente
    buffer[nread] = '\0';
    printf("%s\n", buffer);
    update_data();
     if(strcmp(buffer, "1")==0){ //Se quiser a media das chamadas recebidas pelo grupo
        sprintf(aux,"\n\n\n>>>   Media chamadas recebidas: %.2f   <<<\n",grupo.calls_recebidas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "2")==0){ //Se quiser a media das chamadas feitas pelo grupo
        sprintf(aux,"\n\n\n>>>   Media chamadas feitas: %.2f   <<<\n",grupo.calls_feitas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "3")==0){//Se quiser a media das chamadas perdidas pelo grupo
        sprintf(aux,"\n\n\n>>>   Media chamadas perdidas: %.2f   <<<\n",grupo.calls_perdidas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "4")==0){//Se quiser a duração media das chamadas do grupo
        sprintf(aux,"\n\n\n>>>   Duração media de chamada: %.2f   <<<\n",grupo.calls_duracao);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "5")==0){ //Se quiser a media dos SMS enviados pelo grupo
        sprintf(aux,"\n\n\n>>>   SMS enviados: %.2f   <<<\n",grupo.sms_enviadas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "6")==0){ //Se quiser a media dos SMS recebidos pelo grupo
        sprintf(aux,"\n\n\n>>>   SMS recebidos: %.2f   <<<\n",grupo.sms_recebidas);
        write(client_fd, aux, BUF_SIZE);
    } else if(strcmp(buffer, "7")==0){ ///Se quiser a informação toda
        media_grupo(client_fd);
    } else if(strcmp(buffer, "8")==0){ //Se pedir para sair do sub menu
        write(client_fd,"\n", BUF_SIZE);
    } else{//Se nao escolher nenhuma das opçoes do sub menu
        write(client_fd,"\n\n\n>>>ERRO: Escolha 1 ,2 ,3 ,4 ,5 ,6 ,7 ou 8!\n",BUF_SIZE);
    }
}


//Media do grupo
void media_grupo(int client_fd){

    //Passar para String
    char buff[BUF_SIZE];
    calcula_media();

    sprintf(buff, "\n++++++++++++++++++++++++++++++++++++\n+  Media chamadas recebidas: %.2f  +\n+  Media chamadas feitas: %.2f     +\n+  Media chamadas perdidas: %.2f   +\n+  Duração media de chamada: %.2f  +\n+  SMS enviadas: %.2f              +\n+  SMS recebidas: %.2f             +\n++++++++++++++++++++++++++++++++++++",grupo.calls_recebidas,grupo.calls_feitas,grupo.calls_perdidas,grupo.calls_duracao,grupo.sms_enviadas,grupo.sms_recebidas);
    //Envia para cliente
    write(client_fd,buff, BUF_SIZE);

}


void subscricoes(int client_fd,int i){
    char buffer[BUF_SIZE];
    long nread=0;

    write(client_fd,"++++++++++++++++++++++++++++++++++++++++++++++++\n+  Selecione a que deseja subscrever/cancelar: +\n+  1- Chamadas recebidas                       +\n+  2- Chamdas feitas                           +\n+  3- Chamadas perdidas                        +\n+  4- Duração media de chamada                 +\n+  5- SMS enviados                             +\n+  6- SMS recebidos                            +\n+  7- Subscrever todas as opções               +\n+  8- Voltar                                   +\n++++++++++++++++++++++++++++++++++++++++++++++++\n",BUF_SIZE);
    nread=read(client_fd,buffer,BUF_SIZE);
    buffer[nread]='\0';
    printf("%s\n", buffer);
    update_data();
    if(strcmp(buffer,"1")==0){ //Se quiser subscrever//cancelar a subscrição às chamadas recebidas
        if(total_pessoas[i].sub_call_recebidas==false){
            total_pessoas[i].sub_call_recebidas=true;
            write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
        } else{
            total_pessoas[i].sub_call_recebidas=false;
            write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);

        }
    } else if(strcmp(buffer,"2")==0){//Se quiser subscrever/cancelar a subscrição às chamadas efetuadas
        if(total_pessoas[i].sub_call_feitas==false){
            total_pessoas[i].sub_call_feitas=true;
            write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
        } else{
            total_pessoas[i].sub_call_feitas=false;
            write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
        }
    } else if(strcmp(buffer,"3")==0){//Se quiser subscrever/cancelar a subscrição às chamadas perdidas
        if(total_pessoas[i].sub_call_perdidas==false){
            total_pessoas[i].sub_call_perdidas=true;
            write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
        } else{
            total_pessoas[i].sub_call_perdidas=false;
            write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
        }
    } else if(strcmp(buffer,"4")==0){//Se quiser subscrever/cancelar a subscrição à duração das chamadas
        if(total_pessoas[i].sub_call_duracao==false){
            total_pessoas[i].sub_call_duracao=true;
            write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
        } else{
            total_pessoas[i].sub_call_duracao=false;
            write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
        }
    } else if(strcmp(buffer,"5")==0){ //Se quiser subscrever/cancelar a subscrição às SMS recebidas
        if(total_pessoas[i].sub_sms_enviadas==false){
            total_pessoas[i].sub_sms_enviadas=true;
            write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
        } else{
            total_pessoas[i].sub_sms_enviadas=false;
            write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
        }
    } else if(strcmp(buffer,"6")==0){ //Se quiser subscrever/cancelar a subscrição às SMS efetuadas
        if(total_pessoas[i].sub_sms_recebidas==false){
            total_pessoas[i].sub_sms_recebidas=true;
            write(client_fd,"\n\n\n>>>   Está agora subscrito.   <<<\n",BUF_SIZE);
        } else{
            total_pessoas[i].sub_sms_recebidas=false;
            write(client_fd,"\n\n\n>>>   Deixou de estar subscrito.   <<<\n",BUF_SIZE);
        }

    }else if(strcmp(buffer,"7")==0){ //Se quiser subscrever a tudo
        if(total_pessoas[i].sub_sms_recebidas==false){
            total_pessoas[i].sub_sms_recebidas=true;
        }if(total_pessoas[i].sub_sms_enviadas==false){
            total_pessoas[i].sub_sms_enviadas=true;
        }if(total_pessoas[i].sub_call_duracao==false){
            total_pessoas[i].sub_call_duracao=true;
        }if(total_pessoas[i].sub_call_perdidas==false){
            total_pessoas[i].sub_call_perdidas=true;
        }if(total_pessoas[i].sub_call_feitas==false){
            total_pessoas[i].sub_call_feitas=true;
        }if(total_pessoas[i].sub_call_recebidas==false){
            total_pessoas[i].sub_call_recebidas=true;
        }
        write(client_fd, "Está agora subscrito a tudo!", BUF_SIZE);

    }else if(strcmp(buffer,"8")==0){ //Se quiser sair do sub menu
        write(client_fd,"\n", BUF_SIZE);
    } else{ //Caso nao escolha nenhuma opção que esta no sub menu
        write(client_fd,"\n\n\n>>>ERRO: Escolha 1,2,3,4,5,6,7 ou 8!\n",BUF_SIZE);
    }

}


//thread para o socket secundario
void* notificacoes(void* idp){
    int fd2;
    int noti_fd=0;
    struct sockaddr_in addr2, client_addr2;
    int client_addr_size;
    int i=*((int*) idp);

    //    cria estrutura
    bzero((void *) &addr2, sizeof(addr2));

    // Configure settings of the server address struct
    // Address family = Internet
    addr2.sin_family = AF_INET;
    //Set IP address to localhost
    addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    //Set port number, using htons function to use proper byte order
    addr2.sin_port = htons(SERVER_PORT2);

    //Create the socket.
    if ( (fd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        erro("na funcao socket");
    //Bind the address struct to the socket
    if ( bind(fd2,(struct sockaddr*)&addr2,sizeof(addr2)) < 0)
        erro("na funcao bind");
    //Listen on the socket, with 5 max connection requests queued
    if( listen(fd2, 5) < 0)
        erro("na funcao listen");
    while(noti_fd<=0){ //Enquanto nao houver conexão tenta aceitar uma
        client_addr_size = sizeof(client_addr2);
        noti_fd=accept(fd2,(struct sockaddr *)&client_addr2,(socklen_t *)&client_addr_size);
    }


    while(done==0){ //Enquanto o cliente nao pedir para sair
        update_data(); //Vai buscar dados à ISABELA
        calcula_media(); //Atualiza a informação do grupo
        if(ogrupo.calls_recebidas!=grupo.calls_recebidas){ //Compara se houve alteração na informação
            ogrupo.calls_recebidas=grupo.calls_recebidas; //Se houver atualiza a informação antiga
            if(total_pessoas[i].sub_call_recebidas==true){ //Se estiver subscrito
                write(noti_fd,"O valor das chamadas recebidas foi alterado!",BUF_SIZE); //Envia notificação
            }
        }
        if(ogrupo.calls_feitas!=grupo.calls_feitas){
            ogrupo.calls_feitas=grupo.calls_feitas;
            if(total_pessoas[i].sub_call_feitas==true){
                write(noti_fd,"O valor das chamadas feitas foi alterado!",BUF_SIZE);
            }
        }
        if(ogrupo.calls_duracao!=grupo.calls_duracao){
            ogrupo.calls_duracao=grupo.calls_duracao;
            if(total_pessoas[i].sub_call_duracao==true){
                write(noti_fd,"O valor da duração das chamadas foi alterado!",BUF_SIZE);
            }
        }
        if(ogrupo.calls_perdidas!=grupo.calls_perdidas){
            ogrupo.calls_perdidas=grupo.calls_perdidas;
            if(total_pessoas[i].sub_call_perdidas==true){
                write(noti_fd,"O valor das chamadas perdidas foi alterado!",BUF_SIZE);
            }
        }
        if(ogrupo.sms_recebidas!=grupo.sms_recebidas){
            ogrupo.sms_recebidas=grupo.sms_recebidas;
            if(total_pessoas[i].sub_sms_recebidas==true){
                write(noti_fd,"O valor das SMS recebidas foi alterado!",BUF_SIZE);
            }
        }
        if(ogrupo.sms_enviadas!=grupo.sms_enviadas){
            ogrupo.sms_enviadas=grupo.sms_enviadas;
            if(total_pessoas[i].sub_sms_enviadas==true){
                write(noti_fd,"O valor das SMS enviadas foi alterado!",BUF_SIZE);
            }
        }
        sleep(10); //De 10 em 10 segundos
    }
    close(noti_fd);
    close(fd2);
    pthread_exit(NULL);
    return NULL;
}

void erro(char *msg)
{
    printf("Erro: %s\n", msg);
    exit(-1);
}




