import time
import serial
import pickle
import numpy as np
from sklearn.ensemble import RandomForestClassifier

MAX_RECONNECT_TRIES = 5
READY = 0xAA
FREQ = 80 # Hz
COLLECT_MODE = 0

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
    if COLLECT_MODE:
        outfile = open(f"data_measurements/back_{time.strftime('%d_%m_%Y_%H_%M_%S')}.csv", "wt")
        outfile.write("timestamp,accX,accY,accZ,gyroX,gyroY,gyroZ\n")
    
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
    with open("code/ML/saved_RF_calssifier.pickle", 'rb') as infile:
        model: RandomForestClassifier = pickle.load(infile)
    buffer = np.zeros((1,600))
    now_p = 0
    while True:
        now = time.perf_counter_ns()
        if now - now_p > 1e9/FREQ:
            time_, sample = fetch_data(esp)
            # print(now - now_p, sample)
            now_p = now
            if COLLECT_MODE:
                outfile.write(f"{time_},{','.join([str(i) for i in sample])}\n")
            else:
                buffer = np.hstack([buffer[0,6:].reshape((1,-1)), np.array(sample).reshape((1,-1))])
                predict = model.predict_proba(buffer)
                predicted_label = idx_to_label[np.argmax(predict)+1]
                if predicted_label != 'rest' and np.max(predict) > 0.1:
                    print(predicted_label, predict)
                # print(time_ - time_p)
                # time_p = time_

if __name__ == '__main__':
    main()
