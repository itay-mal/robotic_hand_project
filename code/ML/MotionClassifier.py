import tensorflow as tf
from keras import Model
from keras.layers import GRU, Dense


class MotionClassifier(Model):
    def __init__(self):
        super().__init__()
        self.gru0 = GRU(10, return_sequences=True)
        self.gru1 = GRU(5)
        self.dense = Dense(5, activation='softmax')

    def call(self, x):
        x = self.gru0(x)
        x = self.gru1(x)
        return self.dense(x)
