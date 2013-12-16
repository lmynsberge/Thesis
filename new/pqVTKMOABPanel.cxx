// Include this classes stuff
#include "pqVTKMOABPanel.h"
#include "ui_pqVTKMOABPanel.h"

// Include all PV and VTK classes necessary
#include "pqActiveObjects.h"
#include "pqApplicationCore.h"
#include "pqDataRepresentation.h"
#include "pqDebug.h"
#include "pqDisplayPolicy.h"
#include "pqObjectBuilder.h"
#include "pqOutputPort.h"
#include "pqPipelineFilter.h"
#include "pqPipelineSource.h"
#include "pqProxyModifiedStateUndoElement.h"
#include "pqProxyWidget.h"
#include "pqServerManagerModel.h"
#include "pqSettings.h"
#include "pqUndoStack.h"
#Include "pqActiveView.h"
#include "pqComboBoxDomain.h"
#include "pqPipelineRepresentation.h"
#include "pqPropertyLinks.h"
#include "pqRenderView.h"
#include "pqServer.h"
#include "pqSelectionManager.h"
#include "vtkCollection.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkNew.h"
#include "vtkSMProperty.h"
#include "vtkSMSourceProxy.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSmartPointer.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMProxyManager.h"
#include "vtkSMSessionProxyManager.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkSMSelectionHelper.h"

// Qt Panel Information
#include <QPointer>
#include <QShortcut>
#include <QHeaderView>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>


struct pqVTKMOABPanel::pqImplementation : public Ui::vtkMOABPanel
{
 public:
  pqImplementation()
    {
    this->SelectionLinks = new pqPropertyLinks;
	this->RepLinks = new pqPropertyLinks;
	this->InputPort = 0;
	this->VTKConnectSelInput = vtkEventQtSlotConnect::New();
	this->VTKConnectRep = vtkEventQtSlotConnect::New();
	}
  ~pqImplementation()
    {
	this->SelectionLinks->removeAllPropertyLinks;
	this->RepLinks->removeAllPropertyLinks;
	this->InputPort = 0;
	this->VTKConnectSelInput->Delete();
	this->VTKConnectRep->Delete();
	}
	
  vtkSMSourceProxy* getSelectionSource() const
    {
    return (this->InputPort? this->InputPort->getSelectionInput() :
      static_cast<vtkSMSourceProxy*>(NULL));
    }

  pqDataRepresentation* getSelectionRepresentation() const
    {
    if (this->InputPort && this->ActiveView)
      {
      return this->InputPort->getRepresentation(this->ActiveView);
      }
    return NULL;
    }

  // Selection Labels Properties
  vtkEventQtSlotConnect* VTKConnectSelInput;
  vtkEventQtSlotConnect* VTKConnectRep;
  pqPropertyLinks* SelectionLinks;
  pqPropertyLinks* RepLinks;

  QPointer<pqView> View;
  QPointer<pqOutputPort> Port;
  QPointer<pqOutputPort> InputPort;
  QPointer<pqRenderView> ActiveView;
  QPointer<pqPipelineSource> Source;
  QPointer<pqDataRepresentation> Representation;
  QPointer<pqSelectionManager> SelectionManager;

  vtkNew<vtkEventQtSlotConnect> RepresentationEventConnect;
  bool UseProcessID;
  
  void updateInformationAndDomains()
    {
    if (this->Source)
	  {
	  vtkSMProxy *proxy = this->Source->getProxy();
	  if (vtkSMSourceProxy *sourceProxy = vtkSMSourceProxy::SafeDownCast(proxy))
	    {
	    sourceProxy->UpdatePipelineInformation();
		}
	  else
	    {
		proxy->UpdatePropertyInformation();
		}
		
	  vtkSMProperty *inputProperty = proxy->GetProperty("Input");
	  if (inputProperty)
	    {
		inputProperty->UpdateDependentDomains();
		}
	  }
	}
};

