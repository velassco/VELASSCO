#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "header.h"

#include <QString>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>


#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include "operate.h"
#include "ophadoop.h"
#include "opfs.h"
#include "ophbase.h"
#include "ophive.h"
extern vector< operate* > service;

class serialize
{
public:
    serialize();
    void write();
    int read();
    void reload();
//private:
    QString filename;

    QDomDocument doc;
};

#endif // SERIALIZE_H
