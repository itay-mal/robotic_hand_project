import asyncio
from bleak import BleakScanner, BleakClient

MODEL_NBR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def find_devices():
    found_devices = []
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
        found_devices.append(d)
    return found_devices

async def get_data(address):
    async with BleakClient(address) as client:
        await client.start_notify(MODEL_NBR_UUID, callback)
        await asyncio.sleep(20)
        await client.stop_notify(MODEL_NBR_UUID)
        print('done')
            
async def callback(sender, data:bytearray):
    accX, accY, accZ, gyroX, gyroY, gyroZ = data.decode('utf-8').rstrip('#').split(':')
    print(f'accX:{accX}, accY:{accY}, accZ:{accZ}, gyroX:{gyroX}, gyroY:{gyroY}, gyroZ:{gyroZ}')


available_devices = asyncio.run(find_devices())
my_device = None
for d in available_devices:
    if d.name == 'ESP32':
        my_device = d
        break
asyncio.run(get_data(my_device.address))


