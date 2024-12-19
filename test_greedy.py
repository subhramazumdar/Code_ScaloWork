import os
import sys
s=[2500000, 5000000]
os.system("gcc -O3 testDisgreedy.c -ligraph -lm -fopenmp -o greed1")
for n in s:
    p=[50, 75, 100,125]
    for i in p:
        k="data/read_ungraph_edge_"+str(n)+"_"+str(i)+".txt"
        print(k)
        print(i)
        os.system("./greed1 "+str(k)+" out_greedy_ICDCS.txt")


