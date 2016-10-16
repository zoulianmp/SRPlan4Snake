/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright 2015 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Andras Lasso (PerkLab, Queen's
  University) and Kevin Wang (Princess Margaret Hospital, Toronto) and was
  supported through OCAIRO and the Applied Cancer Research Unit program of
  Cancer Care Ontario.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QToolButton>

// CTK includes
#include <ctkLogger.h>
#include <ctkPopupWidget.h>

// qMRML includes
//#include "qMRMLTableViewControllerWidget.h"
//#include "qMRMLTableView.h"

#include "QTabWidget"
#include "QTableWidget.h"
#include "qMRMLSimpleTableWidget.h"

//--------------------------------------------------------------------------
static ctkLogger logger("org.slicer.libs.qmrmlwidgets.qMRMLSimpleTableWidget");
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// qMRMLSliceViewPrivate
class qMRMLSimpleTableWidgetPrivate
  : public QObject
{
  Q_DECLARE_PUBLIC(qMRMLSimpleTableWidget);
protected:
  qMRMLSimpleTableWidget* const q_ptr;
public:
  qMRMLSimpleTableWidgetPrivate(qMRMLSimpleTableWidget& object);
  ~qMRMLSimpleTableWidgetPrivate();

  void init();

  QTabWidget * tab;
  QTableWidget*       TableWdiget;
 
};


//---------------------------------------------------------------------------
qMRMLSimpleTableWidgetPrivate::qMRMLSimpleTableWidgetPrivate(qMRMLSimpleTableWidget& object)
  : q_ptr(&object)
{
  this->TableWdiget = 0;
  this->tab = 0;
}

//---------------------------------------------------------------------------
qMRMLSimpleTableWidgetPrivate::~qMRMLSimpleTableWidgetPrivate()
{
}


//---------------------------------------------------------------------------
void qMRMLSimpleTableWidgetPrivate::init()
{
	Q_Q(qMRMLSimpleTableWidget);

	QVBoxLayout* layout = new QVBoxLayout(q);
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);




	this->tab = new QTabWidget;

	this->tab->setMaximumHeight(400);

	layout->addWidget(this->tab);


	this->TableWdiget = new QTableWidget;


	this->tab->addTab(this->TableWdiget,tr("Dose Statistics"));

	
}






// --------------------------------------------------------------------------
// qMRMLSimpleTableWidget methods

// --------------------------------------------------------------------------
qMRMLSimpleTableWidget::qMRMLSimpleTableWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new qMRMLSimpleTableWidgetPrivate(*this))
{
  Q_D(qMRMLSimpleTableWidget);
  d->init();
}

// --------------------------------------------------------------------------
qMRMLSimpleTableWidget::~qMRMLSimpleTableWidget()
{
  Q_D(qMRMLSimpleTableWidget);

}


// --------------------------------------------------------------------------
void qMRMLSimpleTableWidget::setMRMLTableViewNode(vtkMRMLTableViewNode* newTableViewNode)
{
 
  this-> tableViewNode = newTableViewNode;
}

// --------------------------------------------------------------------------
vtkMRMLTableViewNode* qMRMLSimpleTableWidget::mrmlTableViewNode()const
{
  return this-> tableViewNode;
}


//---------------------------------------------------------------------------
void qMRMLSimpleTableWidget::setViewLabel(const QString& newTableViewLabel)
{
  this->tableViewLabel = newTableViewLabel;
}

//---------------------------------------------------------------------------
QString qMRMLSimpleTableWidget::viewLabel()const
{
  
  return this->tableViewLabel;
}



QTableWidget* qMRMLSimpleTableWidget::getTableWidget()
{
	Q_D(qMRMLSimpleTableWidget);
	return d->TableWdiget;
}