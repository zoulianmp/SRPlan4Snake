#include "qSRPlanNewPatientDialog.h"


#include "ui_qSRPlanNewPatientDialog.h"
 

// Qt includes
#include <QDesktopServices>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>

// Slicer includes
#include "vtkSRPlanConfigure.h" // For Slicer_BUILD_DICOM_SUPPORT
#include "vtkSRPlanVersionConfigure.h"

// SlicerQt includes
#include "qSlicerWelcomeModuleWidget.h"
#include "ui_qSlicerWelcomeModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerIO.h"
#include "qSlicerIOManager.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerModuleManager.h"
#include "qSlicerAbstractCoreModule.h"
#include "qSlicerModulePanel.h"

// CTK includes
#include "ctkButtonGroup.h"

#include "qSRPlanNewPatientDialog.h"

class qSlicerAppMainWindow;

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SlicerWelcome
class  qSRPlanNewPatientDialogPrivate : public Ui_qSRPlanNewPatientDialog
{
	Q_DECLARE_PUBLIC(qSRPlanNewPatientDialog);
protected:
	qSRPlanNewPatientDialog* const q_ptr;
public:
	qSRPlanNewPatientDialogPrivate(qSRPlanNewPatientDialog& object);
	void setupUi(QDialog* dialog);

	 
};

//-----------------------------------------------------------------------------
// qSlicerWelcomeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSRPlanNewPatientDialogPrivate::qSRPlanNewPatientDialogPrivate(qSRPlanNewPatientDialog& object)
	: q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void qSRPlanNewPatientDialogPrivate::setupUi(QDialog* dialog)
{
	this->Ui_qSRPlanNewPatientDialog::setupUi(dialog);

	/*
	// Create the button group ensuring that only one collabsibleWidgetButton will be open at a time
	ctkButtonGroup * group = new ctkButtonGroup(widget);

	// Add all collabsibleWidgetButton to a button group
	QList<ctkCollapsibleButton*> collapsibles = widget->findChildren<ctkCollapsibleButton*>();
	foreach(ctkCollapsibleButton* collapsible, collapsibles)
	{
	group->addButton(collapsible);
	}

	*/
}


//-----------------------------------------------------------------------------
// qSlicerWelcomeModuleWidget methods

//-----------------------------------------------------------------------------
qSRPlanNewPatientDialog::qSRPlanNewPatientDialog(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSRPlanNewPatientDialogPrivate(*this))
{
}

//-----------------------------------------------------------------------------
qSRPlanNewPatientDialog::~qSRPlanNewPatientDialog()
{
}

//-----------------------------------------------------------------------------
void qSRPlanNewPatientDialog::setup()
{
	Q_D(qSRPlanNewPatientDialog);
	d->setupUi(this);


	/*

	connect(d->OpenPatientButton, SIGNAL(clicked()),
		this, SLOT(loadNonDicomData()));

	connect(d->NewPatientButton, SIGNAL(clicked()),
		this, SLOT(loadNonDicomData()));


	connect(d->LoadDicomDataButton, SIGNAL(clicked()),
	this, SLOT(loadDicomData()));

	connect(d->LoadSampleDataButton, SIGNAL(clicked()),
	this, SLOT (loadRemoteSampleData()));
	connect(d->EditApplicationSettingsButton, SIGNAL(clicked()),
	this, SLOT (editApplicationSettings()));

	#ifndef Slicer_BUILD_DICOM_SUPPORT
	d->LoadDicomDataButton->setDisabled(true);
	#endif

	*/

	//this->Superclass::setup();
}


//-----------------------------------------------------------------------------
bool qSRPlanNewPatientDialog::loadDicomData()
{
	Q_D(qSRPlanNewPatientDialog);

	//return d->selectModule("DICOM");

	return true;
}





//-----------------------------------------------------------------------------
bool qSRPlanNewPatientDialog::loadNonDicomData()
{
	qSlicerIOManager *ioManager = qSlicerApplication::application()->ioManager();
	if (!ioManager)
	{
		return false;
	}
	return ioManager->openAddDataDialog();
}

