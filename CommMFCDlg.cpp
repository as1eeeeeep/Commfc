// CommMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CommMFC.h"
#include "CommMFCDlg.h"
#include <Windows.h>
#include <bluetoothapis.h>
#include <bthsdpdef.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCommMFCDlg 对话框




CCommMFCDlg::CCommMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommMFCDlg::IDD, pParent)
	, m_ReceiveTimeoutMS(0)
    , draw_buffer(&black_front)
    , display_buffer(&black_back)
{
    
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCommMFCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PIC, picview);
    DDX_Control(pDX, IDC_OpenMoniter, OpenMoniterCtrl);
    DDX_Control(pDX, IDC_OpenThread, ConnectLaserCtrl);
    DDX_Control(pDX, IDC_WarningEdit, m_WarningEdit);
    DDX_Control(pDX, IDC_Sensity, m_SensityCtrl);
    DDX_Control(pDX, IDC_ShowSensity, m_ShowSensity);
    DDX_Control(pDX, IDC_LaserType, m_LaserType);
    DDX_Control(pDX, IDC_COMBO2, port_number);
    //DDX_Control(pDX, IDC_Sample, Sample);
    DDX_Control(pDX, IDC_show_Sample, show_Sample);
}

BEGIN_MESSAGE_MAP(CCommMFCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
    ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CCommMFCDlg::OnCbnSelchangeComboBaudrate)
    ON_BN_CLICKED(IDC_OpenMoniter, &CCommMFCDlg::OnBnClickedOpenmoniter)
    ON_BN_CLICKED(IDC_OpenThread, &CCommMFCDlg::OnBnClickedOpenthread)
    ON_BN_CLICKED(IDC_ReSet, &CCommMFCDlg::OnBnClickedReset)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_Sensity, &CCommMFCDlg::OnNMCustomdrawSensity)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CCommMFCDlg::OnCbnSelchangeCombo1)
    ON_CBN_SELCHANGE(IDC_LaserType, &CCommMFCDlg::OnCbnSelchangeLasertype)
//    ON_CBN_SELCHANGE(IDC_COMBO2, &CCommMFCDlg::OnCbnSelchangeCombo2)
ON_CBN_SELCHANGE(IDC_COMBO2, &CCommMFCDlg::OnCbnSelchangeCombo2)
//ON_CBN_DROPDOWN(IDC_COMBO2, &CCommMFCDlg::OnCbnDropdownCombo2)
ON_CBN_DROPDOWN(IDC_COMBO2, &CCommMFCDlg::OnCbnDropdownCombo2)
ON_EN_CHANGE(IDC_ShowSensity, &CCommMFCDlg::OnEnChangeShowsensity)
//ON_STN_CLICKED(IDC_Sample, &CCommMFCDlg::OnStnClickedSample)
ON_STN_CLICKED(IDC_PIC, &CCommMFCDlg::OnStnClickedPic)
END_MESSAGE_MAP()


// CCommMFCDlg 消息处理程序

BOOL CCommMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	 //TODO: 在此添加额外的初始化代码
    CString temp;
    String LaserTypeArray[] = {"","M10_P", "N10_P"};
    for (int i = 0; i < sizeof(LaserTypeArray) / sizeof(std::string); i++)
    {
        temp.Format(_T("%S"), LaserTypeArray[i].c_str());
        m_LaserType.InsertString(i, temp);
    }
    m_LaserType.SetCurSel(0);
    
    //自动获取端口号
    std::vector<wstring> PortNumber;
    get_port_number(PortNumber);
    for (const auto& wstr : PortNumber) // 通常COM端口号不会超过20  
    {
        CString port(wstr.c_str());
        port_number.AddString(port);
    }
    //port_number.SetCurSel(0);

    m_SensityCtrl.SetRange(0, 100);
    m_SensityCtrl.SetPos(35);

    OpenMoniterCtrl.EnableWindow(0);
    ConnectLaserCtrl.EnableWindow(0);

    /*AllocConsole();
    freopen("CONOUT$", "w", stdout);*/

	

	CRect rect;
    GetDlgItem(IDC_PIC)->GetClientRect(rect);
    namedWindow("picView", CV_WINDOW_NORMAL);
    resizeWindow("picView", rect.Width(), rect.Height());
    cout << rect.Width() << " " << rect.Height() << endl;

    HWND hWnd = (HWND)cvGetWindowHandle("picView");
    HWND hParent = ::GetParent(hWnd);
    ::SetParent(hWnd, GetDlgItem(IDC_PIC)->m_hWnd);
    ::ShowWindow(hParent, SW_HIDE);

	// 默认接收超时时间(毫秒)
	//添加波特率到下拉列表
	//校验位
	//数据位
	//停止位
	//获取串口号
	//OnBnClickedButtonOpenClose()
	
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCommMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCommMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCommMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
* 从串口读取的数据为char类型，需要从char转到string
*/

