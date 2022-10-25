import torch
from torch import nn
import torch.nn.functional as F
import numpy as np
import pandas as pd
import torch.utils.data as Data

class LstmRNN(nn.Module):
    def __init__(self,input_size,hidden_size=64,output_size=1,num_layers=1):
        super().__init__()

        self.lstm = nn.LSTM(input_size,hidden_size,num_layers,bidirectional=True,batch_first=True)
        self.fullconnect = nn.Linear(in_features=hidden_size*2,out_features=output_size)
        self.softmax = nn.Softmax()

    def forward(self,_x):
        x,_ = self.lstm(_x)
        s,b,h = x.shape
        # x = x.view(s*b,h)
        x = self.fullconnect(x)
        # x = self.softmax(x)

        # x = x.view(s,b,-1)
        return x


def read_data():
    path = './'
    file_name = 'MeasPDW_All.txt'
    file_dir = path + file_name
    data = pd.read_table(file_dir,sep='\s+',skiprows=2,names=["TOA(s)","RF(MHz)","fPW(us)","fPRI(us)", "fPA(dBm)","fAOA(deg)","iMOP","MOPParam","ID"])
    print(data)
    data['PRI'] = data['TOA(s)'].diff(periods = 1)
    data['ID'] -= data['ID'].min()
    # data.to_csv('allPDW.csv')
    # data['TOA(s)'].to_csv('toa.txt',index=False,header=False)
    return data

if __name__ == "__main__":
    torch.cuda.set_device(5)
    print(torch.cuda.is_available())
    data = read_data()
    pri_data = data['PRI'].to_numpy()[1:].astype('float32')   #丢掉第一个数据
    label_data = data['ID'].to_numpy()[1:]

    data_len = label_data.size
    
    # create dataset
    dataset = np.zeros((data_len, 2))
    dataset[:,0] = pri_data
    dataset[:,1] = label_data
    # dataset = dataset.astype('float32')

    # train data
    time_len = 1000
    batch_Size = 5
    train_data_ratio = 0.5
    train_data_len = int(data_len*train_data_ratio/time_len)*time_len
    train_x = dataset[:train_data_len,0]
    train_y = dataset[:train_data_len,1].astype('int64')
    INPUT_FEATRUE_NUM = 1
    OUTPUT_FEATRUE_NUM = label_data.max()+1

    # test data
    test_x = dataset[train_data_len:,0]
    test_y = dataset[train_data_len:,1].astype('int64')
    
    # train
    hidden_size = 256

    # transfer data to pytorch tensor
    train_x_tensor = train_x.reshape(-1,time_len,INPUT_FEATRUE_NUM) # shape: (-1,time_len,1)
    train_x_tensor = torch.from_numpy(train_x_tensor).to(torch.float32)
    train_x_tensor = F.normalize(train_x_tensor) # normalize

    train_y_tensor = train_y.reshape(-1,time_len) # shape: (-1,time_len)
    train_y_tensor = torch.from_numpy(train_y_tensor).long()
    # train_y_tensor = torch.eye(OUTPUT_FEATRUE_NUM)[train_y_tensor.long(),:] # one hot vector

    torch_dataset = Data.TensorDataset(train_x_tensor,train_y_tensor)
    loader = Data.DataLoader(dataset=torch_dataset,batch_size=batch_Size,shuffle=True,num_workers=2)
    
    lstm_model = LstmRNN(INPUT_FEATRUE_NUM,hidden_size,OUTPUT_FEATRUE_NUM,1).cuda()

    loss_function = nn.CrossEntropyLoss().cuda()
    optimizer = torch.optim.Adam(lstm_model.parameters(),lr=1e-3)

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
        
        print('Epoch [{}/{}], Loss: {:.5f}'.format(epoch+1,max_epochs,loss.item()))
        if loss.item() < 1e-4:
            print('Epoch [{}/{}], Loss: {:.5f}'.format(epoch+1,max_epochs,loss.item()))
            print('The loss value is reached')
            break
        # elif (epoch+1) % 100 == 0:
        #     print('Epoch [{}/{}], Loss: {:.5f}'.format(epoch+1,max_epochs,loss.item()))
     