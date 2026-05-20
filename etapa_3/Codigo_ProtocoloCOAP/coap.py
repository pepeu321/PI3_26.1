import asyncio
from aiocoap import *

async def main():

    payload = b"100"

    request = Message(
        code=POST,
        payload=payload,
        uri="coap://192.168.1.135/vel"
    )

    protocol = await Context.create_client_context()

    response = await protocol.request(request).response

    print("Resposta:")
    print(response.payload.decode())

asyncio.run(main())