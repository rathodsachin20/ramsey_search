from random import random, randint, expovariate
from ramsey import *
import re

np.set_printoptions(threshold=np.nan)

def create_pop_from_indiv(indiv, pop_count=100):
    return [indiv]*pop_count

def read_graph(filename, n):
    try:
        l = []
        with open(filename, "r") as f:
            lines = f.readlines()
            #print lines
            for line in lines:
                #print line
                #ll = line.split("\s")
                #ll = re.split("\s|\t|\n", line)
                ll = [int(x) for x in line if x.isdigit()]
                #print ll, len(ll)
                l.extend([int(i) for i in ll])
        #print(l)
        if len(l)!=n*n:
            print "input file does not match given number of nodes."
            return None
        arr = np.asarray(l)
        arr = arr.reshape(n,n)
        return arr
        
    except Exception as ex:
        print "Exception in reading graph file:", ex

def individual(dim):
    return get_nparr(dim)
    #return [randint(low, high) for i in xrange(length)]

def population(count, dim, pop_in=None):
    if pop_in is not None:
        return pop_in
    return [individual(dim) for i in xrange(count)]

def fitness(individual, target):
    #score = sum(individual)
    score = clique_count(individual)
    if score==0:
        #print "Found counter-example of size:", len(individual)
        if(len(individual))>50:
            toprint = "found counter example of size:"+str(len(individual))
            print toprint
            #print individual
            try:
                with open('counters', 'a') as f:
                    arstr = ''.join([x for x in np.array_str(individual) if x.isdigit()])
                    f.write("\ncounter-example of size:"+str(len(individual))+":\n"+arstr)
            except Exception as ex:
                print "Error while writing:", ex
    return abs(score - target)

def grade(population, target, graded_list=None):
    if graded_list is not None:
        return min(graded_lis)[0]
    return min([fitness(i, target) for i in population])
    #total = sum([fitness(i, target) for i in population])
    #return total/(len(population)*1.0)

def crossover(male, female, male_i, female_i, total):
    dim = len(male)
    mscore = 1.0*(total-male_i)
    fscore = 1.0*(total-female_i)
    mscore *= mscore
    fscore *= fscore
    mprob = mscore/(mscore+fscore)
    total = dim*(dim-1)/2
    melem = int(total*mprob)
    #i_pos = int(mprob*dim)
    #j_pos = int(mprob*dim)
    #if i_pos > j_pos:
    #    temp = i_pos
    #    i_pos = j_pos
    #    j_pos = temp
    l = np.zeros((dim, dim), dtype=int)
    count = 0
    for i in xrange(dim):
        for j in range(i+1, dim):
            if count<melem:
                l[i][j] = male[i][j]
            else:
                l[i][j] = female[i][j]
            count += 1
    return l

def evolve(pop, target=0, retain=0.2, random_select=0.05, mutate=0.1):
    graded = [(fitness(x, target), x) for x in pop]
    #print graded
    #graded = [x[1] for x in sorted(graded)]
    g = sorted(graded, key=lambda x:x[0])
    counts = [x[0] for x in g]
    min_count = min(counts)
    num_zeros = counts.count(0)
    graded = [x[1] for x in g]
    retain_length = int(retain*len(graded))
    parents = graded[:retain_length]

    # randomly add some weak individuals - diversity
    for indiv in graded[retain_length:]:
        if random_select > random():
            parents.append(indiv)

    # mutate
    for indiv in parents:
        if mutate > random:
            i_pos = random(0, len(indiv)-1)
            j_pos = random(0, len(indiv)-1)
            if i_pos > j_pos:
                temp = i_pos
                i_pos = j_pos
                j_pos = temp
            #indiv[pos_to_mutate] = randint(min(indiv), max(indiv))
            indiv[i_pos][j_pos] = 1 - indiv[i_pos][j_pos]
            # try below line later
            #indiv[pos_to_mutate] = randint(min([i[pos_to_mutate] for i in parents]), max(i[pos_to_mutate] for i in parents]))

    # crossover
    num_parents = len(parents)
    num_children = len(pop) - num_parents
    children = []

    while len(children)<num_children:
        #male_i = randint(0, num_parents-1)
        #female_i = randint(0, num_parents-1)
        lambd = 0.04  # or ~0.05
        male_i = int(expovariate(lambd))%num_parents
        female_i  = int(expovariate(lambd))%num_parents
        if male_i != female_i:
            male = parents[male_i]
            female = parents[female_i]
            #half = int(len(male)/2)
            #i_pos = random(0, len(indiv)-1)
            #j_pos = random(0, len(indiv)-1)
            # try with random breakpoint
            child = crossover(male, female, male_i, female_i, len(parents))
            children.append(child)

    parents.extend(children)
    return (parents, num_zeros, min_count)


def ramsey_ga_onegen(pop_in, dim, pop_count, generations, target_count):
    #a = get_nparr(dim)
    target = 0
    p = population(pop_count, dim, pop_in)
    #fitness_history = [grade(p,target),]

    for i in xrange(generations):
        (p, zero_count, min_count) = evolve(p, target)
        #gr = grade(p, target)
        print "dim:", dim, "  poulation #counter-examples:", zero_count, ".. Mincount:",min_count
        if zero_count>=target_count:
            break
        #fitness_history.append(gr)

    #for line in fitness_history:
    #    print line
    #print fitness_history
    return p

def ramsey_ga(start_dim, end_dim, pop_count, generations, graphfile=None):
    if graphfile is not None:
        arr = read_graph( graphfile, start_dim)
        if arr is not None:
            pop_start = create_pop_from_indiv(arr, pop_count)
        else:
            print "Error reading graph"
            return
    else:
        pop_start = population(pop_count, start_dim)
    pop_cur = pop_start
    for dim in range(start_dim, end_dim+1):
        print "Evolving Generation of dimension:", dim
        pop_old = ramsey_ga_onegen(pop_cur, dim, pop_count, generations, 1)
        #pop_new = np.zeros((dim+1,dim+1), dtype=int)
        pop_new = []
        for indiv in pop_old:
            indiv_new = np.zeros((dim+1, dim+1), dtype=int)
            indiv_new[0:dim, 0:dim] = indiv
            for i in xrange(dim+1):
                indiv_new[i][dim] = randint(0,1)
                indiv_new[dim][i] = indiv_new[i][dim]
            pop_new.append(indiv_new)
        pop_cur = pop_new
        dim = dim +1

if __name__ == "__main__":
    #target = 371
    #pop_count = 100
    #i_length = 5
    #i_min = 0
    #i_max = 100
    #p = population(pop_count, i_length, i_min, i_max)
    #fitness_history = [grade(p, target),]

    #for i in xrange(100):
    #    p = evolve(p, target)
    #    fitness_history.append(grade(p, target))

    #for line in fitness_history:
    #    print line
    start = 101
    stop = 180
    pop = 100
    gen = 150
    #ramsey_ga(start, stop, pop, gen, graphfile)
    ramsey_ga(start, stop, pop, gen, "paley-101.txt")