pqVTKMOABPanel::pqVTKMOABPanel(QWidget* parentObject)
  : QWidget(parentObject),
  Implementation(new pqImplementation())
{
  pqSettings *settings = pqApplicationCore::instance()->settings();
  if (settings)
    {
	pqVTKMOABPanel::AutoApply settings->value("autoAccept", false).toBool();
	}
	
  QVBoxLayout* vboxlayout = new QVBoxLayout(this);
  vboxlayout->setSpacing(0);
  vboxlayout->setMargin(0);
  vboxlayout->setObjectName("vboxLayout");

  QWidget* container = new QWidget(this);
  container->setObjectName("scrollWidget");
  container->setSizePolicy(QSizePolicy::MinimumExpanding,
    QSizePolicy::MinimumExpanding);

  QScrollArea* s = new QScrollArea(this);
  s->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  s->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  s->setWidgetResizable(true);
  s->setObjectName("scrollArea");
  s->setFrameShape(QFrame::NoFrame);
  s->setWidget(container);
  vboxlayout->addWidget(s);

  this->Implementation->setupUi(container);
  this->setupGUI();
  
  pqActiveObjects *activeObjects = &pqActiveObjects::instance();
  this->connect(activeObjects, SIGNAL(portChanged(pqOutputPort*)),
                this,SLOT(setOutputPort(pqOutputPort*)));
  this->connect(activeObjects, SIGNAL(viewChanged(pqView*)),
                this, SLOT(setView(pqView*)));
  this->connect(activeObjects, SIGNAL(representationChanged(pqDataRepresentation*)),
                this, SLOT(setRepresentation(pqDataRepresentation*)));

  // listen to server manager changes
  pqServerManagerModel *smm = pqApplicationCore::instance()->getServerManagerModel();
  this->connect(smm, SIGNAL(sourceRemoved(pqPipelineSource*)),
                this, SLOT(proxyDeleted(pqPipelineSource*)));
  this->connect(pqApplicationCore::instance()->getServerManagerModel(),
      SIGNAL(connectionRemoved(pqPipelineSource*, pqPipelineSource*, int)),
      SLOT(updateButtonState()));
  this->connect(pqApplicationCore::instance()->getServerManagerModel(),
      SIGNAL(connectionAdded(pqPipelineSource*, pqPipelineSource*, int)),
      SLOT(updateButtonState()));

  
  QShortcut *tab = new QShortcut(Qt::Key_Tab, this);
  QOjbect::connect(tab, SIGNAL(activated()),
                this, SLOT(changeEntity()));
			
  this->setOutputPort(NULL);
}

pqVTKMOABPanel::~pqVTKMOABPanel()
{
  delete this->Implementation;
}
  
pqVTKMOABPanel::setupGUI()
{
   QObject::connect(this->Implementation->SelectionLinks, SIGNAL(qtWidgetChanged()),
    this, SLOT(updateAllSelectionViews()));
  QObject::connect(this->Implementation->RepLinks, SIGNAL(qtWidgetChanged()),
    this, SLOT(updateRepresentationViews()));
}

//-----------------------------------------------------------------------------
// Set the active server. We need the active server only to determine if this is
// a multiprocess server connection or not.
void pqSelectionInspectorPanel::setServer(pqServer* server)
{
  this->Implementation->UseProcessID =
    (server && server->getNumberOfPartitions() > 1);

  this->Implementation->ProcessIDRange->setVisible(
    this->Implementation->UseProcessID);
  if (server)
    {
    this->Implementation->ProcessIDRange->setText(
      QString("Process ID Range: 0 - %1").arg(server->getNumberOfPartitions()-1));
    }
}

//-----------------------------------------------------------------------------
/// Set the selection manager.
void pqSelectionInspectorPanel::setSelectionManager(pqSelectionManager* mgr)
{
  if (this->Implementation->SelectionManager == mgr)
    {
    return;
    }
  if (this->Implementation->SelectionManager)
    {
    QObject::disconnect(this->Implementation->SelectionManager, 0, this, 0);
    }
  this->Implementation->SelectionManager = mgr;
  if (mgr)
    {
    QObject::connect(
      mgr, SIGNAL(selectionChanged(pqOutputPort*)),
      this, SLOT(onSelectionManagerChanged(pqOutputPort*)));
    }
}

