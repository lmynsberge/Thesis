#ifndef __pqVTKMOABPanel_h
#define __pqVTKMOABPanel_h

#include "pqComponentsModule.h"
#include <QWidget>

class vtkObject;
class pqDataRepresentation;
class pqOutputPort;
class pqPipelineSource;
class pqProxy;
class pqSelectionManager;
class pqServer;
class pqServerManagerModelItem;
class pqView;
class vtkSMProperty;
class vtkSMProxy;
class vtkSMSourceProxy;

class PQCOMPONENTS_EXPORT pqVTKMOABPanel : public QWidget
{
  Q_OBJECT
  typedef QWidget Superclass;
 public:
  pqVTKMOABPanel(QWidget *parent = 0);
  virtual ~pqVTKMOABPanel();
  void setSelectionManager(pqSelectionManager*);
  
  // Some stuff.
  static void setAutoApply(bool enabled);
  //static bool autoApply();
  
  // Some more.  
  static int suggestedMargin() { return 0; }
  static int suggestedHorizontalSpacing() { return 4; }
  static int suggestedVerticalSpacing() { return 4; }
  
 public slots:
  // Some Stuff.
  //void apply();
  void setServer(pqServer* server);
  
 signals:
  //void applied();
  void viewChanged(pqView*);
  
 private slots:
  void setView(pqView*);
  void setOutputPort(pqOutputPort*);
  void setRepresentation(pqDataRepresentation*);
  
  //void proxyDeleted(pqPipelineSource*);
  //void updatedPanel();
  //void updatedDisplayPanel();
  //void renderActiveView();
  
 protected:
  void setupGUI();
  //void updatePanel(pqOutputPort* port);

 protected slots:
 // void select(pqOutputPort* opport, bool createNew=false);
  //void onSelectionManagerChanged(pqOutputPort* opport);
  //void updateRepresentationViews();
  //void updateAllSelectionView();
  //void onCurrentIndexChanged(QTreeWidgetItem* item);
  
 private: 
  static bool AutoApply;
  
  struct pqImplementation;
  pqImplementation* const Implementation;
};

#endif