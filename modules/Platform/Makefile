CCPP=g++ -std=c++0x
CC=gcc
# let's give them the '.exe' extension ...
EXEC=QueryManager.exe
BIN=bin

EXTRA_FLAGS = -Wall -Wcast-align -Wcomment -Wno-unused-local-typedefs
# other interesting warnings not included in Wall:
#  -Wcast-align -Wcomment
#   -Wconversion  --> warnings in boost
#  -Wextra

HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),pez001)
  EXTRA_FLAGS += -DACUARIO
else ifeq ($(HOSTNAME),acuario)
  EXTRA_FLAGS += -DACUARIO
endif

CPP_FLAGS = $(EXTRA_FLAGS)
C_FLAGS = $(EXTRA_FLAGS)

#OPTS = -g 
#-Wextra -Wno-unused-parameter
OPTS = -g -O3  -funroll-loops -finline-functions -fno-strict-aliasing -DNDEBUG

THRIFT_STORAGE_DIR=../thrift/Storage/gen-cpp
THRIFT_QUERY_MANAGER_DIR=../thrift/QueryManager/gen-cpp
THRIFT_HBASE_DIR=../thrift/Storage/hbase/gen-cpp

OBJS_DIR = $(BIN)



# thrift headers and libs
# WARNING: Check this paths
# WARNING: Check this paths
# WARNING: Check this paths

ifeq ($(HOSTNAME),pez001)
  THRIFT_BASE = $(HOME)
  BOOST_BASE = $(HOME)
else 
  ifeq ($(USER),v1mpasen)
    THRIFT_BASE = $(HOME)
    BOOST_BASE = $(HOME)
  else
    #THRIFT_BASE = /exports/applications/apps/community/VELaSSCo/lib/thrift/lib/cpp
    #BOOST_BASE=/exports/applications/apps/community/VELaSSCo/build/boost
    THRIFT_BASE = /local/FRAUNHOFER/dependencies/thrift_0.9.3
    BOOST_BASE=/local/FRAUNHOFER/dependencies/boost_1_61_0
  endif
endif




EXTRA_INC_DIRS = -I$(THRIFT_BASE)/include -I$(BOOST_BASE)/include
EXTRA_LIB_DIRS = -L$(THRIFT_BASE)/lib -L$(BOOST_BASE)/lib

#EXTRA_LIB_DIRS = -L/exports/applications/apps/community/VELaSSCo/build/lib


