CC=g++ -std=c++0x
EXEC=engineLayer
BIN=bin


all: $(BIN) $(EXEC)

$(EXEC): VELaSSCo.o cJSON.o velassco_types.o velassco_constants.o  queryManagerModule.o main.o
	$(CC) $(BIN)/*.o -L/usr/local/lib -lthrift -o $(BIN)/$(EXEC)
	./$(BIN)/$(EXEC)




$(BIN):
	clear
	clear
	mkdir $(BIN)
	
main.o: main.cpp
	$(CC) -c main.cpp   -I../gen-cpp/ -I/usr/local/include -I/usr/local/include -I/opt/local/include/ -o $(BIN)/main.o


velassco_constants.o: 
	$(CC) -c ../gen-cpp/velassco_constants.cpp -I../gen-cpp/ -o $(BIN)/velassco_constants.o


velassco_types.o: 
	$(CC) -c ../gen-cpp/velassco_types.cpp -I../gen-cpp/ -o $(BIN)/velassco_types.o


VELaSSCo.o: 
	$(CC) -c ../gen-cpp/VELaSSCo.cpp -I../gen-cpp/ -o $(BIN)/VELaSSCo.o

queryManagerModule.o:
	$(CC) -c queryManagerModule.cpp -I../gen-cpp/ -o $(BIN)/queryManagerModule.o

cJSON.o: cJSON.c
	$(CC) -c cJSON.c -o $(BIN)/cJSON.o

clean:
	rm -rf $(BIN)

.PHONY: clean