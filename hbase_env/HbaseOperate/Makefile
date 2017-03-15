all:libhbcl.so test
.PHONY:all 

THRIFT_DIR = ./thrift
LIB_DIR = /usr/local/lib

GEN_SRC = ./gen-cpp/Hbase.cpp \
	 ./gen-cpp/Hbase_types.cpp \
	 ./gen-cpp/Hbase_constants.cpp


libhbcl.so:*.o 
	g++  -g  -fPIC  -shared  -o libhbcl.so  *.o  -lthrift  -llog4cxx -lapr-1 -laprutil-1 -ldl -L ./lib -L /usr/local/apr/lib

*.o:*.cpp ${GEN_SRC}
	g++ -g -fPIC -c *.cpp  ${GEN_SRC}   -I${THRIFT_DIR}  -I./gen-cpp   -I./ -I./thrift  -Wl,-rpath,${LIB_DIR}	
test: main.c 
	g++ -g -o test main.c -I./ -I./thrift -I./gen-cpp -I./log4cxx -L ./   -lhbcl -ldl

.PHONY : clean
clean:
	rm -rf test *.so *.o
