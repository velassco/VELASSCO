
#ifndef SERVERLISTENER_H
#define SERVERLISTENER_H

#include "header.h"
#include <QObject>
#include <QTimer>
#include <QThread>
#include "serverservicehandler.h"

class serverListener  : public QThread
{

    Q_OBJECT
    public:
        explicit serverListener( QObject *parent = 0);
        void saveConf();
    protected:
        void run();
};

#endif // SERVERLISTENER_H
