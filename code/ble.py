import asyncio
import signal
from bleak import BleakScanner, BleakClient, BleakGATTCharacteristic
import logging


logging.basicConfig(format='%(asctime)s %(message)s', datefmt='[%H:%M:%S]')
log = logging.getLogger()
log.setLevel(logging.INFO)

CHARACTARISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class DisconnectionException(Exception):
    """Raised when device has disconnected."""
    pass

class ShutdownException(Exception):
    """Raised when the program shuold shutdown."""
    pass


def ask_exit(_, __):
    global exit_flag
    log.info("Shutdown req recieved")
    exit_flag = True

async def find_devices():
    found_devices = []
    devices = await BleakScanner.discover()
    log.debug('found the following devices')
    for d in devices:
        log.debug(d)
        found_devices.append(d)
    return found_devices
            
async def parse_data_cb(sender: BleakGATTCharacteristic, data:bytearray):
    decoded_msg = data.decode('utf-8')
    if not decoded_msg.endswith('#'):
        log.error('recieved message appears to be corrupted')
        log.error(f'message is \"{decoded_msg}\"')
    timestamp,accX, accY, accZ, gyroX, gyroY, gyroZ = data.decode('utf-8').rstrip('#').split(':')
    log.info(f'timestamp:{timestamp}, accX:{accX}, accY:{accY}, accZ:{accZ}, gyroX:{gyroX}, gyroY:{gyroY}, gyroZ:{gyroZ}')

async def notify_and_record(client: BleakClient):
    global exit_flag
    global connected
    connected = await client.is_connected()

    def disconnect_callback(client: BleakClient):
        log.error(f"Client with address {client.address} got disconnected!")
        global connected 
        connected = False
        # if not exit_flag:
        #     log.error("Disconnection wa without request, trying to reconnect")
        # raise DisconnectionException(f"Client with address {client.address} got disconnected!")

    client.set_disconnected_callback(disconnect_callback)

    log.info(f"Connected: {connected}")
    log.info("Starting notification.")
    await client.start_notify(CHARACTARISTIC_UUID, parse_data_cb)
    while not exit_flag:
        if not connected:
            log.error("disconnected withou request")
            await client.connect()
            await client.start_notify(CHARACTARISTIC_UUID, parse_data_cb)
        await asyncio.sleep(1)

    log.info("Shutting Down.")
    raise ShutdownException

async def stay_connected(address: str, name: str, timeout: float = 4.0):
    global exit_flag
    exit_flag = False
    log.info("Starting Loop")
    client = BleakClient(address_or_ble_device=address, timeout=timeout)
    try:
        log.info(f"Connecting to device {name}.")
        await client.connect()
        await notify_and_record(client)
    except DisconnectionException as e:
        log.error(e)
    except ShutdownException as e:
        log.error(e)
    except Exception as e:
        log.error(e)
        pass

    log.info("Shutting down notification.")
    await client.stop_notify(CHARACTARISTIC_UUID)
    log.info("Done shutting down notification.")

    log.info("Disconnecting to device.")
    await client.disconnect()
    log.info("End of Loop Iteration")

async def run():
    available_devices = await (find_devices())
    my_device = None
    for d in available_devices:
        if d.name == 'ESP32':
            my_device = d
            break
    
    log.info("Attempting to connect to device and start recording")
    
    await stay_connected(address=my_device.address, name=d.name)

def main():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)

    signal.signal(signal.SIGINT, ask_exit)
    signal.signal(signal.SIGTERM, ask_exit)

    loop.run_until_complete(run())

    loop.stop()
    loop.close()
    log.info("Program done")

if __name__ == '__main__':
    main()

