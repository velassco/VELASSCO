CC=g++ -std=c++0x
EXEC=loaderHBASe
BIN=bin


all: $(BIN) $(EXEC)

$(EXEC): Hbase_constants1.o Hbase_types1.o hbase1.o main.o
	$(CC) $(BIN)/*.o -L/usr/local/lib -lthrift   -o $(BIN)/$(EXEC)
	./$(BIN)/$(EXEC)

$(BIN):
	clear
	clear
	mkdir $(BIN)
	
main.o: 
	$(CC) -c main.cpp -I/usr/local/include -I/usr/local/include -I/opt/local/include/ -Ithrift/hbase/gen-cpp -o $(BIN)/main.o


Hbase_constants1.o: 
	$(CC) -std=c++0x -c thrift/hbase/gen-cpp/Hbase_constants.cpp  -Ithrift/hbase/gen-cpp/  -o $(BIN)/Hbase_constants1.o

Hbase_types1.o: 
	$(CC) -std=c++0x -c thrift/hbase/gen-cpp/Hbase_types.cpp  -Ithrift/hbase/gen-cpp/  -o $(BIN)/Hbase_types1.o

hbase1.o: 
	$(CC) -std=c++0x -c thrift/hbase/gen-cpp/Hbase.cpp  -Ithrift/hbase/gen-cpp/  -o $(BIN)/hbase1.o

clean:
	rm -rf $(BIN)

.PHONY: clean