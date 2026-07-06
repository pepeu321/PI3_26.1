from aiocoap import *
import asyncio

ESP_IP = "192.168.1.104"


async def enviar_ref(protocol, ref_rpm):
    request = Message(
        code=POST,
        payload=str(ref_rpm).encode(),
        uri=f"coap://{ESP_IP}/vel"
    )

    response = await protocol.request(request).response
    print("Resposta:", response.payload.decode())


async def ler_rpm(protocol):
    request = Message(
        code=GET,
        uri=f"coap://{ESP_IP}/vel"
    )

    response = await protocol.request(request).response
    print("RPM:", response.payload.decode())


async def enviar_direcao(protocol, direcao):
    request = Message(
        code=POST,
        payload=direcao.encode(),
        uri=f"coap://{ESP_IP}/dir"
    )

    response = await protocol.request(request).response
    print("Direção:", response.payload.decode())


async def main():
    protocol = await Context.create_client_context()

    while True:
        cmd = input("\nComando (on/off/rpm/fwd/rev/q ou RPM): ")

        if cmd == "on":
            await enviar_ref(protocol, 15)

        elif cmd == "off":
            await enviar_ref(protocol, 0)

        elif cmd == "rpm":
            await ler_rpm(protocol)

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

            except ValueError:
                print("Comando inválido")


asyncio.run(main())