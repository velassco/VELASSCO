#include "guigdf.h"
#include "ui_guigdf.h"




GuiGDF::GuiGDF(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GuiGDF)
{

    /*
*/
    ui->setupUi(this);
    s = new server();
    ui->menuBar->show();
    if(s->getCorePid() == -1)
    {
        statusBar()->showMessage(tr("Please start a local or a remote Core...."));
    }
    else
    {
        statusBar()->showMessage(tr("Local Core is ready."));
        s->startConnection();
    }
    connect(ui->startAll, SIGNAL(released()), this, SLOT(startAllButton()));
    connect(ui->stopAll, SIGNAL(released()), this, SLOT(stopAllButton()));
    connect(ui->statusAll, SIGNAL(released()), this, SLOT(statusButton()));
    connect(ui->ping, SIGNAL(released()), this, SLOT(pingButton()));

    connect(ui->write, SIGNAL(released()), this, SLOT(writeButton()));
    connect(ui->read, SIGNAL(released()), this, SLOT(readButton()));
    connect(ui->grep, SIGNAL(released()), this, SLOT(grepButton()));

    connect(ui->ls, SIGNAL(released()), this, SLOT(lsButton()));

}



GuiGDF::~GuiGDF()
{
    delete ui;


}

void GuiGDF::changeEvent(QEvent *e)
{

}

void GuiGDF::startAllButton()

{
    s->startAllServices();
    ui->status->setText("start all services");
}

void GuiGDF::stopAllButton()
{
    s->stopAllServices();
    ui->status->setText("stop all services");

}

void GuiGDF::statusButton()
{
    ui->status->setText(s->getStatusOfServices());

}

void GuiGDF::pingButton()
{
    ui->status->setText( s->ping() );

}

void GuiGDF::writeButton()
{
ui->status->setText("");
    ui->status->setText( s->write(ui->type->currentText(),
                                  ui->path->text(),
                                  ui->fileName->text(),
                                  ui->contentText->toPlainText()
                                  )
                         );
    ui->contentText->setText("");
    ui->path->setText("");
    ui->fileName->setText("");
}

void GuiGDF::grepButton()
{
    ui->status->setText("");
    ui->status->setText( s->grep(
                             ui->type->currentText(),
                             ui->path->text(),
                             ui->fileName->text(),
                             ui->contentText->toPlainText()
                             )
                         );
    ui->contentText->setText("");
    ui->path->setText("");
    ui->fileName->setText("");
}

void GuiGDF::readButton()
{
    ui->status->setText("");
    ui->status->setText( s->read(
                             ui->type->currentText(),
                             ui->path->text(),
                             ui->fileName->text()
                             )
                         );
    ui->contentText->setText("");
    ui->path->setText("");
    ui->fileName->setText("");
}

void GuiGDF::lsButton()
{
    ui->status->setText("");
    ui->status->setText( s->ls(
                             ui->type->currentText(),
                             ui->path->text()
                             )
                         );
    ui->contentText->setText("");
    ui->path->setText("");
    ui->fileName->setText("");
}

