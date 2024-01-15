// platform detection

#define PLATFORM_WINDOWS    1
#define PLATFORM_MAC        2
#define PLATFORM_UNIX       3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

// headers for work with sockets

#if PLATFORM == PLATFORM_WINDOWS

	#include <winsock2.h>

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <fcntl.h>

#endif

// lib for Windows

#if PLATFORM == PLATFORM_WINDOWS
#pragma comment(lib, "wsock32.lib")
#endif

#include <stdio.h>

// initialize sockets for Windows

inline bool InitializeSockets()
{
	#if PLATFORM == PLATFORM_WINDOWS
	WSADATA WSAData;
	return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
	#else
	return true;
	#endif
}

inline void ShutdownSockets()
{
	#if PLATFORM == PLATFORM_WINDOWS
	WSACleanup();
	#endif
}

// make UDP socket

bool server(int port, const char *packet_data, int packet_size)
{
	int handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if (handle <= 0) 
	{
		printf("failed to create socket\n");
		return false;
	}
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short)port);

	if (bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0)
	{
		printf("failed to bind socket\n");
		return false;
	}
	return true;

	// switching the socket to non-blocking mode

	#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

		int nonBlocking = 1;
		if (fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1 )
		{
			printf("failed to set non-blocking socket\n");
			return false;
		}

	#elif PLATFORM == PLATFORM_WINDOWS

		DWORD nonBlocking = 1;
		if (ioctlsocket( handle, FIONBIO, &nonBlocking) != 0 )
		{
			printf("failed to set non-blocking socket\n");
			return false;
		}

	#endif

	// sending packages

	int sent_bytes = sendto(handle, (const char*)packet_data, packet_size, 0,\
							(sockaddr*)&address, sizeof(sockaddr_in));
	
	if (sent_bytes != packet_size)
	{
		printf("failed to send packet: return value = %d\n", sent_bytes);
		return false;
	}
}

int main()
{
	server(30000, "Hello LOLO", 11);

	return (0);
}
