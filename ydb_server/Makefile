CC = g++
AR = ar
CFLAGS = -Wall -I../ydb

SRCS = ydb_server.cpp
OBJS = ydb_server.o
LIBS = -lythread

all: ydb_server

.cpp.o:
	$(CC) $(CFLAGS) -c $<

#libydb.a: $(OBJS)
#	$(AR) rvu $@ $(OBJS)
#	ranlib $@

ydb_server: $(OBJS)
	$(CC) $(CFLAGS) -g -o $@ $(OBJS) $(LIBS)

test:	all
	./ydb_map_test
	./ydb_lock_test
	./ydb_sched_test
	./ydb_test

clean: 
	-rm -rf *.o *.a *.core core ydb_test ydb_lock_test ydb_sched_test
