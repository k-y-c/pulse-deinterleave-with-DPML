import os

#A1
# beta = 0
# rate = 0
# for i in range(11):
#     rate = 0
#     for i in range(11):
#         os.system("build/mldp_A4_spurious "+str(beta)+" "+str(rate))
#         rate += 5
#         # os.system("build/mldp_A4_spurious "+str(beta))
#     beta -= 2

#A2
# rate = 0
# type_ = ['jitter','sinusoidal','sliding','stagger']
# # type_ = ['stagger']
# for t in type_:
#     rate = 0
#     for i in range(11):
#         os.system("build/mldp_A4_spurious "+str(t)+" "+str(rate))
#         rate += 5

# A3
# rate = 0
# type_ = ['jitter','sinusoidal','sliding','stagger']
# # type_ = ['stagger']
# for t in type_:
#     rate = 0
#     # for i in range(11):
#     os.system("build/mldp_A4_spurious_A3 "+str(t)+" "+str(rate))
#         # rate += 5

#B1
# rate = 0
# # type_ = ['jitter','sinusoidal','sliding','stagger']
# type_ = ['stagger']
# for t in type_:
#     rate = 0
#     for i in range(11):
#         os.system("build/mldp_A4 "+str(t)+" "+str(rate))
#         os.system("build/mldp_A4_miss "+str(t)+" "+str(rate))
#         rate += 5

#LSTM
# rate = 0
# type_ = ['jitter','sinusoidal','sliding','stagger']
# # type_ = ['stagger']
# for t in type_:
#     rate = 0
#     for i in range(11):
#         os.system("python3 lstm.py --TYPE "+str(t)+" --RATE "+str(rate))
#         # os.system("build/mldp_A4_miss "+str(t)+" "+str(rate))
#         rate += 5

        
# DAE
# rate = 0
# type_ = ['sinusoidal','sliding','stagger','jitter']
# # type_ = ['stagger']
# for t in type_:
#     rate = 0
#     for i in range(11):
#         os.system("python3 dae_spurious.py --TYPE "+str(t)+" --RATE "+str(rate))
#         # os.system("build/mldp_A4_miss "+str(t)+" "+str(rate))
#         rate += 5

#DAE A3
# rate = 0
type_ = ['jitter','sinusoidal','sliding','stagger']
# type_ = ['stagger']
for t in type_:
    rate = 0
    # for i in range(11):
    os.system("python3 dae_spurious_a3.py --TYPE "+str(t)+" --RATE "+str(rate))
        # rate += 5