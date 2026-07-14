Etapa 4
#######

.. contents:: Sumário
   :local:
   :depth: 3


1. Visão geral
**************

A quarta e última etapa deste projeto consolida o desenvolvimento do sistema, resultando na entrega de um protótipo físico e operacional em estado alfa. Esta fase materializa as características e funcionalidades idealizadas no escopo inicial do projeto. Esta etapa compreende:

📌 Confecção da placa de circuito impresso (PCI)

📌 Manufatura aditiva do gabinete via impressão 3D

📌 Apresentação do Sistema Acomodado em Gabinete

📌 Teste de Sobrecarga e Acionamento

📌 Teste de controle de velocidade remoto do sistema

📌Comparação entre resultados obtidos e esperados

É inerente dizer que equívocos e imperícias ocorreram ao longo do desenvolvimento do projeto e que soluções alternativas foram incorporadas para atingir o objetivo final e o sucesso do trabalho.

2. Desenvolvimento
******************

2.1 Fabricação da PCI
====================================================================================

2.1.1 Problemática
------------------

O processo de soldagem dos drivers SMD BTN8982TA constituiu o primeiro desafio na montagem da PCI. Inicialmente, a baixa aderência do terminal de potência — que atua também como base de dissipação térmica — exigiu a aplicação de um volume excessivo de solda para assegurar o contato elétrico.
Durante a soldagem individual dos demais pinos, realizada sob aplicação de fluxo, ocorreu um curto-circuito entre os pinos 6 (IS) e 7 (VCC). Como a remoção do curto-circuito não foi bem-sucedida, os componentes foram removidos para a realização de uma nova tentativa de soldagem. Ressalta-se que o layout da PCI não previu a implementação de vias de alívio térmico sob o pino de potência. A Figura 1 ilustra o relato da primeira soldagem realizada.

.. image:: Prob2.jpeg
   :width: 800px
   :align: center
*Figura 1  – Tentativa falha de solda*

Com os componentes posicionados e pressionados por meio de uma pinça, a placa foi colocada na mesa de solda a uma temperatura de até 320 °C. Embora a soldagem inicial tenha sido bem-sucedida, uma inspeção visual detalhada identificou um curto-circuito entre os pinos 1 (GND) e 2 (PWM). Durante a tentativa de correção desse curto diretamente na mesa aquecida, a placa sofreu superaquecimento. A temperatura excedeu o limite suportado pelo substrato de fibra de vidro (temperatura de transição vítrea), resultando na carbonização da placa, como ilustrado na Figura 2.

*Fonte: Autoria própria*

.. image:: Prob1.jpeg
   :width: 800px
   :align: center
*Figura 2  – Excesso de temperatura e carbonização da PCI*

*Fonte: Autoria própria*


2.1.2 Transferência térmica, corrosão e furação
---------------------------

Após a inutilização do primeiro protótipo, viabilizou-se a confecção de uma segunda placa de circuito impresso a partir do zero. Para esta nova tentativa, substituiu-se o substrato original de fibra de vidro (FR-4) por uma placa de fenolite. O procedimento de manufatura manual iniciou-se com a preparação da superfície de cobre, utilizando lã de aço para a remoção da camada oxidada e álcool isopropílico para eliminar resíduos de cola ou outros detritos que pudessem comprometer a aderência na etapa de transferência térmica. Na sequência, a placa foi devidamente medida e cortada com base nas dimensões finais especificadas no projeto.

A transferência térmica foi realizada empregando papel fotográfico (glossy), que apresenta propriedades ideais para a prototipagem manual de PCIs. A impressão em PDF foi configurada conforme as diretrizes recomendadas pela secretaria do DAELN, aplicando o espelhamento correto em uma das faces para viabilizar a sobreposição precisa em dupla face. Utilizando a prensa térmica disponível no LPEE, efetuou-se a transferência de um lado por vez a uma temperatura de 180 °C com duração de 5 minutos, posicionando uma manta térmica sobre o conjunto para distribuir o calor uniformemente e evitar a carbonização do fenolite. Após a prensagem da primeira face, executaram-se até três pequenos furos guia apenas para garantir a centralização e o perfeito alinhamento entre as duas camadas. O posicionamento foi validado transpassando-se pedaços de fio metálico pelas vias e fixando o papel do lado reverso com fita adesiva para a prensagem final. Concluída a transferência de ambas as faces, realizou-se uma inspeção visual detalhada, corrigindo-se pequenas imperfeições no toner com uma caneta permanente para CD/DVD.

Com o circuito desenhado e protegido pelas trilhas de toner, a placa foi submetida ao processo de corrosão química por meio da imersão em uma solução de percloreto de ferro. Essa etapa, que durou cerca de 1 hora, removeu o cobre exposto e preservou apenas as regiões protegidas pela tinta. Finalizado o ataque químico, a placa foi lavada abundantemente em água corrente para neutralizar o agente corrosivo e devidamente secada. Toda a tinta protetiva restante foi removida novamente com lã de aço para expor o cobre limpo. Por fim, o circuito foi levado à furadeira de bancada, onde se utilizou broca de 0,8 mm para os furos de menor diâmetro e broca de 3,0 mm para os furos de fixação e maior diâmetro, deixando a placa de fenolite perfeitamente usinada e pronta para a etapa de soldagem dos componentes eletrônicos.

O processo completo descrito pode ser visto na Figura 3.


.. image:: ProcessoPCI.png
   :width: 800px
   :align: center
*Figura 3  – Transferência térmica, corrosão e furação*

