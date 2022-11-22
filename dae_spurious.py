# python3
# -*- coding: utf-8 -*-
# @Author  : lina
# @Time    : 2018/11/23 13:05
"""
Denoising Autoencoder with 4 layer encoder and 4 layer decoder.
"""
from turtle import shape
import numpy as np
import pandas as pd

# from keras.datasets import mnist
from keras.models import Model
from keras.layers import Dense, Input
# import matplotlib.pyplot as plt

import argparse

import os
os.environ["CUDA_VISIBLE_DEVICES"] = '5'

np.random.seed(33)   # random seed，to reproduce results.

ENCODING_DIM_INPUT = 1500
ENCODING_DIM_LAYER1 = 128
ENCODING_DIM_LAYER2 = 128
ENCODING_DIM_LAYER3 = 64
ENCODING_DIM_OUTPUT = 32
EPOCHS = 20
BATCH_SIZE = 16


T_UNIT = 1   # 二进制编码单元间隔,单位：ns
P_NOISE = 0.3   #噪声系数

def train(x_train_noisy, x_train):

    # input placeholder
    input_image = Input(shape=(ENCODING_DIM_INPUT, ))

    # encoding layer
    encode_layer1 = Dense(ENCODING_DIM_LAYER1, activation='relu')(input_image)
    # encode_layer2 = Dense(ENCODING_DIM_LAYER2, activation='relu')(encode_layer1)
    # encode_layer3 = Dense(ENCODING_DIM_LAYER3, activation='relu')(encode_layer2)
    encode_output = Dense(ENCODING_DIM_OUTPUT, activation='relu')(encode_layer1)

    # decoding layer
    decode_layer1 = Dense(ENCODING_DIM_LAYER1, activation='relu')(encode_output)
    # decode_layer2 = Dense(ENCODING_DIM_LAYER2, activation='relu')(decode_layer1)
    # decode_layer3 = Dense(ENCODING_DIM_LAYER1, activation='relu')(decode_layer2)
    decode_output = Dense(ENCODING_DIM_INPUT, activation='sigmoid')(decode_layer1)

    # build autoencoder, encoder
    autoencoder = Model(inputs=input_image, outputs=decode_output)
    encoder = Model(inputs=input_image, outputs=encode_output)

    # compile autoencoder
    autoencoder.compile(optimizer='adam', loss='binary_crossentropy')

    # training
    autoencoder.fit(x_train_noisy, x_train, epochs=EPOCHS, batch_size=BATCH_SIZE, shuffle=True)

    return encoder, autoencoder

def add_noise(x_train):
    """
    add noise.
    :return:
    """
    x_train_noisy = np.zeros_like(x_train)
    x_train_noisy[:] += x_train[0]+x_train[1]+x_train[2]
    x_train_noisy[x_train_noisy>0] = 1
    # 模拟杂散脉冲和漏脉冲情况
    x_train_noisy = x_train_noisy + np.random.choice([0,1],size=x_train.shape,p=[1-P_NOISE,P_NOISE])
    # x_test_noisy = x_test + np.random.choice([0,1],size=x_test.shape,p=[1-P_NOISE,P_NOISE])

    x_train_noisy = x_train_noisy%2
    # x_test_noisy = x_test_noisy%2

    # return x_train_noisy, x_test_noisy
    return x_train_noisy

def plot_representation(encode_images, y_test):
    """
    plot the hidden result.
    :param encode_images: the images after encoding
    :param y_test: the label.
    :return:
    """
    # test and plot
    plt.scatter(encode_images[:, 0], encode_images[:, 1], c=y_test, s=3)
    plt.colorbar()
    plt.show()

def show_images(decode_images, x_test):
    """
    plot the images.
    :param decode_images: the images after decoding
    :param x_test: testing data
    :return:
    """
    n = 10
    plt.figure(figsize=(20, 4))
    for i in range(n):
        ax = plt.subplot(2, n, i+1)
        ax.imshow(x_test[i].reshape(20, 50))
        plt.gray()
        ax.get_xaxis().set_visible(False)
        ax.get_yaxis().set_visible(False)

        ax = plt.subplot(2, n, i + 1 + n)
        ax.imshow(decode_images[i].reshape(20, 50))
        plt.gray()
        ax.get_xaxis().set_visible(False)
        ax.get_yaxis().set_visible(False)
    plt.show()

