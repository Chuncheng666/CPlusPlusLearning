// QCCIpHelperApi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <IPHlpApi.h>

#pragma comment(lib, "IPHLPAPI.lib")

#define HPMALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define HPFREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

void QCCShowAllAdaptersInfo()
{
	ULONG bufLen;
	IP_ADAPTER_INFO *pAdapterInfo;//单个网卡信息的结构体指针
	PIP_ADAPTER_INFO pAdapter;//轮询所有网卡适配器使用的单个结构体变量

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	bufLen = sizeof(IP_ADAPTER_INFO);

	//第一次获取buf大小
	if (GetAdaptersInfo(pAdapterInfo, &bufLen) != ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(bufLen);
	}

	//第二次获取网卡信息
	if (GetAdaptersInfo(pAdapterInfo, &bufLen) != ERROR_SUCCESS)
	{
		cout << "Failed to get all adapters info! ";
		free(pAdapterInfo);
		return;
	}

	pAdapter = pAdapterInfo;
	while (pAdapter)
	{
		cout << "网络适配器名称: " << pAdapter->AdapterName << endl;
		cout << "网络适配器描述: " << pAdapter->Description << endl;
		cout << "Mac地址: ";
		for (UINT i = 0; i < pAdapter->AddressLength; i++)
		{
			if (i == (pAdapter->AddressLength - 1))
				printf("%.2X\n", (int)pAdapter->Address[i]);
			else
				printf("%.2X-", (int)pAdapter->Address[i]);
		}
		cout << "Ip地址: " << pAdapter->IpAddressList.IpAddress.String << endl;
		cout << "子网掩码: " << pAdapter->IpAddressList.IpMask.String << endl;
		cout << "网关: " << pAdapter->GatewayList.IpAddress.String << endl << endl;

		if (pAdapter->DhcpEnabled)
		{
			cout << "启用DHCP! " << endl;
			cout << "DHCP服务器: " << pAdapter->DhcpServer.IpAddress.String << endl;
		}
		else
		{
			cout << "未启用DHCP! " << endl;
		}

		pAdapter = pAdapter->Next;
	}
}

void QCCShowNetworkParams()
{
	ULONG bufLength;
	DWORD errCode;
	FIXED_INFO* pFixedInfo;
	IP_ADDR_STRING* pIpAddr;//DNS服务器的Ip地址列表

	pFixedInfo = (FIXED_INFO*)GlobalAlloc(GPTR, sizeof(FIXED_INFO));
	bufLength = sizeof(FIXED_INFO);
	if (GetNetworkParams(pFixedInfo, &bufLength) == ERROR_BUFFER_OVERFLOW)
	{
		GlobalFree(pFixedInfo);
		pFixedInfo = (FIXED_INFO*)GlobalAlloc(GPTR, bufLength);
	}
	
	if (errCode = GetNetworkParams(pFixedInfo, &bufLength) != ERROR_SUCCESS)
	{
		cout << "GetNetworkParams Function failed! error code is : " << errCode << endl;
		GlobalFree(pFixedInfo);
		return;
	}
	else
	{
		cout << "主机名: " << pFixedInfo->HostName << endl;
		cout << "域名: " << pFixedInfo->DomainName << endl;

		//节点信息
		char* pNodeType;
		switch (pFixedInfo->NodeType)
		{
		case BROADCAST_NODETYPE:
			pNodeType = "Broadcase Node";
			break;
		case PEER_TO_PEER_NODETYPE:
			pNodeType = "Peer to Peer Node";
			break;
		case MIXED_NODETYPE:
			pNodeType = "Mixed Node";
			break;
		case HYBRID_NODETYPE:
			pNodeType = "Hybird Node";
			break;
		default:
			pNodeType = "Unknown Node";
			break;
		}
		cout << "节点类型: " << pFixedInfo << " - " << pNodeType << endl;

		cout << "是否启用路由功能 ？ -----  " << ((pFixedInfo->EnableRouting != 0) ? "是" : "否") << endl;
		cout << "是否启用ARP代理功能 ？ -----  " << ((pFixedInfo->EnableProxy != 0) ? "是" : "否") << endl;
		cout << "是否启用DNS功能 ？ -----  " << ((pFixedInfo->EnableDns != 0) ? "是" : "否") << endl << endl;

		cout << "DNS服务器列表: " << endl;
		cout << pFixedInfo->DnsServerList.IpAddress.String << endl;
		pIpAddr = pFixedInfo->DnsServerList.Next;
		while (pIpAddr)
		{
			cout << pIpAddr->IpAddress.String << endl;
			pIpAddr = pIpAddr->Next;
		}
	}
}

void QCCShowInterfaceNumInfo()
{
	DWORD pInterNum;
	if (GetNumberOfInterfaces(&pInterNum) == NO_ERROR)
		cout << "网络接口数量: " << pInterNum << endl;
	else
		cout << "获取网络接口数量失败! " << endl;
}

void QCCShowInterfaceDetailInfo()
{
	PIP_INTERFACE_INFO pInterFaceInfo;
	ULONG bufferLength;

	if (GetInterfaceInfo(NULL, &bufferLength) == ERROR_INSUFFICIENT_BUFFER)
	{
		pInterFaceInfo = (IP_INTERFACE_INFO*)HPMALLOC(bufferLength);
		if (pInterFaceInfo == NULL)
		{
			cout << "无法分配内存空间给IP_INTERFACE_INFO ! " << endl;
			return;
		}
	}

	if (GetInterfaceInfo(pInterFaceInfo, &bufferLength) == NO_ERROR)
	{
		cout << "网络适配器数量: " << pInterFaceInfo->NumAdapters << endl;
		for (LONG i = 0; i < pInterFaceInfo->NumAdapters; i++)
		{
			cout << "网络适配器索引[" << i << "]: " << pInterFaceInfo->Adapter[i].Index << endl;
			wcout << "网络适配器名称[" << i << "]: " << pInterFaceInfo->Adapter[i].Name << endl;
		}
	}
	else if (GetInterfaceInfo(pInterFaceInfo, &bufferLength) == ERROR_NO_DATA)
	{
		cout << "本地计算机没有支持Ipv4的网络适配器 ! " << endl;
		return;
	}
	else
	{
		cout << "本地计算机网络接口获取失败 ! " << endl;
		return;
	}

	HPFREE(pInterFaceInfo);
}

int main()
{
	QCCShowAllAdaptersInfo();
	QCCShowNetworkParams();
	QCCShowInterfaceNumInfo();
	QCCShowInterfaceDetailInfo();
	getchar();
    return 0;
}

