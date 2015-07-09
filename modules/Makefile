CC=g++ -std=c++0x
DL=dataLayer
EL=engineLayer
BIN=bin


all: $(BIN) $(DL) $(EL)

$(DL): $(BIN)/$(DL)/cJSON.o $(BIN)/$(DL)/base64.o  $(BIN)/$(DL)/Hbase_constants1.o $(BIN)/$(DL)/Hbase_types1.o $(BIN)/$(DL)/hbase1.o $(BIN)/$(DL)/hbaseaccess.o $(BIN)/$(DL)/main.o
	$(CC) $(BIN)/$(DL)/*.o -L/usr/local/lib -lthrift -Ithrift/hbase/gen-cpp/ -IStorage/gen-cpp -IStorage/gen-cpp -IStorage/header -o $(BIN)/$(DL).exe
$(EL):
	
$(BIN):
	clear
	clear
	mkdir $(BIN)
	mkdir $(BIN)/$(DL)
	mkdir $(BIN)/$(EL)

$(BIN)/$(DL)/main.o: $(BIN)/$(DL)/hbaseaccess.o
	$(CC) -c Storage/main.cpp -L/usr/local/lib -IStorage/header -lthrift -Ithrift/hbase/gen-cpp/ -Ithrift/hbase/gen-cpp2/ -IStorage/gen-cpp -o $(BIN)/$(DL)/main.o

$(BIN)/$(DL)/hbaseaccess.o: Storage/src/hbaseaccess.cpp
	$(CC) -c Storage/src/hbaseaccess.cpp  -L/usr/local/lib -lthrift -Ithrift/hbase/gen-cpp/ -IStorage/gen-cpp -IStorage/gen-cpp -IStorage/header -o $(BIN)/$(DL)/hbaseaccess.o

$(BIN)/$(DL)/abstractAcces.o: Storage/src/abstractAcces.cpp
	$(CC) -c Storage/src/abstractAcces.cpp -IStorage/gen-cpp -IStorage/header -o $(BIN)/$(DL)/abstractAcces.o

$(BIN)/$(DL)/Hbase_constants1.o: 
	$(CC) -std=c++0x -c thrift/hbase/gen-cpp/Hbase_constants.cpp  -Ithrift/hbase/gen-cpp/  -o $(BIN)/$(DL)/Hbase_constants1.o

$(BIN)/$(DL)/Hbase_types1.o: 
	$(CC) -std=c++0x -c thrift/hbase/gen-cpp/Hbase_types.cpp  -Ithrift/hbase/gen-cpp/  -o $(BIN)/$(DL)/Hbase_types1.o

$(BIN)/$(DL)/hbase1.o: 
	$(CC) -std=c++0x -c thrift/hbase/gen-cpp/Hbase.cpp  -Ithrift/hbase/gen-cpp/  -o $(BIN)/$(DL)/hbase1.o

$(BIN)/$(DL)/cJSON.o: 
	$(CC) -c lib/JSON/cJSON.c -o $(BIN)/$(DL)/cJSON.o

$(BIN)/$(DL)/base64.o:
	$(CC) -std=c++0x -c lib/base64/base64.cpp   -o $(BIN)/$(DL)/base64.o

clean:
	rm -rf $(BIN)

.PHONY: clean