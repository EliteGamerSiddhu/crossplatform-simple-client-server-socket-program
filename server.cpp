#include <iostream>
#include <string.h>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif

#define PORT "8080"
#define BUFLEN 1048

using namespace std;

int main() {
	//Winsock initialization
	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WsaStartup Failed : " << iResult;
		return 1;
	}

	//AdderInfo struct creation
	addrinfo* result = NULL, * ptr = NULL, client_addr;

	ZeroMemory(&client_addr, sizeof(client_addr));

	client_addr.ai_family = AF_INET;
	client_addr.ai_socktype = SOCK_STREAM;
	client_addr.ai_protocol = IPPROTO_TCP;
	client_addr.ai_flags = AI_PASSIVE;


	//Resolving port
	iResult = getaddrinfo(NULL, PORT, &client_addr, &result);
	if (iResult != 0) {
		cerr << "Get addr info failed: " << iResult << endl;
		WSACleanup();
		return -1;
	}


	//Socket Creation
	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		cerr << "Socket creating failed" << endl;
		WSACleanup();
		return -1;
	}

	//Binding port and setting up TCP
	iResult = bind(ListenSocket, result->ai_addr, (int)(result->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
		cerr << "Socket binding failed: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	freeaddrinfo(result);

	//Listening for incoming connection
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Listening failed: " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	//Accepting a single connection
	SOCKET ClientSocket = INVALID_SOCKET;

	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET) {
		cerr << "Accept failed: " << WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	char recvbuf[BUFLEN];
	int iSendResult;
	int recvbuflen = BUFLEN;

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		cout << "Bytes recieved: " << iResult << endl;
		cout << (const string) recvbuf << endl;
	}
	else if (iResult == 0) {
		cout << "Connection closing" << endl;
	}
	else {
		cerr << "Send failed: " << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	char sendbuf[BUFLEN] = "Test buf echo back";

	iSendResult = send(ClientSocket, sendbuf, (int) strlen(sendbuf) + 1, 0);

	if (iSendResult == SOCKET_ERROR) {
		cerr << "Error in echoing back results" << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}

	//Closing the sockets

	closesocket(ClientSocket);
	closesocket(ListenSocket);
	WSACleanup();

	return 0;
}