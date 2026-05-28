Etapa 3
#######

.. contents::
   :local:
   :depth: 2


Visão geral
***********

Na Etapa 3, foram realizados 

📌 Projeto do controlador PID

📌 Implementação preliminar do controle PID no microcontrolador.

📌 Apresentação do gabinete desenvolvido em software 3D

📌 Layout da placa de potência

📌 Layout da placa de controle

📌 Teste de acionamento do motor

📌 Firmware preliminar com teste de comunicação COAP


Desenvolvimento
***************

Apresentar o desenvolvimento da etapa contendo detalhes de implementação (se houver) de hardware e software. Adicionar pesqusisas realizadas bem como testes realizados.


Projeto do controlador PID
======


Implementação preliminar do controle PID no microcontrolador.
======


Apresentação do gabinete desenvolvido em software 3D
======


Layout da placa de potência
======


Layout da placa de controle
======


Teste de acionamento do motor
======


Firmware preliminar com teste de comunicação COAP
======

.. code-block:: c

    #include <stdio.h>
    #include <string.h>


    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"

    #include "esp_wifi.h"     //controla wifi
    #include "esp_event.h"    //sitema de eventos do esp
    #include "esp_log.h"

    #include "lwip/sockets.h" // sockets UDP/TCP
    #include "lwip/netdb.h"
    #include "lwip/inet.h"    // IP e portas

    #include "nvs_flash.h"    //Memória flash usada pelo Wi-Fi
    #include "esp_netif.h"    //Camada de interface de rede

    #include "coap3/coap_forward_decls.h"
    #include "coap3/coap.h" //biblioteca COAP


    #define WIFI_SSID "lpae_wifi"
    #define WIFI_PASS "esp-8266"


    static bool coap_task_started = false;  //flag para ver se a tarefa nao fica se repetindo,criando varias vezes
    static const char *TAG = "WIFI";
    static int velocidade_atual = 0;        //variavel que é o estado atual da esteira



    //função que será chamada AUTOMATICAMENTE quando chegar: POST /vel
    //deixa o esp como servidor COAP, capaz de receber comando externo
    void hnd_post_vel(
        coap_resource_t *resource, //recurso coap acessado (/vel)
        coap_session_t *session,   //sessao COAP de quem enviou a mensagem
        const coap_pdu_t *request, //Pacote que chega no esp
        const coap_string_t *query,
        coap_pdu_t *response)      //Pacote de resposta que o esp vai devolver
    {
        size_t size;     //numero de bytes que chega
        uint8_t *data;   //ponteiro para os bytes que chegam

    //extrai payload recebido.
        if(coap_get_data(request, &size, &data))
        {
            ESP_LOGI(TAG, "Payload recebido no POST");

         //   printf("%.*s\n", (int)size, data);   //imprimi payload
         char buffer[20];   //cria string temporaria

        memcpy(buffer, data, size); //copia os bytes recebidos
        buffer[size] = '\0';        //transforma os bytes em string valida terminada em \0, para usar o atoi

        velocidade_atual = atoi(buffer); //atoi converte para inteiro

        ESP_LOGI(TAG, "Velocidade recebida: %d", velocidade_atual);
        }


        //define o status da resposta CoAP
        coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED); //envia resposta 2.04 Changed/resposta do servidor e que foi alterado alguma coisa
        const char *resp_str = "OK"; //esp respondendo ao servidor que payload foi recebido

        //coloca dentro do pacote de resposta
        coap_add_data(
            response,
            strlen(resp_str),
            (const uint8_t *)resp_str);
    }



    void hnd_get_vel(                 //cria handler GET, notebook pode pegar velocidade atual
        coap_resource_t *resource,    //recurso logico /vel
        coap_session_t *session,
        const coap_pdu_t *request,
        const coap_string_t *query,
        coap_pdu_t *response)
    {
        char resp[50];

    //monta a string "dinamica"
        sprintf(resp,
                "Velocidade atual: 8");

    //define o status da resposta CoAP
        coap_pdu_set_code(
            response,
            COAP_RESPONSE_CODE_CONTENT);

    //coloca dentro do pacote de resposta
        coap_add_data(
            response,
            strlen(resp),
            (const uint8_t *)resp);

        ESP_LOGI(TAG, "GET /vel respondido");
    }



    void coap_client_task(void *pvParameters)
    {
        ESP_LOGI(TAG, "CoAP task iniciou");

        coap_startup(); //inicializa biblioteca coap (libcoap)

        ESP_LOGI(TAG, "Biblioteca CoAP inicializada");


    //estrutura necessaria para contexto COAP(gerenciador de sessões,controle de sockets CoAP)
        coap_context_t *ctx = coap_new_context(NULL); //ctx eh "Sistema operacional do COAP"
        if(ctx == NULL)  //checa se criou contexto
        {
            ESP_LOGE(TAG, "Erro ao criar contexto CoAP");
            vTaskDelete(NULL);
            return;
        }
        ESP_LOGI(TAG, "Contexto CoAP criado");



    //Criacao do servidor
        coap_address_t serv_addr;  //cria estrutura do endereço do servidor.
        coap_address_init(&serv_addr);   //zera/inicializa estrutura.
        serv_addr.addr.sin.sin_family = AF_INET;  //usa IPv4.
        serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;  // INADDR_ANY significa escutar qualquer IP do ESP
        serv_addr.addr.sin.sin_port = htons(5683); //abre porta padrão CoAP(5683):




        coap_endpoint_t *endpoint;  //cria o endpoint servidor CoAP/abre o socket servidor.
        endpoint = coap_new_endpoint(  //abre o “socket CoAP servidor”.
            ctx,               //contexto CoAP criado antes.
            &serv_addr,   //serv_addr é a estrutura do endereço do servidor
            COAP_PROTO_UDP);       // CoAP usa UDP.

            if(endpoint == NULL)  //testa se endpoint foi criado
        {
            ESP_LOGE(TAG, "Erro ao criar endpoint CoAP");
            vTaskDelete(NULL);
            return;
        }
        ESP_LOGI(TAG, "Endpoint servidor CoAP criado");



        //Cria recurso = "endpoint logico"
        coap_resource_t *resource;
        resource = coap_resource_init(
            coap_make_str_const("vel"), //cria endpoint vel
            0);



        coap_register_handler(  //Liga POST ao handler
    //quando chegar POST /vel  chama hnd_post_vel())
            resource,
            COAP_REQUEST_POST,
            hnd_post_vel);


        coap_register_handler(  //Liga GET ao handler
            resource,
            COAP_REQUEST_GET,
            hnd_get_vel);

        //adiciona recurso ao servidor, servidor COAP conhece "/vel "
        coap_add_resource(ctx, resource);  //Adiciona ao servidor(endpoint conhece /vel)
        ESP_LOGI(TAG, "Recurso /vel criado");



        while (1)
        {
           coap_io_process(ctx, COAP_IO_WAIT); //coap_io_process é o que faz tudo rodar
           vTaskDelay(pdMS_TO_TICKS(100));
        }
    }


    static void wifi_event_handler(             //reage a eventos do Wi-Fi.
        void *arg,
        esp_event_base_t event_base,            //qual “categoria” do evento aconteceu,IP ou WIFI.
        int32_t event_id,                       //qual evento específico aconteceu
        void *event_data)                       //dados extras do evento(IO,mascara,gateway)

    {
        // verifica se Wi-Fi iniciou com WIFI_EVENT_STA_START
        if(event_base == WIFI_EVENT &&
           event_id == WIFI_EVENT_STA_START) //quando esp liga, gera o envento WIFI_EVENT_STA_START
        {
            esp_wifi_connect();     //manda o ESP conectar no roteador.
        }



        // Conectou e recebeu IP com IP_EVENT_STA_GOT_IP
        if(event_base == IP_EVENT &&
           event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *event =
                (ip_event_got_ip_t *) event_data;        //pega IP recebido

            ESP_LOGI(TAG,
                     "IP: " IPSTR,
                     IP2STR(&event->ip_info.ip));

            if(!coap_task_started)           //garante que tarefa COAP nao seja criada varias vezes, caso a internet caia,troca IP,etc
            {
                coap_task_started = true;

                xTaskCreate(                     // cria tarefa do coap_client_task
                    coap_client_task,
                    "coap_client",
                    8192,
                    NULL,
                    5,
                    NULL);
            }
        }





        // Desconectou
        if(event_base == WIFI_EVENT &&
           event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            ESP_LOGI(TAG, "Reconectando...");
            esp_wifi_connect();
        }
    }


    void wifi_init_sta(void)    //função que monta toda infraestrutura de rede.
    {
        // Inicializa pilha de rede
        esp_netif_init();

        // Event loop, cria sistema de enventos
        esp_event_loop_create_default();

        // Cria a interface Wi-Fi no modo station
        esp_netif_create_default_wifi_sta();

        //Cria estrutura de configuração padrão do driver Wi-Fi
        wifi_init_config_t cfg =
            WIFI_INIT_CONFIG_DEFAULT();

        esp_wifi_init(&cfg);   //Inicializa driver Wi-Fi do esp

        // Registra eventos de wi-fi, qunado acontece alguma evento WIFI, chama função do evento
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            NULL);


        // Registra eventos de IP
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL,
            NULL);

        // config do wifi
        wifi_config_t wifi_config = {
            .sta = {
                .ssid = WIFI_SSID,
                .password = WIFI_PASS
            },
        };

        esp_wifi_set_mode(WIFI_MODE_STA);  //Modo estação

        esp_wifi_set_config(                   //Aplica config
            WIFI_IF_STA,
            &wifi_config);

        esp_wifi_start();         //inicia wifi
    }


    void app_main(void)
    {
        // Inicializa NVS
        nvs_flash_init();      //Inicializa NVS(memória flash usada pelo Wi-Fi

        // Inicializa Wi-Fi
        wifi_init_sta();
    }

	   


