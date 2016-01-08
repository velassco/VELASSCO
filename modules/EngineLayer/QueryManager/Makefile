CCPP=g++ -std=c++0x
CC=gcc
EXEC=QueryManager
BIN=bin

EXTRA_FLAGS = -Wall -Wcast-align -Wcomment -Wno-unused-local-typedefs
# other interesting warnings not included in Wall:
#  -Wcast-align -Wcomment
#   -Wconversion  --> warnings in boost
#  -Wextra

CPP_FLAGS = $(EXTRA_FLAGS)
C_FLAGS = $(EXTRA_FLAGS)

OPTS = -g

THRIFT_STORAGE_DIR=../../thrift/Storage/gen-cpp
THRIFT_QUERY_MANAGER_DIR=../../thrift/QueryManager/gen-cpp

OBJS_DIR = $(BIN)

# thrift headers and libs
#THRIFT_BASE = /exports/applications/apps/community/VELaSSCo/lib/thrift/lib/cpp/
THRIFT_BASE = $(HOME)
EXTRA_INC_DIRS = -I$(THRIFT_BASE)/include -I$(HOME)/include
EXTRA_LIB_DIRS = -L$(THRIFT_BASE)/lib -L$(HOME)/lib

#EXTRA_LIB_DIRS = -L/exports/applications/apps/community/VELaSSCo/build/lib


THRIFT_STORAGE_FILES = VELaSSCoSM_constants.cpp VELaSSCoSM_types.cpp VELaSSCoSM.cpp
THRIFT_STORAGE_OBJS_TMP = ${THRIFT_STORAGE_FILES:.cpp=.o}
THRIFT_STORAGE_OBJS = ${THRIFT_STORAGE_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_STORAGE_DEPENDENCIES = $(wildcard $(THRIFT_STORAGE_DIR)/*.h)

THRIFT_QUERY_FILES = QueryManager_constants.cpp QueryManager_types.cpp QueryManager.cpp
THRIFT_QUERY_OBJS_TMP = ${THRIFT_QUERY_FILES:.cpp=.o}
THRIFT_QUERY_OBJS = ${THRIFT_QUERY_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_QUERY_DEPENDENCIES = $(wildcard $(THRIFT_QUERY_MANAGER_DIR)/*.h)

ENGINE_CPP_FILES = DataLayerAccess.cpp Analytics.cpp Graphics.cpp Helpers.cpp main.cpp \
	Server.cpp                         \
	Server_direct_result_queries.cpp   \
	Server_monitoring_queries.cpp      \
	Server_result_analysis_queries.cpp \
	Server_session_queries.cpp

ENGINE_C_FILES = cJSON.c
ENGINE_OBJS_TMP = ${ENGINE_CPP_FILES:.cpp=.o} ${ENGINE_C_FILES:.c=.o}
ENGINE_OBJS = ${ENGINE_OBJS_TMP:%=$(OBJS_DIR)/%}

# lazy way: make each source file depend on all include files
ENGINE_INCLUDE_DEPENDENCIES =  $(wildcard ./*.h) \
	$(THRIFT_STORAGE_DEPENDENCIES) $(THRIFT_QUERY_DEPENDENCIES)

ENGINE_INCS = -I$(THRIFT_STORAGE_DIR) -I$(THRIFT_QUERY_MANAGER_DIR) \
	$(EXTRA_INC_DIRS) -I/usr/local/include -I/opt/local/include -I.
ENGINE_LIBS = $(EXTRA_LIB_DIRS) -L/usr/local/lib -L/opt/local/lib -lthrift \
	-lboost_date_time -lboost_thread -lboost_system

all: $(BIN) $(BIN)/$(EXEC)

$(BIN)/$(EXEC):  $(THRIFT_STORAGE_OBJS) $(THRIFT_QUERY_OBJS) $(ENGINE_OBJS)
	$(CCPP) $(OPTS) $(CPP_FLAGS) \
		$(THRIFT_STORAGE_OBJS) $(THRIFT_QUERY_OBJS) $(ENGINE_OBJS) \
		$(ENGINE_LIBS) -o $(BIN)/$(EXEC)

$(BIN):
	clear
	mkdir $(BIN)

# engine files
$(OBJS_DIR)/%.o : %.cpp $(ENGINE_INCLUDE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $< -o $@

$(OBJS_DIR)/%.o : %.c $(ENGINE_INCLUDE_DEPENDENCIES)
	$(CCPP) -c $(C_FLAGS) $(OPTS) $< -o $@

# Storage Thirft API:
$(OBJS_DIR)/%.o : $(THRIFT_STORAGE_DIR)/%.cpp $(THRIFT_STORAGE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) -I$(THRIFT_STORAGE_DIR) $(EXTRA_INC_DIRS) $< -o $@

# Query Manager Thirft API:
$(OBJS_DIR)/%.o : $(THRIFT_QUERY_MANAGER_DIR)/%.cpp $(THRIFT_QUERY_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) -I$(THRIFT_QUERY_MANAGER_DIR) $(EXTRA_INC_DIRS) $< -o $@

clean:
	rm -f $(BIN)/*.o $(BIN)/$(EXEC)

#	rm -rf $(BIN)

.PHONY: clean
