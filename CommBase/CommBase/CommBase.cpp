// CommBase.cpp : 이 파일에는 'main'함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include "./TCP/TCPServer.h"
#include "./TCP/TCPClient.h"

int g_nID = 0;


void recvCallback(char* pBuff, int nSize)
{
    printf("[RECV] size: %d, buff: %s\n", nSize, pBuff);
}

int main(int argc, char* argv[])
{
    int cmd;

    if (argc > 2)
    {
        char* str;

        printf("**************************************************\n");
        for (int idx = 1; idx < argc; ++idx)
        {
            str = argv[idx];

            if (strcmp("-ID", str) == 0)
            {
                g_nID = atoi(argv[idx + 1]);
                ++idx;

                printf("ID : %d \n", g_nID);
            }
        }
        printf("**************************************************\n");
    }

    CTCPServer* pTcpServer = new CTCPServer;
    CTCPClient* pTcpClient = new CTCPClient;

    while (1)
    {
        printf("select command\n1) start tcp server, 2) send data(server -> clinet) \n3) start tcp client, 4) send data(clinet -> server) \n0) exit \n");
        scanf_s("%d", &cmd);
        if (cmd == 1)
        {
            pTcpServer->SetServerInfo(NULL, 7000);
            pTcpServer->SetReceiveFunc(recvCallback);
            pTcpServer->CreateSocket();
            pTcpServer->Init();
            pTcpServer->Start();
        }
        else if (cmd == 2)
        {
            char buff[128];
            strcpy_s(buff, "tcp server send data");
            pTcpServer->Send(0, buff, sizeof(buff));
        }
        else if (cmd == 3)
        {
            pTcpClient->SetServerInfo("192.168.0.7", 7000);
            pTcpClient->SetReceiveFunc(recvCallback);
            pTcpClient->CreateSocket();
            //pTcpClient->SetClientInfo("192.168.59.36", 8000);
            //pTcpClient->Init();
            pTcpClient->Start();
        }
        else if (cmd == 4)
        {
            char buff[128];
            strcpy_s(buff, "tcp clinet send data");
            pTcpClient->Send(buff, sizeof(buff));
        }
        else if (cmd == 5)
        {

        }
        else if (cmd == 6)
        {

        }
        else if (cmd == 7)
        {

        }
        else if (cmd == 8)
        {

        }
        else if (cmd == 9)
        {

        }
        else if (cmd == 0)
        {
            pTcpServer->Stop();
            delete pTcpServer;

            pTcpClient->Stop();
            delete pTcpClient;
        }
    }

    return 0;
}

// 프로그램 실행: <Ctrl+F5>또는 [디버그] > [디버깅하지 않고 시작]메뉴
// 프로그램 디버그: <F5>키 또는 [디버그] > [디버깅 시작]메뉴

// 시작을 위한 팁:
//   1. [솔루션 탐색기]창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기]창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록]창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln파일을 선택합니다.
