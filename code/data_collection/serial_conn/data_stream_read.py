import time
import keyboard
import logging
import signal
import serial
from io import TextIOWrapper

MAX_RECONNECT_TRIES = 5
START     = bytes([0xAA])
GET_DATA  = bytes([0xAB])
STOP      = bytes([0xAC])
HANDSHAKE = bytes([0xAD])

# set script logger 
logging.basicConfig(format='[%(asctime)s @ %(funcName)15s] %(message)s', datefmt='%H:%M:%S')
log = logging.getLogger('script_logger')
log.setLevel(logging.INFO)

exit_flag = False

def ask_exit(_, __):
    global exit_flag
    log.info("Shutdown request recieved")
    exit_flag = True

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
    # clear buffers etc
    esp.close()
    esp.open()
    esp.read_all()
    
    while(esp.readline().decode('utf-8').strip() != "LETS GO"):
        log.info("Sending handshake msg")
        esp.write(HANDSHAKE)
        time.sleep(1)
    log.info("Handshake done")

def fetch_data(data_logger: TextIOWrapper, esp: serial.Serial):
    esp.write(START)
    while not exit_flag:
        key_code = "UP" if keyboard.is_pressed('w') else "DWN" if keyboard.is_pressed('s') else "FWD" if keyboard.is_pressed('a') else "BCK" if keyboard.is_pressed('d') else "-"
        data_logger.write(esp.read_all().decode('utf-8').strip().replace('@','').replace('#',f'{key_code}\n'))
    
    # complete the last line(s)
    esp.write(STOP)
    time.sleep(0.5)
    data_logger.write(esp.read_all().decode('utf-8').strip().replace('@','').replace('#',f'{key_code}\n'))


def main():
    global exit_flag
    signal.signal(signal.SIGINT, ask_exit)

    data_logger = open(f'data_measurements/measurements_{time.strftime("%Y_%m_%d-%H_%M_%S")}.csv', 'wt')
    data_logger.write('timestamp,accX,accY,accZ,gyroX,gyroY,gyroZ,key_flag\n')
    
    esp = connect_to_esp()
    esp_handshake(esp)
    fetch_data(data_logger, esp)
    
    data_logger.close()
    log.info("Program done")

if __name__ == '__main__':
    main()