THRIFT_STORAGE_FILES = VELaSSCoSM_constants.cpp VELaSSCoSM_types.cpp VELaSSCoSM.cpp
THRIFT_STORAGE_OBJS_TMP = ${THRIFT_STORAGE_FILES:.cpp=.o}
THRIFT_STORAGE_OBJS = ${THRIFT_STORAGE_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_STORAGE_DEPENDENCIES = $(wildcard $(THRIFT_STORAGE_DIR)/*.h)

THRIFT_QUERY_FILES = QueryManager_constants.cpp QueryManager_types.cpp QueryManager.cpp
THRIFT_QUERY_OBJS_TMP = ${THRIFT_QUERY_FILES:.cpp=.o}
THRIFT_QUERY_OBJS = ${THRIFT_QUERY_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_QUERY_DEPENDENCIES = $(wildcard $(THRIFT_QUERY_MANAGER_DIR)/*.h)

THRIFT_HBASE_FILES = Hbase_constants.cpp Hbase_types.cpp Hbase.cpp
THRIFT_HBASE_OBJS_TMP = ${THRIFT_HBASE_FILES:.cpp=.o}
THRIFT_HBASE_OBJS = ${THRIFT_HBASE_OBJS_TMP:%=$(OBJS_DIR)/%}
THRIFT_HBASE_DEPENDENCIES = $(wildcard $(THRIFT_HBASE_DIR)/*.h)

GRAPHICS_MODULE_DIR=./graphics
GRAPHICS_MODULE_FILES = Graphics.cpp
GRAPHICS_MODULE_OBJS_TMP = ${GRAPHICS_MODULE_FILES:.cpp=.o}
GRAPHICS_MODULE_OBJS = ${GRAPHICS_MODULE_OBJS_TMP:%=$(OBJS_DIR)/%}
GRAPHICS_MODULE_DEPENDENCIES = $(wildcard $(GRAPHICS_MODULE_DIR)/*.h)

STORAGE_MODULE_DIR=./storage
STORAGE_MODULE_FILES = BinarySerialization.cpp  GidTypes.cpp  HBase.cpp \
	HBase_model_operations.cpp  HBase_result_queries.cpp  HBase_status.cpp \
	HBase_getListOfSelectedVerticesFromMesh.cpp \
	HBase_stored_boundary.cpp HBase_stored_simplified.cpp HBase_stored_common.cpp \
	HBase_stored_volume_lrspline.cpp
STORAGE_MODULE_OBJS_TMP = ${STORAGE_MODULE_FILES:.cpp=.o}
STORAGE_MODULE_OBJS = ${STORAGE_MODULE_OBJS_TMP:%=$(OBJS_DIR)/%}
STORAGE_MODULE_DEPENDENCIES = $(wildcard $(STORAGE_MODULE_DIR)/*.h)

ANALYTICS_MODULE_DIR=./analytics
ANALYTICS_MODULE_FILES = Analytics.cpp AnalyticsSimplification.cpp AnalyticsIsoSurface.cpp \
			 AnalyticsVolumeLRSpline.cpp
ANALYTICS_MODULE_OBJS_TMP = ${ANALYTICS_MODULE_FILES:.cpp=.o}
ANALYTICS_MODULE_OBJS = ${ANALYTICS_MODULE_OBJS_TMP:%=$(OBJS_DIR)/%}
ANALYTICS_MODULE_DEPENDENCIES = $(wildcard $(ANALYTICS_MODULE_DIR)/*.h)

STREAMLINE_UTILS_MODULE_DIR=./analytics/streamlineUtils
STREAMLINE_UTILS_MODULE_FILES = Grid.cpp Streamline.cpp UnstructDataset.cpp StreamTracer.cpp
STREAMLINE_UTILS_MODULE_OBJS_TMP = ${STREAMLINE_UTILS_MODULE_FILES:.cpp=.o}
STREAMLINE_UTILS_MODULE_OBJS = ${STREAMLINE_UTILS_MODULE_OBJS_TMP:%=$(OBJS_DIR)/%}
STREAMLINE_UTILS_MODULE_DEPENDENCIES = $(wildcard $(STREAMLINE_UTILS_MODULE_DIR)/*.h)

ACCESS_DIR=./access
ACCESS_FILES = DataLayerAccess.cpp
ACCESS_OBJS_TMP = ${ACCESS_FILES:.cpp=.o}
ACCESS_OBJS = ${ACCESS_OBJS_TMP:%=$(OBJS_DIR)/%}
ACCESS_DEPENDENCIES = $(wildcard $(ACCESS_DIR)/*.h)

QMM_DIR=./queryManager
QMM_FILES = Server.cpp Server_direct_result_queries.cpp \
					Server_monitoring_queries.cpp Server_result_analysis_queries.cpp \
					Server_session_queries.cpp Compression.cpp 
QMM_OBJS_TMP = ${QMM_FILES:.cpp=.o}
QMM_OBJS = ${QMM_OBJS_TMP:%=$(OBJS_DIR)/%}
QMM_DEPENDENCIES = $(wildcard $(QMM_DIR)/*.h)


COMMON_DIR=./commonFiles
COMMON_FILES = Helpers.cpp base64.cpp 
COMMON_C_FILES = cJSON.c minilzo.c	
COMMON_OBJS_TMP = ${COMMON_FILES:.cpp=.o} ${COMMON_C_FILES:.c=.o}
COMMON_OBJS = ${COMMON_OBJS_TMP:%=$(OBJS_DIR)/%}
COMMON_DEPENDENCIES = $(wildcard $(COMMON_DIR)/*.h)

ENGINE_CPP_FILES = main.cpp 
#ENGINE_C_FILES = cJSON.c 
ENGINE_OBJS_TMP = ${ENGINE_CPP_FILES:.cpp=.o} #${ENGINE_C_FILES:.c=.o}
ENGINE_OBJS = ${ENGINE_OBJS_TMP:%=$(OBJS_DIR)/%}

# lazy way: make each source file depend on all include files
ENGINE_INCLUDE_DEPENDENCIES =  $(wildcard ./*.h) \
	$(THRIFT_STORAGE_DEPENDENCIES) $(THRIFT_HBASE_DEPENDENCIES) \
	$(THRIFT_QUERY_DEPENDENCIES) $(GRAPHICS_MODULE_DEPENDENCIES) \
	$(STORAGE_MODULE_DEPENDENCIES) $(ANALYTICS_MODULE_DEPENDENCIES) \
	$(ACCESS_DEPENDENCIES) $(QMM_DEPENDENCIES) $(COMMON_DEPENDENCIES) #$(ENGINE_DEPENDENCIES)

ENGINE_INCS = -I$(THRIFT_STORAGE_DIR) -I$(THRIFT_QUERY_MANAGER_DIR) -I$(THRIFT_HBASE_DIR)  \
	-I$(GRAPHICS_MODULE_DIR) -I$(STORAGE_MODULE_DIR) -I$(ANALYTICS_MODULE_DIR)  \
	-I$(ACCESS_DIR) -I$(QMM_DIR) -I$(COMMON_DIR) -I. \
	-I$(STREAMLINE_UTILS_MODULE_DIR) -I$(STREAMLINE_UTILS_MODULE_DIR)/glm \
	$(EXTRA_INC_DIRS) -I/usr/local/include -I/opt/local/include 
ENGINE_LIBS = $(EXTRA_LIB_DIRS) -L/usr/local/lib -L/opt/local/lib -lthrift \
	-lboost_date_time -lboost_thread -lboost_system -lz -lthrift  -lcurl -lcrypto

JAR_SRC_FOLDER = AnalyticsYarnJobs
ifeq ($(HOSTNAME),pez001)
  JAR_SUFFIX = pez001
  JAR_DST_FOLDER = /tmp/vv
else ifeq ($(HOSTNAME),acuario)
  JAR_SUFFIX = pez001
  JAR_DST_FOLDER = /tmp/vv
else
  JAR_SUFFIX = velassco-cluster01.ecdf.ed.ac.uk
  JAR_DST_FOLDER = /local/VELaSSCo
endif

# Spark jobs: only on velassco-cluster01
ifeq ($(HOSTNAME),pez001)
else ifeq ($(HOSTNAME),acuario)
else
  SPARK_FOLDER = AnalyticsSparkJobs
  SPARK_FULL_JARS = $(wildcard $(SPARK_FOLDER)/*jar)
  SPARK_JARS = ${SPARK_FULL_JARS:$(SPARK_FOLDER)/%=%}
endif



JAR_SRC_FILES = $(wildcard $(JAR_SRC_FOLDER)/*$(JAR_SUFFIX)*jar)
JAR_FILES = ${JAR_SRC_FILES:$(JAR_SRC_FOLDER)/%=%}

# no need to check jar files in acuario/pez001. Only on EDDIE
ifeq ($(HOSTNAME),pez001)
  JAR_FILES = 
else ifeq ($(HOSTNAME),acuario)
  JAR_FILES = 
endif

all: $(BIN) $(BIN)/$(EXEC) check_jars

$(BIN)/$(EXEC):  $(THRIFT_STORAGE_OBJS) $(THRIFT_HBASE_OBJS) $(THRIFT_QUERY_OBJS) $(ENGINE_OBJS) $(GRAPHICS_MODULE_OBJS) $(STORAGE_MODULE_OBJS) $(ANALYTICS_MODULE_OBJS) $(STREAMLINE_UTILS_MODULE_OBJS) $(ACCESS_OBJS) $(QMM_OBJS) $(COMMON_OBJS)
	$(CCPP) $(OPTS) $(CPP_FLAGS) \
		$(THRIFT_STORAGE_OBJS) $(THRIFT_HBASE_OBJS) $(THRIFT_QUERY_OBJS) $(ENGINE_OBJS) $(GRAPHICS_MODULE_OBJS) $(STORAGE_MODULE_OBJS) $(ANALYTICS_MODULE_OBJS) $(STREAMLINE_UTILS_MODULE_OBJS) $(ACCESS_OBJS) $(QMM_OBJS) $(COMMON_OBJS) \
		$(ENGINE_LIBS) -o $(BIN)/$(EXEC)
	@echo "--> $(BIN)/$(EXEC) created."

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

# HBase Thirft API:
$(OBJS_DIR)/%.o : $(THRIFT_HBASE_DIR)/%.cpp $(THRIFT_HBASE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) -I$(THRIFT_HBASE_DIR) $(EXTRA_INC_DIRS) $< -o $@

# Graphics module (folder ./graphics):
$(OBJS_DIR)/%.o : $(GRAPHICS_MODULE_DIR)/%.cpp $(GRAPHICS_MODULE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# storage module (folder ./storageModule):
$(OBJS_DIR)/%.o : $(STORAGE_MODULE_DIR)/%.cpp $(STORAGE_MODULE_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# analytics module (folder ./analytics): also ACCESS_DEPENDENCIES for the BoundaryBinaryMesh.h and RealTimeFormat.h
$(OBJS_DIR)/%.o : $(ANALYTICS_MODULE_DIR)/%.cpp $(ANALYTICS_MODULE_DEPENDENCIES) $(ACCESS_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# streamlineUtils module (folder ./analytics/streamlineUtils): also ACCESS_DEPENDENCIES for the BoundaryBinaryMesh.h and RealTimeFormat.h
$(OBJS_DIR)/%.o : $(STREAMLINE_UTILS_MODULE_DIR)/%.cpp $(STREAMLINE_UTILS_MODULE_DEPENDENCIES) $(ACCESS_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# access module (folder ./access):
$(OBJS_DIR)/%.o : $(ACCESS_DIR)/%.cpp $(ACCESS_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# QMM module (folder ./queryManagerMod):
$(OBJS_DIR)/%.o : $(QMM_DIR)/%.cpp $(QMM_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# common files (folder ./commonFiles):
$(OBJS_DIR)/%.o : $(COMMON_DIR)/%.cpp $(COMMON_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@

# common C files (folder ./commonFiles):
$(OBJS_DIR)/%.o : $(COMMON_DIR)/%.c $(COMMON_DEPENDENCIES)
	$(CCPP) -c $(CPP_FLAGS) $(OPTS) $(ENGINE_INCS) $(EXTRA_INC_DIRS) $< -o $@	

check_jars:
	@if [ "$(JAR_FILES)" != "" ] ; then \
	  echo "Checking JAR files in $(JAR_DST_FOLDER) --" ; \
	  for jj in $(JAR_FILES) ; do \
	    echo "  checking $$jj ..."; \
	    diff_out=`diff -q $(JAR_SRC_FOLDER)/$$jj $(JAR_DST_FOLDER)/$$jj` ; \
	    if [ "$$diff_out" == "" ] ; then \
	      echo -e "  ... \033[32;1midentical\033[0m"; \
	    else  \
	      echo -e "  ... \033[31;1mDIFFERENT!!!!\033[0m"; \
	    fi \
	  done ; \
	  echo "--> done" ; \
	else \
	  echo "--> No JAR files to check" ; \
	fi;
	@if [ "$(SPARK_JARS)" != "" ] ; then \
	  echo "Checking JAR files in $(SPARK_FOLDER) --" ; \
	  for jj in $(SPARK_JARS) ; do \
	    echo "  checking $$jj ..."; \
	    diff_out=`diff -q $(SPARK_FOLDER)/$$jj $(JAR_DST_FOLDER)/$$jj` ; \
	    if [ "$$diff_out" == "" ] ; then \
	      echo -e "  ... \033[32;1midentical\033[0m"; \
	    else  \
	      echo -e "  ... \033[31;1mDIFFERENT!!!!\033[0m"; \
	    fi \
	  done ; \
	  echo "--> done" ; \
	else \
	  echo "--> No JAR files to check" ; \
	fi;

debug:
	+make "OPTS=-g"

clean:
	rm -f $(BIN)/*.o $(BIN)/$(EXEC)

#	rm -rf $(BIN)

.PHONY: clean
