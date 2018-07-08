// NonBlockingTCPServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 256

using namespace std;

void ShowWSADataInfo(WSADATA wsaData)
{
	cout << "Version info is : " << LOBYTE(wsaData.wVersion) << "." << HIBYTE(wsaData.wVersion) << endl;
	cout << "High Version info is : " << LOBYTE(wsaData.wHighVersion) << "." << HIBYTE(wsaData.wHighVersion) << endl;
	cout << "Description info is : " << wsaData.szDescription << endl;
	cout << "System Status info is : " << wsaData.szSystemStatus << endl;
}

string QccGetCurrentTime()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char sTime[30];
	sprintf(sTime, "%4d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return sTime;
}

int main()
{
	int port;
	cout << "Please input port : " << endl;
	cin >> port;

	//WSADATA�ṹ��ʼ��
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		cout << "Init wsadata failed! " << endl;
		getchar();
		return -1;
	}
	//ShowWSADataInfo(wsaData);

	SOCKET qSrvSocket;
	SOCKET qCliSocket;
	int errCode;
	char buffer[BUF_SIZE];

	//����TCP Socket
	//SOCK_STREAM -> TCP; SOCK_DGRAM -> UDP
	//socket�������ڴ�����ָ���ķ����ṩ�߰��׽���
	qSrvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (qSrvSocket == INVALID_SOCKET)
	{
		cout << "Socket init error ! Error code is : " << WSAGetLastError() << endl;
		WSACleanup();
		getchar();
		return -1;
	}

	//����socketΪ������ģʽ
	int iMode = 1;
	errCode = ioctlsocket(qSrvSocket, FIONBIO, (u_long FAR*)&iMode);
	if (errCode == SOCKET_ERROR)
	{
		cout << "ioctl socket to non blocking failed! " << endl;
		closesocket(qSrvSocket);
		WSACleanup();
		return -1;
	}

	//bind���������ص�ַ��һ��socket����һ��
	struct sockaddr_in qAddr;
	int qAddrlength = sizeof(struct sockaddr_in);

	//����server address
	qAddr.sin_family = AF_INET;
	qAddr.sin_port = htons(port);
	qAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	errCode = bind(qSrvSocket, (const struct sockaddr*)&qAddr, qAddrlength);
	if (errCode == SOCKET_ERROR)
	{
		cout << "Socket bind error ! Error code is : " << WSAGetLastError() << endl;
		closesocket(qSrvSocket);
		WSACleanup();
		getchar();
		return -1;
	}

	//listen�������׽�������Ϊ�����������ӵ�״̬
	errCode = listen(qSrvSocket, 3);
	if (errCode == SOCKET_ERROR)
	{
		cout << "Socket listen error ! Error code is : " << WSAGetLastError() << endl;
		closesocket(qSrvSocket);
		WSACleanup();
		getchar();
		return -1;
	}

	//accept�����ȴ���������
	cout << "TCP Server Start..." << endl;
	getchar();
	sockaddr_in qCliAddr;
	int acceptLength = sizeof(qCliAddr);

	//������ģʽwhileѭ��accept
	while (true)
	{
		qCliSocket = accept(qSrvSocket, (sockaddr FAR*)&qCliAddr, &acceptLength);
		if (qCliSocket == INVALID_SOCKET)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				cout << "Waiting client for connect processing !" << endl;
				Sleep(1000);
				continue;
			}
			else
			{
				cout << "Socket accept error ! Error code is : " << WSAGetLastError() << endl;
				closesocket(qSrvSocket);
				WSACleanup();
				getchar();
				return -1;
			}
			break;
		}
		else
		{
			cout << "Socket connect address info is : " << inet_ntoa(qCliAddr.sin_addr) << endl;
			getchar();
		}
	}

	//whileѭ������
	while (true)
	{
		ZeroMemory(buffer, BUF_SIZE);
		errCode = recv(qCliSocket, buffer, BUF_SIZE, 0);
		if (errCode == SOCKET_ERROR)
		{
			int er = WSAGetLastError();
			if (er == WSAEWOULDBLOCK)
			{
				cout << "No data from buffer on non blocking mode begining!" << endl;
				Sleep(1000);
				continue;
			}
			else
			{
				cout << "Recv failed ! Error code is : " << WSAGetLastError() << endl;
				closesocket(qSrvSocket);
				closesocket(qCliSocket);
				WSACleanup();
				return -1;
			}
		}

		string curTime = QccGetCurrentTime();
		cout << curTime << ", Recv from client[ " << inet_ntoa(qCliAddr.sin_addr) << ":" << qCliAddr.sin_port << "] : " << buffer << endl;

		if (strcmp(buffer, "quit") == 0)
		{
			errCode = send(qCliSocket, "quit", strlen("quit"), 0);
			break;
		}
		else
		{
			char srvMessage[BUF_SIZE];
			sprintf(srvMessage, "Message recv -- %s", buffer);
			while (true)
			{
				errCode = send(qCliSocket, srvMessage, sizeof(srvMessage), 0);
				if (errCode == SOCKET_ERROR)
				{
					int errr = WSAGetLastError();
					if (errr == WSAEWOULDBLOCK)
					{
						cout << "can not finish non blocking mode opreation !" << endl;
						Sleep(1000);
						continue;
					}
					else
					{
						cout << "Send failed ! Error code is : " << WSAGetLastError() << endl;
						closesocket(qSrvSocket);
						closesocket(qCliSocket);
						WSACleanup();
						return -1;
					}
				}
				break;
			}
		}
	}

	closesocket(qSrvSocket);
	closesocket(qCliSocket);
	WSACleanup();

	getchar();

	return 0;
}