CString CCommMFCDlg::char_2_string(unsigned char *buf, int len)
{
    // TODO: 在此处添加实现代码.
    CString str_ret = _T("");
    CString str_temp = _T("");
    int i;
    for (i = 0; i < len; i++)
    {
        str_temp.Format(_T("%.2X "), buf[i]);
        str_ret += str_temp;
    }
    return str_ret;
}

/*
* 从string类型转换为字符数组类型，方便后续的拼接操作
*/

char *CCommMFCDlg::CStringToCharArray(CString str)
{
    char *ptr;
#ifdef _UNICODE
    LONG len;
    len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    ptr = new char[len + 1];
    memset(ptr, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, len + 1, NULL, NULL);
#else
    ptr = new char[str.GetAllocLength() + 1];
    sprintf(ptr, _T("%s"), str);
#endif
    return ptr;
}

/*
*   ascii字符转换为十六进制数据
* 
*/

unsigned char CCommMFCDlg::asciiToHex1(char byte)
{
    unsigned char ret = 0;
    if ((byte >= '0') && (byte <= '9'))
    {
        ret = byte - '0';
    }
    else if ((byte >= 'A') && (byte <= 'F'))
    {
        ret = byte - 'A' + 0xA;
    }
    else if ((byte >= 'a') && (byte <= 'f'))
    {
        ret = byte - 'a' + 0xA;
    }
    return ret;
}

/*
*   双缓存技术显示图像
*   后端绘制更新
*/
void CCommMFCDlg::UpdateRadarData(const scanpoint &point, cv::Mat* &buffer)
{
    // 计算激光点在图像上的位置
    int factor = 100;
    int x = (int)(500 + factor * (point.range * sin(point.theta * 3.14 / 180)));
    int y = (int)(500 - factor * (point.range * cos(point.theta * 3.14 / 180)));

    // 绘制激光点
    cv::circle(*buffer, cv::Point(x, y), 2, cv::Scalar(255), -1);
}

/*
*   双缓存技术显示图像
*   交换前后端
*/
void CCommMFCDlg::UpdateDisplayBuffer()
{
    isFrontbuffer = !isFrontbuffer;
    draw_buffer = isFrontbuffer ? &black_back : &black_front;
    draw_buffer->setTo(Scalar(0));
    display_buffer = isFrontbuffer ? &black_front : &black_back;
}
/*
* 工作线程1、根据串口协议获取雷达的点坐标数据
* 
*/


