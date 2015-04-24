import numpy as np


def clique_count(a):
    count = 0
    gsize = len(a)
    sgsize = 7
    gs = gsize-sgsize
    for i in xrange(gs+1):
        for j in range(i+1, gs+2):
            for k in range(j+1, gs+3):
                ij = a[i][j]
                if ij==a[i][k] and ij==a[j][k]:
                    for l in range(k+1, gs+4):
                        if ij==a[i][l] and ij==a[j][l] and ij==a[k][l]:
                            for m in range(l+1, gs+5):
                                if ij==a[i][m] and ij==a[j][m] and ij==a[k][m] and ij==a[l][m]:
                                    for n in range(m+1, gs+6):
                                        if ij==a[i][n] and ij==a[j][n] and ij==a[k][n] and ij==a[l][n] and ij==a[m][n]:
                                            for o in range(n+1, gs+7):
                                                if ij==a[i][o] and ij==a[j][o] and ij==a[k][o] and ij==a[l][o] and ij==a[m][o] and ij==a[n][o]:
                                                    count += 1

    return count


def get_nparr(dim):
    l = [0]*dim*dim
    ar = np.asarray[l]
    ar = np.reshape(dim, dim)
    return ar


if __name__ == "__main__":
    dim = 10
    l = [0]*dim*dim
    ar = np.asarray(l)
    ar = ar.reshape(dim, dim)
    print ar
    print "cliques=", clique_count(ar)


