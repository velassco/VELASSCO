#include <QMutex>
#include <QString>
#if DEBUG
#define printcli(v) { /* v*/ }
#else
#define printcli(v) {   printf(v);   }
#endif


extern QString execCommand(QString exe, QStringList args);
//extern QMutex lock;

