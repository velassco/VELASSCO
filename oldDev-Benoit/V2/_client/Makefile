# Copyright 2008 The Apache Software Foundation
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Makefile for C++ Hbase Thrift DemoClient
# NOTE: run 'thrift -cpp Hbase.thrift' first

THRIFT_DIR = /usr/local/include
FB303_DIR = /usr/local/include/thrift/fb303/
LIB_DIR = /usr/local/lib

GEN_SRC_HBASE = ./thrift-hbase/Hbase.cpp \
	  ./thrift-hbase/Hbase_types.cpp \
	  ./thrift-hbase/Hbase_constants.cpp

GEN_SRC_HIVE =	./thrift-hive/HiveColumnDesc.cpp \
	./thrift-hive/hive_metastore_types.cpp \
	./thrift-hive/HiveResultSet.cpp \
	./thrift-hive/hive_service_constants.cpp \
	./thrift-hive/HiveRowSet.cpp \
	./thrift-hive/hive_service_types.cpp \
	./thrift-hive/TCLIService.cpp \
	./thrift-hive/hiveclient.cpp \
	./thrift-hive/TCLIService_constants.cpp \
	./thrift-hive/hiveclienthelper.cpp \
	./thrift-hive/TCLIService_types.cpp \
	./thrift-hive/queryplan_constants.cpp \
	./thrift-hive/ThriftHive.cpp \
	./thrift-hive/queryplan_types.cpp \
	./thrift-hive/ThriftHiveMetastore.cpp \
	./thrift-hive/serde_constants.cpp \
	./thrift-hive/complex_constants.cpp \
	./thrift-hive/serde_types.cpp \
	./thrift-hive/complex_types.cpp \
	./thrift-hive/testthrift_constants.cpp \
	./thrift-hive/hive_metastore_constants.cpp \
	./thrift-hive/testthrift_types.cpp

all: DemoClient

default: DemoClient

DemoClient: DemoClient.cpp
	g++ -o DemoClient -I${THRIFT_DIR} -I${FB303_DIR} -I./thrift-hbase -I./thrift-hive -L${LIB_DIR}  -lthrift -lfb303 DemoClient.cpp ${GEN_SRC_HBASE} ${GEN_SRC_HIVE}

clean:
	rm -rf DemoClient
