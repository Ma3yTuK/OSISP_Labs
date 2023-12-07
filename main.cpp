#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <set>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

const LPCWSTR PORT = L"27015";

std::set<SOCKET> clients;

const size_t BUFF_SIZE = 16384;

HANDLE ghMutex;

DWORD WINAPI ClientThread(LPVOID lpParameter)
{
	SOCKET* clientSocket = (SOCKET*)lpParameter;

	wchar_t* buff = new wchar_t[BUFF_SIZE];

	int recvStatus = 0;

	do {
		recvStatus = recv(*clientSocket, (char*)buff, BUFF_SIZE * sizeof(*buff), 0);

		WaitForSingleObject(ghMutex, INFINITE);

		auto it = clients.begin();
		while (it != clients.end())
		{
			if (send(*it, (char*)buff, recvStatus, 0) == SOCKET_ERROR)
			{
				closesocket(*it);
				it = clients.erase(it);
			}
			else
			{
				it++;
			}
		}

		ReleaseMutex(ghMutex);

	} while (recvStatus > 0);

	closesocket(*clientSocket);

	delete clientSocket;
	delete[] buff;

	return 0;
}

int main() {
	wchar_t* buff = new wchar_t[BUFF_SIZE];

	ghMutex = CreateMutex(NULL, FALSE, NULL);

	WSADATA wsaData;

	if (!WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		ADDRINFOW* result = NULL;
		ADDRINFOW pHints = ADDRINFOW();
		pHints.ai_socktype = SOCK_STREAM;

		if (!GetAddrInfoW(NULL, PORT, &pHints, &result))
		{
			SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

			if (listenSocket != INVALID_SOCKET)
			{
				if (bind(listenSocket, result->ai_addr, result->ai_addrlen) != SOCKET_ERROR)
				{
					while (listen(listenSocket, SOMAXCONN) != SOCKET_ERROR)
					{
						SOCKET* clientSocket = new SOCKET;
						*clientSocket = accept(listenSocket, NULL, NULL);

						if (*clientSocket != INVALID_SOCKET)
						{
							std::cout << "Connection accepted!" << std::endl;
							clients.insert(*clientSocket);

							CreateThread(NULL, NULL, &ClientThread, (void*)clientSocket, NULL, NULL);
						}
					}
				}

				closesocket(listenSocket);
			}

			FreeAddrInfoW(result);
		}

		WSACleanup();
	}

	CloseHandle(ghMutex);

	return 0;
}