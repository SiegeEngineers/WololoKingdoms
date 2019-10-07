#pragma once
#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget* parent = nullptr, QString message = "Error",
                  QString title = "Note");
  ~Dialog() override;

private:
  Ui::Dialog* ui;
};
