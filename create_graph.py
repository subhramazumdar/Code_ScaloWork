import networkx as nx
import sys
import logging
import itertools
import time
import random
import hashlib

import math

def main():
	logging.basicConfig(format='\n%(levelname)s: %(message)s', level=logging.INFO)

	n=int(sys.argv[1])
	
	deg=int(sys.argv[2])
	p=float(deg/(n-1))
	print(p)
	G=nx.erdos_renyi_graph(n, p, seed=None, directed=False)
	fh = open(sys.argv[3], "wb")
	nx.write_edgelist(G,fh,data=False)
	fh.close()

if __name__ == '__main__':
    main()	    
