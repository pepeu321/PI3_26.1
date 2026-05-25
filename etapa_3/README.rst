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

.. code-block:: vhdl

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


.. image:: 

.. image:: 







Testes
======

Descrição dos testes/validações realizadas.


(Outras subseções se necessário)
================================


Referências (links/datasheets/livros)
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


