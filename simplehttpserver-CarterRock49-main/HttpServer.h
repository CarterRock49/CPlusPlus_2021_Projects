#ifndef _TESTING
// A simple, blocking, HTTP server to be used for educational purposes only.
#pragma once
#include "Winsock2.h"
#include "Ws2tcpip.h"
#include <iostream>
#include <vector>
#include <string>
#pragma comment (lib, "Ws2_32.lib")

// Additional information
// https://tangentsoft.net/wskfaq/
/////////////////////
namespace SimpleServer
{
	class HttpServer
	{

		/////////////////////////////////////////////////////////
		// Sets up a listener on the given interface and port, returning the listening socket if successful; if not, returns INVALID_SOCKET.
		SOCKET SetUpListener(const std::string sAddress, const int nPort)
		{
			u_long nInterfaceAddr = 0;
			InetPtonA(AF_INET, sAddress.c_str(), &nInterfaceAddr);
			if (nInterfaceAddr == INADDR_NONE)
				throw std::exception("HttpServer::SetUpListener - Invalid address!");

			SOCKET sd = socket(AF_INET, SOCK_STREAM, 0);
			if (sd == INVALID_SOCKET)
				throw std::exception("HttpServer::SetUpListener - Invalid socket!");

			sockaddr_in sinInterface {};
			sinInterface.sin_family = AF_INET;
			sinInterface.sin_addr.s_addr = nInterfaceAddr;
			sinInterface.sin_port = nPort;
			if (bind(sd, (sockaddr*) &sinInterface, sizeof(sockaddr_in)) == SOCKET_ERROR)
				throw std::exception("HttpServer::SetUpListener - Socket error!");

			listen(sd, 1);
			return sd;
		}
		//////////////////////////////////////////////////////
		// Waits for a connection on the given socket.  When one comes in, we return a socket for it.  
		// If an error occurs, we return INVALID_SOCKET.
		SOCKET AcceptConnection(SOCKET ListeningSocket, sockaddr_in& sinRemote)
		{
			int nAddrSize = sizeof(sinRemote);
			SOCKET s = accept(ListeningSocket, (sockaddr*) &sinRemote, &nAddrSize);

			int iVal = 500;
			int ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*) &iVal, sizeof(iVal));
			if (ret == SOCKET_ERROR)
				throw std::exception("HttpServer::AcceptConnection - Socket option error!");

			return s;
		}
		////////////////////////////////////////////////////
		// Gracefully shuts the connection down. Returns true if we're successful, false otherwise.
		bool ShutdownConnection(SOCKET sd)
		{
			// Disallow any further data sends.  This will tell the other side
			// that we want to go away now.  If we skip this step, we don't
			// shut the connection down nicely.
			if (shutdown(sd, SD_SEND) == SOCKET_ERROR)
				throw std::exception("HttpServer::ShutdownConnection - Socket shutdown error!");

			// Receive any extra data still sitting on the socket.  After all
			// data is received, this call will block until the remote host
			// acknowledges the TCP control packet sent by the shutdown above.
			// Then we'll get a 0 back from recv, signalling that the remote
			// host has closed its side of the connection.
			char acReadBuffer[1024];
			while (true)
			{
				int nNewBytes = recv(sd, acReadBuffer, 1024, 0);
				if (nNewBytes == SOCKET_ERROR)
					return false;
				else if (nNewBytes != 0)
					std::cerr << std::endl << "FYI, received " << nNewBytes << " unexpected bytes during shutdown." << std::endl;

				// Okay, we're done!
				return true;
			}

			return false;
		}
		///////////////////////////////////////////////////
		// Reads bytes from the socket. Ends when the socket times out or buffer is not filled.
		bool ReceiveData(SOCKET sd, std::vector<char>& sRequest)
		{
			// Read data from client
			int nReadBytes = 0;
			while (true)
			{
				const int buffSize = 128;
				char acReadBuffer[buffSize + 1];
				nReadBytes = recv(sd, acReadBuffer, buffSize, 0);

				if (nReadBytes == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAETIMEDOUT)
						break;
					else
						return false;
				}

				sRequest.insert(sRequest.end(), acReadBuffer, acReadBuffer + nReadBytes);

				if (nReadBytes < buffSize)
					break;
			}

			return true;
		}
		///////////////////////////////////////////////////
		// Sends all data to the socket. Returns true on success.
		bool SendData(SOCKET sd, const std::vector<char>& sResponse)
		{
			int nSentBytes = 0;
			while (nSentBytes < sResponse.size())
			{
				int nTemp = send(sd, &sResponse[0], static_cast<int>(sResponse.size()), 0);
				if (nTemp > 0)
					nSentBytes += nTemp;
				else if (nTemp == SOCKET_ERROR)
					return false;
			}

			return true;
		}
		///////////////////////////////////////////////////
		// Sends and receives all data.
		// on errors, or true if the client closed the socket normally.
		bool ProcessConncetion(SOCKET sd)
		{
			std::vector<char> sRequest;
			if (ReceiveData(sd, sRequest))
			{
				const std::vector<char> sResponse = ProcessRequest(sRequest);
				return SendData(sd, sResponse);
			}

			return false;
		}

	protected:

		// Inherit this class and override to receive the streams HTTP data.
		virtual const std::vector<char> ProcessRequest(const std::vector<char> sData) = 0;

	public:

		///////////////////////////////////////////////////////////////////////////////
		// Initiaize the Window's sockets library
		HttpServer()
		{
			// RAII - https://docs.microsoft.com/en-us/cpp/cpp/object-lifetime-and-resource-management-modern-cpp?view=msvc-170

			// Initialize the Window's socket library.
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				throw std::exception("HttpServer::HTTPServer - WSAStartup error!");
			}
		}
		///////////////////////////////////////////////////////////////////////////////
		// Shutdown the Window's sockets library
		virtual ~HttpServer()
		{
			// RAII - https://docs.microsoft.com/en-us/cpp/cpp/object-lifetime-and-resource-management-modern-cpp?view=msvc-170
			WSACleanup();
		}
		///////////////////////////////////////////////////////////////////////////////
		// Start the process of being a server. Listen at an IP address and a port (80 (HTTP) is the default).
		bool Start(const std::string sAddress, const int nPort = 80)
		{
			// Begin listening for connections
			SOCKET ListeningSocket = SetUpListener(sAddress, htons(nPort));

			// Spin forever handling clients
			while (true)
			{
				// Wait for a connection, and accepting it when one arrives.
				sockaddr_in sinRemote;
				SOCKET sd = AcceptConnection(ListeningSocket, sinRemote);

				// Sends/Receives packets to/from the client.
				if (ProcessConncetion(sd))
				{
					// Successfully responded to client, so close the connection down gracefully.
					ShutdownConnection(sd);
				}
			}

			return true;
		}
	};
}
#else
namespace SimpleServer
{
	class HttpServer
	{};
}
#endif