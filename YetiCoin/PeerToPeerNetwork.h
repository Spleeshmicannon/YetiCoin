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

		const std::bitset<2> Flags;
		const std::string Data;

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

	class Sender
	{
	private:
		std::string pkt;
		std::string response;

	public:
		Sender() : pkt("") {}

		/// <summary>
		/// Assembles a Packet variable into something
		/// that can be easily sent over the network.
		/// </summary>
		/// <param name="pkt">Packet file containing packet data</param>
		void AssemblePacket(Packet pktInfo)
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

			// keeping the context busy so it doesn't run out of work
			asio::io_context::work idleWork(context);

			std::thread contThread = std::thread([&]() { context.run(); });

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
				socket.async_wait(socket.wait_read, [&](const asio::error_code _ec) 
					{
						ec = _ec;
						if (!ec)
						{
							std::cout << "Wait successful" << std::endl;
							socket.read_some(asio::buffer(response), ec);
						}
						else
						{
							std::cout << "Wait failed:\n" << std::endl;
						}
					}
				);
			}
		}
	};

	class Reciever
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

			// keeping the context busy so it doesn't run out of work
			asio::io_context::work idleWork(context);

			std::thread contThread = std::thread([&]() { context.run(); });

			// the acceptor for accepting connections
			asio::ip::tcp::acceptor acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 100));

			// create a soekct, the context will deliver the implementation
			asio::ip::tcp::socket socket(context);

			// assigning the acceptor the socket
			acceptor.accept(socket);
			asio::streambuf buf;

			while (asio::read(socket, buf, ec))
			{
				std::cout << "recieved: " << &buf << std::endl;
				asio::write(socket, asio::buffer("Thankyou for sending!"), ec);
				

				if (ec)
				{
					std::cout << "error: " << ec.message() << std::endl;
					break;
				}
			}
		}
		Packet getPacket() { return pkt; }
	};
}