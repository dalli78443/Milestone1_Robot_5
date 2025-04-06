#include "pch.h"
#include "CppUnitTest.h"
#include "../Milestone1_Robot_5/pktDef.cpp"
#include "../Milestone1_Robot_5/pktDef.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace pktDefTest
{
	TEST_CLASS(pktDefTest)
	{
	public:
		TEST_METHOD(TestDefaultConstructor)
		{
			PktDef packet;
			Assert::AreEqual(0, (int)packet.GetPktCount());
			Assert::AreEqual(0, (int)packet.GetLength());
			Assert::IsTrue(packet.GetBodyData() == nullptr);
			Assert::IsFalse(packet.GetAck());
		}

		TEST_METHOD(TestSetCmd)
		{
			PktDef packet;
			
			// Test DRIVE command
			packet.SetCmd(CmdType::DRIVE);
			Assert::IsTrue(packet.GetCmd() == CmdType::DRIVE);
			Assert::IsFalse(packet.GetAck());
			
			// Test SLEEP command
			packet.SetCmd(CmdType::SLEEP);
			Assert::IsTrue(packet.GetCmd() == CmdType::SLEEP);
			Assert::IsFalse(packet.GetAck());
			
			// Test RESPONSE command
			packet.SetCmd(CmdType::RESPONSE);
			Assert::IsTrue(packet.GetCmd() == CmdType::RESPONSE);
			Assert::IsFalse(packet.GetAck());
		}

		TEST_METHOD(TestSetPktCount)
		{
			PktDef packet;
			packet.SetPktCount(123);
			Assert::AreEqual(123, (int)packet.GetPktCount());
		}

		TEST_METHOD(TestDriveCommand)
		{
			PktDef packet;
			packet.SetCmd(CmdType::DRIVE);
			
			// Create drive body data
			char driveData[3] = { FORWARD, 5, 85 }; // Direction, Duration, Speed
			packet.SetBodyData(driveData, 3);
			
			Assert::AreEqual(HEADERSIZE + 3 + 1, (int)packet.GetLength()); // Header + Body + CRC
			Assert::IsTrue(packet.GetBodyData() != nullptr);
			
			char* bodyData = packet.GetBodyData();
			Assert::AreEqual((int)FORWARD, (int)bodyData[0]);
			Assert::AreEqual(5, (int)bodyData[1]);
			Assert::AreEqual(85, (int)bodyData[2]);
		}

		TEST_METHOD(TestDriveDirections)
		{
			PktDef packet;
			packet.SetCmd(CmdType::DRIVE);
			
			// Test FORWARD
			char forwardData[3] = { FORWARD, 5, 85 };
			packet.SetBodyData(forwardData, 3);
			char* bodyData = packet.GetBodyData();
			Assert::AreEqual((int)FORWARD, (int)bodyData[0]);
			
			// Test BACKWARD
			char backwardData[3] = { BACKWARD, 5, 85 };
			packet.SetBodyData(backwardData, 3);
			bodyData = packet.GetBodyData();
			Assert::AreEqual((int)BACKWARD, (int)bodyData[0]);
			
			// Test RIGHT
			char rightData[3] = { RIGHT, 5, 85 };
			packet.SetBodyData(rightData, 3);
			bodyData = packet.GetBodyData();
			Assert::AreEqual((int)RIGHT, (int)bodyData[0]);
			
			// Test LEFT
			char leftData[3] = { LEFT, 5, 85 };
			packet.SetBodyData(leftData, 3);
			bodyData = packet.GetBodyData();
			Assert::AreEqual((int)LEFT, (int)bodyData[0]);
		}

		TEST_METHOD(TestDriveSpeedRange)
		{
			PktDef packet;
			packet.SetCmd(CmdType::DRIVE);
			
			// Test minimum speed (80%)
			char minSpeedData[3] = { FORWARD, 5, 80 };
			packet.SetBodyData(minSpeedData, 3);
			char* bodyData = packet.GetBodyData();
			Assert::AreEqual(80, (int)bodyData[2]);
			
			// Test maximum speed (100%)
			char maxSpeedData[3] = { FORWARD, 5, 100 };
			packet.SetBodyData(maxSpeedData, 3);
			bodyData = packet.GetBodyData();
			Assert::AreEqual(100, (int)bodyData[2]);
		}

		TEST_METHOD(TestSleepCommand)
		{
			PktDef packet;
			packet.SetCmd(CmdType::SLEEP);
			
			// Sleep command should have no body data
			Assert::AreEqual(HEADERSIZE + 1, (int)packet.GetLength()); // Header + CRC only
			Assert::IsTrue(packet.GetBodyData() == nullptr);
		}

		TEST_METHOD(TestResponseCommand)
		{
			PktDef packet;
			packet.SetCmd(CmdType::RESPONSE);
			
			// Response command should have no body data
			Assert::AreEqual(HEADERSIZE + 1, (int)packet.GetLength()); // Header + CRC only
			Assert::IsTrue(packet.GetBodyData() == nullptr);
		}

		TEST_METHOD(TestCRC)
		{
			PktDef packet;
			packet.SetCmd(CmdType::DRIVE);
			packet.SetPktCount(1);
			
			char driveData[3] = { FORWARD, 5, 85 };
			packet.SetBodyData(driveData, 3);
			
			packet.CalcCRC();
			char* rawPacket = packet.GenPacket();
			
			Assert::IsTrue(packet.CheckCRC(rawPacket, packet.GetLength()));
		}

		TEST_METHOD(TestPacketGeneration)
		{
			PktDef packet;
			packet.SetCmd(CmdType::DRIVE);
			packet.SetPktCount(1);
			
			char driveData[3] = { FORWARD, 5, 85 };
			packet.SetBodyData(driveData, 3);
			
			char* rawPacket = packet.GenPacket();
			Assert::IsTrue(rawPacket != nullptr);
			
			// Create new packet from raw data
			PktDef newPacket(rawPacket);
			Assert::AreEqual(1, (int)newPacket.GetPktCount());
			Assert::IsTrue(newPacket.GetCmd() == CmdType::DRIVE);
			
			char* newBodyData = newPacket.GetBodyData();
			Assert::AreEqual((int)FORWARD, (int)newBodyData[0]);
			Assert::AreEqual(5, (int)newBodyData[1]);
			Assert::AreEqual(85, (int)newBodyData[2]);
		}

		TEST_METHOD(TestInvalidCRC)
		{
			PktDef packet;
			packet.SetCmd(CmdType::DRIVE);
			packet.SetPktCount(1);
			
			char driveData[3] = { FORWARD, 5, 85 };
			packet.SetBodyData(driveData, 3);
			
			char* rawPacket = packet.GenPacket();
			rawPacket[packet.GetLength() - 1] = 0xFF; // Corrupt CRC
			
			Assert::IsFalse(packet.CheckCRC(rawPacket, packet.GetLength()));
		}

		TEST_METHOD(TestHeaderSize)
		{
			// Verify HEADERSIZE constant is correct (2 bytes PktCount + 1 byte Command + 1 byte Length = 4 bytes)
			Assert::AreEqual(4, HEADERSIZE);
		}

		TEST_METHOD(TestCommandFlags)
		{
			PktDef packet;
			
			// Test DRIVE flag
			packet.SetCmd(CmdType::DRIVE);
			Assert::IsTrue(packet.GetCmd() == CmdType::DRIVE);
			Assert::IsFalse(packet.GetAck());
			
			// Test SLEEP flag
			packet.SetCmd(CmdType::SLEEP);
			Assert::IsTrue(packet.GetCmd() == CmdType::SLEEP);
			Assert::IsFalse(packet.GetAck());
			
			// Test RESPONSE flag
			packet.SetCmd(CmdType::RESPONSE);
			Assert::IsTrue(packet.GetCmd() == CmdType::RESPONSE);
			Assert::IsFalse(packet.GetAck());
		}

		TEST_METHOD(TestPacketLength)
		{
			PktDef packet;
			
			// Test empty packet (header + CRC only)
			packet.SetCmd(CmdType::SLEEP);
			Assert::AreEqual(HEADERSIZE + 1, (int)packet.GetLength());
			
			// Test packet with body data
			packet.SetCmd(CmdType::DRIVE);
			char driveData[3] = { FORWARD, 5, 85 };
			packet.SetBodyData(driveData, 3);
			Assert::AreEqual(HEADERSIZE + 3 + 1, (int)packet.GetLength());
		}
	};
}
