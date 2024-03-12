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
	#include <cstring>
	#include <cerrno>

	#ifndef SD_SEND
		#define SD_SEND SHUT_WR
	#endif

	#ifndef SOCKET
		#define SOCKET unsigned long long int
	#endif

#endif

#define PORT "8080"
#define LOCALHOST "127.0.0.1"
#define BUFLEN 1048

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET (~0)
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR (-1)
#endif

using namespace std;

void cleanup(int sock){
	#ifdef _WIN32

	if(sock != INVALID_SOCKET){
		closesocket(sock);
	}
	WSACleanup();

	#else

	if(sock != INVALID_SOCKET){
		close(sock);
	}

	#endif
}

int main() {

	int iResult;

	//Initializing Winsocket library

	#ifdef _WIN32

	WSADATA wsaData;


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WsaStartup Failed : " << iResult;
		return 1;
	}

	#endif

	//Creating server address variable
	addrinfo *result = NULL, *ptr = NULL, serv_address;

	memset(&serv_address, 0, sizeof(serv_address));

	serv_address.ai_family = AF_INET;
	serv_address.ai_socktype = SOCK_STREAM;
	serv_address.ai_protocol = IPPROTO_TCP;
	

	// Resolve the server address and port
	iResult = getaddrinfo(LOCALHOST, PORT, &serv_address, &result);
	if (iResult != 0) {
		cerr << "Getting addr info failed" << endl;
		cleanup(-1);
		return -1;
	}

	//Creating the socket
	SOCKET ConnectSocket = INVALID_SOCKET;

	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		cerr << "Socket creation failed" << endl;
		freeaddrinfo(ptr);
		cleanup(ConnectSocket);
		return -1;
	}

	//Connection to a socket
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		cleanup(ConnectSocket);
		return -1;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cerr << "Unable to connect to server" << endl;
		cleanup(ConnectSocket);
		return -1;
	}


	//Sending Data
	int recvbuflen = BUFLEN;

	const char* sendbuf = "Test message";
	char recvbuf[BUFLEN];

	iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf) + 1, 0);
	if (iResult == SOCKET_ERROR) {
		cerr << "Message sending failed" << endl;
		cleanup(ConnectSocket);
		return -1;
	}

	cout << "Bytes of data sent : " << iResult << endl;

	//Closing send capability of socket
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cerr << "Shutdown failed" << endl;
		cleanup(ConnectSocket);
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
			#ifdef _WIN32
				cerr << "recv failed " << WSAGetLastError();
			#else
				cerr << "recv failed " << strerror(errno);
			#endif
		}
	} while (iResult > 0);

	//Closing the connection and the connection
	cleanup(ConnectSocket);

	return 0;
}