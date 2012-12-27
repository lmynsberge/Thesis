#include <QMenu>
#include <QActionGroup>

class MyThesisActions : public QActionGroup
{
   Q_OBJECT
public:
   MyThesisActions(QObject* p);
   ~MyThesisActions();
   
public slots:
   void onAction();
   
};