// QCCModifyNetworkConfig.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

using namespace std;

#define HPMALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define HPFREE(x) HeapFree(GetProcessHeap(), 0, (x))

DWORD QCCGetLocalIpInterfaceIndex()
{
	PMIB_IPADDRTABLE pIpAddrTableInfo;
	ULONG ipTableSize;
	DWORD errCode;
	DWORD pInterfaceIndex;

	pIpAddrTableInfo = (MIB_IPADDRTABLE*)HPMALLOC(sizeof(MIB_IPADDRTABLE));
	if (pIpAddrTableInfo)
	{
		if (errCode = GetIpAddrTable(pIpAddrTableInfo, &ipTableSize, 0) == ERROR_INSUFFICIENT_BUFFER)
		{
			HPFREE(pIpAddrTableInfo);
			pIpAddrTableInfo = (MIB_IPADDRTABLE*)HPMALLOC(ipTableSize);
		}

		if (pIpAddrTableInfo == NULL)
		{
			cout << "无法分配内存空间给MIB_IPADDRTABLE ! " << endl;
			return -1;
		}
	}

	if (errCode = GetIpAddrTable(pIpAddrTableInfo, &ipTableSize, 0) == NO_ERROR)
	{
		pInterfaceIndex = pIpAddrTableInfo->table[0].dwIndex;
		cout << "接口序号: " << pIpAddrTableInfo->table[0].dwIndex << endl;
	}

	if (pIpAddrTableInfo)
	{
		HPFREE(pIpAddrTableInfo);
		pIpAddrTableInfo = NULL;
	}
	return pInterfaceIndex;
}

void QCCAddLocalIpAdapterInfo(UINT pAddIpAddress, UINT pAddIpMask, DWORD pInterfaceIndex)
{
	DWORD errCode;
	ULONG pNTEContext = 0, pNTEInstance = 0;
	errCode = AddIPAddress(pAddIpAddress, pAddIpMask, pInterfaceIndex, &pNTEContext, &pNTEInstance);
	if (errCode == NO_ERROR)
		cout << "Successfully add ip address ! " << endl;
	else
		cout << "Failed to add ip address ! " << endl;
}

void QCCRemoveLocalIpAdapterInfo(ULONG pNTEContext)
{
	DWORD errCode;
	errCode = DeleteIPAddress(pNTEContext);
	if (errCode == NO_ERROR)
		cout << "Successfully remove ip address ! " << endl;
	else
		cout << "Failed to remove ip address ! " << endl;
}

int main(int argc, char* argv[])
{
	UINT pAddIpAddress, pAddIpMask;
	DWORD pInterfaceIndex;
	DWORD errCode;
	ULONG pNTEContext = 0, pNTEInstance = 0;

	//check para info
	if (argc != 3)
	{
		cout << "Usage: " << argv[0] << " IPAddress SubnetMask. " << endl;
		return -1;
	}
	pAddIpAddress = inet_addr(argv[1]);
	if (pAddIpAddress == INADDR_NONE)
	{
		cout << "Usage: " << argv[0] << " IPAddress SubnetMask. " << endl;
		return -1;
	}
	pAddIpMask = inet_addr(argv[2]);
	if (pAddIpMask == INADDR_NONE)
	{
		cout << "Usage: " << argv[0] << " IPAddress SubnetMask. " << endl;
		return -1;
	}

	//Get local network info
	pInterfaceIndex = QCCGetLocalIpInterfaceIndex();

	//Set NetAdapter Info
	errCode = AddIPAddress(pAddIpAddress, pAddIpMask, pInterfaceIndex, &pNTEContext, &pNTEInstance);
	if (errCode == NO_ERROR)
		cout << "Successfully add ip address ! " << endl;
	else
		cout << "Failed to add ip address ! " << endl;

	errCode = DeleteIPAddress(pNTEContext);
	if (errCode == NO_ERROR)
		cout << "Successfully remove ip address ! " << endl;
	else
		cout << "Failed to remove ip address ! " << endl;

    return 0;
}

