 
clear all;
format long g;
MU1 = 45000;
MU2 = 60000;
SIGMA = 100;
 
T1 = normrnd(MU1,SIGMA,1,5000);
for i = 2:length(T1)
    T1(i) = T1(i)+T1(i-1);
end
T2 = normrnd(MU2,SIGMA,1,5000);
for i = 2:length(T2)
    T2(i) = T2(i)+T2(i-1);
end
 
T = [T1,T2];
[toa,idx] = sort(T);
idx(idx<=5000) = 0;
idx(idx>5000) = 1;

s = randsrc(1,length(T),[0 1;0 1]);

toa(s==0) = [];
idx(s==0) = [];
 
dlmwrite('toa.txt',int32(toa(1:1000)),'delimiter','\n','precision','%d');
dlmwrite('label.txt',int32(idx(1:1000)),'delimiter','\n','precision','%d');

