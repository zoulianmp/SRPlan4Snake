

#ifndef __qSRPlanNewPatientDialog_h
#define __qSRPlanNewPatientDialog_h


#include "QDialog.h"

#include "qSRPlanWelcomeModuleExport.h"

class qSRPlanNewPatientDialogPrivate;

/// \ingroup Slicer_QtModules_SlicerWelcome
class Q_SRPlan_QTMODULES_WELCOME_EXPORT qSRPlanNewPatientDialog :
	public QDialog
{
	Q_OBJECT

public:

	typedef QDialog Superclass;
	qSRPlanNewPatientDialog(QWidget *parent = 0);
	virtual ~qSRPlanNewPatientDialog();


public slots:

	bool loadNonDicomData();

	bool loadDicomData();

	

protected:
	virtual void setup();
	//void setupUi(QDialog * dialog);

protected:
	QScopedPointer<qSRPlanNewPatientDialogPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qSRPlanNewPatientDialog);
	Q_DISABLE_COPY(qSRPlanNewPatientDialog);
};

#endif


