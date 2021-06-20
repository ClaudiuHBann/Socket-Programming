/*
	IF YOU HAVE ANY OBSERVATION OR FIND ANY MISTAKE LET ME KNOW.


	This code is a server that any client can connect to.
	This server mimics a chat group, which means... When the server receives a block of data from a client it sends it to all clients on the server except the one it came from.

	We will name a client a socket connected to our server (same with the server).
	"We" might mean the server.

	Key words: - API = application programming interface
			   - TCP = transmission control protocol
			   - UDP = user datagram protocol
*/

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <Winsock2.h> // library with which we can use windows socket API functions, already includes Windows.h
#include <iostream> // library with which we can use in and out streams
#include <thread> // library with which we can create new threads for the server's clients
#include <map> // library with which we can hold the server's clients

#pragma comment (lib, "Ws2_32.lib") // link window's API library

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015

std::map<SOCKET, sockaddr_in> clientsList; // variable for holding the server's clients
static bool closeClientThreads = false; // if something happens, turn this into true to close every client thread

/*
	This is the thread that every connected client will have
*/
void NewClient(const SOCKET clientSocket)
{
	/*
		Variable for holding functions returns
	*/
	int iResult = -1;

	/*
		A buffer where data will be stored when receiving data from the client and it's length
	*/
	char receiveBuffer[DEFAULT_BUFLEN];
	int receiveBufferLength = DEFAULT_BUFLEN;

	while (!closeClientThreads && iResult != 0)
	{
		/*
			Receive incoming data from the socket and store it in the buffer

			If recv returns -1 the client disconnected improperly
			If recv returns 0 the client disconnected
			If recv returns x > 0 the client sent x bytes
		*/
		iResult = recv(clientSocket, receiveBuffer, receiveBufferLength, 0);

		if (iResult > 0)
		{
			std::cout << "Bytes received: " << iResult << std::endl;

			/*
				Iterate through the server's clients and send the received data to everyone except yourself
			*/
			for (auto& i : clientsList)
			{
				if (clientSocket != i.first)
				{
					/*
						If send returns 0 the client is not connected to the server
						If send returns x we sent x bytes to the client
					*/
					iResult = send(i.first, receiveBuffer, iResult, 0); // iResult here is the length of the received data
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

			/*
				try ("try" because this will happen if the client disconnected improperly) to shut down and close the client
			*/
			shutdown(clientSocket, SD_BOTH);
			closesocket(clientSocket);
			break;
		}
	}

	// remove client from server's clients
	clientsList.erase(clientSocket);
}

int main()
{
	/*
		Variable for holding data about windows sockets API
	*/
	WSADATA wsaData;

	/*
		Variable for holding functions returns
	*/
	int iResult;

	/*
		Server and client socket variables
	*/
	SOCKET serverSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	/*
		Variable for holding data about the server
	*/
	struct sockaddr_in serverHint;
	memset(&serverHint, 0, sizeof(serverHint)); // setting the memory of the variable to 0
	serverHint.sin_addr.s_addr = INADDR_ANY; // any IP can connect to the server
	serverHint.sin_family = AF_INET; // address family for TCP, UDP etc...
	serverHint.sin_port = htons(DEFAULT_PORT); // the port on which the server will listen for clients

	/*
		Variable for holding data about a client
	*/
	struct sockaddr_in clientHint;
	memset(&clientHint, 0, sizeof(clientHint)); // setting the memory of the variable to 0
	int clientHintLength = sizeof(clientHint);

	/*
		Initialize Windows sockets API
	*/
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cerr << std::endl << " Failed on initializing the windows sockets API! Error: " << iResult << std::endl;
		std::cerr << std::endl << " Exiting..." << std::endl;
		Sleep(3000);
		exit(EXIT_FAILURE);
	}

	/*
		Create the TCP server's socket
	*/
	serverSocket = socket(serverHint.sin_family, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << std::endl << " Failed on creating the server's socket! Error: " << WSAGetLastError() << std::endl;
		std::cerr << std::endl << " Exiting..." << std::endl;
		Sleep(3000);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	/*
		Bind the server's socket to the port that will be listening for clients
	*/
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

	/*
		Let the server listen for incoming clients
	*/
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
		/*
			Accept a client that tries to connect to the server's IP and port
		*/
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
			// create a new thread for a new client
			std::thread newClientThread(NewClient, clientSocket);
			newClientThread.detach();

			// add the client to the server's clients
			clientsList.insert(std::pair<SOCKET, sockaddr_in>(clientSocket, clientHint));
		}
	}

	/*
		shutdown and close the server
	*/
	shutdown(serverSocket, SD_BOTH);
	closesocket(serverSocket);
	WSACleanup(); // close windows sockets API
	return 0;
}
