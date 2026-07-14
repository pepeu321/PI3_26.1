from aiocoap import *
import asyncio
import msvcrt   #biblioteca do Windows para detectar comando enter

ESP_IP = "192.168.1.104"
TIMEOUT_S = 2   #tempo que python espera resposta do esp


async def enviar_ref(protocol, ref_rpm):
    request = Message(
        code=POST,
        payload=str(ref_rpm).encode(), #Transforma o numero em texto e depois em bytes
        uri=f"coap://{ESP_IP}/vel"     #coap://192.168.1.101/vel
    )

    try:
        response = await asyncio.wait_for(      #envia requisicao
            protocol.request(request).response, #espera resposta
            timeout=TIMEOUT_S                   #vai para exept se passar 2s
        )
        print("Resposta:", response.payload.decode())

    except Exception as e:
        print("Erro no POST /vel:", repr(e))


async def ler_rpm(protocol):
    request = Message(
        code=GET,
        uri=f"coap://{ESP_IP}/vel"
    )

    try:
        response = await asyncio.wait_for(
            protocol.request(request).response,
            timeout=TIMEOUT_S
        )
        print(response.payload.decode())

    except Exception as e:
        print("Erro no GET /vel:", repr(e))


async def enviar_direcao(protocol, direcao):
    request = Message(
        code=POST,
        payload=direcao.encode(),
        uri=f"coap://{ESP_IP}/dir"
    )

    try:
        response = await asyncio.wait_for(
            protocol.request(request).response,
            timeout=TIMEOUT_S
        )
        print("Direcao:", response.payload.decode())

    except Exception as e:
        print("Erro no POST /dir:", repr(e))


async def monitorar_rpm(protocol):
    print("\nMonitorando, pressione ENTER para parar.\n")

    while True:
        if msvcrt.kbhit(): #verifica se tecla foi apertada
            tecla = msvcrt.getwch()  #le tecla

            if tecla == "\r":
                print("\nMonitor parado.")	#se for enter, sai do monitor
                break
	
        request = Message(
            code=GET,
            uri=f"coap://{ESP_IP}/vel"
        )

        try:
            response = await asyncio.wait_for(
                protocol.request(request).response,
                timeout=TIMEOUT_S
            )

            texto = response.payload.decode()

            print(
                "\r" + texto.ljust(100), 
                end="",
                flush=True
            )

        except Exception as e:
            print(
                "\rErro no monitor: " + repr(e).ljust(100),
                end="",
                flush=True
            )

        await asyncio.sleep(1) 


async def main():
    protocol = await Context.create_client_context()

    while True:
        cmd = input("\nComando (on/off/monitor/fwd/rev/q para sair): ")

        if cmd == "on":
            await enviar_ref(protocol, 30)

        elif cmd == "off":
            await enviar_ref(protocol, 0)           

        elif cmd == "monitor":
            
            await monitorar_rpm(protocol)

        elif cmd == "fwd":
            await enviar_direcao(protocol, "fwd")
            

        elif cmd == "rev":
            await enviar_direcao(protocol, "rev")
            

        elif cmd == "q":
            await enviar_ref(protocol, 0)
            break

        else:
            try:
                ref_rpm = float(cmd)
                await enviar_ref(protocol, ref_rpm)
                await ler_rpm(protocol)

            except ValueError:
                print("Comando invalido")


asyncio.run(main())