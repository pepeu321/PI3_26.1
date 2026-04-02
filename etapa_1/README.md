# Etapa 1

- Visão Geral
- Desenvolvimento
  - Diagrama de blocos do sistema
  - Estudo e configuração do ESP IDF para aplicar no projeto
  - Definição do sensor de efeito hall
  - Rampa de aceleração linear ou rampa em S
- Referências(links/datasheets/livros)


## Visão geral


A etapa 1 consiste numa fase de estudo para o desenvolvimento do projeto. As atividades a serem desenvolvidas nesta etapa são:

📌 Estudo e configuração do ESP IDF para aplicar no projeto

📌 Definição do sensor de efeito hall

📌 Rampa de aceleração linear ou rampa em S

📌 Diagrama de blocos do sistema 



## Desenvolvimento


Apresentar o desenvolvimento da etapa contendo detalhes de implementação (se houver) de hardware e software. Adicionar pesqusisas realizadas bem como testes realizados.


### 1. Diagrama de blocos do sistema


Essa tarefa está apresentada primeiro porque apresenta uma visão geral do funcionamento do projeto quando ficar pronto.


### 2. Estudo e configuração do ESP IDF para aplicar no projeto


Editar

### 3. Definição do sensor de efeito hall


Essa atividade tem por objetivo definir o sensor de velocidade do motor. Desse modo o microcontrolador terá o rpm do motor e decidirá se é necessário aumentar ou diminuir a corrente do driver do motor para que o motor gire na rotação pré-determinada.

Inicialmente iríamos utilizar o encoder as5600, que utiliza o efeito hall, e por isso o nome desta atividade ficou definição do sensor de efeito hall. Porém o mais indicado para o nosso uso é o encoder óptico de arduino.

![Encoder](https://raw.githubusercontent.com/pepeu321/PI3_26.1/main/etapa_1/encoders2.jpg)

O encoder as5600 utiliza vários sensores de efeito hall para gerar uma posição angular do campo magnético. Então um ímã em determinada posição gerará um equivalente de tensão que é então convertido para um número binário com resolução de 12 bits e isso indicará a posição em graus que o ímã(fonte geradora de campo magnético) está.

Abaixo está um gif do funcionamento desse sensor:

![as5600](https://github.com/pepeu321/PI3_26.1/blob/main/etapa_1/imagens/gif_as5600.gif)

Como pode ser visto, girando o ímã, acende os leds equivalentes a posição em que o ímã girou. 
E para obter o rpm do motor utilizando o as5600, primeiro é necessário ler continuamente o valor de posição angular fornecido pelo sensor. Esse valor é disponibilizado via comunicação I2C como um número inteiro entre 0 e 4095, que representa uma volta completa de 360 graus. Em seguida, deve-se armazenar duas leituras consecutivas do ângulo, uma anterior e outra atual, separadas por um intervalo de tempo conhecido. Com essas duas medições, calcula-se a variação angular subtraindo o valor anterior do atual. Como o sensor trabalha em escala circular, é necessário corrigir possíveis descontinuidades quando ocorre a passagem de 4095 para 0 (ou vice-versa), ajustando a diferença somando ou subtraindo 4096 quando o valor ultrapassar metade da escala.

Após obter a variação angular corrigida, converte-se essa diferença em número de voltas dividindo o resultado por 4096. Em seguida, divide-se esse valor pelo intervalo de tempo entre as medições, obtendo-se a velocidade em rotações por segundo. Por fim, multiplica-se esse resultado por 60 para converter para rpm. Esse processo deve ser repetido continuamente para acompanhar a variação da velocidade do motor ao longo do tempo, sendo recomendável aplicar algum tipo de filtragem ou média para reduzir ruídos e oscilações na medição.

#### Encoder óptico

![Encoder](https://github.com/pepeu321/PI3_26.1/blob/main/etapa_1/imagens/post_encoder3a.png)

Um encoder óptico usa um LED contínuo e um fotossensor separados por um disco com ranhuras que gira com o eixo.
Quando as ranhuras passam, a luz é alternadamente bloqueada/liberada, gerando um sinal digital (0 e 1).
Cada ranhura corresponde a um pulso, permitindo contar quantos “passos” de rotação ocorreram.
Ele possui dois canais (A e B) defasados entre si (quadratura).
Essa defasagem permite determinar o sentido de rotação e aumentar a resolução da medição.
Para o cálculo do rpm: o encoder gera pulsos conforme o eixo gira, onde cada pulso representa uma fração da rotação.
Conta-se o número de pulsos em um intervalo de tempo conhecido usando o microcontrolador.
Divide-se pelos pulsos por volta e converte-se para minutos, obtendo o rpm.


#### Escolha entre encoder as5600 e encoder óptico

O microcontrolador a ser usado será o esp idf. O ESP32 possui o periférico de hardware PCNT (Pulse Counter), um contador de pulsos. Então o esp contará os pulsos do encoder automaticamente sem sobrecarregar a CPU.
Isso permite medições de RPM mais precisas e confiáveis, mesmo em altas velocidades.
Além disso, integrado ao ESP-IDF com FreeRTOS, facilita a leitura periódica e o controle em tempo real do motor.

![pcnt](https://github.com/pepeu321/PI3_26.1/blob/main/etapa_1/imagens/pcnt.jpg)

Referência do esp, comprovando a existência do pcnt.

### 4. Rampa de aceleração linear ou rampa em S

Editar


Referências (links/datasheets/livros)
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


