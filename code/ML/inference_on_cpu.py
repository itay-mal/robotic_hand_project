import time
import serial
import queue
import tensorflow as tf
from MotionClassifier import MotionClassifier
import threading

MAX_RECONNECT_TRIES = 5
READY = 0xAA

def connect_to_esp():
    esp = None
    for try_ in range(MAX_RECONNECT_TRIES):
        try:
            esp = serial.Serial(port='COM5', baudrate=115200, timeout=.1)
            if esp is not None:
                break
        except serial.SerialException as e:
            print(f"Error connecting to ESP, try {try_ + 1} of {MAX_RECONNECT_TRIES}: {e}")
    if esp is None:
        print(f"Could not connect to ESP :(")
        exit(1)
    return esp

def esp_handshake(esp: serial.Serial):
    # clear buffers etc
    esp.close()
    esp.open()
    esp.read_all()
    esp.flush()

def fetch_data(esp: serial.Serial):
    '''reads data from arduino in serial conn and put it in FIFO queue'''
    while True:
        try:
            esp.write(bytes((READY,)))
            msg = esp.read_until(b'@').decode('utf-8').strip().replace('@','').replace('#','')
            # print(msg)
            t_stamp, accelx, accely, accelz, gyrox, gyroy, gyroz, _ = msg.split(',')
            sample = (float(accelx), float(accely), float(accelz), float(gyrox), float(gyroy), float(gyroz))
            return int(t_stamp), sample
        except Exception as e:
            # print(f"could'nt read message: {e}, trying again")
            esp.flush()

def main():

    idx_to_label = {
        0: 'rest',
        1: 'up',
        2: 'down',
        3: 'forward',
        4: 'back'
    }

    #connect to esp to fetch measurements
    esp = connect_to_esp()
    time_p = 0
    model = MotionClassifier()
    model.load_weights('code/ML/model_wheights.index')
    while True:
        time, sample = fetch_data(esp)
        predict = model.call(tf.constant([[sample]]))
        # print(sample)
        # print(predict)
        if int(tf.argmax(predict,axis=1)) == 1:
            print("up")
        # elif int(tf.argmax(predict,axis=1)) == 2:
        #     print("down")
        elif int(tf.argmax(predict,axis=1)) == 3:
            print("forward")
        # elif int(tf.argmax(predict,axis=1)) == 4:
        #     print("back")
        # print(idx_to_label[int(tf.argmax(predict,axis=1))])
        # print(idx_to_label[int(tf.argmax(predict,axis=1))], predict, sample, time - time_p)
        time_p = time


if __name__ == '__main__':
    main()

