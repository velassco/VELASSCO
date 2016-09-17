CCPP=g++ -std=c++0x
CC=gcc

EXTRA_FLAGS = -Wcast-align -Wcomment -Wno-unused-local-typedefs
#OLI EXTRA_FLAGS = -Wall -Wcast-align -Wcomment -Wno-unused-local-typedefs
# other interesting warnings not included in Wall:
#  -Wcast-align -Wcomment
#   -Wconversion  --> warnings in boost
#  -Wextra

HOSTNAME = $(shell hostname)

EDM_FLAGS = -DUNIX -DCPU_64B -DUSE_EDMI -w

#OPTS = -g 
#-Wextra -Wno-unused-parameter
#OPTS = -g -O3  -funroll-loops -finline-functions -fopenmp -fno-strict-aliasing -DNDEBUG
OPTS = -g -funroll-loops -fopenmp -fno-strict-aliasing -DNDEBUG -fPIC


THRIFT_BASE = /home/velassco/cpp
BOOST_BASE=/usr/lib/x86_64-linux-gnu
PLUGIN_DIR = /home/velassco/trunk/modules/Platform/edm_plugin
QM_DIR = /home/velassco/trunk/modules/Platform/edm_qm
PLUGIN_OBJ_DIR = $(PLUGIN_DIR)/objs

EDM_BASE=/home/edm/v6.0.xxx
EDM_BIN=$(EDM_BASE)/output/sunosx/release
PLUGIN_BIN = $(EDM_BIN)/VELaSSCo_plugins

EDM_INC_DIRS = -I$(EDM_BASE)/edmsys/sdai/include -I$(EDM_BASE)/cpp_edmi/web.services -I$(EDM_BASE)/cpp_edmi/cpp10
#EDM_INC_DIRS = -I$(EDM_BASE)/cpp_edmi/web.services -I$(EDM_BASE)/cpp_edmi/cpp10
INCLS = $(EDM_INC_DIRS) -I$(THRIFT_BASE)/src -I$(BOOST_BASE)/include -I$(PLUGIN_DIR)
EDM_LIB_DIRS = -L$(EDM_BASE)/output/sunosx/release
CPP10_OBJ_DIR = $(EDM_LIB_DIRS)/libcpp10_edmi_files
CPP10_SRC_DIR = $(EDM_BASE)/cpp_edmi/cpp10

LIBS = $(EDM_BIN)/libedmappserver.so
CPPPLUGIN_1 = $(PLUGIN_BIN)/libedm_plugin_1.so
OBJS_1 = $(PLUGIN_OBJ_DIR)/dem_schema_velassco.o $(PLUGIN_OBJ_DIR)/EDM_plugin_1.o $(PLUGIN_OBJ_DIR)/FEM_InjectorHandler.o $(PLUGIN_OBJ_DIR)/fem_schema_velassco.o $(PLUGIN_OBJ_DIR)/WindowsFunctionsForLinux.o $(PLUGIN_OBJ_DIR)/dbInterface.o $(PLUGIN_OBJ_DIR)/memoryAllocator.o

all: $(CPPPLUGIN_1)

$(PLUGIN_OBJ_DIR)/dbInterface.o: $(CPP10_SRC_DIR)/dbInterface.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/dbInterface.o $(CPP10_SRC_DIR)/dbInterface.cpp

$(PLUGIN_OBJ_DIR)/memoryAllocator.o: $(CPP10_SRC_DIR)/memoryAllocator.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/memoryAllocator.o $(CPP10_SRC_DIR)/memoryAllocator.cpp

$(PLUGIN_OBJ_DIR)/dem_schema_velassco.o: $(PLUGIN_DIR)/dem_schema_velassco.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/dem_schema_velassco.o $(PLUGIN_DIR)/dem_schema_velassco.cpp

$(PLUGIN_OBJ_DIR)/EDM_plugin_1.o: $(PLUGIN_DIR)/EDM_plugin_1.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/EDM_plugin_1.o $(PLUGIN_DIR)/EDM_plugin_1.cpp

$(PLUGIN_OBJ_DIR)/FEM_InjectorHandler.o: $(PLUGIN_DIR)/FEM_InjectorHandler.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/FEM_InjectorHandler.o $(PLUGIN_DIR)/FEM_InjectorHandler.cpp

$(PLUGIN_OBJ_DIR)/fem_schema_velassco.o: $(PLUGIN_DIR)/fem_schema_velassco.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/fem_schema_velassco.o $(PLUGIN_DIR)/fem_schema_velassco.cpp

$(PLUGIN_OBJ_DIR)/WindowsFunctionsForLinux.o: $(QM_DIR)/WindowsFunctionsForLinux.cpp
	$(CCPP) -c $(EDM_FLAGS) $(EXTRA_FLAGS) $(OPTS) $(INCLS) -o $(PLUGIN_OBJ_DIR)/WindowsFunctionsForLinux.o $(QM_DIR)/WindowsFunctionsForLinux.cpp

$(CPPPLUGIN_1): $(OBJS_1)  $(LIBS) 
	$(CCPP) -shared -o $(CPPPLUGIN_1) $(OBJS_1) $(LIBS) 

clean:
	rm -f $(OBJS_1) $(CPPPLUGIN_1)

