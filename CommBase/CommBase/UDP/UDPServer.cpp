#include "UDPServer.h"

CUDPServer::CUDPServer(void)
{
    m_bStart = false;
    m_nServerSock = INVALID_SOCKET;
    m_pReceiveFunc = NULL;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CUDPServer::~CUDPServer(void)
{
    m_bStart = false;

    closesocket(m_nServerSock);

    m_pThreadReceive->join();
}

void CUDPServer::SetServerInfo(const char* strIP, int nPort)
{
    memset(&m_tServerAddr, 0x00, sizeof(struct sockaddr_in));
    m_tServerAddr.sin_family = AF_INET;
    m_tServerAddr.sin_port = htons(nPort);
    if (strIP == NULL || strIP == "")
        m_tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        m_tServerAddr.sin_addr.s_addr = inet_addr(strIP);
}

void CUDPServer::SetReceiveFunc(RECEIVECALLBACK pFunc)
{
    m_pReceiveFunc = pFunc;
}

int CUDPServer::CreateSocket()
{
    int nRet = UDP_SERVER_ERROR;
    if (m_bStart != true)
    {
        m_nServerSock = socket(AF_INET, SOCK_DGRAM, 0);

        if (m_nServerSock != INVALID_SOCKET)
        {
            int opt_val = 1;
            if (setsockopt(m_nServerSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_val, sizeof(opt_val)) != SOCKET_ERROR)
            {
                nRet = UDP_SERVER_OK;
            }
        }
    }

    return nRet;
}

int CUDPServer::CloseSocket()
{
    return closesocket(m_nServerSock);
}

int CUDPServer::Init()
{
    int nRet = UDP_SERVER_ERROR;
    if (m_bStart != true && m_nServerSock != INVALID_SOCKET)
    {
        if (bind(m_nServerSock, (struct sockaddr*)&m_tServerAddr, sizeof(struct sockaddr_in)) != SOCKET_ERROR)
        {
            nRet = UDP_SERVER_OK;
        }
        else
            closesocket(m_nServerSock);
    }

    return nRet;
}

int CUDPServer::Start()
{
    int nRet = UDP_SERVER_ERROR;
    if (m_bStart != true && m_nServerSock != INVALID_SOCKET)
    {
        m_bStart = true;

        m_pThreadReceive = new std::thread(&CUDPServer::Receive, this, m_nServerSock);

        nRet = UDP_SERVER_OK;
    }
    return nRet;
}

int CUDPServer::Stop()
{
    m_bStart = false;

    return 0;
}

int CUDPServer::Send(const char* pIP, int nPort, const char* pBuff, int nSize)
{
    int nRet = -1;
    struct sockaddr_in tClientAddr;
    memset(&tClientAddr, 0x00, sizeof(struct sockaddr_in));
    tClientAddr.sin_family = AF_INET;
    tClientAddr.sin_port = htons(nPort);
    tClientAddr.sin_addr.s_addr = inet_addr(pIP);

    if (m_nServerSock != INVALID_SOCKET)
        nRet = sendto(m_nServerSock, pBuff, nSize, 0, (struct sockaddr*)&tClientAddr, sizeof(struct sockaddr_in));

    return nRet;
}

bool CUDPServer::IsStart()
{
    return m_bStart;
}

void CUDPServer::Receive(int nSock)
{
    char buff[MAX_BUFF_SIZE];

    struct sockaddr_in tRecvAddr;
    int nAddrLen = sizeof(struct sockaddr_in);

    int nRecvSize = 0;

    while (m_bStart)
    {
        nRecvSize = recvfrom(nSock, buff, MAX_BUFF_SIZE, 0, (struct sockaddr*)&tRecvAddr, &nAddrLen);

        if (nRecvSize > 0 && m_pReceiveFunc != NULL)
            m_pReceiveFunc(buff, nRecvSize);
    }
}
