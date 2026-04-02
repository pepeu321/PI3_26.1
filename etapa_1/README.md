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


<img width="798" height="265" alt="image" src="https://github.com/user-attachments/assets/8bc2e720-88b2-4fef-b79a-4b81943d1b55" />



**Descrição dos Blocos:**


**Celular:** 

O celular é responsável por permitir a interação do usuário com o sistema. Através dele, é 
possível definir a velocidade desejada da esteira e visualizar, em tempo real, os dados de 
funcionamento, como a velocidade atual medida. 

**Controlador (ESP32):** 

O controlador é o elemento central do sistema, responsável por realizar o processamento 
das informações. Ele compara a velocidade desejada, enviada pelo celular, com a 
velocidade real medida pelo sensor, calculando o erro do sistema. A partir desse erro, o 
controlador gera um sinal PWM para ajustar a velocidade do motor. Além disso, realiza a 
comunicação com o celular e o tratamento dos dados provenientes do sensor. 

**Driver do Motor:**

O driver do é responsável por fazer a interface entre o ESP32 e o motor. Ele recebe o sinal 
PWM do controlador e fornece a corrente necessária para o acionamento do motor, 
permitindo o controle da velocidade da esteira de forma eficiente. 

**Motor:**

O sistema utiliza um motor DC escovado (Brushed DC Motor), responsável por converter 
energia elétrica em movimento mecânico para acionar a esteira. Sua velocidade é 
controlada por meio da variação do sinal PWM aplicado, permitindo ajuste contínuo da 
rotação. 

**Esteira:** 

A esteira é o sistema a ser controlado. Seu movimento depende diretamente da atuação do 
motor, em que ela será controlada 

**Sensor:**

O sensor será responsável por medir a velocidade de rotação do motor. Os dados serão enviados ao ESP32, permitindo calcular a velocidade da esteira e realizar o controle em malha fechada.

### 2. Estudo e configuração do ESP IDF para aplicar no projeto


Editar

### 3. Definição do sensor de efeito hall


Essa atividade tem por objetivo definir o sensor de velocidade do motor. Desse modo o microcontrolador terá o rpm do motor e decidirá se é necessário aumentar ou diminuir a corrente do driver do motor para que o motor gire na rotação pré-determinada.

Inicialmente iríamos utilizar o encoder as5600, que utiliza o efeito hall, e por isso o nome desta atividade ficou definição do sensor de efeito hall. Porém o mais indicado para o nosso uso é o encoder óptico de arduino.

![Encoder](https://raw.githubusercontent.com/pepeu321/PI3_26.1/main/etapa_1/encoders2.jpg)

O encoder as5600 utiliza vários sensores de efeito hall para gerar uma posição angular do campo magnético. Então um ímã em determinada posição gerará um equivalente de tensão que é então convertido para um número binário com resolução de 12 bits e isso indicará a posição em graus que o ímã(fonte geradora de campo magnético) está.

Abaixo está um gif do funcionamento desse sensor:

![as5600](https://github.com/pepeu321/PI3_26.1/blob/main/etapa_1/imagens/gif_as5600.gif)

Como pode ser visto, girando o ímã, acende os leds equivalentes à posição em que o ímã girou.

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

O microcontrolador a ser usado será o esp idf. O ESP32 possui o periférico de hardware PCNT (Pulse Counter), um contador de pulsos. Então escolhendo o encoder óptico o esp contará os pulsos do encoder automaticamente sem sobrecarregar a CPU.
Isso permite medições de RPM mais precisas e confiáveis, mesmo em altas velocidades.
Além disso, integrado ao ESP-IDF com FreeRTOS, facilita a leitura periódica e o controle em tempo real do motor.

![pcnt](https://github.com/pepeu321/PI3_26.1/blob/main/etapa_1/imagens/pcnt.jpg)

Referência do esp, comprovando a existência do pcnt.

Por isso a melhor escolha para medir o rpm do motor é o **encoder óptico**, pois se aproveita da arquitetura do microcontrolador e as contas para cálculo do rpm são mais simples do que com o as5600.

### 4. Rampa de aceleração linear ou rampa em S

O soft start é a técnica de partida suave do motor, onde o PWM (ou tensão/corrente) é aumentado gradualmente ao invés de aplicar valor máximo instantaneamente.
Ele é necessário na esteira porque a carga pode variar (peso), e uma partida brusca gera picos de corrente, trancos mecânicos e desgaste.
Sem soft start, o motor pode sofrer quedas de tensão, aquecimento e até perda de controle de velocidade inicial.

Para implementar, usa-se uma rampa no sinal de controle (PWM), aumentando-o ao longo do tempo até atingir o valor desejado.
Essa rampa pode ser baseada em tempo ou em referência de RPM, usando o encoder como feedback.

Os principais tipos são:

**Rampa Linear:**

Uma rampa de aceleração linear é uma maneira mais simples de controlar a variação de velocidade em motores. A velocidade tem um aumento com o tempo, o que resulta numa aceleração constante.

No gráfico de velocidade pelo tempo, que é uma rampa, a reta crescente mostra que a velocidade do motor aumenta de forma uniforme até atingir o valor desejado. Enquanto no gráfico de aceleração possui um valor fixo durante seu intervalo, tendo uma mudança brusca no início e no fim.

<img width="734" height="166" alt="image" src="https://github.com/user-attachments/assets/c38d0026-c35c-4269-9755-d926432ea3cf" />

Esse tipo de rampa apresenta descontinuidade no início e no fim, onde a aceleração sofre uma variação brusca. Essa variação pode causar impactos mecânicos, picos de corrente e até a perda de controle de velocidade inicial. 

**Rampa em S:**

A rampa em S é usada para garantir transições mais suaves na variação de velocidade do motor. A aceleração não é constante, variando de forma gradual ao longo do tempo.
Seu gráfico de velocidade tem o formato de “S”, onde a velocidade tem um início e um fim suave. A aceleração tem um comportamento de crescer gradualmente até um valor máximo e retornar a zero. 

Esse comportamento elimina descontinuidades, reduzindo significativamente esforços mecânicos, vibrações e picos de corrente.

<img width="761" height="176" alt="image" src="https://github.com/user-attachments/assets/4a00f36f-cfdb-4641-b447-9eb2867b966f" />

Portanto, embora a rampa linear seja mais simples de implementar, ela pode causar impactos indesejados no sistema, enquanto a rampa em S, é capaz de oferecer uma variação mais suave de velocidade, melhorando o desempenho de controle do sistema.

O sistema a ser controlado é a velocidade de uma esteira, por meio do feedback do sensor de velocidade. Portanto será utilizado a rampa em "S", já que contribui para uma melhor estabilidade e precisão, além de ter uma melhor resposta dinâmica do sistema, limita a corrente de partida e melhora a estabilidade com carga variável. Garantindo um controle e acionamento suave do motor.

Referências (links/datasheets/livros)
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


