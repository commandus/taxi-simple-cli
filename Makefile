GEN-CPP=\
	thrift/PassengerService.cpp \
	thrift/taxi_constants.cpp \
	thrift/taxi_types.cpp 
OBJS=\
	client.o \
	taxi-simple-cli.o \
	utiltime.cpp

INCS=\
	-I. \
	-Ithrift \
	-I/usr/local/include \
	-I../../../thrift/gen-cpp \
	-I/home/andrei/src/boost_1_56_0

LIBS=\
	-L/usr/local/lib \
	-L/home/andrei/src/boost_1_56_0/stage/lib \
	-lthrift \
	-largtable2 \
	-lstdc++
all:	taxi-simple-cli
taxi-simple-cli: $(OBJS)
	g++ -o $@ $(OBJS) $(GEN-CPP) $(LIBS)
.c.o:
	cc -c -o $@ $(INCS) $? 
.cpp.o:
	g++ -c -o $@ $(INCS) $?

clean:
	rm *.o
