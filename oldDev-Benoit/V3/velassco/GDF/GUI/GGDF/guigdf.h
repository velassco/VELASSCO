#ifndef GUIGDF_H
#define GUIGDF_H

#include <QMainWindow>
#include "server.h"



namespace Ui
{
class GuiGDF;
}

class GuiGDF : public QMainWindow
{
    Q_OBJECT

public:
    explicit GuiGDF(QWidget *parent = 0);
    ~GuiGDF();
    void changeEvent(QEvent *e);

private slots:
    void startAllButton();
    void stopAllButton();
    void statusButton();
    void pingButton();
    void writeButton();
    void grepButton();
    void readButton();
    void lsButton();


private:
    Ui::GuiGDF *ui;
    server * s;

};

#endif // GUIGDF_H
