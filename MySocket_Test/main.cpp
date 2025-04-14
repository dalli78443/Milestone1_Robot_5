#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Run tests
    try {
        Microsoft::VisualStudio::CppUnitTestFramework::UnitTest::RunAllTests();
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        WSACleanup();
        return 1;
    }

    // Cleanup Winsock
    WSACleanup();
    return 0;
} 