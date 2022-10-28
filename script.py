import os

beta = 0
for i in range(100):
    os.system("build/mldp_a2_for_spurious "+str(beta))
    beta -= 1