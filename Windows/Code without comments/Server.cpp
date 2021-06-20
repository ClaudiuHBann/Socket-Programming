#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Winsock2.h>
#include <iostream>
#include <thread>
#include <map>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015

std::map<SOCKET, sockaddr_in> clientsList;
static bool closeClientThreads = false;

void NewClient(const SOCKET clientSocket)
{

	int iResult = -1;

	char receiveBuffer[DEFAULT_BUFLEN];
	int receiveBufferLength = DEFAULT_BUFLEN;

	while (!closeClientThreads && iResult != 0)
	{
		iResult = recv(clientSocket, receiveBuffer, receiveBufferLength, 0);

		if (iResult > 0)
		{
			std::cout << "Bytes received: " << iResult << std::endl;

			for (auto& i : clientsList)
			{
				if (clientSocket != i.first)
				{
					iResult = send(i.first, receiveBuffer, iResult, 0);
					if (iResult == SOCKET_ERROR)
					{
						std::cerr << std::endl << " Failed on sending data to the client! Error: " << WSAGetLastError() << std::endl;
					}

					std::cout << "Bytes sent: " << iResult << std::endl;
				}
			}
		}
		else
		{
			std::cerr << std::endl << " Failed on receiving data from the client! Error: " << WSAGetLastError() << std::endl;

			shutdown(clientSocket, SD_BOTH);
			closesocket(clientSocket);
			break;
		}
	}

	clientsList.erase(clientSocket);
}

int main()
{
	WSADATA wsaData;

	int iResult;

	SOCKET serverSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	struct sockaddr_in serverHint;
	memset(&serverHint, 0, sizeof(serverHint));
	serverHint.sin_addr.s_addr = INADDR_ANY;
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(DEFAULT_PORT);

	struct sockaddr_in clientHint;
	memset(&clientHint, 0, sizeof(clientHint));
	int clientHintLength = sizeof(clientHint);

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cerr << std::endl << " Failed on initializing the windows sockets API! Error: " << iResult << std::endl;
		std::cerr << std::endl << " Exiting..." << std::endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}

	serverSocket = socket(serverHint.sin_family, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << std::endl << " Failed on creating the server's socket! Error: " << WSAGetLastError() << std::endl;
		std::cerr << std::endl << " Exiting..." << std::endl;
		Sleep(3000);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	iResult = bind(serverSocket, (sockaddr*)&serverHint, sizeof(serverHint));
	if (iResult == SOCKET_ERROR)
	{
		std::cerr << std::endl << " Failed on binding the server's socket! Error: " << WSAGetLastError() << std::endl;
		std::cerr << std::endl << " Exiting..." << std::endl;
		Sleep(3000);
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	iResult = listen(serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		std::cerr << std::endl << " Failed on creating the server's socket! Error: " << WSAGetLastError() << std::endl;
		std::cerr << std::endl << " Exiting..." << std::endl;
		Sleep(3000);
		closesocket(serverSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		clientSocket = accept(serverSocket, (sockaddr*)&clientHint, &clientHintLength);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cerr << std::endl << " Failed on accepting a client to the server! Error: " << WSAGetLastError() << std::endl;
			std::cerr << std::endl << " Exiting..." << std::endl;
			Sleep(3000);
			closesocket(serverSocket);
			WSACleanup();
			exit(EXIT_FAILURE);
		}
		else
		{
			std::thread newClientThread(NewClient, clientSocket);
			newClientThread.detach();

			clientsList.insert(std::pair<SOCKET, sockaddr_in>(clientSocket, clientHint));
		}
	}

	shutdown(serverSocket, SD_BOTH);
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}
