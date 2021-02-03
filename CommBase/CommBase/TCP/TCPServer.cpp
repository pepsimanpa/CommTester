#include "TCPServer.h"

CTCPServer::CTCPServer(void)
{
    m_bStart = false;
    m_nServerSock = INVALID_SOCKET;
    m_nClientCount = 0;
    m_pReceiveFunc = NULL;
    memset(m_nClientSock, 0x00, sizeof(m_nClientSock));
    memset(m_tClientInfo, 0x00, sizeof(m_tClientInfo));

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CTCPServer::~CTCPServer(void)
{
    m_bStart = false;

    closesocket(m_nServerSock);
    for (int idx = 0; idx < m_nClientCount; ++idx)
        closesocket(m_nClientSock[idx]);

    m_pThreadAccept->join();
    m_pThreadReceive->join();
}

void CTCPServer::SetServerInfo(const char* strIP, int nPort)
{
    memset(&m_tServerInfo, 0x00, sizeof(struct sockaddr_in));
    m_tServerInfo.sin_family = AF_INET;
    m_tServerInfo.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tServerInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tServerInfo.sin_addr.s_addr = inet_addr(strIP);
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
        if (bind(m_nServerSock, (struct sockaddr*)&m_tServerInfo, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
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

int CTCPServer::Send(int nClientIndex, char* pBuff, int nSize)
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
            memcpy(&m_tClientInfo[m_nClientCount], &tClientInfo, sizeof(struct sockaddr));

            // 클라이언트 소켓 별 리시브 스레드 생성 하려면...
            //m_pThreadReceive[m_nClientCount] = new std::thread(&CTCPServer::Receive, this, m_nClientSock[m_nClientCount]);

            TCP_PRINT("[TCPServer] Accepted!!\n");

            m_nClientCount++;
        }
    }
}

void CTCPServer::Receive(int* pSock, int* pSockCount)
{
    fd_set reads;
    int nMaxFd = 0;
    char buff[MAX_BUFF_SIZE];

    struct timeval tTimeOut;
    tTimeOut.tv_sec = 5;
    tTimeOut.tv_usec = 0;

    while (m_bStart)
    {
        if (*pSockCount > 0)
        {
            FD_ZERO(&reads);
            for (int idx = 0; idx < *pSockCount; ++idx)
            {
                FD_SET(pSock[idx], &reads);
                if (nMaxFd > pSock[idx])
                    nMaxFd = pSock[idx];
            }

            int ret = select(nMaxFd + 1, &reads, NULL, NULL, (struct timeval*)&tTimeOut);

            if (ret == SOCKET_ERROR)
            {
                break;
            }
            else if (ret == 0)            // timeout
            {
                continue;
            }
            else
            {
                for (int idx = 0; idx < *pSockCount; ++idx)
                {
                    if (FD_ISSET(pSock[idx], &reads))
                    {
                        int nSize = recv(pSock[idx], buff, MAX_BUFF_SIZE, 0);

                        if (nSize > 0 && m_pReceiveFunc != NULL)
                            m_pReceiveFunc(buff, nSize);

                        break;
                    }
                }
            }
        }
        else
            TCP_SLEEP(1000);
    }
}
