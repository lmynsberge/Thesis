#ifndef __myThesisToolbar_h
#define __myThesisToolbar_h

#include <QToolBar>

class myThesisToolbar : public QToolBar
{
  Q_OBJECT
  
  public:
    myThesisToolbar(const QString& title, QWidget* parent = 0);
	~myThesisToolbar();
	
};

#endif
