#ifndef __TCP_SERVER__
#define __TCP_SERVER__

/**
@class    CTCPServer
@author   pepsimanpa
@brief    TCP Server 클래스
@see
 @remark
@date
   - 2021. 02. 03 : 최초 작성
*/

#include "winsock2.h"
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define TCP_SERVER_OK              1
#define TCP_SERVER_ERROR         -1

#define MAX_CLIENT_COUNT       10
#define MAX_BUFF_SIZE             8192

#define TCP_SLEEP                                         Sleep
#define TCP_PRINT                                         printf

typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);

class CTCPServer
{
private:
    int m_nServerSock;
    struct sockaddr_in m_tServerAddr;
    bool m_bStart;

    int m_nClientCount;
    int m_nClientSock[MAX_CLIENT_COUNT];
    struct sockaddr_in m_tClientAddr[MAX_CLIENT_COUNT];

    RECEIVECALLBACK m_pReceiveFunc;

    std::thread* m_pThreadAccept;
    std::thread* m_pThreadReceive;

public:
    CTCPServer(void);
    ~CTCPServer(void);

    void SetServerInfo(const char* strIP, int nPort);
    void SetReceiveFunc(RECEIVECALLBACK pFunc);
    int CreateSocket();                     // create socket & set socket option
    int CloseSocket();

    int Init();                      // bind & listen
    int Start();                               // accept & receive
    int Stop();
    int Send(int nClientIndex, const char* pBuff, int nSize);

    bool IsStart();

private:
    void Accept();
    void Receive(int* pSock, int* pSockCount);
};


#endif   // __TCP_SERVER__
