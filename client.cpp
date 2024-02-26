#include <iostream>

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
#define LOCALHOST "127.0.0.1"
#define BUFLEN 1048

using namespace std;

int main() {

	//Initializing Winsocket library
	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WsaStartup Failed : " << iResult;
		return 1;
	}

	//Creating server address variable
	addrinfo *result = NULL, *ptr = NULL, serv_address;

	ZeroMemory(&serv_address, sizeof(serv_address));

	serv_address.ai_family = AF_INET;
	serv_address.ai_socktype = SOCK_STREAM;
	serv_address.ai_protocol = IPPROTO_TCP;
	

	// Resolve the server address and port
	iResult = getaddrinfo(LOCALHOST, PORT, &serv_address, &result);
	if (iResult != 0) {
		cerr << "Getting addr info failed" << endl;
		WSACleanup();
		return -1;
	}

	//Creating the socket
	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		cerr << "Socket creation failed" << endl;
		freeaddrinfo(ptr);
		WSACleanup();
		return -1;
	}

	//Connection to a socket
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cerr << "Unable to connect to server" << endl;
		WSACleanup();
		return -1;
	}


	//Sending Data
	int recvbuflen = BUFLEN;

	const char* sendbuf = "Test message";
	char recvbuf[BUFLEN];

	iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf) + 1, 0);
	if (iResult == SOCKET_ERROR) {
		cerr << "Message sending failed" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	cout << "Bytes of data sent : " << iResult << endl;

	//Closing send capability of socket
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cerr << "Shutdown failed" << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}


	//Listening for incoming data
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cout << "Bytes recieved: " << iResult << endl;
			cout << recvbuf << endl;
		}
		else if (iResult == 0) {
			cout << "Connection closed" << endl;
		}
		else {
			cerr << "recv failed " << WSAGetLastError();
		}
	} while (iResult > 0);

	//Closing the connection and the connection
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}