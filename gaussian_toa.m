 
clear all;
format long g;
MU1 = 45000;
MU2 = 60000;
SIGMA = 100;
MISS_RATE = 0.1;
SPURIOUS_RATE = 0;
 
T1 = normrnd(MU1,SIGMA,1,5000);
t1 = struct('toa',T1(1),'label',0);
for i = 2:length(T1)
    T1(i) = T1(i)+T1(i-1);
    t1 = [t1 struct('toa',T1(i),'label',0)];
end
T2 = normrnd(MU2,SIGMA,1,5000);
t2 = struct('toa',T2(1),'label',1);
for i = 2:length(T2)
    T2(i) = T2(i)+T2(i-1);
    t2 = [t2 struct('toa',T2(i),'label',1)];
end
 
T = [t1,t2];
[toa,idx] = sort([T.toa]);
T = T(idx);

s = randsrc(1,length(T),[0 1;MISS_RATE 1-MISS_RATE]);

T(s==0) = [];

t = T(end).toa;
spurious_toa = [];
for i = 1:floor(SPURIOUS_RATE*length(T))
    spurious_toa = [spurious_toa struct('toa',randi([1,floor(t)],1,1),'label',-1)];
end
T = [T,spurious_toa];
[toa,idx] = sort([T.toa]);
T = T(idx);
 
dlmwrite('toa.txt',int32([T(1:1000).toa]),'delimiter','\n','precision','%d');
dlmwrite('label.txt',int32([T(1:1000).label]),'delimiter','\n','precision','%d');

