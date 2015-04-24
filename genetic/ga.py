from random import random, randint
from ramsey import *

def individual(dim):
    return get_nparr(dim)
    #return [randint(low, high) for i in xrange(length)]

def population(count, dim):
    return [individual(dim) for i in xrange(count)]

def fitness(individual, target):
    #score = sum(individual)
    score = clique_count(individual)
    if score==0:
        print "Found counter-example of size:", len(individual)
        print individual
    return abs(score - target)

def grade(population, target):
    return min([fitness(i, target) for i in population])
    #total = sum([fitness(i, target) for i in population])
    #return total/(len(population)*1.0)

def crossover(male, female, i_pos, j_pos):
    dim = len(male)
    if i_pos > j_pos:
        temp = i_pos
        i_pos = j_pos
        j_pos = temp
    l = np.zeros((dim, dim), dtype=int)
    for i in xrange(dim):
        for j in xrange(dim):
            if i<i_pos and j<j_pos:
                l[i][j] = male[i][j]
            else:
                l[i][j] = female[i][j]
    return l
    

def evolve(pop, target=0, retain=0.2, random_select=0.05, mutate=0.1):
    graded = [(fitness(x, target), x) for x in pop]
    #print graded
    #graded = [x[1] for x in sorted(graded)]
    g = sorted(graded, key=lambda x:x[0])
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
        male = randint(0, num_parents-1)
        female = randint(0, num_parents-1)
        if male != female:
            male = parents[male]
            female = parents[female]
            dim = len(male)
            #half = int(len(male)/2)
            i_pos = int(dim/2)
            j_pos = int(dim/2)
            #i_pos = random(0, len(indiv)-1)
            #j_pos = random(0, len(indiv)-1)
            # try with random breakpoint
            child = crossover(male, female, i_pos, j_pos)
            children.append(child)

    parents.extend(children)
    return parents



def ramsey_ga(dim, pop_count, generations):
    a = get_nparr(dim)
    target = 0
    p = population(pop_count, dim)
    fitness_history = [grade(p,target),]

    for i in xrange(generations):
        p = evolve(p, target)
        gr = grade(p, target)
        print "poulation score:", gr
        fitness_history.append(gr)

    for line in fitness_history:
        print line

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
    graph_size = 55
    pop = 100
    gen = 100
    ramsey_ga(graph_size, pop, gen)
