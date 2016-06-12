/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// Segmentations includes
#include "qMRMLWindowLevelPanel.h"

#include "ui_qMRMLWindowLevelPanel.h"


// Qt includes
#include <QDebug>

//-----------------------------------------------------------------------------
class qMRMLWindowLevelPanelPrivate: public Ui_qMRMLWindowLevelPanel
{
  Q_DECLARE_PUBLIC(qMRMLWindowLevelPanel);

protected:
  qMRMLWindowLevelPanel* const q_ptr;
public:
  qMRMLWindowLevelPanelPrivate(qMRMLWindowLevelPanel& object);
  void init();

public:
 
};

//-----------------------------------------------------------------------------
qMRMLWindowLevelPanelPrivate::qMRMLWindowLevelPanelPrivate(qMRMLWindowLevelPanel& object)
  : q_ptr(&object)
{
  
}

//-----------------------------------------------------------------------------
void qMRMLWindowLevelPanelPrivate::init()
{
  Q_Q(qMRMLWindowLevelPanel);
  this->setupUi(q);


  this->spinBox_l->setReadOnly(true);
  this->spinBox_w->setReadOnly(true);




  QObject::connect(this->toolButton_abdomen, SIGNAL(clicked()),
	  q, SLOT(onWLAbdomenSelected( )));


  QObject::connect(this->toolButton_brain, SIGNAL(clicked()),
	  q, SLOT(onWLBrainSelected()));

  QObject::connect(this->toolButton_air, SIGNAL(clicked()),
	  q, SLOT(onWLAirSelected()));

  QObject::connect(this->toolButton_bone, SIGNAL(clicked()),
	  q, SLOT(onWLBoneSelected()));

  QObject::connect(this->toolButton_lung, SIGNAL(clicked()),
	  q, SLOT(onWLLungSelected()));

  QObject::connect(this->toolButton_customize, SIGNAL(clicked()),
	  q, SLOT(onWLCustomizedSelected()));




  QObject::connect(this->spinBox_w, SIGNAL(valueChanged(int)),
	  q, SLOT(onWindowChanged(int)));

  QObject::connect(this->spinBox_l, SIGNAL(valueChanged(int)),
	  q, SLOT(onLevelChanged(int)));

  
}


//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// qMRMLWindowLevelPanel methods

//-----------------------------------------------------------------------------
qMRMLWindowLevelPanel::qMRMLWindowLevelPanel(QWidget* _parent)
  : QWidget(_parent)
  , d_ptr(new qMRMLWindowLevelPanelPrivate(*this))
{
  Q_D(qMRMLWindowLevelPanel);
  d->init();
  
}

//-----------------------------------------------------------------------------
qMRMLWindowLevelPanel::~qMRMLWindowLevelPanel()
{
}





void qMRMLWindowLevelPanel::onWLAbdomenSelected()
{
	Q_D(qMRMLWindowLevelPanel);
	//Abdomen W/L = 350/40
	d->spinBox_l->setReadOnly(true);
	d->spinBox_w->setReadOnly(true);


	d->spinBox_l->setValue(350);
	d->spinBox_w->setValue(40);

}


void qMRMLWindowLevelPanel::onWLBrainSelected()
{
	Q_D(qMRMLWindowLevelPanel);
	//Brain W/L = 100/50
	d->spinBox_l->setReadOnly(true);
	d->spinBox_w->setReadOnly(true);

	d->spinBox_w->setValue(100);
	d->spinBox_l->setValue(50);


}


void qMRMLWindowLevelPanel::onWLAirSelected()
{
	Q_D(qMRMLWindowLevelPanel);
	//Air W/L = 1000/-426
	d->spinBox_l->setReadOnly(true);
	d->spinBox_w->setReadOnly(true);

	d->spinBox_w->setValue(1000);
	d->spinBox_l->setValue(-426);

}


void qMRMLWindowLevelPanel::onWLBoneSelected()
{
	Q_D(qMRMLWindowLevelPanel);
	//Bone W/L = 1000/400
	d->spinBox_l->setReadOnly(true);
	d->spinBox_w->setReadOnly(true);

	d->spinBox_w->setValue(1000);
	d->spinBox_l->setValue(400);

}

void qMRMLWindowLevelPanel::onWLLungSelected()
{
	Q_D(qMRMLWindowLevelPanel);


	d->spinBox_l->setReadOnly(true);
	d->spinBox_w->setReadOnly(true);

	//Lung W/L = 1400/-500
	d->spinBox_w->setValue(1400);
	d->spinBox_l->setValue(-500);

}

void qMRMLWindowLevelPanel::onWLCustomizedSelected()
{
	Q_D(qMRMLWindowLevelPanel);

	d->spinBox_l->setReadOnly(false);
	d->spinBox_w->setReadOnly(false);



}







void qMRMLWindowLevelPanel::onWindowChanged(int)
{

}

void qMRMLWindowLevelPanel::onLevelChanged(int)
{


}
