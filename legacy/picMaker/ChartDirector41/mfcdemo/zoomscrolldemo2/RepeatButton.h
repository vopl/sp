#if !defined(AFX_REPEATBUTTON_H__3C256529_C3FE_45D0_8752_E369482BB7AA__INCLUDED_)
#define AFX_REPEATBUTTON_H__3C256529_C3FE_45D0_8752_E369482BB7AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepeatButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRepeatButton window
//
// A simple control that sents BN_CLICKED repeatedly to its parent when it is being pressed.
//

class CRepeatButton : public CButton
{
public:
// Construction
	CRepeatButton();

// Public interface
	virtual void setRepeatInterval(int ms);
	virtual int getRepeatInterval();
	virtual bool isPressed();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepeatButton)
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRepeatButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private :
	int m_RepeatInterval;
	bool m_ButtonIsPressed;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPEATBUTTON_H__3C256529_C3FE_45D0_8752_E369482BB7AA__INCLUDED_)
