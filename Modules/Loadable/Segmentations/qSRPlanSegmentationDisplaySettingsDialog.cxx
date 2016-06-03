#include "qSRPlanSegmentationDisplaySettingsDialog.h"


#include "ui_qSRPlanSegmentationDisplaySettingsDialog.h"


#include "vtkMRMLPatientInfoNode.h"
#include "vtkMRMLScene.h"


#include "vtkMRMLSegmentationDisplayNode.h"

#include "qSlicerApplication.h"

#include "vtkSlicerSubjectHierarchyModuleLogic.h"
#include "vtkMRMLSubjectHierarchyConstants.h"
#include "vtkMRMLSubjectHierarchyNode.h"


//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SlicerWelcome
class  qSRPlanSegmentationDisplaySettingsDialogPrivate : public Ui_qSRPlanSegmentationDisplaySettingsDialog
{
	Q_DECLARE_PUBLIC(qSRPlanSegmentationDisplaySettingsDialog);
protected:
	qSRPlanSegmentationDisplaySettingsDialog* const q_ptr;
public:
	qSRPlanSegmentationDisplaySettingsDialogPrivate(qSRPlanSegmentationDisplaySettingsDialog& object);
	void setupUi(QDialog* dialog);

	 
};

//-----------------------------------------------------------------------------
// qSlicerWelcomeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSRPlanSegmentationDisplaySettingsDialogPrivate::qSRPlanSegmentationDisplaySettingsDialogPrivate(qSRPlanSegmentationDisplaySettingsDialog& object)
	: q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void qSRPlanSegmentationDisplaySettingsDialogPrivate::setupUi(QDialog* dialog)
{
	this->Ui_qSRPlanSegmentationDisplaySettingsDialog::setupUi(dialog);

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
qSRPlanSegmentationDisplaySettingsDialog::qSRPlanSegmentationDisplaySettingsDialog(QWidget* _parent)
	: Superclass(_parent)
	, d_ptr(new qSRPlanSegmentationDisplaySettingsDialogPrivate(*this))
{
	this->setup();
}

//-----------------------------------------------------------------------------
qSRPlanSegmentationDisplaySettingsDialog::~qSRPlanSegmentationDisplaySettingsDialog()
{
}

//-----------------------------------------------------------------------------
void qSRPlanSegmentationDisplaySettingsDialog::setup()
{
	Q_D(qSRPlanSegmentationDisplaySettingsDialog);
	d->setupUi(this);




	/*

	connect(d->pushButton_loadimage, SIGNAL(clicked()),
		this, SLOT(onConfirmInfoAndLoadImageClicked()));

	connect(d->pushButton_cancel, SIGNAL(clicked()),
		this, SLOT(reject()));




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

void qSRPlanSegmentationDisplaySettingsDialog::onConfirmInfoAndLoadImageClicked()
{/*
	this->CreateBaseSubjectHierarchy();
	this->accept();
	*/
}



bool qSRPlanSegmentationDisplaySettingsDialog::CreateBaseSubjectHierarchy()
{	/*
	Q_D(qSRPlanNewPatientDialog);

	qSlicerApplication * app = qSlicerApplication::application();
	vtkMRMLScene * scene = app->mrmlScene();



	vtkStdString srpatientid = (d->lineEdit_id->text()).toStdString();  
	vtkStdString srcourseid = (d->lineEdit_course_id->text()).toStdString();  
	vtkStdString srplanid = (d->lineEdit_plan_id->text()).toStdString(); 


	vtkSlicerSubjectHierarchyModuleLogic::InsertSRPlanInHierarchy(scene, srpatientid.c_str(), srcourseid.c_str(), srplanid.c_str());


	vtkMRMLSubjectHierarchyNode* patientnode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNodeByUID(scene,vtkMRMLSubjectHierarchyConstants::GetSRPlanPatientUIDName(), srpatientid.c_str());




	patientnode->SetAttribute("PatientName", (d->lineEdit_name->text()).toStdString().c_str());
	patientnode->SetAttribute("PatientAge",  (d->lineEdit_age->text()).toStdString().c_str());
	

	vtkStdString genderstr = (d->comboBox_gender->currentText()).toStdString();

	patientnode->SetAttribute("PatientGender", genderstr.c_str());







	if (!strcmp(genderstr, "Male"))
	{
		infornode->SetPatientGender(vtkMRMLPatientInfoNode::Male);
		patientnode->SetAttribute("PatientGender", );
	}
	else if (!strcmp(genderstr, "Female"))
	{
		infornode->SetPatientGender(vtkMRMLPatientInfoNode::Female);
	}
	else if (!strcmp(genderstr, "Other"))
	{
		infornode->SetPatientGender(vtkMRMLPatientInfoNode::Other);
	}

    //scene->AddNode(infornode);

	infornode->SetName("PatientInfoNode");
	infornode->SetScene(scene);

	scene->AddNode(infornode);
	

	vtkMRMLPatientInfoNode *node = vtkMRMLPatientInfoNode::SafeDownCast( scene->GetNthNodeByClass(0,"vtkMRMLPatientInfoNode"));

	int num = scene->GetNumberOfNodesByClass("vtkMRMLPatientInfoNode");

	*/

	return true;

}


bool qSRPlanSegmentationDisplaySettingsDialog::updateFromDisplayNode(vtkMRMLDisplayNode* display)
{

	Q_D(qSRPlanSegmentationDisplaySettingsDialog);

	vtkMRMLSegmentationDisplayNode* displayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(display);
	if (!displayNode)
	{
		return false;
	}


	// Update display property in dialog
	d->checkBox_Visible->setChecked( displayNode->GetVisibility() );
	
	d->checkBox_SliceIntersectionVisible->setChecked( displayNode->GetSliceIntersectionVisibility() );
	d->spinBox_SliceIntersectionThickness->setValue( displayNode->GetSliceIntersectionThickness() );

	// Populate model representations combobox and select preferred poly data representation
	this->populateRepresentationsCombobox(displayNode);

	// Set display node to display widgets
	d->DisplayNodeViewComboBox->setMRMLDisplayNode(displayNode);



	return true;

}


bool qSRPlanSegmentationDisplaySettingsDialog::updateToDisplayNode(vtkMRMLDisplayNode* display)
{

	Q_D(qSRPlanSegmentationDisplaySettingsDialog);

	vtkMRMLSegmentationDisplayNode* displayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(display);
	if (!displayNode)
	{
		return false;
	}


	// Update display property in to displayNode
	displayNode->SetVisibility(d->checkBox_Visible->checkState());

	displayNode->SetSliceIntersectionVisibility(d->checkBox_SliceIntersectionVisible->checkState());
	
	displayNode->SetSliceIntersectionThickness(d->spinBox_SliceIntersectionThickness->value());



	QString representationName = d->comboBox_DisplayedModelRepresentation->currentText();

	displayNode->SetPreferredPolyDataDisplayRepresentationName(representationName.toLatin1().constData());


	return true;


}





void qSRPlanSegmentationDisplaySettingsDialog::populateRepresentationsCombobox(vtkMRMLDisplayNode* display)
{
	Q_D(qSRPlanSegmentationDisplaySettingsDialog);

	vtkMRMLSegmentationDisplayNode* displayNode = vtkMRMLSegmentationDisplayNode::SafeDownCast(display);
	if (!displayNode)
	{
		return ;
	}

	// Note: This function used to collect existing poly data representations from
	// the segmentation and was connected to events like this:
	// qvtkConnect( segmentationNode, vtkSegmentation::RepresentationCreated, this, SLOT( populateRepresentationsCombobox() ) );
	// It was then decided that a preferred poly data representation can be selected
	// regardless its existence, thus the combobox is populated only once at initialization.

	// Prevent selecting incrementally added representations thus changing MRML properties

	d->comboBox_DisplayedModelRepresentation->blockSignals(true);
	d->comboBox_DisplayedModelRepresentation->clear();

	
	if (!displayNode)
	{
	d->comboBox_DisplayedModelRepresentation->blockSignals(false);
	return;
	}

	// Populate model representations combobox
	std::set<std::string> modelRepresentationNames;
	displayNode->GetPolyDataRepresentationNames(modelRepresentationNames);
	for (std::set<std::string>::iterator reprIt = modelRepresentationNames.begin();
	reprIt != modelRepresentationNames.end(); ++reprIt)
	{
	d->comboBox_DisplayedModelRepresentation->addItem(reprIt->c_str());
	}

	// Unblock signals
	d->comboBox_DisplayedModelRepresentation->blockSignals(false);

	// Set selection from display node
	d->comboBox_DisplayedModelRepresentation->setCurrentIndex( d->comboBox_DisplayedModelRepresentation->findText(
	displayNode->GetPreferredPolyDataDisplayRepresentationName() ) );


}