import sys
import time
import keyboard
import logging
import signal
import serial

MAX_RECONNECT_TRIES = 5
START    = bytes([0xAA])
GET_DATA = bytes([0xAB])

# set script logger 
logging.basicConfig(format='[%(asctime)s @ %(funcName)10s] %(message)s', datefmt='%H:%M:%S')
log = logging.getLogger('script_logger')
log.setLevel(logging.INFO)

exit_flag = False

def ask_exit(_, __):
    global exit_flag
    log.info("Shutdown req recieved")
    exit_flag = True

def write_read(x: bytes, device: serial.Serial) -> str:
    device.write(x)
    data = device.read_all().decode("utf-8").strip()
    return  data

def connect_to_esp():
    esp = None
    for try_ in range(MAX_RECONNECT_TRIES):
        try:
            esp = serial.Serial(port='COM5', baudrate=115200, timeout=.1)
            if esp is not None:
                break
        except serial.SerialException as e:
            log.error(f"Error connecting to ESP, try {try_ + 1} of {MAX_RECONNECT_TRIES}: {e}")
    if esp is None:
        log.error(f"Could not connect to ESP :(")
        exit(1)
    return esp

def esp_handshake(esp: serial.Serial):
    # refresh everything
    esp.close()
    esp.open()
    esp.read_all()
    while(esp.readline().decode('utf-8').strip() != "Starting"):
        log.info("sending start msg")
        esp.write(START)
        time.sleep(1)
    log.info("handshake done")

def main():
    global exit_flag
    signal.signal(signal.SIGINT, ask_exit)
    # signal.signal(signal.SIGTERM, ask_exit)

    data_logger = open(f'data_measurements/measurements_{time.strftime("%Y_%m_%d-%H_%M_%S")}.csv', 'wt')
    data_logger.write('timestamp,accX,accY,accZ,gyroX,gyroY,gyroZ,key_flag\n')
    
    esp = connect_to_esp()
    esp_handshake(esp)
    
    while not exit_flag:
        key_code = "UP" if keyboard.is_pressed('w') else "DWN" if keyboard.is_pressed('s') else "FWD" if keyboard.is_pressed('a') else "BCK" if keyboard.is_pressed('d') else "-"
        data_logger.write(esp.read_all().decode('utf-8').strip().replace('@','').replace('#',f'{key_code}\n'))
    
    data_logger.close()
    log.info("Program done")

if __name__ == '__main__':
    main()

