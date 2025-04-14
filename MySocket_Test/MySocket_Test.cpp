#include "pch.h"
#include "CppUnitTest.h"
#include "../Milestone1_Robot_5/MySocket.h"
#include "../Milestone1_Robot_5/MySocket.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MySocketTest
{
	TEST_CLASS(MySocketTest)
	{
	public:
		TEST_METHOD(TestConstructor)
		{
			// Test TCP Client
			MySocket tcpClient(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);
			Assert::AreEqual(std::string("127.0.0.1"), tcpClient.GetIPAddr());
			Assert::AreEqual(27015, tcpClient.GetPort());
			Assert::IsTrue(tcpClient.GetType() == SocketType::CLIENT);

			// Test UDP Server
			MySocket udpServer(SocketType::SERVER, "127.0.0.1", 27016, ConnectionType::UDP);
			Assert::AreEqual(std::string("127.0.0.1"), udpServer.GetIPAddr());
			Assert::AreEqual(27016, udpServer.GetPort());
			Assert::IsTrue(udpServer.GetType() == SocketType::SERVER);

			// Test with custom buffer size
			MySocket customBuffer(SocketType::CLIENT, "127.0.0.1", 27017, ConnectionType::TCP, 2048);
			// No exception should be thrown
		}

		TEST_METHOD(TestInvalidBufferSize)
		{
			// Test with zero buffer size (should use DEFAULT_SIZE)
			MySocket zeroBuffer(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP, 0);
			// No exception should be thrown

			// Test with negative buffer size (should use DEFAULT_SIZE)
			MySocket negativeBuffer(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP, -1);
			// No exception should be thrown
		}

		TEST_METHOD(TestSetIPAddr)
		{
			MySocket socket(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);
			
			// Test valid IP change
			socket.SetIPAddr("192.168.1.1");
			Assert::AreEqual(std::string("192.168.1.1"), socket.GetIPAddr());

			// Test invalid IP change while connected
			socket.ConnectTCP();
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SetIPAddr("10.0.0.1"); 
			});
			socket.DisconnectTCP();
		}

		TEST_METHOD(TestSetPort)
		{
			MySocket socket(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);
			
			// Test valid port change
			socket.SetPort(27020);
			Assert::AreEqual(27020, socket.GetPort());

			// Test invalid port change while connected
			socket.ConnectTCP();
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SetPort(27025); 
			});
			socket.DisconnectTCP();
		}

		TEST_METHOD(TestSetType)
		{
			MySocket socket(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);
			
			// Test valid type change
			socket.SetType(SocketType::SERVER);
			Assert::IsTrue(socket.GetType() == SocketType::SERVER);

			// Test invalid type change while connected
			socket.ConnectTCP();
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SetType(SocketType::CLIENT); 
			});
			socket.DisconnectTCP();
		}

		TEST_METHOD(TestTCPConnection)
		{
			// Create server and client
			MySocket server(SocketType::SERVER, "127.0.0.1", 27015, ConnectionType::TCP);
			MySocket client(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);

			// Server should be able to accept connections
			server.ConnectTCP();

			// Client should be able to connect
			client.ConnectTCP();

			// Test data transmission
			const char* testData = "Hello TCP!";
			char receiveBuffer[1024];

			client.SendData(testData, strlen(testData) + 1);
			int bytesReceived = server.GetData(receiveBuffer);

			Assert::AreEqual(strlen(testData) + 1, static_cast<size_t>(bytesReceived));
			Assert::AreEqual(0, strcmp(testData, receiveBuffer));

			// Cleanup
			client.DisconnectTCP();
			server.DisconnectTCP();
		}

		TEST_METHOD(TestUDPCommunication)
		{
			// Create server and client
			MySocket server(SocketType::SERVER, "127.0.0.1", 27015, ConnectionType::UDP);
			MySocket client(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::UDP);

			// Test data
			const char* testData = "Hello UDP!";
			char receiveBuffer[1024];

			// Send data from client to server
			client.SendData(testData, strlen(testData) + 1);

			// Server should receive the data
			int bytesReceived = server.GetData(receiveBuffer);
			Assert::AreEqual(strlen(testData) + 1, static_cast<size_t>(bytesReceived));
			Assert::AreEqual(0, strcmp(testData, receiveBuffer));
		}

		TEST_METHOD(TestInvalidOperations)
		{
			MySocket socket(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);

			// Try to change IP while connected
			socket.ConnectTCP();
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SetIPAddr("192.168.1.1"); 
			});
			socket.DisconnectTCP();

			// Try to change port while connected
			socket.ConnectTCP();
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SetPort(27020); 
			});
			socket.DisconnectTCP();

			// Try to change type while connected
			socket.ConnectTCP();
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SetType(SocketType::SERVER); 
			});
			socket.DisconnectTCP();

			// Try to connect UDP socket
			MySocket udpSocket(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::UDP);
			Assert::ExpectException<std::runtime_error>([&]() { 
				udpSocket.ConnectTCP(); 
			});
		}

		TEST_METHOD(TestBufferOverflow)
		{
			// Create socket with small buffer
			MySocket socket(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP, 1024);
			
			// Create data larger than buffer
			const int largeSize = 2048;
			char* largeData = new char[largeSize];
			memset(largeData, 'A', largeSize);
			largeData[largeSize - 1] = '\0';

			// Try to send data larger than buffer
			Assert::ExpectException<std::runtime_error>([&]() { 
				socket.SendData(largeData, largeSize); 
			});

			delete[] largeData;
		}

		TEST_METHOD(TestMultipleConnections)
		{
			// Create server and two clients
			MySocket server(SocketType::SERVER, "127.0.0.1", 27015, ConnectionType::TCP);
			MySocket client1(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);
			MySocket client2(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP);

			// Server accepts first connection
			server.ConnectTCP();
			client1.ConnectTCP();

			// Second client should fail to connect (server only accepts one connection)
			Assert::ExpectException<std::runtime_error>([&]() { 
				client2.ConnectTCP(); 
			});

			// Cleanup
			client1.DisconnectTCP();
			server.DisconnectTCP();
		}

		TEST_METHOD(TestLargeData)
		{
			// Create server and client with large buffer
			const int largeBufferSize = 8192;
			MySocket server(SocketType::SERVER, "127.0.0.1", 27015, ConnectionType::TCP, largeBufferSize);
			MySocket client(SocketType::CLIENT, "127.0.0.1", 27015, ConnectionType::TCP, largeBufferSize);

			server.ConnectTCP();
			client.ConnectTCP();

			// Create large data buffer
			const int largeSize = 8192;
			char* largeData = new char[largeSize];
			memset(largeData, 'A', largeSize);
			largeData[largeSize - 1] = '\0';

			char* receiveBuffer = new char[largeSize];

			try {
				// Send and receive large data
				client.SendData(largeData, largeSize);
				int bytesReceived = server.GetData(receiveBuffer);

				Assert::AreEqual(largeSize, bytesReceived);
				Assert::AreEqual(0, memcmp(largeData, receiveBuffer, largeSize));
			}
			catch (...) {
				// Cleanup in case of exception
				delete[] largeData;
				delete[] receiveBuffer;
				client.DisconnectTCP();
				server.DisconnectTCP();
				throw;
			}

			// Cleanup
			delete[] largeData;
			delete[] receiveBuffer;
			client.DisconnectTCP();
			server.DisconnectTCP();
		}
	};
}
