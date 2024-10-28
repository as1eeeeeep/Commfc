// CommMFCDlg.h : ͷ�ļ�
//

#pragma once

#include <string>
#include <stdlib.h>
#include <locale.h>
#include <Windows.h>
#include <SetupAPI.h>
#include <vector>


//About CSerialPort start
#include "iostream"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <cmath>
#include <queue>
#include <algorithm>
#include <afxmt.h>


//About CSerialPort end
using namespace std;
using namespace cv;

typedef struct
{
    double range = 0;
    double theta = 0;
} scanpoint;

constexpr auto M10_P_delta_angle = 0.2173;;
constexpr auto N10_P_delta_angle = 0.3375;
;

enum Lidar
{
    N10_P,
    M10_P,
};

// CCommMFCDlg �Ի���
class CCommMFCDlg : public CDialog // About CSerialPort 
{
// ����
public:
	CCommMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_COMMMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��



// ʵ��
protected:
	HICON m_hIcon;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
    CButton OpenMoniterCtrl;
    CButton ConnectLaserCtrl;
	UINT m_ReceiveTimeoutMS;
    CStatic picview;
    CSliderCtrl m_SensityCtrl;
    CRichEditCtrl m_WarningEdit;
    CEdit m_ShowSensity;

	afx_msg void OnClose();
    afx_msg void OnCbnSelchangeComboBaudrate();
    afx_msg void OnBnClickedOpenmoniter();
    afx_msg void OnBnClickedOpenthread();

public:
    CString char_2_string(unsigned char *buf, int len);
    unsigned char asciiToHex1(char byte);
    char *CStringToCharArray(CString str);
    void UpdateRadarData(const scanpoint &point, cv::Mat* &buffer);
    void UpdateDisplayBuffer();
    double calculateRMSE(const std::array<scanpoint, 1587> &array1, const std::array<scanpoint, 1587> &array2,int size);
    void ModifyRichEditRed();
    void ModifyRichEditGreen();
    void  CCommMFCDlg::get_port_number(std::vector<wstring>& PortNumber);
    
    //��������
    HANDLE hSerial;
    //HANDLE hSerial = CreateFile(TEXT("COM3"),                 // ���ںţ����� "COM3"
    //                            GENERIC_READ | GENERIC_WRITE, // �����ڶ�д
    //                            0,                            // ��ռ����
    //                            NULL,                         // ��ȫ����
    //                            OPEN_EXISTING,                // �����е��豸
    //                            FILE_ATTRIBUTE_NORMAL,        // ��������
                                //NULL);
    DCB dcbSerialParams = {0};

	//���߳�����
	void CCommMFCDlg::DoWork();
    void ComputeData();
    static UINT ThreadProc(LPVOID pParam);
    static UINT ThreadProc2(LPVOID pParam);

    bool isOpenMoniter=FALSE,isFrontbuffer=TRUE,isReadData = TRUE,isCompute_Data=FALSE,isFirstRecordData=TRUE,isFirstCompute = TRUE,isRecodData=FALSE;
    Mat black_front = Mat::zeros(1000, 1000, CV_8UC1);
    Mat black_back = Mat::zeros(1000, 1000, CV_8UC1);
    Mat *draw_buffer,*display_buffer;
    Lidar Lidar_Type = M10_P;

	double last_angle, start_angle,stop_angle;
    int sum_point;
    scanpoint laser_point_current, laser_point_last;

    array<scanpoint,1587> WriteData_array, ComputeData_array, Environment_array;

    CCriticalSection criticalSection;


    afx_msg void OnBnClickedReset();
    
    afx_msg void OnNMCustomdrawSensity(NMHDR *pNMHDR, LRESULT *pResult);
    
    
    afx_msg void OnCbnSelchangeCombo1();
    CComboBox m_LaserType;
    afx_msg void OnCbnSelchangeLasertype();
private:
    CComboBox port_number;
public:
    afx_msg void OnCbnSelchangeCombo2();
    afx_msg void OnCbnDropdownCombo2();
};
