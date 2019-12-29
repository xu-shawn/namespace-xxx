#pragma once

#include <Mstcpip.h>
#include <vector>

namespace xxx
{
	template <class T>
	class clientSocket
	{
	public:
		bool bDelete = false;

		long snedNum = 0;
		long revNum = 0;
		COleDateTime starttime;
		COleDateTime endtime;
		HWND hmain = nullptr;
		int message=0;
		std::vector<char> sendvector{};
		std::mutex socketMutex;

		std::string ip;
		T a;
		unsigned int port;

		SOCKET xsocket = 0;
		sockaddr_in addr;
		int addrlen = 0;
		long long frequency = 1;

		clientSocket()
		{
			xsocket = 0;
			addrlen = sizeof(addr);
			starttime = COleDateTime::GetCurrentTime();
			endtime = COleDateTime::GetCurrentTime();
			LARGE_INTEGER xhhtime;
			QueryPerformanceFrequency(&xhhtime);
			frequency = xhhtime.QuadPart;
		}
		clientSocket(HWND sethmain,int setmessage)
		{
			hmain = sethmain;
			message = setmessage;
			xsocket = 0;
			addrlen = sizeof(addr);
			starttime = COleDateTime::GetCurrentTime();
			endtime = COleDateTime::GetCurrentTime();
			LARGE_INTEGER xhhtime;
			QueryPerformanceFrequency(&xhhtime);
			frequency = xhhtime.QuadPart;
		}
		clientSocket(std::string setip, unsigned int setport)
		{
			ip = setip;
			port = setport;

			xsocket = 0;
			addrlen = sizeof(addr);
			starttime = COleDateTime::GetCurrentTime();
			endtime = COleDateTime::GetCurrentTime();
			LARGE_INTEGER xhhtime;
			QueryPerformanceFrequency(&xhhtime);
			frequency = xhhtime.QuadPart;
		}


		~clientSocket()
		{
			std::lock_guard<std::mutex> l(socketMutex);
			if (xsocket > 0)
			{
				::closesocket(xsocket);
				xsocket = 0;
			}
		}

		BOOL link()
		{
			std::lock_guard<std::mutex> l(socketMutex);
		
			if (xsocket > 0)
				return true;

			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

			try
			{
				xsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (xsocket <= 0)
				{
					xsocket = 0;
					return false;
				}
			}
			catch (...)
			{
				xsocket = 0;
				return false;
			}

			try
			{
				if (::connect(xsocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
				{
					if (xsocket > 0)
						::closesocket(xsocket);
					xsocket = 0;

					return FALSE;
				}



				if (::WSAAsyncSelect(xsocket, hmain, message, FD_READ | FD_CLOSE) != 0)
				{
					if (xsocket > 0)
						::closesocket(xsocket);
					xsocket = 0;
					return FALSE;
				}

				tcp_keepalive keepalive;
				keepalive.onoff = 1;
				keepalive.keepaliveinterval = 3000;
				keepalive.keepalivetime = 3000;
				DWORD out;
				if (WSAIoctl(xsocket, SIO_KEEPALIVE_VALS, &keepalive, sizeof(keepalive), NULL, 0, &out, NULL, NULL))
				{
					if (xsocket > 0)
						::closesocket(xsocket);
					xsocket = 0;
					return FALSE;
				}
			}
			catch (...)
			{
				if (xsocket > 0)
					::closesocket(xsocket);
				xsocket = 0;

				return FALSE;
			}
			return true;
		}

		BOOL close()
		{
			std::lock_guard<std::mutex> l(socketMutex);
			try
			{
				if(xsocket>0)
					::closesocket(xsocket);
				xsocket = 0;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		int send(const std::vector<char>& vs)
		{
			std::lock_guard<std::mutex> l(socketMutex);
			if (xsocket <= 0)
				return 0;
			auto sendlen{ 0 };
			if (xsocket > 0)
			{
				sendlen = ::send(xsocket, vs.data(), vs.size(), 0);

			}
			return sendlen;
		}

		int send(char *sendch,int len)
		{
			std::lock_guard<std::mutex> l(socketMutex);
			if (xsocket <= 0)
				return 0;
			auto sendlen{ 0 };
			if (xsocket > 0)
			{
				sendlen = ::send(xsocket, sendch, len, 0);
			}
			return sendlen;
		}
		std::vector<char> recivedv()
		{
			std::vector<char> recvector{};

			std::lock_guard<std::mutex> l(socketMutex);
			if (xsocket <= 0)
				return recvector;


			auto reclen = 0;
			std::unique_ptr<char> recbuf{ nullptr }; 
			try
			{
				recbuf.reset(new char[64000]);

				LARGE_INTEGER xhhtime;
				::QueryPerformanceCounter(&xhhtime);
				LONGLONG start = xhhtime.QuadPart;
				LONGLONG end = xhhtime.QuadPart;

				LONGLONG t = (end - start) / frequency;

				LONGLONG timeout = 10;

				while (t <= timeout)
				{
					::QueryPerformanceCounter(&xhhtime);
					LONGLONG end = xhhtime.QuadPart;
					t = (end - start) / frequency;

					reclen = ::recv(xsocket, recbuf.get(), 64000, 0);
					if (SOCKET_ERROR == reclen)
					{
						int err = WSAGetLastError();
						if (err == WSAEWOULDBLOCK)
						{
							Sleep(100);
							std::cout << "waiting back msg!" << endl;
							continue;
						}
						else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
						{
							std::cout << "read socket err!" << endl;
							break;
						}
						break;
					}
					break;
				}
			}
			catch (...)
			{
				reclen = 0;
			}

			if (reclen > 0)
			{
				recvector.assign(recbuf.get(), recbuf.get() + reclen);
			}
		
		return recvector;
		}
	};
}//namespace xxx
