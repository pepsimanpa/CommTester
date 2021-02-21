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

typedef enum E_TCP_SRV_EVENT {
    eTCP_SRV_EVENT_ERROR = 0,
    eTCP_SRV_EVENT_ACCEPT = 1
}E_TCP_SRV_EVENT_e;

typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);
typedef  void (*EVENTCALLBACK)(int nEventNum, char* pEventString);

class CTCPServer
{
private:
    int m_nServerSock;
    struct sockaddr_in m_tServerAddr;

    int m_nClientCount;
    int m_nClientSock[MAX_CLIENT_COUNT];
    struct sockaddr_in m_tClientAddr[MAX_CLIENT_COUNT];

    bool m_bStart;

    RECEIVECALLBACK m_pReceiveFunc;
    EVENTCALLBACK m_pEventFunc;

    std::thread* m_pThreadAccept;
    std::thread* m_pThreadReceive;

public:
    CTCPServer(void);
    ~CTCPServer(void);

    void SetServerAddr(const char* strIP, int nPort);

    void SetReceiveFunc(RECEIVECALLBACK pFunc);
    void SetEventFunc(EVENTCALLBACK pFunc);

    int CreateSocket();                     // create socket & set socket option
    int CloseSocket();

    int Listen();                              // bind & listen
    int Start();                               // start accept thread & receive thread
    int Stop();
    int Send(int nClientIndex, const char* pBuff, int nSize);

    bool IsStart();

private:
    void Accept();
    void Receive(int* pSock, int* pSockCount);
};


#endif   // __TCP_SERVER__
