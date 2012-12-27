#include "pqThesis.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QStyle>
#include <QMessageBox>
#include <QMenu>

pqThesisWindow::pqThesisWindow(pqProxy* pxy, QWidget* parent)
   : pqLoadedFormObjectPanel(":/pqThesis/pqThesis.ui",pxy,parent)
  
{
  this->layout()->addWidget(new QLabel("This is from a plugin",this)); 

}

pqThesisWindow::~pqThesisWindow()
{
}

void pqThesisWindow::onAction()
{
   QMessageBox::information(NULL, "Create Material", "A material was created\n");
   
 }