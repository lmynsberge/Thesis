#include "myThesisToolbar.h"

#include <QWidget>
#include <QComboBox>
#include <QVariant>

void myThesisToolbar::constructor()
{
  this->setWindowTitle("My Thesis Toolbar");
  QComboBox* widget = new QComboBox(this)
  this->addWidget(widget);
  this->addWidget(widget)->addItem("Steel",const QVariant &userData = QVariant(double double))
}
