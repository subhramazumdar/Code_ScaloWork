import os
import sys
s=[500000, 2500000, 5000000]
os.system("gcc -O3 chrisimos_test.c -I/usr/include -ligraph -lssl -lcrypto -lm -o chrisimos_test")
for n in s:
    p=[50, 75, 100,125]
    for i in p:
        k="data/read_ungraph_edge_"+str(n)+"_"+str(i)+".txt"
        print(k)
        print(i)
        os.system("./chrisimos_test "+str(k)+" out_chrisimos.txt")


