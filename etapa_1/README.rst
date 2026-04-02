Etapa 1
#######

- Visão Geral
- Desenvolvimento
      - Diagrama de blocos do sistema 
      - Estudo e configuração do ESP IDF para aplicar no projeto
      - Definição do sensor de efeito hall
      - Rampa de aceleração linear ou rampa em S
- Referências(links/datasheets/livros)


Visão geral
***********

A etapa 1 consiste numa fase de estudo para o desenvolvimento do projeto. As atividades a serem desenvolvidas nesta etapa são:

📌 Estudo e configuração do ESP IDF para aplicar no projeto

📌 Definição do sensor de efeito hall

📌 Rampa de aceleração linear ou rampa em S

📌 Diagrama de blocos do sistema 



Desenvolvimento
***************

Apresentar o desenvolvimento da etapa contendo detalhes de implementação (se houver) de hardware e software. Adicionar pesqusisas realizadas bem como testes realizados.


1. Diagrama de blocos do sistema
======

Essa tarefa está apresentada primeiro porque apresenta uma visão geral do funcionamento do projeto quando ficar pronto.


2. Estudo e configuração do ESP IDF para aplicar no projeto
=======

Editar

3. Definição do sensor de efeito hall


Essa atividade tem por objetivo definir o sensor de velocidade do motor. Desse modo o microcontrolador terá o rpm do motor e decidirá se é necessário aumentar ou diminuir a corrente do driver do motor para que o motor gire na rotação pré-determinada.

Inicialmente iríamos utilizar o encoder as5600, que utiliza o efeito hall, e por isso o nome desta atividade ficou definição do sensor de efeito hall. Porém o mais indicado para o nosso uso é o encoder óptico de arduino.

###################### imagem

![Encoder](https://raw.githubusercontent.com/pepeu321/PI3_26.1/main/etapa_1/encoders2.jpg)

O encoder as5600 utiliza vários sensores de efeito hall para gerar uma posição angular do campo magnético. Então um ímã em determinada posição gerará um equivalente de tensão que é então convertido para um número binário com resolução de 12 bits e isso indicará a posição em graus que o ímã(fonte geradora de campo magnético) está.

Abaixo está um gif do funcionamento desse sensor:

##################### gif as5600

![alternativol](https://github.com/pepeu321/PI3_26.1/blob/main/etapa_1/imagens/gif_as5600.gif)

![Encoder](https://raw.githubusercontent.com/pepeu321/PI3_26.1/main/etapa_1/imagens/gif_as5600.gif)

4. Rampa de aceleração linear ou rampa em S
=======

Editar


Referências (links/datasheets/livros)
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


