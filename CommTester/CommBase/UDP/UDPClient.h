#ifndef __UDP_CLIENT__
#define __UDP_CLIENT__

/**
@class    CUDPClient
@author   pepsimanpa
@brief    UDP Client 클래스
@see
@remark
@date
   - 2021. 02. 04 : 최초 작성
*/

#include "winsock2.h"
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define UDP_CLIENT_OK              1
#define UDP_CLIENT_ERROR         -1

#define MAX_BUFF_SIZE             8192

#define UDP_SLEEP                                          Sleep
#define UDP_PRINT                                         printf

typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);


class CUDPClient
{
private:
    int m_nServerSock;
    struct sockaddr_in m_tServerAddr;
    bool m_bStart;
    bool m_bConnected;

    int m_nClientSock;
    struct sockaddr_in m_tClientAddr;

    RECEIVECALLBACK m_pReceiveFunc;

    std::thread* m_pThreadReceive;

public:
    CUDPClient(void);
    ~CUDPClient(void);

    void SetServerAddr(const char* strIP, int nPort);
    void SetClientAddr(const char* strIP, int nPort);

    void SetReceiveFunc(RECEIVECALLBACK pFunc);

    int CreateSocket();                     // create socket & set socket option
    int CloseSocket();

    int Init();                                  // bind (필수아님, clinet port지정이 필요할 때 사용)
    int Start();                               // connect (UDP 성능 향상, send, recv 사용 가능)
    int Stop();
    int Send(char* pBuff, int nSize);

    bool IsStart();

private:
    void Connect();
    void Receive(int nSock);
};


#endif   // __UDP_CLIENT__
