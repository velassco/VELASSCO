#-------------------------------------------------
#
# Project created by QtCreator 2014-12-03T09:17:30
#
#-------------------------------------------------

QT       += core gui xml
TARGET = coreGDF
CONFIG   += console

CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp


SOURCES += main.cpp \
    deploy.cpp \
    operate.cpp \
    communicate.cpp \
    ophadoop.cpp \
    ../../own/gen-cpp/com_constants.cpp \
    ../../own/gen-cpp/com_types.cpp \
    ../../own/gen-cpp/ServerService.cpp \
    #../../hbase/gen-cpp2/hbase_constants.cpp \
    #../../hbase/gen-cpp2/hbase_types.cpp \
    #../../hbase/gen-cpp2/THBaseService.cpp \
    ../../hbase/gen-cpp/Hbase_constants.cpp \
    ../../hbase/gen-cpp/Hbase_types.cpp \
    ../../hbase/gen-cpp/Hbase.cpp \
    ../../hive/thrift-hive/HiveColumnDesc.cpp \
        ../../hive/thrift-hive/hive_metastore_types.cpp \
        ../../hive/thrift-hive/HiveResultSet.cpp \
        ../../hive/thrift-hive/hive_service_constants.cpp \
        ../../hive/thrift-hive/HiveRowSet.cpp \
        ../../hive/thrift-hive/hive_service_types.cpp \
        ../../hive/thrift-hive/TCLIService.cpp \
        ../../hive/thrift-hive/hiveclient.cpp \
        ../../hive/thrift-hive/TCLIService_constants.cpp \
        ../../hive/thrift-hive/hiveclienthelper.cpp \
        ../../hive/thrift-hive/TCLIService_types.cpp \
        ../../hive/thrift-hive/queryplan_constants.cpp \
        ../../hive/thrift-hive/ThriftHive.cpp \
        ../../hive/thrift-hive/queryplan_types.cpp \
        ../../hive/thrift-hive/ThriftHiveMetastore.cpp \
        ../../hive/thrift-hive/serde_constants.cpp \
        ../../hive/thrift-hive/complex_constants.cpp \
        ../../hive/thrift-hive/serde_types.cpp \
        ../../hive/thrift-hive/complex_types.cpp \
        ../../hive/thrift-hive/testthrift_constants.cpp \
        ../../hive/thrift-hive/hive_metastore_constants.cpp \
        ../../hive/thrift-hive/testthrift_types.cpp \
    serverservicehandler.cpp \
    updater.cpp \
    serialize.cpp \
    opfs.cpp \
    ophbase.cpp \
    ophive.cpp \
    serverlistener.cpp

QMAKE_MAC_SDK = macosx10.9

HEADERS += \
    deploy.h \
    operate.h \
    communicate.h \
    ophadoop.h \
    ../../own/gen-cpp/com_constants.h \
    ../../own/gen-cpp/com_types.h \
    ../../own/gen-cpp/ServerService.h \
    #../../hbase/gen-cpp2/hbase_constants.h \
    #../../hbase/gen-cpp2/hbase_types.h \
    #../../hbase/gen-cpp2/THBaseService.h \
    ../../hbase/gen-cpp/Hbase_constants.h \
    ../../hbase/gen-cpp/Hbase_types.h \
    ../../hbase/gen-cpp/Hbase.h \
../../hive/thrift-hive/HiveColumnDesc.h \
        ../../hive/thrift-hive/hive_metastore_types.h \
        ../../hive/thrift-hive/HiveResultSet.h \
        ../../hive/thrift-hive/hive_service_constants.h \
        ../../hive/thrift-hive/HiveRowSet.h \
        ../../hive/thrift-hive/hive_service_types.h \
        ../../hive/thrift-hive/TCLIService.h \
        ../../hive/thrift-hive/hiveclient.h \
        ../../hive/thrift-hive/TCLIService_constants.h \
        ../../hive/thrift-hive/hiveclienthelper.h \
        ../../hive/thrift-hive/TCLIService_types.h \
        ../../hive/thrift-hive/queryplan_constants.h \
        ../../hive/thrift-hive/ThriftHive.h \
        ../../hive/thrift-hive/queryplan_types.h \
        ../../hive/thrift-hive/ThriftHiveMetastore.h \
        ../../hive/thrift-hive/serde_constants.h \
        ../../hive/thrift-hive/complex_constants.h \
        ../../hive/thrift-hive/serde_types.h \
        ../../hive/thrift-hive/complex_types.h \
        ../../hive/thrift-hive/testthrift_constants.h \
        ../../hive/thrift-hive/hive_metastore_constants.h \
        ../../hive/thrift-hive/testthrift_types.h \
        /usr/local/include/thrift/fb303/fb303_types.h \
/usr/local/include/thrift/fb303/FacebookBase.h \
/usr/local/include/thrift/fb303/fb303_constants.h \
/usr/local/include/thrift/fb303/ServiceTracker.h \
/usr/local/include/thrift/fb303/FacebookService.h \
    serverservicehandler.h \
    updater.h \
    serialize.h \
    opfs.h \
    ophbase.h \
    ophive.h \
    serverlistener.h \
    header.h

 LIBS += -L/usr/local/lib -lthrift
LIBS += -L/usr/local/include/thrift/fb303/  -lfb303
 INCLUDEPATH = /usr/local/include /opt/local/include/ /usr/local/include/thrift/fb303/
