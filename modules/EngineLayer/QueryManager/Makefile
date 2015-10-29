CC=g++ -std=c++0x
EXEC=QueryManager
BIN=bin
 

THRIFT_STORAGE=../../thrift/Storage/gen-cpp
THRIFT_QUERY_MANAGER=../../thrift/QueryManager/gen-cpp

all: $(BIN) $(EXEC)

$(EXEC):  querymanager_QueryManager.o querymanager_velassco_types.o querymanager_velassco_constants.o storage_VELaSSCo.o storage_velassco_types.o storage_velassco_constants.o  cJSON.o DataLayerAccess.o Server.o main.o Helpers.o
	$(CC) $(BIN)/*.o -L/usr/local/lib -lthrift -lboost_date_time -lboost_thread -o $(BIN)/$(EXEC)

$(BIN):
	clear
	clear
	mkdir $(BIN)

main.o: main.cpp
	$(CC) -c main.cpp -I$(THRIFT_STORAGE) -I/usr/local/include -I/usr/local/include -I/opt/local/include/ -o $(BIN)/main.o


Helpers.o: Helpers.cpp
	$(CC) -c Helpers.cpp -I$(THRIFT_STORAGE) -I/usr/local/include -I/usr/local/include -I/opt/local/include/ -o $(BIN)/Helpers.o


storage_velassco_constants.o: 
	$(CC) -c $(THRIFT_STORAGE)/VELaSSCoSM_constants.cpp -I$(THRIFT_STORAGE)  -o $(BIN)/storage_velassco_constants.o


storage_velassco_types.o: 
	$(CC) -c $(THRIFT_STORAGE)/VELaSSCoSM_types.cpp -I$(THRIFT_STORAGE)  -o $(BIN)/storage_velassco_types.o


storage_VELaSSCo.o: 
	$(CC) -c $(THRIFT_STORAGE)/VELaSSCoSM.cpp -I$(THRIFT_STORAGE)  -o $(BIN)/storage_VELaSSCo.o



querymanager_velassco_constants.o: 
	$(CC) -c $(THRIFT_QUERY_MANAGER)/QueryManager_constants.cpp -I$(THRIFT_QUERY_MANAGER)  -o $(BIN)/querymanager_velassco_constants.o


querymanager_velassco_types.o: 
	$(CC) -c $(THRIFT_QUERY_MANAGER)/QueryManager_types.cpp -I$(THRIFT_QUERY_MANAGER)  -o $(BIN)/querymanager_velassco_types.o


querymanager_QueryManager.o: 
	$(CC) -c $(THRIFT_QUERY_MANAGER)/QueryManager.cpp -I$(THRIFT_QUERY_MANAGER)  -o $(BIN)/querymanager_QueryManager.o



Server.o:
	$(CC) -c Server.cpp -I$(THRIFT_QUERY_MANAGER) -I$(THRIFT_STORAGE) -o $(BIN)/Server.o

DataLayerAccess.o:
	$(CC) -c DataLayerAccess.cpp -I$(THRIFT_STORAGE) -o $(BIN)/DataLayerAccess.o

cJSON.o: cJSON.c
	$(CC) -c cJSON.c -o $(BIN)/cJSON.o

clean:
	rm -rf $(BIN)

.PHONY: clean
