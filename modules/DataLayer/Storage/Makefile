CCPP=g++ -std=c++0x
CC=gcc
EXEC=Storage
BIN=bin

EXTRA_FLAGS = -Wall -Wcast-align -Wcomment
# other interesting warnings not included in Wall:
#  -Wcast-align -Wcomment
#   -Wconversion  --> warnings in boost
#  -Wextra

CPP_FLAGS = $(EXTRA_FLAGS)
C_FLAGS = $(EXTRA_FLAGS)

OPTS = -g

THRIFT_STORAGE_DIR=../../thrift/Storage/gen-cpp
THRIFT_HBASE_DIR=../../thrift/Storage/hbase/gen-cpp

OBJS_DIR = $(BIN)

THRIFT_STORAGE_FILES = VELaSSCoSM_constants.cpp VELaSSCoSM_types.cpp VELaSSCoSM.cpp
THRIFT_STORAGE_OBJS_TMP = ${THRIFT_STORAGE_FILES:.cpp=.o}
THRIFT_STORAGE_OBJS = ${THRIFT_STORAGE_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_STORAGE_DEPENDENCIES = $(wildcard $(THRIFT_STORAGE_DIR)/*.h)

THRIFT_HBASE_FILES = Hbase_constants.cpp Hbase_types.cpp Hbase.cpp
THRIFT_HBASE_OBJS_TMP = ${THRIFT_HBASE_FILES:.cpp=.o}
THRIFT_HBASE_OBJS = ${THRIFT_HBASE_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_HBASE_DEPENDENCIES = $(wildcard $(THRIFT_HBASE_DIR)/*.h)

DATA_LAYER_CPP_FILES = \
	HBase.cpp HBase_status.cpp HBase_model_operations.cpp \
	HBase_result_queries.cpp \
	EDM.cpp \
	velasscoThrift.cpp storageModule.cpp Helpers.cpp main.cpp

DATA_LAYER_C_FILES = cJSON.c base64.c
DATA_LAYER_OBJS_TMP = ${DATA_LAYER_CPP_FILES:.cpp=.o} ${DATA_LAYER_C_FILES:.c=.o}
DATA_LAYER_OBJS = ${DATA_LAYER_OBJS_TMP:%=$(OBJS_DIR)/%}

# lazy way: make each source file depend on all include files
DATA_LAYER_INCLUDE_DEPENDENCIES =  $(wildcard ./*.h) \
	$(THRIFT_STORAGE_DEPENDENCIES) $(THRIFT_HBASE_DEPENDENCIES)

DATA_LAYER_INCS = -I$(THRIFT_STORAGE_DIR) -I$(THRIFT_HBASE_DIR) \
	-I/usr/local/include -I/opt/local/include
DATA_LAYER_LIBS = -L/usr/local/lib -lthrift -lcurl

all: $(BIN) $(BIN)/$(EXEC)

$(BIN)/$(EXEC):  $(THRIFT_STORAGE_OBJS) $(THRIFT_HBASE_OBJS) $(DATA_LAYER_OBJS)
	$(CCPP) $(OPTS) $(CPP_FLAGS) \
		$(THRIFT_STORAGE_OBJS) $(THRIFT_HBASE_OBJS) $(DATA_LAYER_OBJS) \
		$(DATA_LAYER_LIBS) -o $(BIN)/$(EXEC)

$(BIN):
	clear
	mkdir $(BIN)


# Storage files
$(OBJS_DIR)/%.o : %.cpp $(DATA_LAYER_INCLUDE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(DATA_LAYER_INCS) $< -o $@

$(OBJS_DIR)/%.o : %.c $(DATA_LAYER_INCLUDE_DEPENDENCIES)
	$(CCPP) -c $(C_FLAGS) $(OPTS) $< -o $@

# Storage Thirft API:
$(OBJS_DIR)/%.o : $(THRIFT_STORAGE_DIR)/%.cpp $(THRIFT_STORAGE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) -I$(THRIFT_STORAGE_DIR) $< -o $@

# HBase Thirft API:
$(OBJS_DIR)/%.o : $(THRIFT_HBASE_DIR)/%.cpp $(THRIFT_HBASE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) -I$(THRIFT_HBASE_DIR) $< -o $@

clean:
	rm -rf $(BIN)

.PHONY: clean
