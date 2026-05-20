#include <stdio.h>
#include <string.h>
#include "coap3/coap_forward_decls.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/sockets.h" // sockets UDP/TCP
#include "lwip/netdb.h"
#include "lwip/inet.h"    // IP e portas

#include "nvs_flash.h"
#include "esp_netif.h"

#include "coap3/coap.h" //biblioteca COAP


#define WIFI_SSID "Condominio"
#define WIFI_PASS "condominio18"


static bool coap_task_started = false;  //flag para ver se a tarefa nao fica se repetindo
static const char *TAG = "WIFI";


static coap_response_t response_handler(     //função que vai ser chamada quando coap.me responder ao GET /test
    coap_session_t *session,
    const coap_pdu_t *sent,
    const coap_pdu_t *received,
    const coap_mid_t id)
{
    ESP_LOGI(TAG, "Resposta CoAP recebida");
    
    unsigned char *data;
	size_t data_len;
	
	if(coap_get_data(received, &data_len, &data)) 
	//coap_get_data-Extrai o payload da resposta CoAP.
	//data_len- Quantidade de bytes recebidos.
	//data- Ponteiro para os bytes do payload.
{
    ESP_LOGI(TAG, "Payload recebido:");
    printf("%.*s\n", (int)data_len, data);//imprime tamanho correto da string.
}
    

    return COAP_RESPONSE_OK;
}


void hnd_post_vel(					//função que será chamada AUTOMATICAMENTE quando chegar: POST /vel
    coap_resource_t *resource,
    coap_session_t *session,
    const coap_pdu_t *request,
    const coap_string_t *query,
    coap_pdu_t *response)
{
    size_t size;
    uint8_t *data;

    if(coap_get_data(request, &size, &data))  //extrai payload recebido.
    {
        ESP_LOGI(TAG, "Payload recebido no POST:");

        printf("%.*s\n", (int)size, data);   //imprimi payload
    }
    
    
	coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED); //envia resposta 2.04 Changed
	const char *resp_str = "OK";
	
	coap_add_data(
	    response,
	    strlen(resp_str),
	    (const uint8_t *)resp_str);
    
}






