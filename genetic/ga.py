from random import random, randint

def individual(length, low, high):
    return [randint(low, high) for i in xrange(length)]

def population(count, length, low, high):
    return [individual(length, low, high) for i in xrange(count)]

def fitness(individual, target):
    score = sum(individual)
    return abs(score - target)

def grade(population, target):
    total = sum([fitness(i, target) for i in population])
    return total/(len(population)*1.0)

def evolve(pop, target, retain=0.2, random_select=0.05, mutate=0.1):
    graded = [(fitness(x, target), x) for x in pop]
    graded = [x[1] for x in sorted(graded)]
    retain_length = int(retain*len(graded))
    parents = graded[:retain_length]

    # randomly add some weak individuals - diversity
    for indiv in graded[retain_length:]:
        if random_select > random():
            parents.append(indiv)

    # mutate
    for indiv in parents:
        if mutate > random:
            pos_to_mutate = random(0, len(indiv)-1)
            indiv[pos_to_mutate] = randint(min(indiv), max(indiv))
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
            half = int(len(male)/2)
            # try with random breakpoint
            child = male[:half] + female[half:]
            children.append(child)

    parents.extend(children)
    return parents


if __name__ == "__main__":
    target = 371
    pop_count = 100
    i_length = 5
    i_min = 0
    i_max = 100
    p = population(pop_count, i_length, i_min, i_max)
    fitness_history = [grade(p, target),]

    for i in xrange(100):
        p = evolve(p, target)
        fitness_history.append(grade(p, target))

    for line in fitness_history:
        print line
