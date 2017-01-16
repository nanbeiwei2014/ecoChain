CC = g++
CFLAGS1  := -Wall -g
# -Wall -O2 -fPIC
CFLAGS  := -std=c++11  -Wall -g -fPIC   

OBJS = testbatchcase.o main.o http.o

blockchain_c_interface = blockchain_c_interface 
INCLUDE_PATH =-I ./  -I ../include/json -I ../include
LIB_PATH = -L  ../lib/lib_X64  -L ../lib


OPENCV_LIB = -ljson-c -lbtcsign -lgmp -lboost_system -lboost_thread -lcrypto  -lssl -ldl 

all : $(blockchain_c_interface)


%.o : %.cpp
	$(CC) $(CFLAGS)  -c $< -o $@  $(INCLUDE_PATH) $(LIB_PATH) $(OPENCV_LIB) 

$(blockchain_c_interface) : $(OBJS)
	$(CC) $^ $(LIB_PATH) -o $@   $(OPENCV_LIB) 


install:
	rm -rf ../bin/blockchain_c_interface
	cp blockchain_c_interface ../bin
	
clean:
	rm -f $(OBJS) $(blockchain_c_interface) *.o
	
