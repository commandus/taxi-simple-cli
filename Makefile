OBJS=\
	client.o \
	taxi-simple-cli.o \
	argvcharsetconv.o

INCS=\
	-I. \
	-I/usr/local/include \
	-I../../../thrift/gen-cpp \
	-I/home/andrei/src/boost_1_56_0

LIBS=\
	-L/usr/local/lib \
	-L/home/andrei/src/boost_1_56_0/stage/lib \
	-largtable2	

all:	taxi-client
taxi-client: $(OBJS)
	cc -o $@ $(OBJS)
.c.o:
	cc -c -o $@ $(INCS) $?
.cpp.o:
	c++ -c -o $@ $(INCS) $(LIBS) $?

