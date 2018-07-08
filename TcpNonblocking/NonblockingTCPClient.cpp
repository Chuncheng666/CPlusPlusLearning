// NonblockingTCPClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 256

using namespace std;

int main()
{
	int errCode;
	char buffer[BUF_SIZE];
	SOCKET qHostSocket;
	WSADATA wsaData;
	char ipAddress[20];
	int port;

	cout << "Please input ipAddress : " << endl;
	cin >> ipAddress;
	cout << "Please input port : " << endl;
	cin >> port;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Init wsadata failed! " << endl;
		getchar();
		return -1;
	}

	//创建TCP Socket
	//SOCK_STREAM -> TCP; SOCK_DGRAM -> UDP
	//socket函数用于创建与指定的服务提供者绑定套接字
	qHostSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (qHostSocket == INVALID_SOCKET)
	{
		cout << "Socket init error ! Error code is : " << WSAGetLastError() << endl;
		WSACleanup();
		getchar();
		return -1;
	}

	SOCKADDR_IN srvAddr;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.S_un.S_addr = inet_addr(ipAddress);
	srvAddr.sin_port = htons(port);
	int srvAddressLength = sizeof(srvAddr);

	errCode = connect(qHostSocket, (LPSOCKADDR)&srvAddr, srvAddressLength);
	if (errCode == SOCKET_ERROR)
	{
		cout << "Socket connect error ! Error code is : " << WSAGetLastError() << endl;
		closesocket(qHostSocket);
		WSACleanup();
		getchar();
		return -1;
	}

	while (true)
	{
		cout << "Please input the message you want to send : " << endl;
		string str;
		getline(cin, str);
		cin >> str;

		ZeroMemory(buffer, BUF_SIZE);
		strcpy(buffer, str.c_str());
		errCode = send(qHostSocket, buffer, strlen(buffer), 0);
		if (errCode == SOCKET_ERROR)
		{
			cout << "Socket send error ! Error code is : " << WSAGetLastError() << endl;
			closesocket(qHostSocket);
			WSACleanup();
			getchar();
			return -1;
		}

		errCode = recv(qHostSocket, buffer, sizeof(buffer), 0);
		cout << "Recv from server : " << buffer << endl;

		if (strcmp(buffer, "quit") == 0)
		{
			cout << "Client process quit!" << endl;
			break;
		}
	}

	closesocket(qHostSocket);
	WSACleanup();
	getchar();
	return 0;
}