void coap_client_task(void *pvParameters)
{
    ESP_LOGI(TAG, "CoAP task iniciou");

    coap_startup(); //inicializa coap
    
    ESP_LOGI(TAG, "Biblioteca CoAP inicializada");

	coap_context_t *ctx = coap_new_context(NULL); //estrutura necessaria para COAP(gerenciador de sessões,controle de sockets CoAP)

	if(ctx == NULL)  //checa se criou contexto
    {
        ESP_LOGE(TAG, "Erro ao criar contexto CoAP");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Contexto CoAP criado");
    
    coap_register_response_handler(    //registra o handler na libcoap(usa essa função quando chega resposta)
    ctx,
    response_handler);
    
    
    
    
    coap_address_t serv_addr;  //cria estrutura do endereço do servidor.

	coap_address_init(&serv_addr);   //zera/inicializa estrutura.
	
	serv_addr.addr.sin.sin_family = AF_INET;  //usa IPv4.
	
	serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;  // INADDR_ANY significa escutar qualquer IP do ESP
	
	serv_addr.addr.sin.sin_port = htons(5683); //abre porta padrão CoAP(5683):
    
    
    
    coap_endpoint_t *endpoint;  //cria o endpoint servidor CoAP.

	endpoint = coap_new_endpoint(  //abre o “socket CoAP servidor”.
	    ctx,  			   //contexto CoAP criado antes.
	    &serv_addr,   //serv_addr é o endereço local(0.0.0.0:5683)
	    COAP_PROTO_UDP);	   // CoAP usa UDP.
	    
	    if(endpoint == NULL)  //testa se endpoint foi criado
	{
	    ESP_LOGE(TAG, "Erro ao criar endpoint CoAP");
	    vTaskDelete(NULL);
	    return;
	}
	
	ESP_LOGI(TAG, "Endpoint servidor CoAP criado");
    
    
    
    
    
    
    coap_resource_t *resource;

	resource = coap_resource_init( 					//Cria recurso /vel
	    coap_make_str_const("vel"),
	    0);
	
	coap_register_handler(	 //Liga POST ao handler
//quando chegar POST /vel  chama hnd_post_vel())
	    resource,
	    COAP_REQUEST_POST,
	    hnd_post_vel);
	 
	coap_add_resource(ctx, resource);  //Adiciona ao servidor(endpoint conhece /vel)
	
	ESP_LOGI(TAG, "Recurso /vel criado");
    
    
    
    
    
    
    
    
	coap_address_t dst;
	coap_address_init(&dst);  // Cria estrutura de endereço CoAP(parecido com sockaddr_in).
	dst.addr.sin.sin_family = AF_INET;
	dst.addr.sin.sin_port = htons(5683); //porta padrao COAP
	dst.addr.sin.sin_addr.s_addr = inet_addr("134.102.218.18"); //ip do coap.me
	
	coap_session_t *session; //cria sessão 
	
	session = coap_new_client_session(
    ctx,
    NULL,
    &dst,
    COAP_PROTO_UDP);

	if(session == NULL)  //checa se criou sessao
{
    ESP_LOGE(TAG, "Erro ao criar sessao CoAP");
    vTaskDelete(NULL);
    return;   
}
	ESP_LOGI(TAG, "sessao CoAP criada");

	//Metodo get
	//coap_pdu_t *pdu;     //cria pacote PDU(Protocol Data Unit)
	//pdu = coap_pdu_init(
    //COAP_MESSAGE_CON,   //servidor deve responder ACK
    //COAP_REQUEST_CODE_GET,  //metodo get
    //coap_new_message_id(session), //Cria ID único do pacote.
    //coap_session_max_pdu_size(session)); //Tamanho máximo do pacote.
    
    
    //metodo post
	coap_pdu_t *pdu;     //cria pacote PDU(Protocol Data Unit)
	pdu = coap_pdu_init(
    COAP_MESSAGE_CON,
    COAP_REQUEST_CODE_POST,
    coap_new_message_id(session),
    coap_session_max_pdu_size(session));
    
    ESP_LOGI(TAG, "Pacote POST criado");
    
    
    
    if(pdu == NULL)  //checa se criou certo
{
    ESP_LOGE(TAG, "Erro ao criar PDU");
    vTaskDelete(NULL);
    return;
}
	//ESP_LOGI(TAG, "Pacote GET criado");
	
	
	coap_add_option(   			//Adiciona ao pacote:URI_PATH = test (equivale a coap://coap.me/test)
    pdu,
    COAP_OPTION_URI_PATH,
    strlen("test"),
    (const uint8_t *)"test");
	ESP_LOGI(TAG, "URI /test adicionada");

	
	char payload[] = "Hello from ESP32 POST";
	coap_add_data(
	    pdu,
	    strlen(payload),
	    (const uint8_t *)payload);
	ESP_LOGI(TAG, "Payload POST adicionado");
	
	
	coap_send(session, pdu); 
//pega o pacote (pdu)
//usa a sessão (session)
//envia via UDP para o servidor CoAP
	ESP_LOGI(TAG, "Requisicao CoAP enviada");
	
	coap_io_process(ctx, COAP_IO_WAIT); //o nome da variável do contexto criado precisa bater.
	//COAP_IO_WAIT fica esperando eventos de rede
	
	
    while (1)
    {
	   coap_io_process(ctx, COAP_IO_WAIT);
       vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void udp_client_task(void *pvParameters)  //tarefa paralela do wifi  ; cria um socket UDP e envia uma mensagem
{
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);  //Criação do socket

	if(sock < 0)    // Ver se pocket foi criado corretamente
    {
        ESP_LOGE(TAG, "Erro ao criar socket"); //E - ERRO

        vTaskDelete(NULL);  //Finaliza tarefa
        return;
    }

    ESP_LOGI(TAG, "Socket UDP criado");
    
    struct sockaddr_in dest_addr; //cria struct para guardar destino( ip,porta,tipo de endereço(IPv4,IPv3...))
    
    dest_addr.sin_addr.s_addr =
    inet_addr("192.168.1.129");  // IP destino = notebook
    
    dest_addr.sin_family = AF_INET;
    
    dest_addr.sin_port = htons(3333);
    
    char payload[] = "Hello UDP from ESP32";
    

ESP_LOGI(TAG, "Payload POST adicionado");
    
    int err = sendto(           //Envio UDP
        sock,
        payload,
        strlen(payload),
        0,
        (struct sockaddr *)&dest_addr,
        sizeof(dest_addr));

    if(err < 0)                //checa envio
    {
        ESP_LOGE(TAG, "Erro ao enviar UDP");
    }
    else
    {
        ESP_LOGI(TAG, "Mensagem UDP enviada");
    }

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

	
static void wifi_event_handler(   			//reage a eventos do Wi-Fi.
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    // Wi-Fi iniciou
    if(event_base == WIFI_EVENT &&
       event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    // Conectou e recebeu IP
    if(event_base == IP_EVENT &&
       event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *) event_data;        //pega IP recebido

        ESP_LOGI(TAG,
                 "IP: " IPSTR,
                 IP2STR(&event->ip_info.ip));

        if(!coap_task_started)           //garante que tarefa seja criada apenas uma vez
        {
            coap_task_started = true;

            xTaskCreate(
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
    // Inicializa rede
    esp_netif_init();  

    // Event loop
    esp_event_loop_create_default();

    // Interface station
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT(); 

    esp_wifi_init(&cfg);   //Inicializa driver Wi-Fi

    // Registra eventos de wi-fi 
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

    esp_wifi_set_config( 						//Aplica config
        WIFI_IF_STA,
        &wifi_config);

    esp_wifi_start();			//inicia wifi
}


void app_main(void)
{
    // Inicializa NVS
    nvs_flash_init();      //Inicializa NVS(memória flash usada pelo Wi-Fi

    // Inicializa Wi-Fi
    wifi_init_sta();
}