void pqVTKMOABPanel::setAutoApply(bool enabled)
{
  pqVTKMOABPanel::AutoApply = enabled;
}
 
 /*
//-----------------------------------------------------------------------------
pqView* pqVTKMOABPanel::view() const
{
  return this->Internals->View;
}
//*/

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::setRepresentation(pqDataRepresentation* repr)
{
  if (repr)
    {
    this->setView(repr->getView());
    }
  //this->updateDisplayPanel(repr);
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::setView(pqView* pqview)
{
  if (this->Implementation->View != pqview)
    {
    this->Implementation->View = pqview;
    emit this->viewChanged(pqview);
    }
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::setOutputPort(pqOutputPort* port)
{
  //this->updatePanel(port);
}
/*
//-----------------------------------------------------------------------------
void pqVTKMOABPanel::updatePanel()
{
  this->updatePanel(this->Internals->Port);
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::updatePanel(pqOutputPort* port)
{
  this->Internals->Port = port;

  // Determine if the proxy/repr has changed. If so, we have to recreate the
  // entire panel, else we simply update the widgets.
  this->updatePropertiesPanel(port? port->getSource() : NULL);
  this->updateDisplayPanel(port? port->getRepresentation(this->view()) : NULL);

}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::updatePropertiesPanel(pqPipelineSource *source)
{
  if (this->Internals->Source != source)
    {
    // Panel has changed.
    if (this->Internals->Source &&
      this->Internals->SourceWidgets.contains(this->Internals->Source))
      {
      this->Internals->SourceWidgets[
        this->Internals->Source]->hide();
      }
    this->Internals->Source = source;
    this->Internals->updateInformationAndDomains();

    if (source && !this->Internals->SourceWidgets.contains(source))
      {
      // create the panel for the source.
      pqProxyWidgets* widgets = new pqProxyWidgets(source, this);
      this->Internals->SourceWidgets[source] = widgets;

      QObject::connect(widgets->Panel, SIGNAL(changeAvailable()),
                       this, SLOT(sourcePropertyChangeAvailable()));
      QObject::connect(widgets->Panel, SIGNAL(changeFinished()),
                       this, SLOT(sourcePropertyChanged()));
      }

    if (source)
      {
      this->Internals->SourceWidgets[source]->show(
        this->Internals->Ui.PropertiesFrame);
      }
    }

  // update widgets.
  if (source)
    {
    this->Internals->Ui.PropertiesButton->setText(
      QString("Properties (%1)").arg(source->getSMName()));
    this->Internals->SourceWidgets[source]->showWidgets(
      this->Internals->Ui.AdvancedButton->isChecked(),
      this->Internals->Ui.SearchLineEdit->text());

    if (source->modifiedState() == pqProxy::UNINITIALIZED &&
        pqVTKMOABPanel::AutoApply)
      {
      QTimer::singleShot(pqVTKMOABPanel::AutoApplyDelay, this, SLOT(apply()));
      }
    }
  else
    {
    this->Internals->Ui.PropertiesButton->setText("Properties");
    }
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::updateDisplayPanel()
{
  this->updateDisplayPanel(this->Internals->Representation);
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::updateDisplayPanel(pqDataRepresentation* repr)
{
  // since this->Internals->Representation is QPointer, it can go NULL (e.g. during
  // disconnect) before we get the chance to clear the panel's widgets. Hence we
  // do the block of code if (repr==NULL) event if nothing has changed.
  if (this->Internals->Representation != repr || repr == NULL)
    {
    // Representation has changed, destroy the current display panel and create
    // a new one. Unlike properties panels, display panels are not cached.
    if (this->Internals->DisplayWidgets)
      {
      this->Internals->DisplayWidgets->hide();
      delete this->Internals->DisplayWidgets;
      }
    this->Internals->RepresentationEventConnect->Disconnect();
    this->Internals->Representation = repr;
    if (repr)
      {
      if (repr->getProxy()->GetProperty("Representation"))
        {
        this->Internals->RepresentationEventConnect->Connect(
          repr->getProxy()->GetProperty("Representation"),
          vtkCommand::ModifiedEvent, this, SLOT(updateDisplayPanel()));
        }

      // create the panel for the repr.
      pqProxyWidgets* widgets = new pqProxyWidgets(repr, this);
      widgets->Panel->setApplyChangesImmediately(true);
      QObject::connect(widgets->Panel, SIGNAL(changeFinished()),
                       this, SLOT(renderActiveView()));
      this->Internals->DisplayWidgets = widgets;
      this->Internals->DisplayWidgets->show(
        this->Internals->Ui.DisplayFrame);
      }
    }

  if (repr)
    {
    this->Internals->Ui.DisplayButton->setText(
      QString("Display (%1)").arg(repr->getProxy()->GetXMLName()));
    this->Internals->DisplayWidgets->showWidgets(
      this->Internals->Ui.AdvancedButton->isChecked(),
      this->Internals->Ui.SearchLineEdit->text());
    }
  else
    {
    this->Internals->Ui.DisplayButton->setText("Display");
    }
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::renderActiveView()
{
  if (this->view())
    {
    this->view()->render();
    }
}

//-----------------------------------------------------------------------------
void pqVTKMOABPanel::sourcePropertyChanged(bool change_finished/*=true*/)
{
  // FIXME:
  QString senderClass("(unknown)");
  QString senderLabel("(unknown)");
  QObject *signalSender = this->sender();
  if (signalSender)
    {
    senderClass = signalSender->metaObject()->className();
    //pqPropertyWidget *senderWidget = qobject_cast<pqPropertyWidget *>(signalSender);
    //if (senderWidget)
    //  {
    //  senderLabel = senderWidget->property()->GetXMLLabel();
    //  }
    }

  if (!change_finished)
    {
    this->Internals->ReceivedChangeAvailable = true;
    }
  if (change_finished && !this->Internals->ReceivedChangeAvailable)
    {
    DEBUG_APPLY_BUTTON()
      << "Received change-finished before change-available. Ignoring it.";
    return;
    }

  DEBUG_APPLY_BUTTON()
      << "Property change "
      << (change_finished? "finished" : "available")
      << senderLabel << "(" << senderClass << ")";

  if (this->Internals->Source &&
      this->Internals->Source->modifiedState() == pqProxy::UNMODIFIED)
    {
    this->Internals->Source->setModifiedState(pqProxy::MODIFIED);
    }
  if (pqVTKMOABPanel::AutoApply && change_finished)
    {
    QTimer::singleShot(pqVTKMOABPanel::AutoApplyDelay, this, SLOT(apply()));
    }

  this->updateButtonState();
}
//*/