Nesta etapa do projeto foi desenvolvido um sistema de comunicação utilizando o protocolo CoAP com objetivo de fazer com que o microcontrolador envie e receba mensagens do servidor, que posteriormente será adptado, para conseguir acompanhar a velocidade atual da esteira e setar uma nova velocidade desejada. 

Inicialmente foi feita a configuração da conexão wi-fi em modo Station (WIFI_MODE_STA). Foram utilizados os módulos: esp_wifi, esp_event e esp_netif. A conexão foi feita usando os eventos do sistema, fazendo com que o esp: inicializasse automaticamente o wifi, conectasse ao roteador, pega o endereço IP, e caso perca o sinal, seja capaz de se reconectar.

Com o esp conectado, apereceu no terminal o IP obtido, comprovando a conexão.

Antes de implementar o COAP em si, foi feito um teste usando comunicação UDP pura, onde foi criado uma tarefa,  responsável pela criação do socket UDP, e envio de uma mensagem para um notebook conectado na mesma rede, com o objetivo de compreender melhor o protocolo, e validar a comunicação wifi e o envio de dados usando sockets UDP. A mensagem foi enviada usando a função "sendto()"

Com essa comunicação validada, foi adicionada a bbilbioteca "libcoap", que é a responsável pela implementação do protocolo. A implementação foi feita através do arquivo "idf_component.yml" e adicionada na pasta do projeto. 

