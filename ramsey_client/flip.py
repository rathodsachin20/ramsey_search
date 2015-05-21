import itertools
from subprocess import call
import os

def flipfile(fname):
	result = []
	with open(fname) as f:
		idx = 0
		for line in f:
			if idx > 0:
				tmp = line.strip().replace('0','6')
				tmp = tmp.replace('1','7')
				tmp = tmp.replace('7', '0')
				tmp = tmp.replace('6', '1')
				result.append( tmp )
			idx +=1
	return result

def permutations(iterable, r=None):
    pool = tuple(iterable)
    n = len(pool)
    r = n if r is None else r
    for indices in product(range(n), repeat=r):
        if len(set(indices)) == r:
            yield tuple(pool[i] for i in indices)

def flipfile2(from_file , to_file):
	result = flipfile(from_file)
	resultfile = open(to_file, 'w')
	resultfile.write('64\n')
	for r in result:
		resultfile.write(r+"\n")

def getPerms():
	d = range(0,3)
	e = len(d)
	idx = 1
	for p in itertools.permutations(d, e):
	    idx +=1
	return p

def getArray(fname):
	result = []
	with open(fname) as f:
		idx = 0
		for line in f:
			if idx > 0:
				result.append( line.strip() )
			idx += 1
	return result

def combine(a1, a2, perm=None):
	result = []
	for idx in range(0, len(a1)):
		tmp = a1[idx] +" "+ a2[idx]
		result.append(tmp)
	return result

def run():
	array = []
	for num in range(1, 11):
		array.append( getArray("graph64_"+str(num)+".state") )

	d = range(1,10)
	e = 4
	idx = 1
	for p in itertools.permutations(d, e):
		idx += 1
		r1 = combine(array[ p[0] ], array[ p[1] ] )
		r2 = combine(array[ p[2] ], array[ p[3] ] )
		r1.extend(r2)

		fname= "graph128_"+str(idx)+".state"
		resultfile = open(fname, 'w')

		resultfile.write('128\n')
		for r in r1:
			r = r.replace("-1048575", "0")
			r = r.replace("1048576", "")
			resultfile.write(r+"\n")
		resultfile.close()
		call(["./test_clique_count", "-f", fname])
		os.remove(fname) 

		r1 = combine(array[ p[0] ], array[ p[1] ] )
		r2 = combine(array[ p[2] ], array[ p[3] ] )
		r1.extend(r2)

		fname= "graph128_"+str(idx)+".state"
		resultfile = open(fname, 'w')

		resultfile.write('128\n')
		for r in r1:
			r = r.replace("-1048575", "0")
			r = r.replace("1048576", "")
			resultfile.write(r+"\n")
		resultfile.close()
		call(["./test_clique_count", "-f", fname])
		os.remove(fname) 

run()
