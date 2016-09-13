

#ifndef __qSRPlanSegmentationDisplaySettingsDialog_h
#define __qSRPlanSegmentationDisplaySettingsDialog_h


#include "QDialog.h"

#include "vtkMRMLDisplayNode.h"

#include "qSRPlanSegmentationsModuleExport.h"

class qSRPlanSegmentationDisplaySettingsDialogPrivate;

/// \ingroup Slicer_QtModules_SlicerWelcome
class Q_SRPlan_QTMODULES_SEGMENTATIONS_EXPORT qSRPlanSegmentationDisplaySettingsDialog :
	public QDialog
{
	Q_OBJECT

public:

	typedef QDialog Superclass;
	qSRPlanSegmentationDisplaySettingsDialog(QWidget *parent = 0);
	virtual ~qSRPlanSegmentationDisplaySettingsDialog();

	void populateRepresentationsCombobox(vtkMRMLDisplayNode* display);

public slots:

	void onConfirmInfoAndLoadImageClicked();

	bool CreateBaseSubjectHierarchy();

	bool updateFromDisplayNode(vtkMRMLDisplayNode* display);

	bool updateToDisplayNode(vtkMRMLDisplayNode* display);


protected:
	virtual void setup();
	//void setupUi(QDialog * dialog);

protected:
	QScopedPointer<qSRPlanSegmentationDisplaySettingsDialogPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qSRPlanSegmentationDisplaySettingsDialog);
	Q_DISABLE_COPY(qSRPlanSegmentationDisplaySettingsDialog);
};

#endif


