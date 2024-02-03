import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler, LabelEncoder
import scipy.stats as stats
import sys
from pprint import pprint
from collections import namedtuple

samples= {
    '-':  [],
    'UP':  [],
    'DWN': [],
    'FWD': [],
    'BCK': []
}

with open('data_measurements/measurements_2024_02_02-10_48_48.csv') as infile:
    _ = infile.readline() # skip headers row
    while(True):
        l = infile.readline().strip()
        if not l:
            break
        dir = l.rsplit(',',1)[-1]
        if dir in ('UP','DWN','BCK','FWD'):
            tmp = []
            for _ in range(400):
                time, accX, accY, accZ, gyroX, gyroY, gyroZ, _ = infile.readline().strip().split(',')
                tmp.append([int(time), float(accX), float(accY), float(accZ)])
            samples[dir].append(tmp) 

for axes_idx, axes in enumerate(('X','Y','Z')):
    figure = plt.figure()
    figure.suptitle(f'Acceleration {axes}')
    for dir_idx, dir in enumerate(('UP', 'DWN', 'FWD', 'BCK')):
        ax = figure.add_subplot(2,2,dir_idx+1)
        ax.set_title(dir)
        for sam in samples[dir]:
            ax.plot([i[axes_idx+1] for i in sam])


plt.show()