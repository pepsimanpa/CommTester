#include "UDPClient.h"

CUDPClient::CUDPClient(void)
{
    m_bStart = false;
    m_nServerSock = INVALID_SOCKET;
    m_nClientSock = INVALID_SOCKET;
    m_pReceiveFunc = NULL;
    m_bConnected = false;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CUDPClient::~CUDPClient(void)
{
    m_bStart = false;

    closesocket(m_nServerSock);
    closesocket(m_nClientSock);

    m_pThreadReceive->join();
}

void CUDPClient::SetServerInfo(const char* strIP, int nPort)
{
    memset(&m_tServerInfo, 0x00, sizeof(struct sockaddr_in));
    m_tServerInfo.sin_family = AF_INET;
    m_tServerInfo.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tServerInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tServerInfo.sin_addr.s_addr = inet_addr(strIP);
}

void CUDPClient::SetClientInfo(const char* strIP, int nPort)
{
    memset(&m_tClientInfo, 0x00, sizeof(struct sockaddr_in));
    m_tClientInfo.sin_family = AF_INET;
    m_tClientInfo.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tClientInfo.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tClientInfo.sin_addr.s_addr = inet_addr(strIP);
}

void CUDPClient::SetReceiveFunc(RECEIVECALLBACK pFunc)
{
    m_pReceiveFunc = pFunc;
}

int CUDPClient::CreateSocket()
{
    int nRet = UDP_CLIENT_ERROR;
    if (m_bStart != true)
    {
        m_nClientSock = socket(AF_INET, SOCK_DGRAM, 0);

        if (m_nClientSock != INVALID_SOCKET)
        {
            int opt_val = 1;
            if (setsockopt(m_nClientSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = UDP_CLIENT_OK;
            }
        }
    }

    return nRet;
}

int CUDPClient::CloseSocket()
{
    return closesocket(m_nClientSock);
}

int CUDPClient::Init()
{
    int nRet = UDP_CLIENT_ERROR;
    if (m_bStart != true && m_nClientSock != INVALID_SOCKET)
    {
        if (bind(m_nClientSock, (struct sockaddr*)&m_tClientInfo, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            nRet = UDP_CLIENT_OK;
        }
        else
            closesocket(m_nClientSock);
    }

    return nRet;
}

int CUDPClient::Start()
{
    m_bStart = true;

    m_pThreadReceive = new std::thread(&CUDPClient::Connect, this);

    return 0;
}

int CUDPClient::Stop()
{
    m_bStart = false;

    return 0;
}

int CUDPClient::Send(char* pBuff, int nSize)
{
    int nRet = -1;
    if (m_nClientSock != INVALID_SOCKET)
        nRet = send(m_nClientSock, pBuff, nSize, 0);

    return nRet;
}

bool CUDPClient::IsStart()
{
    return m_bStart;
}

void CUDPClient::Connect()
{
    if (m_bStart)
    {
        if (connect(m_nClientSock, (struct sockaddr*)&m_tServerInfo, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            m_bConnected = true;

            m_pThreadReceive = new std::thread(&CUDPClient::Receive, this, m_nClientSock);

//#ifdef _CONSOLE
//            UDP_PRINT("[UDPClient] Connected!!\n");
//#endif
        }
    }
}

void CUDPClient::Receive(int nSock)
{
    fd_set reads;
    char buff[MAX_BUFF_SIZE];

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
