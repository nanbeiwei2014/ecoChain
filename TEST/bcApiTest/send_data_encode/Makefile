CC = g++
CFLAGS1  := -Wall -g
# -Wall -O2 -fPIC
CFLAGS  := -std=c++11  -Wall -g -fPIC   

OBJS = main.o http.o

send_data_encode = send_data_encode 
INCLUDE_PATH = -I ./   -I ../include/json   -I ../include
LIB_PATH = -L  ../lib/lib_X64  -L ../lib


OPENCV_LIB = -ljson-c -lbtcsign -lgmp -lboost_system -lboost_thread -lcrypto  -lssl -ldl 

all : $(send_data_encode)


%.o : %.c
	$(CC) $(CFLAGS)  -c $< -o $@  $(INCLUDE_PATH) $(LIB_PATH) $(OPENCV_LIB) 

$(send_data_encode) : $(OBJS)
	$(CC) $^ $(LIB_PATH) -o $@   $(OPENCV_LIB) 


install:
	rm -rf ../bin/send_data_encode
	cp send_data_encode ../bin
	
clean:
	rm -f $(OBJS) $(send_data_encode) *.o
	
