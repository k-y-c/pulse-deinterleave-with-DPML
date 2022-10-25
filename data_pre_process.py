import numpy as np
import pandas as pd

def process_data():
    path = './'
    file_name = 'MeasPDW_All.txt'
    file_dir = path + file_name
    data = pd.read_table(file_dir,sep='\s+',skiprows=2,names=["TOA(s)","RF(MHz)","fPW(us)","fPRI(us)", "fPA(dBm)","fAOA(deg)","iMOP","MOPParam","ID"])
    print(data)
    data['PRI'] = data['TOA(s)'].diff(periods = 1)
    data['ID'] -= data['ID'].min()
    # data.to_csv('allPDW.csv')
    data['TOA(s)'].to_csv('toa.txt',index=False,header=False)
    return data

if __name__ == '__main__':
    process_data()