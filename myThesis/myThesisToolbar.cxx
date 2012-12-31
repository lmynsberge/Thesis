#include "myThesisToolbar.h"

myThesisToolbar::myThesisToolbar(const QString& title, QWidget* parent = 0) 
     : QToolBar(title, parent)
{
  this->setWindowTitle("My Thesis Toolbar);
  QTabWidget* tabWidget = new QTabWidget;
  this->addWidget(tabWidget);
}