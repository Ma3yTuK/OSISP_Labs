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

int main() {
	wchar_t* buff = new wchar_t[BUFF_SIZE];

	WSADATA wsaData;

	if (!WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		ADDRINFOW* result = NULL;

		if (!GetAddrInfoW(NULL, PORT, NULL, &result))
		{
			SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

			if (listenSocket != INVALID_SOCKET)
			{
				if (bind(listenSocket, result->ai_addr, result->ai_addrlen) != SOCKET_ERROR)
				{
					while (listen(listenSocket, SOMAXCONN) != SOCKET_ERROR)
					{
						SOCKET clientSocket = accept(listenSocket, NULL, NULL);

						if (clientSocket != INVALID_SOCKET)
						{
							std::cout << "Connection accepted!" << std::endl;

							int recvStatus = 0;
							int recvBytes = 0;

							do {
								recvStatus = recv(clientSocket, (char*)buff, BUFF_SIZE * sizeof(*buff), 0);
								recvBytes += recvStatus;
							} while (recvStatus > 0);

							if (recvStatus < 0)
							{
								closesocket(clientSocket);
								break;
							}

							clients.insert(clientSocket);

							auto it = clients.begin();
							while (it != clients.end())
							{
								if (*it != clientSocket && send(*it, (char*)buff, recvBytes, 0) == SOCKET_ERROR)
								{
									closesocket(*it);
									it = clients.erase(it);
								}
								else
								{
									it++;
								}
							}
						}
					}

					for (auto& client : clients)
					{
						closesocket(client);
					}
				}

				closesocket(listenSocket);
			}

			FreeAddrInfoW(result);
		}

		WSACleanup();
	}

	return 0;
}