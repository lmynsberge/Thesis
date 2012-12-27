#ifndef _pqThesis_h
#define _pqThesis_h

#include "pqLoadedFormObjectPanel.h"
#include "pqObjectPanelInterface.h"
#include <QMainWindow>
#include <QMenu>
#include <QActionGroup>

class pqThesisWindow : public pqLoadedFormObjectPanel
{
   Q_OBJECT
public:
   pqThesisWindow(pqProxy* proxy, QWidget *parent);
   ~pqThesisWindow();
   
public slots:
   void onAction();
   
};

#endif // pqTHESIS_H