import itertools
from subprocess import call
import os

count = 0

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

def flipfile2(from_file , to_file):
	result = flipfile(from_file)
	resultfile = open(to_file, 'w')
	resultfile.write('64\n')
	for r in result:
		resultfile.write(r+"\n")

def getArray(fname):
	result = []
	with open(fname) as f:
		idx = 0
		for line in f:
			if idx > 0:
				result.append( line.strip() )
			idx += 1
	return result

def joingraph(a1, a2, perm=None):
	result = []
	for idx in range(0, len(a1)):
		tmp = a1[idx] +" "+ a2[idx]
		result.append(tmp)
	return result

def checkCE(fname):
	CE = 0
	from subprocess import Popen, PIPE
	p = Popen(['./test_clique_count', '-f', fname], stdin=PIPE, stdout=PIPE, stderr=PIPE)
	output, err = p.communicate(b" stdin")
	if "counter example" in output:
		CE+=1
	print output
	rc = p.returncode
	return CE


def reanneal(fname):
	CE = 0
	from subprocess import Popen, PIPE
	p = Popen(['./anneal', '-f', fname], stdin=PIPE, stdout=PIPE, stderr=PIPE)
	output, err = p.communicate(b" stdin")
	if "counter example" in output:
		CE+=1
	rc = p.returncode
	return CE

def splitgraph(resultFileName, resultFileSize, removeFlag = False):
	resultFile = open(resultFileName, "r")
	resultContent = []
	idx = 0
	for r in resultFile:
		if idx > 0:
			resultContent.append(r.rstrip('\n'))
		idx += 1

	splitSize = resultFileSize / 2

	splitGraph = []
	splitGraph.append ( resultContent[:splitSize] )
	splitGraph.append ( resultContent[splitSize: resultFileSize] )

	newGraph = []
	idx = 1
	fnames = []
	for graph in splitGraph:
		subGraph_1 = []
		subGraph_2 = []
		for line in graph:
			subGraph_1.append( line[:resultFileSize] )
			subGraph_2.append( line[resultFileSize: 2*resultFileSize] )
		fnames.append ( writegraph(idx,subGraph_1, len(subGraph_1), len(subGraph_1[0]) ) )
		fnames.append ( writegraph(idx+1,subGraph_2, len(subGraph_2), len(subGraph_2[0])) )
		idx +=2

	CE = []
	for fname in fnames:
		CE.append( (fname, checkCE(fname)) )
		if removeFlag:
			os.remove(fname)
	return CE
	#call(["./test_clique_count", "-f", fname])

def writegraph(idx, graph, size_graph, size_lines ):
	global count
	count +=1
	fname = "graph"+str(size_graph)+"_"+str(idx)+str(count)+".state"
	f = open(fname, 'w+')
	f.write(str(size_graph)+"\n")
	for line in graph:
		f.write(line+"\n")
	f.close()
	return fname

def combinegraphs(file_names, val):
	graphs = []
	for name in file_names:
		graphs.append(getArray(name))
	r1 = joingraph(graphs[0], graphs[1])
	r2 = joingraph(graphs[2], graphs[3])
	r1.extend(r2)
	return writegraph(2, r1, val, val)

def split(val, fname, keep=False):
	sval = val /2
	for y in [val]:
		result = splitgraph( fname, y , keep)
		output = []
		for fname2, val in result:
			if val == 0:
				output.append(fname2)
				output.append(split(sval, fname2))
			else:
				os.remove(fname2)
		return output

def join(fnames, val):
	sval = val /2 
	
	if val > 16:
		for x in range(0,4):
			print "file-name: ",fnames[x]
			print str(x), splitgraph(fnames[x], sval, True)

	filename = combinegraphs(fnames, val)
	return filename, checkCE(filename)


def generateseeds(num):
	count= 0

	for i in range(0, num):
		f = open("seed"+str(count)+".state", "w+")
		f.write(str('8')+"\n")
		for i in xrange(0, 8):
			for j in xrange(0, 8):
				if i*8 + j == count:
					f.write(str('1 '))
				else:
					f.write(str('0 '))
			f.write('\n')
		f.close()
		count += 1

#generateseeds(10)

print splitgraph( "temp.state", 122 , False)
#fnames = ['graph61_1.state', 'graph61_2.state', 'graph61_3.state', 'graph61_4.state']
#join(fnames, 122)


''''
import itertools

fnames = ['graph32_225.state', 'graph32_226.state', 'graph32_227.state', 'graph32_227.state']
size = 32

perms = itertools.permutations(fnames)
newlist = []
fcount = 0
for perm in perms:
	fcount +=1
	filename, val = join(perm, size*2)
	print filename, val
	os.remove(filename)
	if val == 0:
		reanneal(filename)
	print filename, checkCE(filename)
	newlist.append(filename)
	if fcount == 4:
		break
size *=2'''
