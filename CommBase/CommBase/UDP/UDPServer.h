#ifndef __UDP_SERVER__
#define __UDP_SERVER__

/**
@class    CUDPServer
@author   pepsimanpa
@brief    UDP Server 클래스
@see
 @remark
@date
   - 2021. 02. 04 : 최초 작성
*/

#include "winsock2.h"
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define UDP_SERVER_OK              1
#define UDP_SERVER_ERROR         -1

#define MAX_CLIENT_COUNT       10
#define MAX_BUFF_SIZE             8192

#define UDP_SLEEP                                          Sleep
#define UDP_PRINT                                         printf

typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);

class CUDPServer
{
private:
    int m_nServerSock;
    struct sockaddr_in m_tServerAddr;
    bool m_bStart;

    RECEIVECALLBACK m_pReceiveFunc;

    std::thread* m_pThreadReceive;

public:
    CUDPServer(void);
    ~CUDPServer(void);

    void SetServerInfo(const char* strIP, int nPort);
    void SetReceiveFunc(RECEIVECALLBACK pFunc);
    int CreateSocket();                    // create socket & set socket option
    int CloseSocket();

    int Init();                                 // bind
    int Start();                               // receive
    int Stop();
    int Send(const char* pIP, int nPort, const char* pBuff, int nSize);

    bool IsStart();

private:
    void Receive(int nSock);
};


#endif   // __UDP_SERVER__
