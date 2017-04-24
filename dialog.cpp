#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent,std::string message) :
	QDialog(parent),
	ui(new Ui::Dialog)
{
	ui->setupUi(this);
	QObject::connect( this->ui->button, SIGNAL( released() ), this, SLOT(close()) );
	ui->label->setText(message.c_str());
}

Dialog::~Dialog()
{
	delete ui;
}
