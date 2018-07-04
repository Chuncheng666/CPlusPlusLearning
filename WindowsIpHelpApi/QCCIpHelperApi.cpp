// QCCIpHelperApi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include <IPHlpApi.h>

#pragma comment(lib, "IPHLPAPI.lib")

using namespace std;

void QCCShowAllAdaptersInfo()
{
	ULONG bufLen;
	IP_ADAPTER_INFO *pAdapterInfo;//����������Ϣ�Ľṹ��ָ��
	PIP_ADAPTER_INFO pAdapter;//��ѯ��������������ʹ�õĵ����ṹ�����

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	bufLen = sizeof(IP_ADAPTER_INFO);

	//��һ�λ�ȡbuf��С
	if (GetAdaptersInfo(pAdapterInfo, &bufLen) != ERROR_SUCCESS)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(bufLen);
	}

	//�ڶ��λ�ȡ������Ϣ
	if (GetAdaptersInfo(pAdapterInfo, &bufLen) != ERROR_SUCCESS)
	{
		cout << "Failed to get all adapters info! ";
		free(pAdapterInfo);
		return;
	}

	pAdapter = pAdapterInfo;
	while (pAdapter)
	{
		cout << "��������������: " << pAdapter->AdapterName << endl;
		cout << "��������������: " << pAdapter->Description << endl;
		cout << "Mac��ַ: ";
		for (UINT i = 0; i < pAdapter->AddressLength; i++)
		{
			if (i == (pAdapter->AddressLength - 1))
				printf("%.2X\n", (int)pAdapter->Address[i]);
			else
				printf("%.2X-", (int)pAdapter->Address[i]);
		}
		cout << "Ip��ַ: " << pAdapter->IpAddressList.IpAddress.String << endl;
		cout << "��������: " << pAdapter->IpAddressList.IpMask.String << endl;
		cout << "����: " << pAdapter->GatewayList.IpAddress.String << endl << endl;

		if (pAdapter->DhcpEnabled)
		{
			cout << "����DHCP! " << endl;
			cout << "DHCP������: " << pAdapter->DhcpServer.IpAddress.String << endl;
		}
		else
		{
			cout << "δ����DHCP! " << endl;
		}

		pAdapter = pAdapter->Next;
	}
}

void QCCShowNetworkParams()
{
	ULONG bufLength;
	DWORD errCode;
	FIXED_INFO* pFixedInfo;
	IP_ADDR_STRING* pIpAddr;//DNS��������Ip��ַ�б�

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
		cout << "������: " << pFixedInfo->HostName << endl;
		cout << "����: " << pFixedInfo->DomainName << endl;

		//�ڵ���Ϣ
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
		cout << "�ڵ�����: " << pFixedInfo << " - " << pNodeType << endl;

		cout << "�Ƿ�����·�ɹ��� �� -----  " << ((pFixedInfo->EnableRouting != 0) ? "��" : "��") << endl;
		cout << "�Ƿ�����ARP������ �� -----  " << ((pFixedInfo->EnableProxy != 0) ? "��" : "��") << endl;
		cout << "�Ƿ�����DNS���� �� -----  " << ((pFixedInfo->EnableDns != 0) ? "��" : "��") << endl << endl;

		cout << "DNS�������б�: " << endl;
		cout << pFixedInfo->DnsServerList.IpAddress.String << endl;
		pIpAddr = pFixedInfo->DnsServerList.Next;
		while (pIpAddr)
		{
			cout << pIpAddr->IpAddress.String << endl;
			pIpAddr = pIpAddr->Next;
		}
	}
}

int main()
{
	QCCShowAllAdaptersInfo();
	QCCShowNetworkParams();
	getchar();
    return 0;
}

