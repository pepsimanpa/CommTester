#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

/**
@class    CTCPClient
@author   조용희
@brief    TCP Client 클래스
@see
@remark
@date
   - 2021. 02. 03 : 최초 작성
*/

#include "winsock2.h"
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define TCP_CLIENT_OK              1
#define TCP_CLIENT_ERROR         -1

#define MAX_BUFF_SIZE             8192

#define TCP_SLEEP                                          Sleep
#define TCP_PRINT                                         printf

typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);


class CTCPClient
{
private:
    int m_nServerSock;
    struct sockaddr_in m_tServerInfo;
    bool m_bStart;
    bool m_bConnected;

    int m_nClientSock;
    struct sockaddr_in m_tClientInfo;

    RECEIVECALLBACK m_pReceiveFunc;

    std::thread* m_pThreadReceive;

public:
    CTCPClient(void);
    ~CTCPClient(void);

    void SetServerInfo(const char* strIP, int nPort);
    void SetClientInfo(const char* strIP, int nPort);
    void SetReceiveFunc(RECEIVECALLBACK pFunc);
    int CreateSocket();                     // create socket & set socket option
    int CloseSocket();

    int Init();                      // bind (필수아님, clinet port지정을 위해 사용)
    int Start();                               // connect
    int Stop();
    int Send(char* pBuff, int nSize);

    bool IsStart();

private:
    void Connect();
    void Receive(int nSock);
};


#endif   // __TCP_CLIENT__
