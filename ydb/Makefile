CC = g++
AR = ar
CFLAGS = -Wall

SRCS = ydb_lock.cpp ydb_op.cpp ydb_trans.cpp ydb_sched.cpp ydb_map.cpp ydb.cpp \
       ydb_tree.cpp
OBJS = ydb_lock.o ydb_op.o ydb_trans.o ydb_sched.o ydb_map.o ydb.o ydb_tree.o
LIBS = -lythread

all: ydb_test ydb_lock_test ydb_sched_test ydb_map_test ydb_tree_test

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -g -c $*.cpp

#libydb.a: $(OBJS)
#	$(AR) rvu $@ $(OBJS)
#	ranlib $@

ydb_tree_test: ydb_tree_test.cpp $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ ydb_tree_test.cpp $(OBJS) $(LIBS)

ydb_lock_test: ydb_lock_test.cpp $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ ydb_lock_test.cpp $(OBJS) $(LIBS)

ydb_sched_test: ydb_sched_test.cpp $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ ydb_sched_test.cpp $(OBJS) $(LIBS)

ydb_map_test: ydb_map_test.cpp $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ ydb_map_test.cpp $(OBJS) $(LIBS)

ydb_test: ydb_test.cpp $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ ydb_test.cpp $(OBJS) $(LIBS)

test:	all
	./ydb_map_test
	./ydb_lock_test
	./ydb_sched_test
	./ydb_test

clean: 
	-rm -rf *.o *.a *.core core ydb_test ydb_lock_test ydb_sched_test \
            ydb_server ydb_tree_test ydb_map_test 
