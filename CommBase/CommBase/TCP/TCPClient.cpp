#include "TCPClient.h"

CTCPClient::CTCPClient(void)
{
    m_bStart = false;
    m_nServerSock = INVALID_SOCKET;
    m_nClientSock = INVALID_SOCKET;
    m_pReceiveFunc = NULL;
    m_bConnected = false;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CTCPClient::~CTCPClient(void)
{
    m_bStart = false;

    closesocket(m_nServerSock);
    closesocket(m_nClientSock);

    m_pThreadReceive->join();
}

void CTCPClient::SetServerInfo(const char* strIP, int nPort)
{
    memset(&m_tServerInfo, 0x00, sizeof(struct sockaddr_in));
    m_tServerInfo.sin_family = AF_INET;
    m_tServerInfo.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tServerInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tServerInfo.sin_addr.s_addr = inet_addr(strIP);
}

void CTCPClient::SetClientInfo(const char* strIP, int nPort)
{
    memset(&m_tClientInfo, 0x00, sizeof(struct sockaddr_in));
    m_tClientInfo.sin_family = AF_INET;
    m_tClientInfo.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tClientInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tClientInfo.sin_addr.s_addr = inet_addr(strIP);
}

void CTCPClient::SetReceiveFunc(RECEIVECALLBACK pFunc)
{
    m_pReceiveFunc = pFunc;
}

int CTCPClient::CreateSocket()
{
    int nRet = TCP_CLIENT_ERROR;
    if (m_bStart != true)
    {
        m_nClientSock = socket(AF_INET, SOCK_STREAM, 0);

        if (m_nClientSock != INVALID_SOCKET)
        {
            int opt_val = 1;
            if (setsockopt(m_nClientSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = TCP_CLIENT_OK;
            }

            if (setsockopt(m_nClientSock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = TCP_CLIENT_OK;
            }
        }
    }

    return nRet;
}

int CTCPClient::CloseSocket()
{
    return closesocket(m_nClientSock);
}

int CTCPClient::Init()
{
    int nRet = TCP_CLIENT_ERROR;
    if (m_bStart != true && m_nClientSock != INVALID_SOCKET)
    {
        if (bind(m_nClientSock, (struct sockaddr*)&m_tClientInfo, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            nRet = TCP_CLIENT_OK;
        }
        else
            closesocket(m_nClientSock);
    }

    return nRet;
}

int CTCPClient::Start()
{
    m_bStart = true;

    m_pThreadReceive = new std::thread(&CTCPClient::Connect, this);

    return 0;
}

int CTCPClient::Stop()
{
    m_bStart = false;

    return 0;
}

int CTCPClient::Send(char* pBuff, int nSize)
{
    int nRet = -1;
    if (m_nClientSock != INVALID_SOCKET)
        nRet = send(m_nClientSock, pBuff, nSize, 0);

    return nRet;
}

bool CTCPClient::IsStart()
{
    return m_bStart;
}

void CTCPClient::Connect()
{
    while (m_bStart)
    {
        if (connect(m_nClientSock, (struct sockaddr*)&m_tServerInfo, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            m_bConnected = true;

            m_pThreadReceive = new std::thread(&CTCPClient::Receive, this, m_nClientSock);

            TCP_PRINT("[TCPClient] Connected!!\n");
            break;
        }

        TCP_SLEEP(100);
    }
}

void CTCPClient::Receive(int nSock)
{
    fd_set reads;
    char buff[MAX_BUFF_SIZE];

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    while (m_bStart)
    {
        FD_ZERO(&reads);
        FD_SET(nSock, &reads);

        int ret = select(nSock + 1, &reads, NULL, NULL, (struct timeval*)&timeout);

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
            int nSize = recv(nSock, buff, MAX_BUFF_SIZE, 0);

            if (nSize > 0 && m_pReceiveFunc != NULL)
                m_pReceiveFunc(buff, nSize);
        }
    }
}
