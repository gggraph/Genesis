#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")          // Winsock library file
#include <WS2tcpip.h>                       // Header file for Winsock functions
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include <vector>

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1


void ConnectToPeerList(const char* filePath);
void ConnectToPeer(std::string ServerIP, int port);
int GetFreePeerPointer();
void DisconnectPeer(int peerIndex);

void SendToAllPeers(unsigned char* data, int datalength);
void PeerSend(int peerIndex, unsigned char* data, int datalength);



/*
int Connect(std::string ServerIP, int port);
void ClientSend(unsigned char* data, int datalength);
*/
class TCPClient;

// Callback to data received
typedef void(*ClientRecievedHandler)(unsigned char* data, int datasize);


class TCPClient {
private:
	std::string     m_ipAddress;            // IP Address of the server
	int             m_port;                 // Listening port # on the server
	SOCKET          m_socket;
	sockaddr_in     m_hint;
	bool            m_recv_thread_running;  //thread end control
	std::thread     m_recv_thread;

	// Message received event handler, just a function pointer to handle it externally from class
	ClientRecievedHandler  MessageReceived;

	// Initialize winsock
	bool Init()
	{
		WSAData data;
		WORD ver = MAKEWORD(2, 2);

		int wsInit = WSAStartup(ver, &data);
		if (wsInit != 0)
		{
			std::cerr << "Can't start Winsock, Err #" << wsInit << std::endl;
			return false;
		}

		return wsInit == 0;
	}


	// Create a socket for send/recv
	SOCKET CreateSocket()
	{
		SOCKET sock_client = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_client != INVALID_SOCKET)
		{
			std::vector<int> vect;

			std::stringstream ss(m_ipAddress);

			for (int i; ss >> i;) {
				vect.push_back(i);
				if (ss.peek() == '.')
					ss.ignore();
			}
			if (vect.size() < 4) {
				return SOCKET_ERROR;
			}

			uint8_t dip[4] = { vect[0],vect[1],vect[2],vect[3] }; // SHOULD BE AUTOMATICALLY SET OMG

			//struct sockaddr_in serv_addr;
			memset(&m_hint, 0, sizeof(m_hint));
			m_hint.sin_family = AF_INET;
			m_hint.sin_port = m_port;
			//memcpy(&m_hint, dip, 4);
			memcpy(&(m_hint.sin_addr), dip, 4);
			inet_ntoa(m_hint.sin_addr);

			//inet_ntoa(m_hint.sin_addr);
			//inet_pton(AF_INET, m_ipAddress.c_str(), &m_hint.sin_addr);



		}
		else {
			std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
			WSACleanup();
		}

		return sock_client;
	}

	void ThreadRecv()
	{
		//std::cout << "Recv thread started, m_socket: " << m_socket << std::endl;
		m_recv_thread_running = true;
		while (m_recv_thread_running)
		{
			//std::cout << "Recv thread running... m_socket: " << this->m_socket << std::endl;

			char buf[4096];
			ZeroMemory(buf, 4096);

			int bytesReceived = recv(m_socket, buf, 4096, 0);
			if (bytesReceived > 0)
			{
				if (MessageReceived != NULL)
				{
					MessageReceived((unsigned char*)buf, bytesReceived);
				}
			}
			else {
				//std::cout << "Received nothing" << std::endl;
			}
		}
		//std::cout << "Recv thread ended" << std::endl;
	}

public:

	TCPClient() {
		m_socket = INVALID_SOCKET;
		m_recv_thread_running = false;
	}

	~TCPClient() {
		closesocket(m_socket);
		WSACleanup();
		if (m_recv_thread_running)
		{
			m_recv_thread_running = false;  //stopping loop in ThreadRecv()
			//std::cout << "Finishing RECV thread..." << std::endl;
			m_recv_thread.join();           //wait it to finish properly (naturally)
			//std::cout << "Done." << std::endl;
		}
	}


	bool Connect(std::string ipAddress, int port)
	{
		m_ipAddress = ipAddress;
		m_port = port;

		// Initialize winsock
		if (!Init()) return false;

		//Creating the socket for client to send and recv
		m_socket = CreateSocket();
		if (m_socket == INVALID_SOCKET) {
			std::cerr << "Can't create socks, Err #" << WSAGetLastError() << std::endl;
			return false;
		}

		// Connect to server
		int connResult = connect(m_socket, (sockaddr*)&m_hint, sizeof(m_hint));
		if (connResult == SOCKET_ERROR)
		{
			std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
			return false;
		}
		return true;
	}

	bool Send(unsigned char* data, int datalength)
	{

		if (data != NULL && m_socket != INVALID_SOCKET)
		{
			return send(m_socket, (const char*)data, datalength, 0) != SOCKET_ERROR;
			//return send(m_socket, message.c_str(), message.size() + 1, 0) != SOCKET_ERROR;
		}
		/*
		if (!message.empty() && m_socket != INVALID_SOCKET)
		{
			return send(m_socket, message.c_str(), message.size() + 1, 0) != SOCKET_ERROR;
		}
		*/
		return false;
	}

	void ListenRecvInThread(ClientRecievedHandler handler)
	{
		MessageReceived = handler;
		//creating the recv thread //This method also works
		//std::thread recv_t(&TCPClient::ThreadRecv, this);
		//moving thread variable to class member, so we can join it later
		//this->m_recv_thread = std::move(recv_t);

		//creating the recv thread using lambda's
		this->m_recv_thread = std::thread([&]()
			{
				ThreadRecv();
			});

	}

	bool Recv(ClientRecievedHandler handler)
	{
		MessageReceived = handler;

		if (m_socket == INVALID_SOCKET) return false;

		char buf[4096];

		// Wait for response
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(m_socket, buf, 4096, 0);
		if (bytesReceived > 0)
		{
			if (MessageReceived != NULL)
			{
				//MessageReceived(std::string(buf, 0, bytesReceived));
			}

			// Echo response to console
			//std::cout << "SERVER> " << std::string(buf, 0, bytesReceived) << std::endl;
		}
		return true;
	}


};
