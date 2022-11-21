import torch
from torch import nn
import torch.nn.functional as F
import numpy as np
import pandas as pd
import torch.utils.data as Data
import argparse

class LstmRNN(nn.Module):
    def __init__(self,input_size,hidden_size=128,output_size=1,num_layers=1):
        super().__init__()
        self.lstm = nn.LSTM(input_size,hidden_size,num_layers,bidirectional=True,batch_first=True)     
        self.fullconnect = nn.Linear(in_features=hidden_size*2,out_features=output_size)
        self.softmax = nn.Softmax(dim=2)

    def forward(self,_x):
        x,_ = self.lstm(_x)
        x = self.fullconnect(x)
        return x

MISS_RATE = 0.1
MODU = 'jitter'
def read_data():
    path = '/home/cky/pulse-deinterleave-with-DPML/data/original/'
    vali = '/home/cky/pulse-deinterleave-with-DPML/data/miss/'
    modulation = MODU+'/'
    rate = str(int(MISS_RATE*100))+'/'
    # print('rate:' + rate)
    toa_name = 'toa.txt'
    label_name = 'label.txt'
    toa_dir = path +modulation+ toa_name
    label_dir = path +modulation+ label_name
    toa = pd.read_csv(toa_dir,header=None).to_numpy()
    label = pd.read_csv(label_dir,header=None).to_numpy()
    test_toa_dir = vali +modulation+ rate+toa_name
    test_label_dir = vali +modulation+rate+ label_name
    test_toa = pd.read_csv(test_toa_dir,header=None).to_numpy()
    test_label = pd.read_csv(test_label_dir,header=None).to_numpy()
    return toa,label,test_toa,test_label



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Personal information')
    parser.add_argument('--RATE', dest='RATE', type=int, help='MISS_RATE')
    parser.add_argument('--TYPE', dest='TYPE', type=str, help='TYPE')
    MISS_RATE = parser.parse_args().RATE/100.0
    MODU = parser.parse_args().TYPE

    torch.cuda.set_device(5)
    # print(torch.cuda.is_available())
    toa,label,test_toa,test_label = read_data()

    s = np.random.choice([0,1],size = toa.size,p = [MISS_RATE,1-MISS_RATE])
    toa = np.delete(toa,np.where(s==0))
    label = np.delete(label,np.where(s==0))

    dtoa = toa[1:]-toa[0:-1]
    label = label[1:]
    dtoa = dtoa.reshape(-1)
    label = label.reshape(-1)
    
    dtest_toa = test_toa[1:]-test_toa[0:-1]
    test_label = test_label[1:]
    dtest_toa = dtest_toa.reshape(-1)
    test_label = test_label.reshape(-1)

    data_len = dtoa.size    
    # create dataset
    dataset = np.zeros((data_len, 2))
    dataset[:,0] = dtoa
    dataset[:,1] = label
    # dataset = dataset.astype('float32')

    # train data
    time_len = 500
    batch_Size = 5
    train_data_ratio = 1
    train_data_len = int(data_len*train_data_ratio/time_len)*time_len
    train_x = dataset[:train_data_len,0]
    train_y = dataset[:train_data_len,1].astype('int64')
    INPUT_FEATRUE_NUM = 1
    OUTPUT_FEATRUE_NUM = label.max()+1

    # test data
    test_x = dtest_toa
    test_y = test_label.astype('int64')
    test_x_tensor = test_x.reshape(1,-1,INPUT_FEATRUE_NUM) # shape: (-1,time_len,1)
    test_x_tensor = torch.from_numpy(test_x_tensor).to(torch.float32).cuda()
    # test_x_tensor = F.normalize(test_x_tensor) # normalize

    test_y_tensor = test_y.reshape(1,-1) # shape: (-1,time_len)
    test_y_tensor = torch.from_numpy(test_y_tensor).long().cuda()
    
    # train
    hidden_size = 128

    # transfer data to pytorch tensor
    train_x_tensor = train_x.reshape(-1,time_len,INPUT_FEATRUE_NUM) # shape: (-1,time_len,1)
    train_x_tensor = torch.from_numpy(train_x_tensor).to(torch.float32)
    # train_x_tensor = F.normalize(train_x_tensor) # normalize

    train_y_tensor = train_y.reshape(-1,time_len) # shape: (-1,time_len)
    train_y_tensor = torch.from_numpy(train_y_tensor).long()
    # train_y_tensor = torch.eye(OUTPUT_FEATRUE_NUM)[train_y_tensor.long(),:] # one hot vector

    torch_dataset = Data.TensorDataset(train_x_tensor,train_y_tensor)
    loader = Data.DataLoader(dataset=torch_dataset,batch_size=batch_Size,shuffle=True,num_workers=2)
    
    lstm_model = LstmRNN(INPUT_FEATRUE_NUM,hidden_size,OUTPUT_FEATRUE_NUM,1).cuda()

    loss_function = nn.CrossEntropyLoss().cuda() # softmax+log+nll_loss
    optimizer = torch.optim.Adam(lstm_model.parameters(),lr=1e-3,weight_decay=1e-5)

    max_epochs = 100
    for epoch in range(max_epochs):
        for step,(batch_x,batch_y) in enumerate(loader):
            """
            batch_x shape:(batch_size,time_len,1)
            batch_y shape:(batch_size,time_len)
            """
            batch_x = batch_x.cuda()
            batch_y = batch_y.cuda()

            optimizer.zero_grad()
            output = lstm_model(batch_x)
            output = output.view(-1,OUTPUT_FEATRUE_NUM)
            batch_y = batch_y.view(-1)
            loss = loss_function(output,batch_y)
            loss.backward()
            optimizer.step()
        
        # print('Epoch [{}/{}], Loss: {:.5f}'.format(epoch+1,max_epochs,loss.item()))
        if loss.item() < 1e-4:
            # print('Epoch [{}/{}], Loss: {:.5f}'.format(epoch+1,max_epochs,loss.item()))
            # print('The loss value is reached')
            break
        # elif (epoch+1) % 100 == 0:
        #     print('Epoch [{}/{}], Loss: {:.5f}'.format(epoch+1,max_epochs,loss.item()))
    prediction_y = lstm_model(test_x_tensor)
    prediction_y  = torch.argmax(prediction_y ,-1)
    difference = test_y_tensor - prediction_y

    non_zero = torch.count_nonzero(difference,dim = 1)
    acc = 1-torch.sum(non_zero)/(difference.shape[0]*difference.shape[1])
    print(MODU + ' ' + str(int(MISS_RATE*100)) +' '+ 'accuracy: {:.5f}'.format(acc))