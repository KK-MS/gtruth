
#include "stdafx.h"
// ----------------

// TODO fix
// Severity	Code	Description	Project	File	Line	Suppression State
// Error	C4996	'inet_addr': Use inet_pton() or InetPton() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	stereo	C : \prj\ocv\vs\gtruth\stereo\stereo\stereo_output.cpp	220
#define _WINSOCK_DEPRECATED_NO_WARNINGS
 
#include <vector>
#include <iostream>
#include <iomanip>

// File operation
#include <fstream> 

// Threads: to run streaming server
#include <process.h>

// Network related includes
//#undef UNICODE

// Need older API support e.g. IPV4. Else result in "Error E0040"
// E0040 expected an identifier ws2def.h
#define WIN32_LEAN_AND_MEAN 

#include <windows.h> // DEFAULT_PORT
#include <ws2tcpip.h> // getaddrinfo, includes #include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>


#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 100
#define DEFAULT_PORT "27015"

using namespace std;

// GLOBAL VARIABLES

static ofstream myFile;

static SOCKET ListenSocket = INVALID_SOCKET;
static SOCKET ClientSocket = INVALID_SOCKET;
static int gTerminateServer = FALSE;
static HANDLE hServerThread;
//// OUTPUT TO FILE /////

int file_open () {
	
	// File: out, binary, not appended, thus will reset size to zero
	myFile.open("C:\\prj\\ocv\\vs\\gtruth\\stereo\\x64\\Release\\stereo_out.mjpeg",
		ios::out | ios::binary); // | ios::app to append
	if (!myFile.is_open()) {
		std::cout << "Output operation not successful\n";
		return -1;
	}
	return 0;
}

int file_write(std::vector<unsigned char> &buf) {
	char* data = reinterpret_cast<char*>(buf.data());	
	
	// Program assume the file is already opened.
	myFile.write(data, buf.size());
	return 0;
}

//// NETWORK /////
/*********************************************************************
* Server with UDP connection
*/
#define INPUT_SERVER_PORT     27015 
#define INPUT_SERVER_IP     "127.0.0.1"
static SOCKET s;
static SOCKADDR_IN servaddr;

void network_udp_server(void *param)
{
	int iResult;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//struct sockaddr cliaddr;
	int len = sizeof(sockaddr_in); // need to specify lenght of address expected, else we get error 10014

	sockaddr cliaddr;


	cout << "In Stereo UDP Server" << endl;
	// Receive until the peer shuts down the connection
	do {
		//printf("Stereo UDP recvfrom...\n");
		//iResult = recvfrom(ListenSocket, recvbuf, recvbuflen, 0, &cliaddr, &len);
		//iResult = recvfrom(s, recvbuf, recvbuflen, 0, NULL, 0);
		iResult = recvfrom(s, recvbuf, recvbuflen, 0, &cliaddr, &len);
		//	printf("Stereo bytes received: %d\n", iResult);

		if (iResult > 0) {
			recvbuf[iResult] = '\0';
			// Echo the buffer back to the sender
			 
			//iSendResult = sendto(ListenSocket, recvbuf, recvbuflen, 0,
			//	(const struct sockaddr *) &cliaddr, len);
			iSendResult = sendto(s, recvbuf, recvbuflen, 0,
          &cliaddr, len);
				//(const struct sockaddr *) NULL, 0);

			if (iSendResult < 0) {
				printf("stereo send failed with error: %d\n", WSAGetLastError());
				//goto end_server;

			}
		//	printf("stereo Bytes sent: %d\n", iSendResult);
			
		}
		else if (iResult == 0) {
			printf("NULL packet...\n");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			goto end_server;

		}

		// TODO: Don't use global variable to pass termination signal. Use signals.
	} while ((iResult > 0) || (gTerminateServer == FALSE));

end_server:
	printf("UDP Server closing...\n");
	closesocket(s);
	WSACleanup();
}

/*********************************************************************
* Server with TCP connection
*/
void network_tcp_server(void *param)
{
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	printf("Network streaming server started listening...\n");

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) { 
		printf("listen error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	
	// Accept a client socket
	printf("Network streaming server started accepting...\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		int iErrno = WSAGetLastError();
		printf("accept error: %d\n", iErrno); // WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// No longer need server socket
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				goto end_server;
				
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0) {
			printf("NULL packet...\n");
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			goto end_server;
			
		}

		// TODO: Don't use global variable to pass termination signal. Use signals.
	} while ((iResult > 0) || (gTerminateServer == FALSE));
	
end_server:
	printf("Server closing...\n");
	closesocket(ClientSocket);
	WSACleanup();
}



int network_init(int isUDPConnection)
{
	WSADATA wsaData;
	int iResult;

	// Function pointer to hold the server thread function
	void(*server_thread)(void *param);

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0); //Create socket
	if (s == INVALID_SOCKET) {
		printf("Error localize_network_input_init socket %d\n", WSAGetLastError());
		return false; //Couldn't create the socket
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(INPUT_SERVER_PORT); //Port to connect on
	//servaddr.sin_addr.s_addr = INADDR_ANY; // inet_addr(INPUT_SERVER_IP); //Target IP
	servaddr.sin_addr.s_addr = inet_addr(INPUT_SERVER_IP);
	servaddr.sin_port = IPPROTO_UDP;

	// Setup the TCP listening socket
	iResult = bind(s, (struct sockaddr*) &servaddr, (int)sizeof(servaddr));
	if (iResult < 0) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Starting thread" << endl;
	gTerminateServer == FALSE;
	hServerThread = (HANDLE)_beginthread(network_udp_server, 0, NULL);

	return 0;
}

