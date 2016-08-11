// financedemoDlg.h : header file
//

#if !defined(AFX_FINANCEDEMODLG_H__36AEB9E1_3D1F_4493_9EDE_C2C67E83FC8C__INCLUDED_)
#define AFX_FINANCEDEMODLG_H__36AEB9E1_3D1F_4493_9EDE_C2C67E83FC8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChartViewer.h"

/////////////////////////////////////////////////////////////////////////////
// CFinancedemoDlg dialog

class CFinancedemoDlg : public CDialog
{
public:
// Construction
	CFinancedemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CFinancedemoDlg();

// Dialog Data
	//{{AFX_DATA(CFinancedemoDlg)
	enum { IDD = IDD_FINANCEDEMO_DIALOG };
	CComboBox	m_TickerSymbol;
	CButton	m_VGrid;
	CButton	m_Volume;
	CComboBox	m_TimeRange;
	CEdit	m_MovAvg2;
	CEdit	m_MovAvg1;
	CButton	m_LogScale;
	CComboBox	m_Indicator4;
	CComboBox	m_Indicator3;
	CComboBox	m_Indicator2;
	CComboBox	m_Indicator1;
	CButton	m_HGrid;
	CChartViewer	m_ChartViewer;
	CComboBox	m_ChartType;
	CComboBox	m_ChartSize;
	CComboBox	m_Band;
	CComboBox	m_AvgType2;
	CComboBox	m_AvgType1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFinancedemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	virtual void OnOK();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFinancedemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelectChange();
	afx_msg void OnTextChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// The timeStamps of the data points. It can include data points that are before the
	// startDate (extra leading points) to facilitate moving averages computation.
	double *m_timeStamps;	
	double *m_volData;		// The volume values.
	double *m_highData;		// The high values.
	double *m_lowData;		// The low values.
	double *m_openData;		// The open values.
	double *m_closeData;	// The close values.
	int m_noOfPoints;		// No of data points

    // The moving average periods
	int m_avgPeriod1;
	int m_avgPeriod2;

	// Routines to get data into the data arrays
	virtual void get15MinData(CString ticker, double startDate, double endDate);
	virtual void getDailyData(CString ticker, double startDate, double endDate);
	virtual void getWeeklyData(CString ticker, double startDate, double endDate);
	virtual void getMonthlyData(CString ticker, double startDate, double endDate);

	// Utilities to aggregate data if the data source do not already have aggregated data
	virtual void convertDailyToWeeklyData();
	virtual void convertDailyToMonthlyData();
	virtual void aggregateData(ArrayMath &aggregator);

	// In the demo, we just use random numbers for data.
	virtual void generateRandomData(CString ticker, double startDate, double endDate, 
		int resolution);

	// Draw the chart
	virtual void drawChart(CChartViewer *viewer);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINANCEDEMODLG_H__36AEB9E1_3D1F_4493_9EDE_C2C67E83FC8C__INCLUDED_)
