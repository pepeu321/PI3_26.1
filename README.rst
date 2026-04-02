Desenvolvimento de Sistema de Acionamento e Controle de Velocidade para Motor CC Escovado com PID e Telemetria IOT via Protocolo COAP Aplicado a Esteira Transportadora do Laboratório LPAE
#####################################################################

.. image:: imagens/Esteira.png


.. contents::
   :local:
   :depth: 2



Requisitos
**********

Este projeto foi implementado com os seguintes módulos/softwares/hardwares...

- `XIAO nRF52840 <https://wiki.seeedstudio.com/XIAO_BLE/>`_


Visão geral
***********

Este projeto consiste no desenvolvimento de um sistema de controle de velocidade para uma esteira acionada por um motor DC, utilizando o microcontrolador ESP32. O sistema permite ao usuário definir a velocidade desejada pelo celular, enquanto um sensor realiza a medição da velocidade real do motor. Com base nessas informações, o controlador ajusta o sinal PWM aplicado ao motor, garantindo um controle preciso.

O desenvolvimento foi divido em quatro etapas:

- Etapa 1 (02/04/2026): (Nesta etapa, foi realizado o estudo do microcontrolador ESP32, tipo do sensor que será utilizado, a comparação entre rampa de aceleração linear e rampa em S e o diagrama de blocos do sistema, permitindo a visualização geral do funcionamento do projeto.)
- Etapa 2 (30/04/2026): (breve resumo da etapa)
- Etapa 3 (28/05/2026): (breve resumo da etapa)
- Etapa 4 (09/07/2026): (breve resumo da etapa)

Configuração
*************

Projeto foi implementado com o nRF OpenConnect SDK versão 2.4.x.
Consulte `Configuring your application <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_ para obter informações sobre como alterar a configuração permanente ou temporariamente.

A configuração do perfil do dispositivo é realizada no arquivo de configuração `prj.conf <prj.conf>`_:

- End-Device:

.. code:: C 

  (...)
  CONFIG_ZIGBEE_ROLE_END_DEVICE=y  
  # CONFIG_ZIGBEE_ROLE_ROUTER=y
  // Versão do firmware
  CONFIG_MCUBOOT_IMAGE_VERSION="0.0.3"
  CONFIG_ZIGBEE_FOTA_COMMENT="ruido_zigbee_endpoint"
  # CONFIG_ZIGBEE_FOTA_COMMENT="ruido_zigbee_router"


Interface do usuário
********************

LED 1:
  Pisca enquanto o filtro estiver ativo.

Botão 1:
  Ativa o módulo xyz.


Compilando e executando
***********************

Colocar detalhes na construção da applicação. Exemplo: 

Para compilar o projeto com o Visual Studio Code, siga as etapas listadas na página `How to build an application <https://nrfconnect.github.io/vscode-nrf-connect/get_started/build_app_ncs.html>`_  na documentação da extensão nRF Connect for VS Code.  `Building and programming an application  <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/programming.html#gs-programming>`_ para outros cenários de construção e programação e `Testing and debugging an application <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/testing.html#gs-testing>`_ para obter informações gerais sobre testes e depuração no nRF Connect SDK.

Recomenda-se o uso do J-Link para gravação e/ou depuração.

Testando
========

Após programar o microcontrolador, conclua as etapas a seguir para testá-lo:

1. ...

Montagem
********

Breve descrição da montagem final do projeto.

