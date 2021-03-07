#include "TCPClient.h"

CTCPClient::CTCPClient(void)
{
    m_bStart = false;
    m_nServerSock = INVALID_SOCKET;
    m_nClientSock = INVALID_SOCKET;
    m_pReceiveFunc = NULL;
    m_bConnected = false;
    m_pThreadReceive = NULL;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CTCPClient::~CTCPClient(void)
{
    m_bStart = false;

    closesocket(m_nServerSock);
    closesocket(m_nClientSock);

    if(m_pThreadReceive != NULL)
        m_pThreadReceive->join();
}

void CTCPClient::SetServerAddr(const char* strIP, int nPort)
{
    memset(&m_tServerAddr, 0x00, sizeof(struct sockaddr_in));
    m_tServerAddr.sin_family = AF_INET;
    m_tServerAddr.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tServerAddr.sin_addr.s_addr = inet_addr(strIP);
}

void CTCPClient::SetClientAddr(const char* strIP, int nPort)
{
    memset(&m_tClientAddr, 0x00, sizeof(struct sockaddr_in));
    m_tClientAddr.sin_family = AF_INET;
    m_tClientAddr.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tClientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tClientAddr.sin_addr.s_addr = inet_addr(strIP);
}

void CTCPClient::SetReceiveFunc(TCP_CLIENT_RECEIVECALLBACK pFunc)
{
    m_pReceiveFunc = pFunc;
}

//void CTCPClient::SetReceiveFunc(RECEIVECALLBACK pFunc)
//{
//    m_pReceiveFunc = pFunc;
//}

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

int CTCPClient::Bind()
{
    int nRet = TCP_CLIENT_ERROR;
    if (m_bStart != true && m_nClientSock != INVALID_SOCKET)
    {
        if (bind(m_nClientSock, (struct sockaddr*)&m_tClientAddr, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
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
    int nRet = TCP_CLIENT_ERROR;
    if (m_bStart != true && m_nClientSock != INVALID_SOCKET)
    {
        m_bStart = true;

        m_pThreadReceive = new std::thread(&CTCPClient::Connect, this);

        nRet = TCP_CLIENT_OK;
    }

    return nRet;
}

int CTCPClient::Stop()
{
    m_bStart = false;

    return TCP_CLIENT_OK;
}

int CTCPClient::Send(const char* pBuff, int nSize)
{
    int nRet = TCP_CLIENT_ERROR;
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
        if (connect(m_nClientSock, (struct sockaddr*)&m_tServerAddr, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            m_bConnected = true;

            m_pThreadReceive = new std::thread(&CTCPClient::Receive, this, m_nClientSock);

#ifdef _CONSOLE
            TCP_PRINT("[TCPClient] Connected!!\n");
#endif
            break;
        }

        TCP_SLEEP(100);
    }
}

void CTCPClient::Receive(int nSock)
{
    fd_set reads;
    char buff[MAX_BUFF_SIZE];
    memset(buff, 0x00, sizeof(buff));

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int nRet = 0;
    int nRecvSize = 0;

    while (m_bStart)
    {
        FD_ZERO(&reads);
        FD_SET(nSock, &reads);

        nRet = select(nSock + 1, &reads, NULL, NULL, (struct timeval*)&timeout);

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
            nRecvSize = recv(nSock, buff, MAX_BUFF_SIZE, 0);

            if (nRecvSize > 0 && m_pReceiveFunc != NULL)
                m_pReceiveFunc(buff, nRecvSize);
        }
    }
}
