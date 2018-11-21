#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent,QString message,QString title) :
	QDialog(parent),
	ui(new Ui::Dialog)
{
	ui->setupUi(this);
    this->setWindowTitle(title);
	QObject::connect( this->ui->button, SIGNAL( released() ), this, SLOT(close()) );
    ui->label->setText(message);
}

Dialog::~Dialog()
{
	delete ui;
}
