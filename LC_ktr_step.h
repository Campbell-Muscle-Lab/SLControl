#pragma once


// LC_ktr_step dialog

class LC_ktr_step : public LC_generic_page
{
	DECLARE_DYNAMIC(LC_ktr_step)

public:
	LC_ktr_step(CWnd* pParent = NULL);   // standard constructor
	virtual ~LC_ktr_step();

// Dialog Data
	enum { IDD = IDD_LC_KTR_STEP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
