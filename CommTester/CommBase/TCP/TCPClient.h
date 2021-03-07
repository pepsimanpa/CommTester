#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

/**
@class    CTCPClient
@author   pepsimanpa
@brief    TCP Client 클래스
@see
@remark
@date
   - 2021. 02. 03 : 최초 작성
*/

#include "winsock2.h"
#include <thread>
#include <functional>

#pragma comment(lib, "ws2_32.lib")

#define TCP_CLIENT_OK              1
#define TCP_CLIENT_ERROR         -1

#define MAX_BUFF_SIZE             8192

#define TCP_SLEEP                                          Sleep
#define TCP_PRINT                                         printf

//typedef  void (*RECEIVECALLBACK)(char* pBuff, int nSize);
#define TCP_CLIENT_RECEIVECALLBACK    std::function<void(char* pBuff, int nSize)>


class CTCPClient
{
private:
    int m_nServerSock;
    struct sockaddr_in m_tServerAddr;

    int m_nClientSock;
    struct sockaddr_in m_tClientAddr;

    bool m_bStart;
    bool m_bConnected;

    //RECEIVECALLBACK m_pReceiveFunc;
    TCP_CLIENT_RECEIVECALLBACK m_pReceiveFunc;

    std::thread* m_pThreadReceive;

public:
    CTCPClient(void);
    ~CTCPClient(void);

    void SetServerAddr(const char* strIP, int nPort);
    void SetClientAddr(const char* strIP, int nPort);

    void SetReceiveFunc(TCP_CLIENT_RECEIVECALLBACK pFunc);

    int CreateSocket();                     // create socket & set socket option
    int CloseSocket();

    int Bind();                                 // bind (필수아님, clinet port 지정을 위해 사용)
    int Start();                               // start receive thread after tcp server connect
    int Stop();
    int Send(const char* pBuff, int nSize);

    bool IsStart();

private:
    void Connect();
    void Receive(int nSock);
};


#endif   // __TCP_CLIENT__
