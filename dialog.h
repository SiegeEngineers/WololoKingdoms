#pragma once
#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
	Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0, QString message = "Error", QString title = "Note");
	~Dialog();

private:
	Ui::Dialog *ui;
};
