#include "../../Include/System/NetSocket.h"
#include "../../Include/Utils/EugeneLibException.h"

#ifdef EUGENE_NETWORK
#pragma comment(lib, "ws2_32.lib")

namespace Eugene
{
	void TCPSocket::Init()
	{
		WSADATA wsaData;
		if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
		{
			throw EugeneLibException{ "WSAStartupé∏îs" };
		}
	}

	void TCPSocket::Finalize()
	{
		WSACleanup();
	}
}

#endif