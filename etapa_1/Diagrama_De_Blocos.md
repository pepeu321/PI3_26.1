Diagrama de Blocos:

<img width="798" height="265" alt="image" src="https://github.com/user-attachments/assets/8bc2e720-88b2-4fef-b79a-4b81943d1b55" />



Descrição dos Blocos:  
Celular: 
O celular é responsável por permitir a interação do usuário com o sistema. Através dele, é 
possível definir a velocidade desejada da esteira e visualizar, em tempo real, os dados de 
funcionamento, como a velocidade atual medida. 
Controlador (ESP32): 
O controlador é o elemento central do sistema, responsável por realizar o processamento 
das informações. Ele compara a velocidade desejada, enviada pelo celular, com a 
velocidade real medida pelo sensor, calculando o erro do sistema. A partir desse erro, o 
controlador gera um sinal PWM para ajustar a velocidade do motor. Além disso, realiza a 
comunicação com o celular e o tratamento dos dados provenientes do sensor. 
Driver do Motor: 
O driver do é responsável por fazer a interface entre o ESP32 e o motor. Ele recebe o sinal 
PWM do controlador e fornece a corrente necessária para o acionamento do motor, 
permitindo o controle da velocidade da esteira de forma eficiente. 
Motor: 
O sistema utiliza um motor DC escovado (Brushed DC Motor), responsável por converter 
energia elétrica em movimento mecânico para acionar a esteira. Sua velocidade é 
controlada por meio da variação do sinal PWM aplicado, permitindo ajuste contínuo da 
rotação. 
Esteira 
A esteira é o sistema a ser controlado. Seu movimento depende diretamente da atuação do 
motor, em que ela será controlada 
Sensor: 
**O sensor utilizado é o AS5600, um sensor magnético de posição angular. Ele mede a 
rotação do eixo do motor a partir da variação do campo magnético de um ímã acoplado ao 
eixo. Esses dados são enviados ao ESP32, permitindo calcular a velocidade da esteira e 
realizar o controle em malha fechada.**
