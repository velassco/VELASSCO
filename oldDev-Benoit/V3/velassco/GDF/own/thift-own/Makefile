THRIFT_DIR = /usr/local/include
LIB_DIR = /usr/local/lib

GEN_SRC = ./gen-cpp/com_constants.cpp \
	  ./gen-cpp/com_types.cpp \
	  ./gen-cpp/ServerService.cpp


all: GEN_CPP DemoClient DemoServer

default: GEN_CPP DemoClient DemoServer

GEN_CPP:
	thrift -r --gen cpp com.thrift 

DemoClient: Client.cpp
	g++ -o DemoClient -I${THRIFT_DIR} -I./gen-cpp -L${LIB_DIR}  -lthrift Client.cpp ${GEN_SRC}


DemoServer: Server.cpp
	g++ -o DemoServer -I${THRIFT_DIR} -I./gen-cpp -L${LIB_DIR}  -lthrift Server.cpp ${GEN_SRC}


clean:
	rm -rf DemoClient DemoServer gen-cpp

