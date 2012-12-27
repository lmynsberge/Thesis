#include "MyThesis.h"

#include <QApplication>
#include <QStyle>
#include <QMessageBox>
#include <QMenu>

MyThesisActions::MyThesisActions(QObject* p)
   : QActionGroup(p)
{
   QIcon icon = qApp->style()->standardIcon(QStyle::SP_MessageBoxCritical);
   QAction* a = new QAction(icon, "Create Material", this);
   
   QPushButton *matAdd = new QPushButton(centralWidget);
   matAdd->setObjectName(QString::fromUtf8("matAdd"));
   matAdd->setGeometry(QRect(100,130,75,23));
   

}

MyThesisActions::~MyThesisActions()
{
}

void MyThesisActions::onAction()
{
   QMessageBox::information(NULL, "Create Material", "A material was created\n");
   
 }