MISS_RATE = 0.2
MODU = 'jitter'
def load_data():
    '''
        加载数据
    '''
    path = '/home/cky/pulse-deinterleave-with-DPML/data/original/'
    vali = '/home/cky/pulse-deinterleave-with-DPML/data/spurious/'
    modulation = MODU+'/'
    rate = str(int(MISS_RATE*100))+'/'
    # print('rate:' + rate)
    toa_name = 'toa.txt'
    label_name = 'label.txt'
    toa_dir = path +modulation+ toa_name
    toa_dir = '/home/cky/pulse-deinterleave-with-DPML/testdata/origin/toa.txt'
    label_dir = path +modulation+ label_name
    label_dir = '/home/cky/pulse-deinterleave-with-DPML/testdata/origin/label.txt'
    toa = pd.read_csv(toa_dir,header=None).to_numpy().reshape(-1)
    label = pd.read_csv(label_dir,header=None).to_numpy().reshape(-1)
    toa1 = toa[label==0][:3000]
    toa2 = toa[label==1][:3000]
    toa3 = toa[label==2][:3000]
    toa_all = np.array([toa1,toa2,toa3])

    # 截取toa串

    size_max = int(np.ceil((toa_all[:][-1].max()+1e-5)/T_UNIT/ENCODING_DIM_INPUT)*ENCODING_DIM_INPUT)

    # 二进制编码
    TOA = []
    for i in range(3):

        TOA_seq = np.zeros(size_max)
        for toa in toa_all[i]:
            idx = np.floor(toa/T_UNIT).astype('int64')
            if idx >= size_max:
                break 
            TOA_seq[idx] = 1
        TOA_seq = TOA_seq.reshape((-1,ENCODING_DIM_INPUT))
        TOA.append(TOA_seq)
    # seq_len = TOA_seq.shape[0]
    # train_size = int(seq_len*0.9)
    # TOA_seq_train = TOA_seq[:train_size]
    # TOA_seq_test = TOA_seq[train_size:-2]
    
    test_toa_dir = vali +modulation+ rate+toa_name
    test_toa_dir = '/home/cky/pulse-deinterleave-with-DPML/testdata/miss/toa.txt'
    test_label_dir = vali +modulation+rate+ label_name
    test_label_dir = '/home/cky/pulse-deinterleave-with-DPML/testdata/miss/label.txt'
    test_toa = pd.read_csv(test_toa_dir,header=None).to_numpy().reshape(-1)
    test_label = pd.read_csv(test_label_dir,header=None).to_numpy().reshape(-1)+1
    size_max = int(np.ceil((test_toa[-1]+1e-5)/T_UNIT/ENCODING_DIM_INPUT)*ENCODING_DIM_INPUT)
    test_seq = np.zeros(size_max)
    label_seq = np.zeros(size_max)
    for i,toa in enumerate(test_toa):
        idx = np.floor(toa/T_UNIT).astype('int64')
        
        test_seq[idx] = 1
        label_seq[idx] = test_label[i] if test_label[i]>0 else 0
    test_seq = test_seq.reshape((-1,ENCODING_DIM_INPUT))
    label_seq = label_seq.reshape((-1,ENCODING_DIM_INPUT))

    return np.array(TOA),test_seq,label_seq

def cal_results(x,y):
    '''
    计算召回率和精度,x为输入,y为输出
    '''
    TP = (x*y).sum()
    FP = y.sum()-TP
    FN = x.sum()-TP
    recall = TP/(TP+FN)
    precision = TP/(TP+FP)
    return recall,precision

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Personal information')
    parser.add_argument('--RATE', dest='RATE', type=int, help='MISS_RATE')
    parser.add_argument('--TYPE', dest='TYPE', type=str, help='TYPE')
    MISS_RATE = parser.parse_args().RATE/100.0
    MODU = parser.parse_args().TYPE

    # Step1： load data  x_train: (n, 1000), x_test: (m, 1000)
    x_train, x_test, y_test  = load_data()

    # Step2: add noisy
    x_train_noisy = add_noise(x_train)
    # show the contrast before noising and after noising.
    # show_images(x_test_noisy, x_test)

    # Step5： train
    encoder1,autoencoder1 = train(x_train_noisy=x_train_noisy[0], x_train=x_train[0])
    encoder2,autoencoder2 = train(x_train_noisy=x_train_noisy[1], x_train=x_train[1])
    encoder3,autoencoder3 = train(x_train_noisy=x_train_noisy[2], x_train=x_train[2])

    # test and plot
    # encode_images = encoder.predict(x_test_noisy)
    # plot_representation(encode_images, y_test)

    # 三个radar的预测
    prediction1 = autoencoder1.predict(x_test)
    prediction2 = autoencoder2.predict(x_test)
    prediction3 = autoencoder3.predict(x_test)
    
    prediction1[x_test==0] = 0
    prediction2[x_test==0] = 0
    prediction3[x_test==0] = 0

    label = np.zeros_like(prediction1)
    label[np.logical_and(prediction1>np.maximum(prediction2,prediction3),prediction1>=0.2)] = 1 
    label[np.logical_and(prediction2>np.maximum(prediction1,prediction3),prediction2>=0.2)] = 2 
    label[np.logical_and(prediction3>np.maximum(prediction2,prediction1),prediction3>=0.2)] = 3 
    
    wrong = np.count_nonzero(label-y_test)
    acc = 1 - wrong/np.count_nonzero(y_test)
    # decode_toa = np.around(decode_toa)

    
    # loss = np.abs(x_test-decode_toa).sum()/decode_toa.size
    # recall,precision = cal_results(x_test,decode_toa)
    # print('recall:{:.2f} ;   precision:{:.2f}'.format(recall,precision))
    f = open('daedata.txt','a')
    f.writelines(MODU + ' ' + str(int(MISS_RATE*100)) +' '+ 'accuracy: {:.5f}\r\n'.format(acc))
    f.close()
    # print(MODU + ' ' + str(int(MISS_RATE*100)) +' '+ 'accuracy: {:.5f}'.format(acc))
    # show_images(decode_toa, x_test_noisy)
