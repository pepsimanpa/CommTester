#ifndef __UDP__
#define __UDP__

/**
@class    CUDP
@author   pepsimanpa
@brief    UDP Client Ŭ����
@see
@remark
@date
   - 2021. 02. 04 : ���� �ۼ�
*/

#include "winsock2.h"
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define UDP_OK              1
#define UDP_ERROR         -1

#define MAX_BUFF_SIZE             8192

#define UDP_SLEEP                                          Sleep
#define UDP_PRINT                                         printf

typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);


class CUDP
{
private:
    int m_nTargetSock;
    struct sockaddr_in m_tTargetAddr;

    int m_nMySock;
    struct sockaddr_in m_tMyAddr;

    bool m_bStart;
    bool m_bConnected;

    RECEIVECALLBACK m_pReceiveFunc;

    std::thread* m_pThreadReceive;

public:
    CUDP(void);
    ~CUDP(void);

    void SetTargetAddr(const char* strIP, int nPort);
    void SetMyAddr(const char* strIP, int nPort);

    void SetReceiveFunc(RECEIVECALLBACK pFunc);

    int CreateSocket();                     // create socket & set socket option
    int CloseSocket();

    int Bind();                                 // bind
    int Connect();                           // connect (UDP ���� ���, send ��� ����)

    int Start();
    int Stop();
    bool IsStart();
    bool IsConnect();

    int Send(char* pBuff, int nSize);
    int Send(const char* pIP, int nPort, const char* pBuff, int nSize);

private:
    void Receive(int nSock);
};


#endif   // __UDP__
