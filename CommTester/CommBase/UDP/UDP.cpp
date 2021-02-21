#include "UDP.h"

CUDP::CUDP(void)
{
    m_bStart = false;
    m_nTargetSock = INVALID_SOCKET;
    m_nMySock = INVALID_SOCKET;
    m_pReceiveFunc = NULL;
    m_bConnected = false;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CUDP::~CUDP(void)
{
    m_bStart = false;

    closesocket(m_nTargetSock);
    closesocket(m_nMySock);

    m_pThreadReceive->join();
}

void CUDP::SetTargetAddr(const char* strIP, int nPort)
{
    memset(&m_tTargetAddr, 0x00, sizeof(struct sockaddr_in));
    m_tTargetAddr.sin_family = AF_INET;
    m_tTargetAddr.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tTargetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tTargetAddr.sin_addr.s_addr = inet_addr(strIP);
}

void CUDP::SetMyAddr(const char* strIP, int nPort)
{
    memset(&m_tMyAddr, 0x00, sizeof(struct sockaddr_in));
    m_tMyAddr.sin_family = AF_INET;
    m_tMyAddr.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tMyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tMyAddr.sin_addr.s_addr = inet_addr(strIP);
}

void CUDP::SetReceiveFunc(RECEIVECALLBACK pFunc)
{
    m_pReceiveFunc = pFunc;
}

int CUDP::CreateSocket()
{
    int nRet = UDP_ERROR;
    if (m_bStart != true)
    {
        m_nMySock = socket(AF_INET, SOCK_DGRAM, 0);

        if (m_nMySock != INVALID_SOCKET)
        {
            int opt_val = 1;
            if (setsockopt(m_nMySock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = UDP_OK;
            }
        }
    }

    return nRet;
}

int CUDP::CloseSocket()
{
    return closesocket(m_nMySock);
}

int CUDP::Bind()
{
    int nRet = UDP_ERROR;
    if (m_bStart != true && m_nMySock != INVALID_SOCKET)
    {
        if (bind(m_nMySock, (struct sockaddr*)&m_tMyAddr, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            nRet = UDP_OK;
        }
        else
            closesocket(m_nMySock);
    }

    return nRet;
}

int CUDP::Start()
{
    int nRet = UDP_ERROR;
    if (m_bStart != true && m_nMySock != INVALID_SOCKET)
    {
        m_bStart = true;

        m_pThreadReceive = new std::thread(&CUDP::Receive, this, m_nMySock);
        
        nRet = UDP_OK;
    }

    return nRet;
}

int CUDP::Stop()
{
    m_bStart = false;

    return UDP_OK;
}

int CUDP::Send(char* pBuff, int nSize)
{
    int nRet = UDP_ERROR;
    if (m_bConnected)
    {
        if (m_nMySock != INVALID_SOCKET)
            nRet = send(m_nMySock, pBuff, nSize, 0);
    }
    return nRet;
}

int CUDP::Send(const char* pIP, int nPort, const char* pBuff, int nSize)
{
    int nRet = UDP_ERROR;
    struct sockaddr_in tTargetAddr;
    memset(&tTargetAddr, 0x00, sizeof(struct sockaddr_in));
    tTargetAddr.sin_family = AF_INET;
    tTargetAddr.sin_port = htons(nPort);
    tTargetAddr.sin_addr.s_addr = inet_addr(pIP);

    if (m_nMySock != INVALID_SOCKET)
        nRet = sendto(m_nMySock, pBuff, nSize, 0, (struct sockaddr*)&tTargetAddr, sizeof(struct sockaddr_in));

    return nRet;
}

bool CUDP::IsStart()
{
    return m_bStart;
}

bool CUDP::IsConnect()
{
    return m_bConnected;
}

int CUDP::Connect()
{
    int nRet = UDP_ERROR;

    if (connect(m_nMySock, (struct sockaddr*)&m_tTargetAddr, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
    {
        m_bConnected = true;

#ifdef _CONSOLE
        UDP_PRINT("[UDP] Connected!!\n");
#endif
        nRet = UDP_OK;
    }

    return nRet;
}

void CUDP::Receive(int nSock)
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
