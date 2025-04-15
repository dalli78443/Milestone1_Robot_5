#include "pch.h"
#include "CppUnitTest.h"
#include "../Milestone1_Robot_5/MySocket.h"
#include <climits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MySocketTest
{
	TEST_CLASS(MySocketTest)
	{
	private:
		// Robot Configuration Parameters
		struct RobotConfig {
			const char* ipAddress = "127.0.0.1";  // Default IP
			int port = 27015;                     // Default Port
			SocketType socketType = SocketType::CLIENT;
			ConnectionType connType = ConnectionType::TCP;
			int bufferSize = 1024;                // Default Buffer Size
		};

		std::wstring ConvertToWString(const std::string& str) {
			return std::wstring(str.begin(), str.end());
		}

		// Helper function to create socket with configuration
		MySocket* CreateSocket(const RobotConfig& config) {
			return new MySocket(
				config.socketType,
				config.ipAddress,
				config.port,
				config.connType,
				config.bufferSize
			);
		}

	public:
		TEST_METHOD_INITIALIZE(Initialize)
		{
			// Initialize Winsock
			WSADATA wsaData;
			int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (result != 0) {
				throw std::runtime_error("WSAStartup failed");
			}
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			WSACleanup();
		}

		TEST_METHOD(TestInvalidIPAddress)
		{
			RobotConfig config;
			
			// Test invalid IP address format
			Assert::ExpectException<std::runtime_error>([&]() {
				config.ipAddress = "invalid.ip.address";
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			// Test empty IP address
			Assert::ExpectException<std::runtime_error>([&]() {
				config.ipAddress = "";
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			// Test NULL IP address
			Assert::ExpectException<std::runtime_error>([&]() {
				config.ipAddress = nullptr;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});
		}

		TEST_METHOD(TestInvalidPort)
		{
			RobotConfig config;
			
			// Test port out of range
			Assert::ExpectException<std::runtime_error>([&]() {
				config.port = 0;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			Assert::ExpectException<std::runtime_error>([&]() {
				config.port = 65536;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			// Test negative port
			Assert::ExpectException<std::runtime_error>([&]() {
				config.port = -1;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});
		}

		TEST_METHOD(TestInvalidSocketType)
		{
			RobotConfig config;
			
			// Test invalid socket type
			Assert::ExpectException<std::runtime_error>([&]() {
				config.socketType = static_cast<SocketType>(999);
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			// Test invalid connection type
			Assert::ExpectException<std::runtime_error>([&]() {
				config.connType = static_cast<ConnectionType>(999);
				MySocket* socket = CreateSocket(config);
				delete socket;
			});
		}

		TEST_METHOD(TestInvalidBufferSize)
		{
			RobotConfig config;
			
			// Test negative buffer size
			Assert::ExpectException<std::runtime_error>([&]() {
				config.bufferSize = -1;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			// Test zero buffer size
			Assert::ExpectException<std::runtime_error>([&]() {
				config.bufferSize = 0;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});

			// Test extremely large buffer size
			Assert::ExpectException<std::runtime_error>([&]() {
				config.bufferSize = INT_MAX;
				MySocket* socket = CreateSocket(config);
				delete socket;
			});
		}

		TEST_METHOD(TestInvalidDataOperations)
		{
			RobotConfig config;
			MySocket* socket = nullptr;

			try {
				socket = CreateSocket(config);

				// Try to send data before connecting
				char data[] = "test";
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->SendData(data, sizeof(data));
				});

				// Try to get data before connecting
				char buffer[1024];
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->GetData(buffer);
				});

				// Try to send NULL data
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->SendData(nullptr, 10);
				});

				// Try to send data with invalid size
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->SendData(data, -1);
				});

				// Try to get data with NULL buffer
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->GetData(nullptr);
				});
			}
			catch (const std::exception& e) {
				if (socket) delete socket;
				Assert::Fail(ConvertToWString(std::string("Invalid data operations test failed with exception: ") + e.what()).c_str());
			}

			delete socket;
		}

		TEST_METHOD(TestInvalidDisconnect)
		{
			RobotConfig config;
			MySocket* socket = nullptr;

			try {
				socket = CreateSocket(config);

				// Try to disconnect TCP when not connected
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->DisconnectTCP();
				});

				// Try to disconnect UDP when not connected
				Assert::ExpectException<std::runtime_error>([&]() {
					socket->DisconnectUDP();
				});
			}
			catch (const std::exception& e) {
				if (socket) delete socket;
				Assert::Fail(ConvertToWString(std::string("Invalid disconnect test failed with exception: ") + e.what()).c_str());
			}

			delete socket;
		}
	};
} 