void CCommMFCDlg::DoWork()
{
    char *buffer = NULL;
    unsigned char *data = new unsigned char[4096 + 1];
    DWORD bytesRead = 0;
    CString str1;
    unsigned int list_lenght = 0, data_lenght = 0;
    double speed, range = 0.0;
    int j = 0;
    int i;
    int clear_count=0;
    int save_point_num=0;
    double integerPart = 0,Last_integerPart = 400; // 整数部分
    double fractionalPart=0;    //小数部分
    while (isReadData)
    {
        switch (Lidar_Type)
        {
            case N10_P:
            {
                    /*
                        ReadFile(): 根据串口配置读取对应长度的数据，并把数据存储到缓存中
                        输入 （串口类，数据缓存，期望读取的数据长度，实际读取的数据长度）
                    */
                if (ReadFile(hSerial, data, 1, &bytesRead, NULL))       
                {
                    // 处理读取到的数据
                    if (bytesRead > 0)
                    {
                        str1 = char_2_string(data, bytesRead);
                        buffer = CStringToCharArray(str1);
                        if (buffer[0] == 'A' && buffer[1] == '5')               // 解析数据协议
                        {
                            if (ReadFile(hSerial, data, 1, &bytesRead, NULL))
                            {
                                str1 = char_2_string(data, bytesRead);
                                buffer = CStringToCharArray(str1);
                                if (buffer[0] == '5' && buffer[1] == 'A')
                                {
                                    if (ReadFile(hSerial, data, 1, &bytesRead, NULL))
                                    {
                                        str1 = char_2_string(data, bytesRead);
                                        buffer = CStringToCharArray(str1);
                                        //buffer为十六进制数组，因此在处理过程中要注意高低位
                                        list_lenght = (unsigned int)asciiToHex1(buffer[0]) * 16 + (unsigned int)asciiToHex1(buffer[1]);     
                                        if (list_lenght > 0)
                                        {
                                            data_lenght = list_lenght - 3;
                                            if (ReadFile(hSerial, data, data_lenght, &bytesRead, NULL))
                                            {
                                                str1 = char_2_string(data, bytesRead);              //把读出来的数据转为CSTRNG
                                                buffer = CStringToCharArray(str1);                  //CString to char*
                                                last_angle = start_angle;
                                                speed = (double)60/((((unsigned int)asciiToHex1(buffer[0]) * 16 + (unsigned int)asciiToHex1(buffer[1])) * 256 +
                                                                 ((unsigned int)asciiToHex1(buffer[3]) * 16 + (unsigned int)asciiToHex1(buffer[4]))) * 0.000024);                                                
                                                start_angle = (double)(((unsigned int)asciiToHex1(buffer[6]) * 16 + (unsigned int)asciiToHex1(buffer[7])) * 256 +
                                                        ((unsigned int)asciiToHex1(buffer[9]) * 16 + (unsigned int)asciiToHex1(buffer[10])))/100;
                                                stop_angle = (double)(((unsigned int)asciiToHex1(buffer[306]) * 16 + (unsigned int)asciiToHex1(buffer[307])) * 256 +
                                                        ((unsigned int)asciiToHex1(buffer[309]) * 16 + (unsigned int)asciiToHex1(buffer[310])))/100;
                                                if (start_angle <= 12)                  //判断是否为第一次从0度角开始转 
                                                    isRecodData = TRUE;
                                                for (i = 4; i < 100; i += 3)
                                                {
                                                    range = (double)((asciiToHex1(buffer[3 * i]) * 16 + asciiToHex1(buffer[3 * i + 1])) * 256 +
                                                                        (asciiToHex1(buffer[3 * (i + 1)]) * 16 + asciiToHex1(buffer[3 * (i + 1) + 1])));
                                                    range /= 1000;
                                                    laser_point_current.range = range;
                                                    laser_point_current.theta = start_angle + (double)(((i - 4) / 3) * N10_P_delta_angle);
                                                    if (isRecodData)
                                                    {
                                                        fractionalPart = modf(laser_point_current.theta, &integerPart);         //把最接近整数角的点当作距离点
                                                        if (integerPart >= 360)
                                                        {
                                                            integerPart -= 360;
                                                        }                         
                                                        if (fractionalPart < 0.3375)            //0.3375最小分辨率
                                                        {
                                                            if (integerPart != Last_integerPart)
                                                            {
                                                                if (laser_point_current.range == 0)
                                                                {
                                                                    WriteData_array[integerPart] = laser_point_last;
                                                                }                                                               
                                                                else
                                                                    WriteData_array[integerPart] = laser_point_current; // 保证每个角度都只取一次
                                                                save_point_num++;
                                                                Last_integerPart = integerPart;
                                                            }
                                                        }
                                                    }
                                                    laser_point_last = laser_point_current;
                                                    try
                                                    {
                                                        UpdateRadarData(laser_point_current, draw_buffer);
                                                    }
                                                    catch (const std::exception &e)
                                                    {
                                                        cout << "sth happenning" << endl;
                                                        continue;
                                                    }
                                                }
                                                if (save_point_num >= 360)
                                                {
                                                    isCompute_Data = TRUE;
                                                    isRecodData = FALSE;
                                                    save_point_num = 0;
                                                }

                                                if (((last_angle - start_angle) > 100) && clear_count > 0)
                                                {
                                                    UpdateDisplayBuffer();          //双缓存显示图像
                                                    clear_count = 0;
                                                }
                                                clear_count++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }
            case M10_P:
            {
                if (ReadFile(hSerial, data, 1, &bytesRead, NULL))
                {
                    // 处理读取到的数据
                    if (bytesRead > 0)
                    {
                        str1 = char_2_string(data, bytesRead);
                        buffer = CStringToCharArray(str1);
                        if (buffer[0] == 'A' && buffer[1] == '5')
                        {
                            if (ReadFile(hSerial, data, 1, &bytesRead, NULL))
                            {
                                str1 = char_2_string(data, bytesRead);
                                buffer = CStringToCharArray(str1);
                                if (buffer[0] == '5' && buffer[1] == 'A')
                                {
                                    if (ReadFile(hSerial, data, 1, &bytesRead, NULL))
                                    {
                                        str1 = char_2_string(data, bytesRead);
                                        buffer = CStringToCharArray(str1);
                                        if (buffer[0] == '0' && buffer[1] == '0')
                                        {
                                            if (ReadFile(hSerial, data, 1, &bytesRead, NULL))
                                            {
                                                str1 = char_2_string(data, bytesRead);
                                                buffer = CStringToCharArray(str1);
                                                list_lenght = (unsigned int)asciiToHex1(buffer[0]) * 16 + (unsigned int)asciiToHex1(buffer[1]);
                                                if (list_lenght > 0)
                                                {
                                                    data_lenght = list_lenght - 4;
                                                    if (ReadFile(hSerial, data, data_lenght, &bytesRead, NULL))
                                                    {
                                                        str1 = char_2_string(data, bytesRead);
                                                        buffer = CStringToCharArray(str1);
                                                        last_angle = start_angle;
                                                        start_angle = (double)(((unsigned int)asciiToHex1(buffer[0]) * 16 + (unsigned int)asciiToHex1(buffer[1])) * 256 +
                                                                               ((unsigned int)asciiToHex1(buffer[3]) * 16 + (unsigned int)asciiToHex1(buffer[4]))) /
                                                                      100;
                                                        if (start_angle == 8)
                                                            isRecodData = TRUE;
                                                        speed = ((unsigned int)asciiToHex1(buffer[6]) * 16 + (unsigned int)asciiToHex1(buffer[7])) * 256 +
                                                                ((unsigned int)asciiToHex1(buffer[9]) * 16 + (unsigned int)asciiToHex1(buffer[10]));
                                                        speed = (double)2500000 / speed;
                                                        for (i = 4; i < 142; i += 2)
                                                        {
                                                            if ((asciiToHex1(3 * i) * 16 + asciiToHex1(3 * i + 1)) & 0x80)
                                                            {
                                                                range = (double)(((asciiToHex1(buffer[3 * i]) * 16 + asciiToHex1(buffer[3 * i + 1])) & 0x7F) * 256 +
                                                                                 (asciiToHex1(buffer[3 * (i + 1)]) * 16 + asciiToHex1(buffer[3 * (i + 1) + 1])));
                                                                range /= 1000;
                                                                laser_point_current.range = range;
                                                                laser_point_current.theta = start_angle + (double)(((i - 4) / 2) * M10_P_delta_angle);
                                                            }
                                                            else
                                                            {
                                                                range = (double)((asciiToHex1(buffer[3 * i]) * 16 + asciiToHex1(buffer[3 * i + 1])) * 256 +
                                                                                 (asciiToHex1(buffer[3 * (i + 1)]) * 16 + asciiToHex1(buffer[3 * (i + 1) + 1])));
                                                                range /= 1000;
                                                                laser_point_current.range = range;
                                                                laser_point_current.theta = start_angle + (double)(((i - 4) / 2) * M10_P_delta_angle);
                                                            }

                                                            if (isRecodData)
                                                            {
                                                                WriteData_array[save_point_num] = laser_point_current;
                                                                save_point_num++;
                                                            }

                                                            try
                                                            {
                                                                UpdateRadarData(laser_point_current, draw_buffer);
                                                            }
                                                            catch (const std::exception &e)
                                                            {
                                                                cout << "sth happenning" << endl;
                                                                continue;
                                                            }
                                                        }             
                                                        if (save_point_num == 1587)
                                                        {
                                                            isCompute_Data = TRUE;
                                                            isRecodData = FALSE;
                                                            if (isSample) 
                                                            { 
                                                                imwrite("sample_buffer.png", *draw_buffer);   
                                                            }
                                                            isSample = FALSE;
                                                            save_point_num = 0;
                                                        }

                                                        if (((last_angle - start_angle) > 100) && clear_count > 5)
                                                        {
                                                            UpdateDisplayBuffer();
                                                            clear_count = 0;
                                                        }
                                                        clear_count++;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }
                
            default:
                break;
        }
        
    }
    delete data;
}

UINT CCommMFCDlg::ThreadProc(LPVOID pParam)
{
    CCommMFCDlg *pThis = (CCommMFCDlg *)pParam;

    // 调用成员函数
    pThis->DoWork();

    return 0;
}

UINT CCommMFCDlg::ThreadProc2(LPVOID pParam)
{
    CCommMFCDlg *pThis = (CCommMFCDlg *)pParam;

    // 调用成员函数
    pThis->ComputeData();
    return 0;
}

void CCommMFCDlg::OnClose()
{
	CDialog::OnClose();
    isOpenMoniter = FALSE;
    isReadData = FALSE;
    cv::destroyAllWindows();
    black_front.release();
    black_back.release();
    Sleep(200);
    if (hSerial != INVALID_HANDLE_VALUE)
    {
        PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
    CDialog::OnClose();
    /*delete draw_buffer;
    draw_buffer = nullptr;
    delete display_buffer;
    display_buffer = nullptr;*/
}

void CCommMFCDlg::OnCbnSelchangeComboBaudrate()
{
    // TODO: 在此添加控件通知处理程序代码
}

/*
* 均方值误差计算
* 
*/

double CCommMFCDlg::calculateRMSE(const array<scanpoint, 1587> &Environment_array, const array<scanpoint, 1587> &ComputeData_array,int size)
{
    double sumOfSquares = 0.0;
    double error = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        // 假设我们只比较 range 成员
        
        error = std::pow(Environment_array[i].range - ComputeData_array[i].range, 2);
        if (error < 1e-04)
        {
            error = 0.0;
        }
        else if (error > 15)
        {
            //cout << "angle*********" << ComputeData_array[i].theta << "error*********" << error << endl;
            error = 0.0;
        }
        
        sumOfSquares += error;
    }
    double rmse = std::sqrt(sumOfSquares / size);
    return rmse;
}

void CCommMFCDlg::ModifyRichEditRed()
{
    CRichEditCtrl *m_pRichEdit;
    m_pRichEdit = (CRichEditCtrl *)GetDlgItem(IDC_WarningEdit); // RichEdit
    // 设置字体大小和颜色
    CHARFORMAT cf{0};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_SIZE;
    cf.yHeight = 20 * 20;            // 20点字体大小
    cf.crTextColor = RGB(255, 0, 0); // 文本颜色设置为红色
    m_pRichEdit->SetDefaultCharFormat(cf);
}

void CCommMFCDlg::ModifyRichEditGreen()
{
    CRichEditCtrl *m_pRichEdit;
    m_pRichEdit = (CRichEditCtrl *)GetDlgItem(IDC_WarningEdit); // RichEdit
    // 设置字体大小和颜色
    CHARFORMAT cf{0};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_SIZE;
    cf.yHeight = 20 * 20;            // 20点字体大小
    cf.crTextColor = RGB(0, 255, 0); // 文本颜色设置为绿色
    m_pRichEdit->SetDefaultCharFormat(cf);
}

/*
* 工作线程2：均方根误差计算，阈值比较，判断环境是否改变
* 
*/

void CCommMFCDlg::ComputeData()
{
    CString isSafety = _T("无入侵");
    CString notSafety = _T("检测到入侵！");
    double outcome = 0.0, First_Outcome = 0.0, result = 0.0;
    double sum_outcome = 0.0, Max_data = 0.0;
    int sum_count = 0;
    int count = 1587,Count_First=0;
    double Sensity = 0.0;
    CString strSensity;
    while (1)
    {
        Sensity = (double)m_SensityCtrl.GetPos()/100;
        strSensity.Format(_T("%f"), Sensity);
        m_ShowSensity.SetWindowText(strSensity);

        //cout << "Sensity" << Sensity << endl;
        switch (Lidar_Type)
        {
            case N10_P:
            {
                if (isCompute_Data && isOpenMoniter)
                {
                    count = 360;
                    criticalSection.Lock();
                    if (isFirstRecordData)                                                  //记录基准点云
                    {
                        isFirstRecordData = FALSE;
                        while (count--)
                        {
                            Environment_array[count] = WriteData_array[count];
                        }
                        count = 360;
                        outcome = calculateRMSE(Environment_array, WriteData_array,360);
                    }
                    outcome = calculateRMSE(Environment_array, WriteData_array,360);
                    sum_outcome += outcome;                                                 //平均值滤波
                    sum_count++;
                    if (sum_count > 5)
                    {
                        outcome = sum_outcome / sum_count;
                        if (isFirstCompute)
                        {
                            First_Outcome = outcome;
                            isFirstCompute = FALSE;
                        }

                        if (outcome > Max_data)
                            Max_data = outcome;

                        result = (outcome - First_Outcome) / (outcome + First_Outcome);     //与基准做对比

                        sum_count = 0;
                        sum_outcome = 0.0;
                        if (result > Sensity)
                        {
                            ModifyRichEditRed();
                            m_WarningEdit.SetWindowText(notSafety);
                        }
                        else
                        {
                            ModifyRichEditGreen();
                            m_WarningEdit.SetWindowText(isSafety);
                        }
                    }
                    criticalSection.Unlock();
                    isCompute_Data = FALSE;
                }
                break;
            }
            case M10_P:
            {
                if (isCompute_Data && isOpenMoniter)
                {
                    criticalSection.Lock();
                    if (isFirstRecordData)                                      //记录第一帧点云数据
                    {   
                        isFirstRecordData = FALSE;
                        while (count--)
                        {
                            Environment_array[count] = WriteData_array[count];
                        }
                        count = 1587;
                    }

                    outcome = calculateRMSE(Environment_array, WriteData_array,1587);
                    sum_outcome += outcome;
                    sum_count++;
                    if (sum_count > 10) 
                    {
                        outcome = sum_outcome / sum_count;
                        if (isFirstCompute)
                        {
                            First_Outcome = outcome;
                            isFirstCompute = FALSE;
                        }

                        if (outcome > Max_data)
                            Max_data = outcome;

                        result = (outcome - First_Outcome) / (outcome + First_Outcome);

                        sum_count = 0;
                        sum_outcome = 0.0;
                        if (result > Sensity)
                        {
                            ModifyRichEditRed();
                            m_WarningEdit.SetWindowText(notSafety);
                        }
                        else
                        {
                            ModifyRichEditGreen();
                            m_WarningEdit.SetWindowText(isSafety);
                        }
                    }
                    criticalSection.Unlock();
                    isCompute_Data = FALSE;
                }
                break;
            }
            default:
                break;
        }
        
        
    }
}

void CCommMFCDlg::OnBnClickedOpenmoniter()
{    
    show_buffer();
    if (!isOpenMoniter)
    {
        OpenMoniterCtrl.SetWindowText(_T("关闭监控")); 
		isOpenMoniter = TRUE;
    }
	else
	{
        OpenMoniterCtrl.SetWindowText(_T("打开监控"));
        isOpenMoniter = FALSE;
        imshow("picView", Mat::zeros(1000, 1000, CV_8UC1));
        int key = waitKey(50);
    }

    // 将opencv的窗体嵌入到页面中
    while (isOpenMoniter)
    {        
        imshow("picView", *display_buffer);
        int key = waitKey(20);
        if (key == 27)
        {
            break;
        }
    }
    //cv::destroyAllWindows();
    // TODO: 在此添加控件通知处理程序代码
 
}
/*
*   按钮“连接雷达”消息的映射函数，当连接雷达被点击时触发该函数
* 
*   根据雷达名字初始化相关的串口和波特率，并启动工作线程
*   1、线程1：根据串口读取雷达的点数据
*   2、线程2：根据点的数据计算前后帧图像的均方根误差
* 
*/


void CCommMFCDlg::OnBnClickedOpenthread()
{
    // TODO: 在此添加控件通知处理程序代码
    CString temp;
    m_LaserType.GetWindowText(temp);
    CStringA strA(temp);
    string Laser_name(strA);
    if (Laser_name == "M10_P")
    {
        Lidar_Type = M10_P;
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (GetCommState(hSerial, &dcbSerialParams))
        {
            dcbSerialParams.BaudRate = 512000;     // 设置波特率
            dcbSerialParams.ByteSize = 8;          // 数据位
            dcbSerialParams.StopBits = ONESTOPBIT; // 停止位
            dcbSerialParams.Parity = NOPARITY;     // 奇偶校验

            SetCommState(hSerial, &dcbSerialParams);
        }
        cout << "success init M10_P" << endl;
        Sleep(500);
    }
    else if (Laser_name == "N10_P")
    {
        Lidar_Type = N10_P;
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        if (GetCommState(hSerial, &dcbSerialParams))
        {
            dcbSerialParams.BaudRate = 460800;     // 设置波特率
            dcbSerialParams.ByteSize = 8;          // 数据位
            dcbSerialParams.StopBits = ONESTOPBIT; // 停止位
            dcbSerialParams.Parity = NOPARITY;     // 奇偶校验

            SetCommState(hSerial, &dcbSerialParams);
        }
        cout << "success init N10_P" << endl;
        Sleep(500);
    }
    

    CWinThread *pThread = AfxBeginThread(CCommMFCDlg::ThreadProc, this);
    CWinThread *pThread2 = AfxBeginThread(CCommMFCDlg::ThreadProc2, this);
    ConnectLaserCtrl.EnableWindow(0);
    OpenMoniterCtrl.EnableWindow(1);
}

void  CCommMFCDlg::get_port_number(std::vector<wstring>& PortNumber)
{
    HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    SP_CLASSIMAGELIST_DATA _spImageData = { 0 };
    _spImageData.cbSize = sizeof(SP_CLASSIMAGELIST_DATA);
    SetupDiGetClassImageList(&_spImageData);

    SP_DEVINFO_DATA spDevInfoData = { 0 };
    spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++)
    {
        char  szBuf[MAX_PATH] = { 0 };
        int  wImageIdx = 0;
        short  wItem = 0;
        if (!SetupDiGetDeviceRegistryPropertyA(hDevInfo, &spDevInfoData, SPDRP_CLASS, NULL, (PBYTE)szBuf, MAX_PATH, 0))
        {
            continue;
        };
        if (strcmp(szBuf, "Ports") != 0) //只取端口
        {
            continue;
        }
        if (SetupDiGetClassImageIndex(&_spImageData, &spDevInfoData.ClassGuid, &wImageIdx))
        {
            wchar_t szName[MAX_PATH] = { 0 };
            DWORD  dwRequireSize;

            if (!SetupDiGetClassDescription(&spDevInfoData.ClassGuid, (PWSTR)szBuf, MAX_PATH, &dwRequireSize))
            {
                continue;
            };

            if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_FRIENDLYNAME, NULL, (PBYTE)szName, MAX_PATH - 1, 0))
            {
                std::wstring port(szName);
                PortNumber.push_back(port);
                //size_t length = wcslen(szName);
                //size_t start = length - 5;
                //size_t end = length - 2;
                //std::wstring ws(szName + start, end - start + 1); // 使用wstring的构造函数，指定起始位置和长度
                //const wchar_t* wcs_sub = ws.c_str();
                //MessageBox(wcs_sub);

            }
            //if (SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_DEVICEDESC, NULL, (PBYTE)szName, MAX_PATH - 1, 0))
            //{
            //    MessageBox(szName);
            //}
        }
    }
    SetupDiDestroyClassImageList(&_spImageData);
}


void CCommMFCDlg::OnBnClickedReset()
{
    // TODO: 在此添加控件通知处理程序代码
    isFirstRecordData = TRUE;
    isFirstCompute = TRUE;
    isSample = TRUE;
}

void CCommMFCDlg::OnNMCustomdrawSensity(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;
}

void CCommMFCDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
    // 串口配置

    ConnectLaserCtrl.EnableWindow(1);
    cout << "change now" << endl;
}


void CCommMFCDlg::OnCbnSelchangeLasertype()
{
    // TODO: 在此添加控件通知处理程序代码
}



void CCommMFCDlg::OnCbnSelchangeCombo2()
{
    // TODO: 在此添加控件通知处理程序代码
    //int nSel = port_number.GetCurSel();
    //CString SelectedPort;
    //port_number.GetLBText(nSel, SelectedPort);

    //CString strSerialPath;
    //strSerialPath.Format(_T("\\\\.\\%s"), (LPCTSTR)SelectedPort);

    //// 打开串口  
    //hSerial = CreateFile(strSerialPath,
    //    GENERIC_READ | GENERIC_WRITE,
    //    0, // 共享模式，0表示不共享  
    //    nullptr, // 安全属性  
    //    OPEN_EXISTING,
    //    0, // 文件属性  
    //    nullptr); // 模板文件 
    int nSel = port_number.GetCurSel();
    CString SelectedPort;
    port_number.GetLBText(nSel, SelectedPort);
    int length = SelectedPort.GetLength();
    int startIndex = length - 5; // 倒数第五个字符的索引  
    int endIndex = length - 2;   // 倒数第二个字符的索引（包含）
    CString Portnumber = SelectedPort.Mid(startIndex, 4);
    Portnumber.Format(_T("\\\\.\\%s"), (LPCTSTR)Portnumber);
    hSerial = CreateFile(Portnumber,
    GENERIC_READ | GENERIC_WRITE,
    0, // 共享模式，0表示不共享  
    nullptr, // 安全属性  
    OPEN_EXISTING,
    0, // 文件属性  
    nullptr); // 模板文件 
}




void CCommMFCDlg::OnCbnDropdownCombo2()
{
    // TODO: 在此添加控件通知处理程序代码
    CClientDC dc(this);
    int nWitdh = 10;
    int nSaveDC = dc.SaveDC();

    //获取字体信息，
    dc.SelectObject(GetFont());

    //计算最大的显示长度
    for (int i = 0; i < port_number.GetCount(); i++)
    {
        CString strLable = _T("");
        port_number.GetLBText(i, strLable);
        nWitdh = max(nWitdh, int(dc.GetTextExtent(strLable).cx));
    }


    //设置下拉列表宽度
    port_number.SetDroppedWidth(nWitdh+10);
    //恢复实际dc
    dc.RestoreDC(nSaveDC);
  
}


void CCommMFCDlg::OnEnChangeShowsensity()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
}


void CCommMFCDlg::show_buffer()
{
    // TODO: 在此处添加实现代码.
    CImage sample_image;
    sample_image.Load(TEXT("sample_buffer.png"));
    CRect rectControl;
    show_Sample.GetClientRect(rectControl);
    CDC* pDC = show_Sample.GetDC();
    sample_image.Draw(pDC->m_hDC, rectControl);
    sample_image.Destroy();
    show_Sample.ReleaseDC(pDC);
}


void CCommMFCDlg::OnStnClickedPic()
{
    // TODO: 在此添加控件通知处理程序代码
}
