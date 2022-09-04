#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")          // Winsock library file
#include <WS2tcpip.h>                       // Header file for Winsock functions
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include <vector>
#include "Utils.h"

#define MAX_DL 8
#define MAX_CLIENT 8
#define MAX_DL_SIZE 4000000
#define MAX_DL_ALLOCATION 10000000 // 10MB
#define MAX_DL_SUSPENDTIME 60

void StartServer(const char* filePath);
void StartServer(std::string IP, int port);
void StopServer();
void StopDL(int index);
void PrintDLinformation(int index);
void RefreshDLs();
int GetDlPointerByID(unsigned char* id);
int GetFreeDLPointer();

class TCPServer;


// Callback to data received
typedef void(*ServerRecievedHandler)(TCPServer* listener, SOCKET socketId, unsigned char* data, int datalength);

class TCPServer {
private:
	std::string     m_ipAddress;            // IP Address of the server 
	int             m_port;                 // Listening port # on the server
	sockaddr_in     m_hint;
	bool            m_run_thread_running;   //end thread control or run control
	std::thread     m_run_thread;
	SOCKET          m_listening_sock;

	fd_set          master;                 // Create the master file descriptor

	// Message received event handler, just a function pointer to handle it externally from class
	ServerRecievedHandler  MessageReceived;

	// Create a socket for send/recv
	SOCKET CreateSocket()
	{
		SOCKET sock_server = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_server != INVALID_SOCKET)
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
			m_hint.sin_port = m_port;          // SHOULD BE AUTOMATICALLY SET OMG 
			//memcpy(&m_hint, dip, 4);
			memcpy(&(m_hint.sin_addr), dip, 4);
			inet_ntoa(m_hint.sin_addr);
			// Bind the ip address and port to a socket
			int bindOk = bind(sock_server, (sockaddr*)&m_hint, sizeof(m_hint));
			if (bindOk != SOCKET_ERROR)
			{
				// Tell Winsock the socket is for listening
				int listenOk = listen(sock_server, SOMAXCONN);
				if (listenOk == SOCKET_ERROR)
				{
					std::cerr << "Can't listen to socsket, Err #" << WSAGetLastError() << std::endl;
					return SOCKET_ERROR;
				}
			}
			else {
				std::cerr << "Can't bind to socket, Err #" << WSAGetLastError() << std::endl;
				return SOCKET_ERROR;
			}

		}
		else {
			std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
			WSACleanup();
		}

		return sock_server;
	}



public:

	TCPServer(std::string ipAddress, int port, ServerRecievedHandler handler)
		: m_ipAddress(ipAddress), m_port(port), MessageReceived(handler)
	{
		FD_ZERO(&master);
		m_run_thread_running = false;
	}

	~TCPServer() {

		//closing all sockets =========================================
		// Message to let users know what's happening.
		std::string msg = "Server is shutting down. Goodbye\r\n";
		while (master.fd_count > 0)
		{
			// Get the socket number
			SOCKET sock = master.fd_array[0];

			// Send the goodbye message
			send(sock, msg.c_str(), (int)(msg.size() + 1), 0);

			// Remove it from the master file list and close the socket
			FD_CLR(sock, &master);
			closesocket(sock);
		}
		//closing all sockets =========================================

		WSACleanup();

		//finishing running thread
		if (m_run_thread_running)
		{
			m_run_thread_running = false;  //stopping loop in ThreadRecv()
			//std::cout << "Finishing RUN thread..." << std::endl;
			m_run_thread.join();            //wait it to finish properly (naturally)
			//std::cout << "Done." << std::endl;
		}
	}

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

	//send message to 1 client
	bool Send(SOCKET clientSocket, unsigned char * data, int datasize)
	{
		if (data != NULL)
		{
			return send(clientSocket,(const char *) data, datasize, 0) != SOCKET_ERROR;
		}
		return false;
	}

	//send broadcast message, except for 1 client (usually who send the message).
	//if sending message from server, set clientSocket = NULL in the function call.
	void SendToAllExcept(SOCKET clientSocket, std::string message)
	{
		if (!message.empty())
		{
			// Send message to other clients, and definiately NOT the listening socket
			for (unsigned int i = 0; i < master.fd_count; i++)
			{
				SOCKET outSock = master.fd_array[i];
				if ((outSock != m_listening_sock) && (outSock != clientSocket))
				{
					send(outSock, message.c_str(), (int)(message.size() + 1), 0);
				}
			}


		}
	}

	void RunInThread() //unblocked
	{
		//creating the run thread using lambda's
		this->m_run_thread = std::thread([&]()
			{
				Run();
			});
	}

	void Run()
	{
		// Create a listening socket
		m_listening_sock = CreateSocket();
		if (m_listening_sock == INVALID_SOCKET)
		{
			std::cerr << "Error in Run(), could not start server." << std::endl;
			return;
		}

		// Add our first socket that we're interested in interacting with; the listening socket!
		// It's important that this socket is added for our server or else we won't 'hear' incoming
		// connections
		FD_SET(m_listening_sock, &master);

		// this will be changed by the \quit command (see below, bonus not in video!)
		m_run_thread_running = true;
		std::cout << "Server is running" << std::endl;
		while (m_run_thread_running)
		{

			fd_set copy = master;

			// See who's talking to us
			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

			// Loop through all the current connections / potential connect
			for (int i = 0; i < socketCount; i++)
			{
				// Makes things easy for us doing this assignment
				SOCKET sock = copy.fd_array[i];

				// Is it an inbound communication?
				if (sock == m_listening_sock)
				{
					// Accept a new connection
					SOCKET client = accept(m_listening_sock, nullptr, nullptr);

					// Add the new connection to the list of connected clients
					FD_SET(client, &master);

				}
				else // It's an inbound message
				{
					char buf[4096];
					ZeroMemory(buf, 4096);

					// Receive message
					int bytesIn = recv(sock, buf, 4096, 0);
					if (bytesIn <= 0)
					{
						// Drop the client
						closesocket(sock);
						FD_CLR(sock, &master);
					}
					else
					{

						//handle msg externally from class
						if (MessageReceived != NULL)
						{
							MessageReceived(this, sock, (unsigned char*)buf, bytesIn);//MessageReceived(this, sock, std::string(buf, 0, bytesIn));
						}
					}
				}
			}
		}





		// Remove the listening socket from the master file descriptor set and close it
		// to prevent anyone else trying to connect.
		FD_CLR(m_listening_sock, &master);
		closesocket(m_listening_sock);
		std::cout << "Server stopped" << std::endl;

		// Message to let users know what's happening.
		std::string msg = "Server is shutting down. Goodbye\r\n";

		while (master.fd_count > 0)
		{
			// Get the socket number
			SOCKET sock = master.fd_array[0];
			// close it
			FD_CLR(sock, &master);
			closesocket(sock);
		}

	}

};
