#include "../../Include/System/NetSocket.h"
#include "../../Include/Debug/Debug.h"

#ifdef EUGENE_NETWORK
#pragma comment(lib, "ws2_32.lib")

namespace Eugene
{
	void TCPSocket::Init()
	{
		WSADATA wsaData;
		EUGENE_ASSERT_MSG(WSAStartup(WINSOCK_VERSION, &wsaData) == 0, "WSAStartupé∏îs");
	}

	void TCPSocket::Finalize()
	{
		WSACleanup();
	}
}

#endif