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

Nesta etapa do projeto foi desenvolvido um sistema de comunicação utilizando o protocolo CoAP com objetivo de fazer com que o microcontrolador envie e receba mensagens do servidor. 

Inicialmente foi feita a configuração da conexão wi-fi em modo Station (WIFI_MODE_STA). Foram utilizados os módulos: esp_wifi, esp_event e esp_netif. A conexão foi feita usando os eventos do sistema, fazendo com que o esp: inicializasse automaticamente o wifi, conectasse ao roteador, pega o endereço IP, e caso perca o sinal, seja capaz de se reconectar.

Com o esp conectado, apereceu no terminal o IP obtido, comprovando a conexão.

Antes de implementar o COAP em si, foi feito um teste usando comunicação UDP pura, onde foi criado uma tarefa,  responsável pela criação do socket UDP, e envio de uma mensagem para um notebook conectado na mesma rede, com o objetivo de compreender melhor o protocolo, e validar a comunicação wifi e o envio de dados usando sockets UDP. A mensagem foi enviada usando a função "sendto()"

Com essa comunicação validada, foi adicionada a bbilbioteca "libcoap", que é a responsável pela implementação do protocolo. A implementação foi feita através do arquivo "idf_component.yml" e adicionada na pasta do projeto. 

Com a biblioteca adicionada, foi feito a criação da tarefa principal do cliente COAP, responsável por inicializar a biblioteca, criar o contexo COAP, estabelecer sessões e enviar requisições ao servidor externo. A inicialização da pilha CoAP foi realizada com "coap_startup()", enquanto o contexto principal foi criado utilizando "coap_new_context(NULL)"

Posteriormente foi estabelecida uma sessão cliente UDP direcionada ao servidor público coap.me, utilizando a porta padrão CoAP 5683. Essa sessão passou a permitir o envio de requisições CoAP diretamente para servidores externos.

O primeiro método implementado foi o "GET", foi criado um pacote PDU (Protocolo Data Unit), utlizando "coap_pdu_init()", configurado com uma requisição confirmável "COAP_MESSAGE_CON" do tipo GET "COAP_REQUEST_CODE_GET". Em seguida foi adicionada a URI "/test" através da função "coap_add_option()".

Após o GET, foi implementado o método "post". O pacote COAP foi modificado para utilizar COAP_REQUEST_CODE_POST, e um payload contendo a string "Hello from ESP32 POST" foi adicionado ao pacote através da função coap_add_data().
A requisição POST foi enviada ao servidor coap.me, que respondeu com a mensagem "POST OK", confirmando que o ESP32-S3 já conseguia enviar dados via COAP, usando um payload definido.

Com o cliente COAP implementado, foi feito um servidor COAP no esp-32 para que consiga receber mensaagens também.
Foi criado um endpoint local utilizando "coap_new_endpoint()", fazendo com que outros dispositivos da rede enviassem requisições diretamente ao ESP.

Em seguida foi criado o recurso "/vel", representando uma URI responsável pelo recebimento de comandos externos relacionados à velocidade. Esse recurso foi registrado utilizando coap_resource_init() e coap_add_resource().

Para tratar as requisições recebidas foi implementada a função hnd_post_vel(), responsável por: receber requisições POST, extrair o payload enviado imprimir os dados recebidos no terminal e enviar respostas ao cliente CoAP.

O payload recebido era extraído com coap_get_data(), permitindo interpretar mensagens externas enviadas ao micro.

Por fim, para validar o funcionamento do servido COAP, foi feito um cliente externo em Python usando a bilbioteca "aiocoap". Onde é enviado requisições post, para o recuso "/vel", com valore no payload de "100"

Ao receber a requisição, era imprimido no terminal e respondia o cliente com "Ok"
Imagem








Testes
======

Descrição dos testes/validações realizadas.


(Outras subseções se necessário)
================================


Referências (links/datasheets/livros)
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


