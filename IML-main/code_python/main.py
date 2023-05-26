import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.keras.layers import Input,  Flatten
from tensorflow.keras.layers import Conv2D, BatchNormalization, ReLU, MaxPool2D, Dense, Dropout
import tensorflowjs as tfjs
from sklearn.model_selection import StratifiedShuffleSplit
from timeit import default_timer as timer

train_full = pd.read_csv('train')
test = pd.read_csv('test')

label_map = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
             'V', 'W', 'X', 'Y', 'Z']

X_train_full = train_full.values[:, 1:].astype(np.float32())
y_train_full = train_full.values[:, 0]
X_test = test.values[:, 1:].astype(np.float32())
y_test = test.values[:, 0]


split = StratifiedShuffleSplit(n_splits=1, test_size=2500)
for train_index, test_index in split.split(X_train_full, y_train_full):
    X_train, X_val = X_train_full[train_index], X_train_full[test_index]
    y_train, y_val = y_train_full[train_index], y_train_full[test_index]

print((X_train[0]))
print(X_train.shape[0])
X_train = X_train.reshape((X_train.shape[0], 28, 28, 1))

X_val = X_val.reshape((X_val.shape[0], 28, 28, 1))
X_test = X_test.reshape((X_test.shape[0], 28, 28, 1))
X_train = np.stack((X_train[:, :, :, 0],)*3, axis=3)
X_val = np.stack((X_val[:, :, :, 0],)*3, axis=3)
X_test = np.stack((X_test[:, :, :, 0],)*3, axis=3)



image_shape = X_train[0].shape


print(image_shape)

model = tf.keras.Sequential([
    Input(shape=image_shape),
    Conv2D(8, kernel_size=(3, 3), strides=1, padding="same"),
    BatchNormalization(),
    ReLU(),
    Conv2D(8, kernel_size=(3, 3), strides=1, padding="same"),
    BatchNormalization(),
    ReLU(),
    MaxPool2D(pool_size=2, strides=2, padding="valid"),
    Conv2D(16, kernel_size=(3, 3), strides=1, padding="same"),
    BatchNormalization(),
    ReLU(),
    Conv2D(16, kernel_size=(3, 3), strides=1, padding="same"),
    BatchNormalization(),
    ReLU(),
    MaxPool2D(pool_size=2, strides=2, padding="valid"),
    Conv2D(32, kernel_size=(3, 3), strides=1, padding="same"),
    BatchNormalization(),
    ReLU(),
    Conv2D(32, kernel_size=(3, 3), strides=1, padding="same"),
    BatchNormalization(),
    ReLU(),
    MaxPool2D(pool_size=2, strides=2, padding="valid"),
    Flatten(),
    Dense(100),
    Dropout(0.50),
    ReLU(),
    Dense(26, activation='softmax')
])
model.summary()
model.compile(loss="sparse_categorical_crossentropy", optimizer=tf.keras.optimizers.Adam(0.0005), metrics=["accuracy"])
history = model.fit(X_train, y_train, epochs=29, batch_size=32, validation_data=(X_val, y_val))
tf.saved_model.save(model, 'saved_model')
tfjs.converters.save_keras_model(model, 'jsmodels')
