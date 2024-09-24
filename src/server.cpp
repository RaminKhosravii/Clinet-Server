#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <nlohmann/json.hpp>

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json; // Alias for nlohmann::json to simplify usage

// Define constants for port and buffer size
constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

// Structure to represent device status
struct Device {
    std::string id;
    std::string status;
};

// Map to store device information with device name as the key
std::map<std::string, Device> deviceRegistry;

// Function to handle incoming JSON requests and generate a JSON response
json handleRequest(const json& request) {
    json response;

    const std::string requestType = request.value("request_type", "");

    if (requestType == "connect") {
        // Handle connection request
        response["result"] = "success";
        response["message"] = "Connected to server.";
    }
    else if (requestType == "add_device") {
        // Handle device addition request
        const std::string deviceName = request.value("name", "");
        const std::string deviceStatus = request.value("status", "");

        // Add device to the registry
        deviceRegistry[deviceName] = {deviceName, deviceStatus};

        response["result"] = "success";
        response["message"] = "Device added.";
    }
    else if (requestType == "control_device") {
        // Handle device control request
        const std::string deviceName = request.value("name", "");
        const std::string newStatus = request.value("new_status", "");

        // Check if the device exists in the registry
        const auto it = deviceRegistry.find(deviceName);
        if (it != deviceRegistry.end()) {
            it->second.status = newStatus;
            response["result"] = "success";
            response["message"] = "Device status updated.";
        } else {
            response["result"] = "error";
            response["message"] = "Device not found.";
        }
    }
    else if (requestType == "get_devices") {
        // Handle request for getting device information
        json devicesJson = json::array();

        // Populate the devices JSON array with each device's information
        for (const auto& [name, device] : deviceRegistry) {
            devicesJson.push_back({{"name", device.id}, {"status", device.status}});
        }

        response["result"] = "success";
        response["devices"] = devicesJson;
    }
    else {
        response["result"] = "error";
        response["message"] = "Invalid request.";
    }

    return response;
}

// Function to initialize and start the server
void startServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    // Create a server socket
    const SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return;
    }

    // Allow the socket to be reused
    constexpr int option = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&option), sizeof(option)) != 0) {
        std::cerr << "Failed to set socket options." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    sockaddr_in serverAddress{};
    // Set up the server address structure
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, SOMAXCONN) < 0) {
        std::cerr << "Listening failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    sockaddr_in clientAddress{};
    int addrlen = sizeof(clientAddress);

    // Accept a client connection
    const SOCKET clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), (socklen_t*)&addrlen);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    char buffer[BUFFER_SIZE] = {0};
    // Main loop to handle client requests
    while (true) {
        const int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            break;
        }
        buffer[bytesRead] = '\0';

        try {
            json request = json::parse(buffer);
            json response = handleRequest(request);
            std::string responseString = response.dump();
            // Send the response back to the client
            send(clientSocket, responseString.c_str(), static_cast<int>(responseString.size()), 0);
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
    }

    // Clean up resources on exit
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    startServer();
    return 0;
}