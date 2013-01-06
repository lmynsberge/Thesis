#ifndef __myThesisToolbar_h
#define __myThesisToolbar_h

#include <QToolBar>
#include "pqApplicationCOmponentsModule.h"

class myThesisToolbar : pbulic ToolBar
{
  Q_OBJECT
  typedef QToolBar Superclass;
public:
  myThesisToolbar(const QString& title, QWidget* parentObject=0)
  : Superclass(title, parentObject)
  {
  this->constructor();
  }
  myThesisToolbar(QWidget* parentObject=0)
  : Superclass(parentObject)
  {
  this->constructor();
  }
private:
  Q_DISABLE_COPY(myThesisToolbar)
  void constructor();
};

#endif  