*Fonte: Figura gerada por inteligência artificial*

2.1.3 Solda
-----------

.. image:: PCIsolda.png
   :width: 800px
   :align: center
*Figura 4  – PCI após solda da camada inferior e superior*

*Fonte: Figura com auxílio de inteligência artificial*

2.2 Apresentação do gabinete confeccionado em impressão 3D e sistema final acomodado
====================================================================================

2.2.1 Problemática
------------------

.. image:: AcomodacaoRuim.jpeg
   :width: 800px
   :align: center
*Figura 1  – Incompatibilidade de espaço e posicionamento ruim das PCIs*

*Fonte: Autoria própria*

2.2.2 Processo de impressão
---------------------------

.. image:: Impressora.png
   :width: 800px
   :align: center
*Figura 1  – Impressoras utilizadas para impressão 3D*

*Fonte: Autoria própria*


2.2.3 Alterações técnicas no design do gabinete
-----------------------------------------------


.. image:: Gabinete.png
   :width: 800px
   :align: center
*Figura 1  – Alterações técnicas para melhoria de design do gabinete*

*Fonte: Autoria própria*

.. image:: Case3.jpeg
   :width: 800px
   :align: center
*Figura 1  – Acessórios do gabinete*

*Fonte: Autoria própria*

2.2.4 Apresentação definitiva do gabinete
-----------------------------------------

Resultados do gabinete...


2.2.5 Apresentação definitiva do sistema acomodado
--------------------------------------------------

Resultados do sistema...


2.3 Teste de Sobrecarga e Acionamento do Motor
=======================================================

O teste de acionamento do motor utilizando o driver confeccionado ocorreu com sucesso. Para realizar este teste foi utilizada uma fonte de bancada com duas tensões independentes, 24V e 5V, ambas com limitações de 2A. 

.. image:: Imagens/Fonte_Bancada.jpg
   :width: 400px
   :align: center
*Figura 1  – Configuração da Fonte de Bancada*

*Fonte: Autoria própria*

.. image:: Imagens/Driver.jpg
   :width: 400px
   :align: center
*Figura 1  – Em destaque o Driver*

*Fonte: Autoria própria*

Esse teste foi feito para conferir se o driver estava funcionando corretamente ao ligar na fonte de alimentação. E funcionou, a esteira foi acionada pelo driver, como pode ser visto no gif abaixo:

.. image:: Imagens/acionamento_drive.gif
   :width: 1500px
   :align: center
*Figura 1  – Funcionamento do driver ao ligar na fonte de alimentação*

*Fonte: Autoria própria*

O teste de sobrecarga foi feito adicionando pesos à esteira juntamente com o código de controle e monitoramento remoto da velocidade da esteira, o intuito deste teste foi verificar o comportamento da esteira quando é exigida sua potência máxima. Então foi definida a velocidade máxima de 60 RPM, e com a adição de pesos o que ocorreu foi o aumento da corrente na fonte de alimentação para manter essa velocidade. Como a fonte de alimentação estava com proteção para 2A, funcionou normalmente. Podendo ser visualizado nas 4 imagens seguintes que representam a medição do RPM da esteira sem peso e a medição do RPM após a adição do peso excedente.

.. image:: Imagens/Sobrecarga.jpg
   :width: 1000px
   :align: center
*Figura 1  – Teste de Sobrecarga*

Então como pode ser visto, o sistema inicialmente tinha uma velocidade de 61,55 RPM (o que é um valor aceitável para um RPM definido de 60RPM) e acomodou-se com 60,21 RPM. Com o detalhe do aumento da fonte de corrente na fonte de bancada. Mais explicações sobre o código de controle e medição do RPM remoto serão vistos a seguir.

2.3.1 Firmware definitivo
-------------------------

Descrição do firmware definitivo...


3. Testes de validação
**********************


3.1 Acionamento remoto
======================

MOSTRAR GIF DE ACIONAMENTO NORMAL E REMOTO DA ESTEIRA


3.2 Controle de velocidade
==========================

MOSTRAR GIF DE DIFERENTES VELOCIDADES ENVIADAS PELO O USUÁRIO E ALTERAÇÃO DE VELOCIDADE DA ESTEIRA


3.3 Proteção de sobrecarga
==========================

MOSTRAR GIF DA PROTEÇÃO DO SISTEMA VIA LIMITE DIGITAL APÓS SOBREPESO


3.4 Inversão de sentido
=======================

MOSTRAR GIF DO ACIONAMENTO PARA SENTIDO FRENTE E TRÁS


4. Resultados finais
********************


4.1 Comparação entre resultados esperados e obtidos
===================================================

COMPARAR CURVA DE CONTROLE DO DRIVER PRONTO COM O DRIVER CONFECCIONADO
COMPARAR CURVA DE CONTROLE OBTIDA COM O DRIVER CONFECCIONADO COM A CURVA DE CONTROLE IDEALIZADA E CALCULAR ERRO
FAZER TABELA DE ERRO PARA DIFERENTES VALORES DE VELOCIDADE E SEM CARGA
FAZER TABELA DE ERRO PARA DIFERENTES VALORES DE VELOCIDADE E CARGA BAIXA
FAZER TABELA DE ERRO PARA DIFERENTES VALORES DE VELOCIDADE E CARGA ALTA
FAZER TABELA PARA PESO MÁXIMO E VELOCIDADE PARA ERRO MÁXIMO PERMITIDO

4.2 Melhorias futuras
===================================================

5. Referências (links/datasheets/livros)
****************************************

* `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_