Com a biblioteca adicionada, foi feito a criação da tarefa principal "coap_client_task()", responsável por inicializar a biblioteca CoAP utilizando coap_startup() e criar o contexto principal do protocolo pela da função coap_new_context(NULL). Esse contexto é o núcelo da comunicação COAP, responsável pelo gerenciamento de sessões, sockets e processar as mensagens.

Em seguida foi implementado um servidor CoAP local dentro do ESP32-S3. Para isso foi criada uma estrutura de endereço utilizando coap_address_t, configurada para utilizar IPv4 (AF_INET), protocolo UDP e a porta padrão do CoAP (5683). O servidor foi então criado utilizando a função coap_new_endpoint(), permitindo que outros dispositivos da rede enviassem requisições diretamente para o ESP.

Com o servidor criado, foi implementado o recurso "/vel", que vai representar uma URI responsável pelo controle e monitoramento da velocidade da esteira. Foi criado com coap_resource_init() e adicionado ao contexto COAP, pela função coap_add_resource().

Para tratar as requisições foi implementado dois handlers: hnd_post_vel() e hnd_get_vel().

A função post, é exectuada quando o esp receber uma mensagem no recusro /vel, ela vai extrair o payload enviado pelo cliente, extrair os dados com coap_get_data() , converter os bytes em string e transformar em inteiro. Após receber e processar o valor, o esp envia uma resposta ao client com "OK", indicando o recebimento da mensagem.  

Para hnd_get_value, foi implementada para que quando o notebook realiza um GET no recurso /vel, o ESP monta uma string contendo o estado atual da variável velocidade e envia essa informação de volta ao cliente CoAP

Para manter o funcionamento continuo foi feito um laço com a função coap_io_process(ctx, COAP_IO_WAIT), essa função é responsável por processar continuamente os eventos da pilha CoAP, como recebimento de requisições, envio de respostas e gerenciamento do protocolo.

Para validar o funcionamento, foi criado um cliente CoAP em Python utilizando a biblioteca aiocoap. O programa em Python atua como cliente da aplicação, permitindo enviar requisições POST para alterar a velocidade da esteira e requisições GET para consultar o valor atual armazenado no ESP32-S3.

.. code-block:: python

   from aiocoap import *    #implementa o protocolo CoAP em Python
   import asyncio		 #sistema assíncrono do Python

   async def main():        #cria funcao assincrona, para usar o await

   protocol = await Context.create_client_context()    #cria o cliente CoAP


   while True:

   #Post
        vel = input("Digite a velocidade: ")

        request = Message(      #cria uma requisição Post
            code=POST,		#defini o tipo(post)
            payload=vel.encode(),  #converte string em bytes
            uri="coap://192.168.1.119/vel"   #destino
        )
	
	#Envia requisição e espera resposta do esp com ok
        response = await protocol.request(request).response

        print("Resposta do ESP que recebeu:")
        print(response.payload.decode())  #converte dnv bytes em string


   #GET
        request_get = Message(     #cria uma requisição GET
            code=GET,
            uri="coap://192.168.1.119/vel"
        )

        response_get = await protocol.request(request_get).response

	            
        print("Resposta GET:")
        print(response_get.payload.decode())

    asyncio.run(main())


No cliente Python, o usuário digita a velocidade desejada pelo prompt. O valor é convertido para bytes e enviado ao ESP através de uma requisição POST direcionada para a URI coap://IP_DO_ESP/vel. 
Em seguida, o programa Python realiza automaticamente uma requisição GET para o mesmo recurso /vel, recebendo do ESP a velocidade atual e imprimindo no mesmo prompt de comando

.. image:: Imagens/PromptCOAP.png
   :width: 600px
   :align: center

.. image:: Imagens/TerminalCOAP.png
   :width: 600px
   :align: center






Testes
======

Descrição dos testes/validações realizadas.


(Outras subseções se necessário)
================================


Referências (links/datasheets/livros)
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


