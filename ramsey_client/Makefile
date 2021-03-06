CC = gcc -O2
TOP = .
INCLUDES = -I$(TOP)
CFLAGS = -g
THREADS = -lpthread
COMPILE = $(CC) $(INCLUDES) $(CFLAGS) 

INCL=./graph_utils.h ./jrb.h ./jval.h ./dllist.h ./fifo.h 
COMMON=.
PRED=.
BOBJ= $(COMMON)/jval.o $(COMMON)/jrb.o $(COMMON)/dllist.o $(COMMON)/fifo.o 
LIBS= -lm 

PROGRAMS =  ramsey_ce204 simple_taboo_search simple_taboo_search-7 test_clique_count multi_flip multi_flip_3 mf3n ramsey_proj ramsey_proj_basic ramsey_proj_basic_3 ramsey_proj_new hybrid_search ramsey_graph ramsey_multi_thread

all: $(PROGRAMS)

ramsey_graph: ramsey_graph.c $(INCL) $(BOBJ)
	$(COMPILE) ${INCLUDES} -o ramsey_graph ramsey_graph.c $(BOBJ) $(LIBS)

ramsey_multi_thread: ramsey_multi_thread.c $(INCL) $(BOBJ)
	$(COMPILE) $(THREADS) ${INCLUDES} -o ramsey_multi_thread ramsey_multi_thread.c $(BOBJ) $(LIBS)

anneal: anneal.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o anneal anneal.c $(BOBJ) $(LIBS)

random: simple_taboo_search-random.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o simple_taboo_search-random simple_taboo_search-random.c $(BOBJ) $(LIBS)

multi_flip: ramsey_multi_flip.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_multi_flip ramsey_multi_flip.c $(BOBJ) $(LIBS)

multi_flip_3: ramsey_multi_flip_3.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_multi_flip_3 ramsey_multi_flip_3.c $(BOBJ) $(LIBS)

ramsey_ce204: ramsey_ce204.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_ce204 ramsey_ce204.c $(BOBJ) $(LIBS)

mf3n: ramsey_mf3_new.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_mf3_new ramsey_mf3_new.c $(BOBJ) $(LIBS)

ramsey_proj: ramsey_proj.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_proj ramsey_proj.c $(BOBJ) $(LIBS)

ramsey_proj_basic: ramsey_proj_basic.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_proj_basic ramsey_proj_basic.c $(BOBJ) $(LIBS)

ramsey_proj_basic_3: ramsey_proj_basic_3.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_proj_basic_3 ramsey_proj_basic_3.c $(BOBJ) $(LIBS)

ramsey_proj_new: ramsey_proj_new.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_proj_new ramsey_proj_new.c $(BOBJ) $(LIBS)

hybrid_search: simple_hybrid_search-7.c  $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o ramsey_hybrid_search simple_hybrid_search-7.c $(BOBJ) $(LIBS)

simple_taboo_search: simple_taboo_search.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o simple_taboo_search simple_taboo_search.c $(BOBJ) $(LIBS)

simple_taboo_search-7: simple_taboo_search-7.c $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o simple_taboo_search-7 simple_taboo_search-7.c $(BOBJ) $(LIBS)

test_clique_count: test_clique_count.c graph_utils.o graph_utils.h clique_count.o clique_count.h $(INCL) ${BOBJ}
	$(COMPILE) ${INCLUDES} -o test_clique_count test_clique_count.c clique_count.o graph_utils.o $(BOBJ) $(LIBS)

fifo.o: fifo.c fifo.h jrb.h jval.h dllist.h
	$(COMPILE) ${INCLUDES} -c fifo.c

jrb.o: jrb.c jrb.h jval.h
	$(COMPILE) ${INCLUDES} -c jrb.c

jval.o: jval.c jval.h
	$(COMPILE) ${INCLUDES} -c jval.c

dllist.o: dllist.c dllist.h jval.h
	$(COMPILE) ${INCLUDES} -c dllist.c

graph_utils.o: graph_utils.c graph_utils.h
	$(COMPILE) ${INCLUDES} -c graph_utils.c

clique_count.o: clique_count.c clique_count.h
	$(COMPILE) ${INCLUDES} -c clique_count.c

connect.o: connect.c connect.h
	$(COMPILE) ${INCLUDES} -c connect.c

clean:
	rm -f *.o core ramsey_multi_flip_3 ramsey_proj ramsey_proj_basic ramsey_proj_new ramsey_proj_basic_3 ramsey_mf3_new ramsey_ce204
