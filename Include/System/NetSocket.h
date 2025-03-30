#pragma once
#ifdef EUGENE_NETWORK
#ifdef EUGENE_WINDOWS
#include <WS2tcpip.h>
#endif
#include <cstdint>
#include <string_view>

namespace Eugene
{
	using SocketHandle = SOCKET;

	/// <summary>
	/// TCP用ソケットクラス
	/// </summary>
	class TCPSocket
	{
	public:
#ifdef EUGENE_WINDOWS
		/// <summary>
		/// 初期化
		/// </summary>
		static void Init();

		/// <summary>
		/// 終了処理
		/// </summary>
		static void Finalize();

		/// <summary>
		/// ソケットハンドル指定コンストラクタ
		/// </summary>
		/// <param name="socketHandle"></param>
		TCPSocket(SocketHandle socketHandle) : socketHandle(socketHandle), isServer(false)
		{
		}

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		TCPSocket() : socketHandle(INVALID_SOCKET), isServer(false)
		{

		}
		~TCPSocket()
		{
			Close();
		}

		
		TCPSocket(TCPSocket&& tcpSocket) : socketHandle{ tcpSocket.socketHandle }, isServer{ tcpSocket.isServer }
		{
			tcpSocket.socketHandle = INVALID_SOCKET;
			tcpSocket.isServer = false;
		}

		TCPSocket& operator=(TCPSocket&& tcpSocket)
		{
			socketHandle = tcpSocket.socketHandle;
			tcpSocket.socketHandle = INVALID_SOCKET;
			isServer = tcpSocket.isServer;
			tcpSocket.isServer = false;
			return *this;
		}

		/// <summary>
		/// サーバーとしてポート番号をバインド
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		bool Bind(int port)
		{
			socketHandle = socket(AF_INET, SOCK_STREAM, 0);
			if (socketHandle < 0) return false;
			bool yes = true;
			setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = INADDR_ANY;
			addr.sin_port = htons(port);

			if (bind(socketHandle, (sockaddr*)&addr, sizeof(addr)) < 0) {
				Close();
				return false;
			}
			isServer = true;
			return true;
		}

		/// <summary>
		/// サーバーとして接続待ち
		/// </summary>
		/// <returns></returns>
		bool Listen()
		{
			return isServer && (listen(socketHandle, SOMAXCONN) == 0);
		}

		/// <summary>
		/// サーバーとしてクライアントを受け入れる
		/// </summary>
		/// <returns> 受け入れたクライアントのソケット </returns>
		TCPSocket Accept()
		{
			if (!isServer) return {};

			sockaddr_in clientAddr = {};
			int clientLen = sizeof(clientAddr);
			SocketHandle clientSocket = accept(socketHandle, (sockaddr*)&clientAddr, &clientLen);
			if (clientSocket < 0) return {};
			return TCPSocket{ clientSocket };
		}

		/// <summary>
		/// クライアントとして接続
		/// </summary>
		/// <param name="ip"> アドレス </param>
		/// <param name="port"> ポート </param>
		/// <returns></returns>
		bool Connect(std::string_view ip, int port)
		{
			socketHandle = socket(AF_INET, SOCK_STREAM, 0);
			if (socketHandle < 0) return false;

			struct sockaddr_in server;
			server.sin_port = htons(port);
			server.sin_family = AF_INET;
			if (inet_pton(AF_INET, ip.data(), &server.sin_addr) != 1) {
				struct addrinfo hints;
				addrinfo* res{ nullptr };
				std::memset(&hints, 0, sizeof(hints));
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_family = AF_INET;
				if (getaddrinfo(ip.data(), NULL, &hints, &res) != 0)
				{
					return false;
				}
				server.sin_addr.S_un = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.S_un;
				freeaddrinfo(res);
			}
			return connect(socketHandle, (struct sockaddr*)&server, sizeof(server)) == 0;
		}

		/// <summary>
		/// 送受信処理
		/// </summary>
		/// <param name="data"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		std::int32_t Send(const void* data, size_t size)
		{
			return send(socketHandle, (const char*)data, size, 0);
		}

		/// <summary>
		/// 読み取り処理
		/// </summary>
		/// <param name="buffer"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		std::int32_t Receive(void* buffer, size_t size)
		{
			return recv(socketHandle, (char*)buffer, size, 0);
		}

		// 接続確認
		bool IsConnected() const
		{
			return isServer;
		}

		// クローズ
		void Close()
		{
			closesocket(socketHandle);
			socketHandle = INVALID_SOCKET;
		}

	private:
		TCPSocket(const TCPSocket&) = delete;
		TCPSocket& operator=(const TCPSocket&) = delete;
		SocketHandle socketHandle;
		bool isServer;
#else
	public:
		static constexpr void Init() {}
		static constexpr void Finalize() {}
#endif	
	};

}
#endif