#pragma once


// Stimulus dialog

class Stimulus : public CDialog
{
	DECLARE_DYNAMIC(Stimulus)

public:
	Stimulus(CWnd* pParent = NULL);   // standard constructor
	virtual ~Stimulus();

// Dialog Data
	enum { IDD = IDD_STIMULUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
