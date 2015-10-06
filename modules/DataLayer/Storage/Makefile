CC=g++ -std=c++0x
EXEC=Storage
BIN=bin

THRIFT_STORAGE=../../thrift/Storage/gen-cpp
THRIFT_HBASE=../../thrift/Storage/hbase/gen-cpp

all: $(BIN) $(EXEC)

$(EXEC): base64.o cJSON.o                                                  \
         Hbase_constants1.o Hbase_types1.o hbase1.o                        \
         VELaSSCo.o velassco_types.o velassco_constants.o velasscoThrift.o \
         storageModule.o HBase.o EDM.o Helpers.o main.o \
	 HBase_status.o HBase_model_operations.o
	$(CC) $(BIN)/*.o -L/usr/local/lib -lthrift -lcurl -o $(BIN)/$(EXEC)

$(BIN):
	clear
	clear
	mkdir $(BIN)


main.o: main.cpp
	$(CC) -c main.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -I/usr/local/include -o $(BIN)/main.o

Helpers.o: Helpers.cpp
	$(CC) -c Helpers.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -I/usr/local/include -o $(BIN)/Helpers.o

HBase.o: HBase.cpp
	$(CC) -c HBase.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -I/usr/local/include -o $(BIN)/HBase.o

HBase_status.o: HBase_status.cpp
	$(CC) -c HBase_status.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -I/usr/local/include -o $(BIN)/HBase_status.o

HBase_model_operations.o: HBase_model_operations.cpp
	$(CC) -c HBase_model_operations.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -I/usr/local/include -o $(BIN)/HBase_model_operations.o

EDM.o: EDM.cpp
	$(CC) -c EDM.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -I/usr/local/include -o $(BIN)/EDM.o


velasscoThrift.o: 
	$(CC) -c velasscoThrift.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE) -o $(BIN)/velasscoThrift.o


velassco_constants.o: 
	$(CC) -c $(THRIFT_STORAGE)/VELaSSCo_constants.cpp -I$(THRIFT_STORAGE) -o $(BIN)/velassco_constants.o


velassco_types.o: 
	$(CC) -c $(THRIFT_STORAGE)/VELaSSCo_types.cpp -I$(THRIFT_STORAGE) -o $(BIN)/velassco_types.o


VELaSSCo.o: 
	$(CC) -c $(THRIFT_STORAGE)/VELaSSCo.cpp -I$(THRIFT_STORAGE) -o $(BIN)/VELaSSCo.o


storageModule.o:
	$(CC) -c storageModule.cpp -I$(THRIFT_STORAGE) -I$(THRIFT_HBASE)  -o $(BIN)/storageModule.o	

cJSON.o: cJSON.c
	$(CC) -c cJSON.c -o $(BIN)/cJSON.o


Hbase_constants1.o: 
	$(CC) -std=c++0x -c $(THRIFT_HBASE)/Hbase_constants.cpp  -I$(THRIFT_HBASE)  -o $(BIN)/Hbase_constants1.o

Hbase_types1.o: 
	$(CC) -std=c++0x -c $(THRIFT_HBASE)/Hbase_types.cpp  -I$(THRIFT_HBASE)  -o $(BIN)/Hbase_types1.o

hbase1.o: 
	$(CC) -std=c++0x -c $(THRIFT_HBASE)/Hbase.cpp  -I$(THRIFT_HBASE)  -o $(BIN)/hbase1.o

base64.o:
	$(CC) -std=c++0x -c base64.cpp  -I$(THRIFT_HBASE)  -o $(BIN)/base64.o

clean:
	rm -rf $(BIN)

.PHONY: clean
