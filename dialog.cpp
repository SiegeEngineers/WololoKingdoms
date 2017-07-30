#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent,std::string message,std::string title) :
	QDialog(parent),
	ui(new Ui::Dialog)
{
	ui->setupUi(this);
	this->setWindowTitle(title.c_str());
	QObject::connect( this->ui->button, SIGNAL( released() ), this, SLOT(close()) );
	ui->label->setText(message.c_str());
}

Dialog::~Dialog()
{
	delete ui;
}
