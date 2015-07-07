#ifndef UPDATER_H
#define UPDATER_H

#include "header.h"

#include <QObject>
#include <QTimer>
#include <QThread>


#include "operate.h"
extern vector< operate* > service;

#include "serialize.h"
extern serialize* s;

#include "deploy.h"
extern deploy* d;

class updater : public QThread
{
    Q_OBJECT
public:
    explicit updater( QObject *parent = 0);
    void saveConf();
protected:
    void run();
signals:

public slots:
    void updateServices();

};

#endif // UPDATER_H
