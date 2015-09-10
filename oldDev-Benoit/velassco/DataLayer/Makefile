CC=g++ -std=c++0x
EXEC=dataLayer
BIN=bin


all: $(BIN) $(EXEC)

$(EXEC): base64.o  Hbase_constants1.o Hbase_types1.o hbase1.o  VELaSSCo.o cJSON.o velassco_types.o velassco_constants.o  storageModule.o velasscoThrift.o main.o
	$(CC) $(BIN)/*.o -L/usr/local/lib -lthrift -lcurl -o $(BIN)/$(EXEC)
	./$(BIN)/$(EXEC)




$(BIN):
	clear
	clear
	mkdir $(BIN)

main.o: main.cpp
	$(CC) -c main.cpp   -I../gen-cpp/ -I/usr/local/include -I/usr/local/include -I../thrift/hbase/gen-cpp/ -o $(BIN)/main.o

velasscoThrift.o: 
	$(CC) -c velasscoThrift.cpp -I../gen-cpp/ -o $(BIN)/velasscoThrift.o


velassco_constants.o: 
	$(CC) -c ../gen-cpp/velassco_constants.cpp -I../gen-cpp/ -o $(BIN)/velassco_constants.o


velassco_types.o: 
	$(CC) -c ../gen-cpp/velassco_types.cpp -I../gen-cpp/ -o $(BIN)/velassco_types.o


VELaSSCo.o: 
	$(CC) -c ../gen-cpp/VELaSSCo.cpp -I../gen-cpp/ -o $(BIN)/VELaSSCo.o


storageModule.o:
	$(CC) -c storageModule.cpp -I../gen-cpp/ -o $(BIN)/storageModule.o	

cJSON.o: cJSON.c
	$(CC) -c cJSON.c -o $(BIN)/cJSON.o


Hbase_constants1.o: 
	$(CC) -std=c++0x -c ../thrift/hbase/gen-cpp/Hbase_constants.cpp  -I../thrift/hbase/gen-cpp/  -o $(BIN)/Hbase_constants1.o

Hbase_types1.o: 
	$(CC) -std=c++0x -c ../thrift/hbase/gen-cpp/Hbase_types.cpp  -I../thrift/hbase/gen-cpp/  -o $(BIN)/Hbase_types1.o

hbase1.o: 
	$(CC) -std=c++0x -c ../thrift/hbase/gen-cpp/Hbase.cpp  -I../thrift/hbase/gen-cpp/  -o $(BIN)/hbase1.o

base64.o:
	$(CC) -std=c++0x -c base64.cpp  -I../thrift/hbase/gen-cpp/  -o $(BIN)/base64.o

clean:
	rm -rf $(BIN)

.PHONY: clean