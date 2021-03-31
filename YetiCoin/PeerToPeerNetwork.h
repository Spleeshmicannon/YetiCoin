#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <bitset>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace p2p
{
	/// <summary>
	/// An easily buildable packet object for managing the data
	/// sent over the p2p network.
	/// </summary>
	struct Packet
	{
		/// <summary>
		/// Simple flags to be used in the packet when initialising.
		/// Format is  Flag0 | Flag1 | Flag2 etc..
		/// </summary>
		struct Flag
		{
			static constexpr std::bitset<2> Miner{ 0b00 }, 
				Wallet{ 0b01 }, Sender{ 0b00 }, Reciever{ 0b10 };
		};

		std::bitset<2> Flags;
		std::string Data;

		/// <summary>
		/// The main constructor for the Packet struct.
		/// </summary>
		/// <param name="data"></param>
		/// <param name="flags"></param>
		Packet(std::string data, std::bitset<2> flags) : Data(data), Flags(flags)
		{}

		/// <summary>
		/// Default Packet initialisation. Shouldn't be used,
		/// unless in testing or as a placeholder.
		/// </summary>
		Packet() : Data("DATA UNDEFINED!"), Flags(0b01) {}
	};

	class Network
	{
	protected:
		static void setupContext(asio::io_context &ioc)
		{
			// keeping the context busy so it doesn't run out of work
			asio::io_context::work idleWork(ioc);

			// running on seperate thread
			std::thread contThread = std::thread([&]() { ioc.run(); });
			contThread.detach(); // Watch this, it could cause problems
		}

		static void closeGracefully(asio::io_context& ioc, asio::ip::tcp::socket& sock, asio::error_code &ec)
		{
			sock.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
			ioc.stop();
			std::cout << "shutdown: " << ec.message() << std::endl;
		}
	};
	class Sender : Network
	{
	private:
		std::string pkt;
		std::string response;

	public:
		Sender() : pkt("Hello!") {}

		/// <summary>
		/// Assembles a Packet variable into something
		/// that can be easily sent over the network.
		/// </summary>
		/// <param name="pkt">Packet file containing packet data</param>
		void AssemblePacket(const Packet pktInfo)
		{
			// assigning packet data to the packet source
			pkt = pktInfo.Data;
		}

		/// <summary>
		/// Sends the packet to 'ip' 
		/// </summary>
		/// <param name="ip"></param>
		/// <returns></returns>
		void SendPacket(std::string ip)
		{
			asio::error_code ec;

			// the platform sepcific interface
			asio::io_context context;

			// setting up context running with work
			Network::setupContext(context);

			// the network location
			asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip, ec), 100);

			// create a soekct, the context will deliver the implementation
			asio::ip::tcp::socket socket(context);

			// tell the socket to try and connect
			socket.connect(endpoint, ec);

			if (!ec)
			{
				std::cout << "Connected!" << std::endl;
			}
			else
			{
				std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
			}

			if (socket.is_open())
			{
				socket.write_some(asio::buffer(pkt.data(), pkt.size()), ec);
				socket.wait(socket.wait_read);
				size_t bytesRead = socket.read_some(asio::buffer(response), ec);
				std::cout << "response: " << response << ec.message() << std::endl;
				std::cout << "Bytes read: " << bytesRead << std::endl;
			}

			Network::closeGracefully(context, socket, ec);
		}
	};

	class Reciever : Network
	{
	private:
		Packet pkt;
		std::string response;

	public:
		Reciever() : pkt({Packet()}) {}
		void asyncWait()
		{
			asio::error_code ec;

			// the platform sepcific interface
			asio::io_context context;

			// setting up context running with work
			Network::setupContext(context);

			// the acceptor for accepting connections
			asio::ip::tcp::acceptor acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 100));

			// preparing acceptor
			acceptor.listen();

			// create a soekct, the context will deliver the implementation
			asio::ip::tcp::socket socket(context);

			// assigning the acceptor the socket
			acceptor.accept(socket);
			std::cout << "Connection accepted" << std::endl;

			if (socket.is_open())
			{
				std::string tmp;
				socket.read_some(asio::buffer(tmp), ec);
				std::cout << "response: " << tmp << ec.message() << std::endl;
				tmp = "Thankyou for your message!";
				socket.write_some(asio::buffer(tmp.data(), tmp.size()), ec);
			}
			else
			{
				std::cout << "Socket closed" << std::endl;
			}

			Network::closeGracefully(context, socket, ec);
		}
		Packet getPacket() { return pkt; }
	};
}