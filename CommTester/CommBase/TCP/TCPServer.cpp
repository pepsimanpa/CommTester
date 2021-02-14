#include "TCPServer.h"

CTCPServer::CTCPServer(void)
{
    m_bStart = false;
    m_nServerSock = INVALID_SOCKET;
    m_nClientCount = 0;
    m_pReceiveFunc = NULL;
    m_pThreadAccept = NULL;
    m_pThreadReceive = NULL;

    memset(m_nClientSock, 0x00, sizeof(m_nClientSock));
    memset(m_tClientAddr, 0x00, sizeof(m_tClientAddr));

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CTCPServer::~CTCPServer(void)
{
    m_bStart = false;

    closesocket(m_nServerSock);
    for (int idx = 0; idx < m_nClientCount; ++idx)
        closesocket(m_nClientSock[idx]);

    if (m_pThreadAccept != NULL)
        m_pThreadAccept->join();
    if (m_pThreadReceive != NULL)
        m_pThreadReceive->join();
}

void CTCPServer::SetServerInfo(const char* strIP, int nPort)
{
    memset(&m_tServerAddr, 0x00, sizeof(struct sockaddr_in));
    m_tServerAddr.sin_family = AF_INET;
    m_tServerAddr.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tServerAddr.sin_addr.s_addr = inet_addr(strIP);
}

void CTCPServer::SetReceiveFunc(RECEIVECALLBACK pFunc)
{
    m_pReceiveFunc = pFunc;
}

int CTCPServer::CreateSocket()
{
    int nRet = TCP_SERVER_ERROR;
    if (m_bStart != true)
    {
        m_nServerSock = socket(AF_INET, SOCK_STREAM, 0);

        if (m_nServerSock != INVALID_SOCKET)
        {
            int opt_val = 1;
            if (setsockopt(m_nServerSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = TCP_SERVER_OK;
            }

            if (setsockopt(m_nServerSock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = TCP_SERVER_OK;
            }
        }
    }

    return nRet;
}

int CTCPServer::CloseSocket()
{
    return closesocket(m_nServerSock);
}

int CTCPServer::Init()
{
    int nRet = TCP_SERVER_ERROR;
    if (m_bStart != true && m_nServerSock != INVALID_SOCKET)
    {
        if (bind(m_nServerSock, (struct sockaddr*)&m_tServerAddr, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            if (listen(m_nServerSock, SOMAXCONN) != SOCKET_ERROR)
            {
                nRet = TCP_SERVER_OK;
            }
            else
                closesocket(m_nServerSock);
        }
        else
            closesocket(m_nServerSock);
    }

    return nRet;
}

int CTCPServer::Start()
{
    int nRet = TCP_SERVER_ERROR;
    if (m_bStart != true && m_nServerSock != INVALID_SOCKET)
    {
        m_bStart = true;

        m_pThreadAccept = new std::thread(&CTCPServer::Accept, this);
        m_pThreadReceive = new std::thread(&CTCPServer::Receive, this, m_nClientSock, &m_nClientCount);

        nRet = TCP_SERVER_OK;
    }
    return nRet;
}

int CTCPServer::Stop()
{
    m_bStart = false;

    return 0;
}

int CTCPServer::Send(int nClientIndex, const char* pBuff, int nSize)
{
    int nRet = -1;
    if (m_nClientSock[nClientIndex] != INVALID_SOCKET)
        nRet = send(m_nClientSock[nClientIndex], pBuff, nSize, 0);

    return nRet;
}

bool CTCPServer::IsStart()
{
    return m_bStart;
}

void CTCPServer::Accept()
{
    while (m_bStart)
    {
        struct sockaddr tClientInfo;
        memset(&tClientInfo, 0x00, sizeof(tClientInfo));
        int nAddrLen = sizeof(struct sockaddr_in);

        m_nClientSock[m_nClientCount] = accept(m_nServerSock, (struct sockaddr*)&tClientInfo, &nAddrLen);

        if (m_nClientSock[m_nClientCount] != INVALID_SOCKET)
        {
            memcpy(&m_tClientAddr[m_nClientCount], &tClientInfo, sizeof(struct sockaddr));

            // 클라이언트 소켓 별 리시브 스레드 생성 하려면...
            //m_pThreadReceive[m_nClientCount] = new std::thread(&CTCPServer::Receive, this, m_nClientSock[m_nClientCount]);

#ifdef _CONSOLE
            TCP_PRINT("[TCPServer] Accepted!!\n");
#endif

            m_nClientCount++;
        }
    }
}

void CTCPServer::Receive(int* pSock, int* pSockCount)
{
    fd_set reads;
    int maxfd = 0;
    char buff[MAX_BUFF_SIZE];
    memset(buff, 0x00, sizeof(buff));

    struct timeval tTimeOut;
    tTimeOut.tv_sec = 5;
    tTimeOut.tv_usec = 0;

    int nRet = 0;
    int nRecvSize = 0;

    while (m_bStart)
    {
        if (*pSockCount > 0)
        {
            FD_ZERO(&reads);
            for (int idx = 0; idx < *pSockCount; ++idx)
            {
                FD_SET(pSock[idx], &reads);
                if (maxfd > pSock[idx])
                    maxfd = pSock[idx];
            }

            nRet = select(maxfd + 1, &reads, NULL, NULL, (struct timeval*)&tTimeOut);

            if (nRet == SOCKET_ERROR)
            {
                break;
            }
            else if (nRet == 0)            // timeout
            {
                continue;
            }
            else
            {
                for (int idx = 0; idx < *pSockCount; ++idx)
                {
                    if (FD_ISSET(pSock[idx], &reads))
                    {
                        nRecvSize = recv(pSock[idx], buff, MAX_BUFF_SIZE, 0);

                        if (nRecvSize > 0 && m_pReceiveFunc != NULL)
                            m_pReceiveFunc(buff, nRecvSize);

                        break;
                    }
                }
            }
        }
        else
            TCP_SLEEP(1000);
    }
}