int network_init1(int isUDPConnection)
{
	WSADATA wsaData;
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Function pointer to hold the server thread function
	void (* server_thread )(void *param);
	
	// Need to memset hints to 0,
	// Without this getaddrinfo fails with 11003
	// info: 11003 "indicates nonrecoverable error occurred during a database lookup"
	memset(&hints, 0, sizeof(hints)); 
	
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;

	if (isUDPConnection) {
		hints.ai_socktype = SOCK_DGRAM;
		server_thread = network_udp_server; 
	}
	else {
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		server_thread = network_tcp_server;
	}
	
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);
	
	cout << "Starting thread" << endl;
	gTerminateServer == FALSE;
	hServerThread = (HANDLE)_beginthread(server_thread, 0, NULL);
	
	return 0;
}

/*********************************************************************
 * Network deinit
 */
int network_deinit() {
	// deinit the network connection
	WSACleanup();
	// WAIT for Server loop to end
	WaitForSingleObject(hServerThread, INFINITE);
	cout << "stereo network_deinit ends" << endl;

	return 0;
}


int network_udp_init() {

}

int network_udp_deinit() {

}


int network_outstream() {

}

int network_instream() {

}


int stream_to_network(std::vector<unsigned char> &buf) {
	char* data = reinterpret_cast<char*>(buf.data());

	// Program assume the file is already opened.
	myFile.write(data, buf.size());
	return 0;
}

//// STEREO OUTPUT API's /////
int stereo_output_init()
{
	//file_open();
	printf("In stereo_output_init\n");
	network_init(TRUE);


	DWORD result;
	do {
		result = WaitForSingleObject(hServerThread, 0);

		if (result == WAIT_OBJECT_0) {
			// the thread handle is signaled - the thread has terminated
			printf("\n Thread is signaled \n");
		}
		else {
			printf("\n Thread is not terminated \n");
			// the thread handle is not signaled - the thread is still alive
		}
	} while (result == WAIT_OBJECT_0);

	return 0;
}

int stereo_output_deinit()
{
	//myFile.close();
	network_deinit();
	return 0;
}

int stereo_output_camera(std::vector<unsigned char> &buf)
{\
	 //file_write(buf);

	 //stream_to_network(buf);

	 return 0;
}

// Error code links:
// https://docs.microsoft.com/en-us/windows/desktop/debug/system-error-codes--0-499-
//
// https://docs.microsoft.com/de-de/windows/desktop/WinSock/windows-sockets-error-codes-2
// WSAEINTR 10004: Interrupted function call. 
//    A blocking operation was interrupted by a call to WSACancelBlockingCall.
// WSAEFAULT 10014: Bad address. The system detected an invalid pointer address 
//    in attempting to use a pointer argument of a call. 
//    This error occurs if an application passes an invalid pointer value, 
//    or if the length of the buffer is too small. For instance, if the length 
//    of an argument, which is a sockaddr structure, is smaller than the sizeof(sockaddr).
// WSAENOTSOCK 10038: Socket operation on nonsocket.
// WSAEMSGSIZE 10040: Message too long.
//   A message sent on a datagram socket was larger than the internal message 
//   buffer or some other network limit, or the buffer used to receive a 
//   datagram was smaller than the datagram itself.
// WSAEPROTONOSUPPORT 10043: Protocol not supported.
//   The requested protocol has not been configured into the system, or no 
//   implementation for it exists.
//   e.g. A socket call requests a SOCK_DGRAM socket, but specifies a stream protocol.
// WSAEOPNOTSUPP 10045: Operation not supported.
//   The attempted operation is not supported for the type of object referenced.
//   Usually this occurs when a socket descriptor to a socket that cannot 
//   support this operation is trying to accept a connection on a datagram socket.
// WSAEAFNOSUPPORT 10047: Address family not supported by protocol family.
//    An address incompatible with the requested protocol was used. 
//    All sockets are created with an associated address family 
//    (i.e. AF_INET for Internet Protocols) and a generic protocol type
//    (i.e. SOCK_STREAM). This error is returned if an incorrect protocol is 
//    explicitly requested in the socket call, or if an address of the wrong 
//    family is used for a socket, for example, in sendto.
// WSAEADDRNOTAVAIL 10049: Cannot assign requested address.
//    The requested address is not valid in its context. This normally results
//    from an attempt to bind to an address that is not valid for the local 
//    computer. This can also result from connect, sendto, WSAConnect, 
//    WSAJoinLeaf, or WSASendTo when the remote address or port is not valid 
//    for a remote computer(for example, address or port 0).
// WSAECONNRESET 10054: Connection reset by peer.
//    An existing connection was forcibly closed by the remote host. This 
//    normally results if the peer application on the remote host is suddenly
//    stopped, the host is rebooted, the host or remote network interface is 
//    disabled, or the remote host uses a hard close (see setsockopt for more 
//    information on the SO_LINGER option on the remote socket).This error may
//    also result if a connection was broken due to keep - alive activity detecting a failure while one or more operations are in progress.Operations that were in progress fail with WSAENETRESET.Subsequent operations fail with WSAECONNRESET.
// WSAENOTCONN 10057: Socket is not connected. A request to send or receive 
//    data was disallowed because the socket is not connected and (when 
//    sending on a datagram socket using sendto) no address was supplied.
//    Any other type of operation might also return this error—for example, 
//    setsockopt setting SO_KEEPALIVE if the connection has been reset.
