#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <string>

// Link the ws2_32 library
#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;

// Server configuration constants
auto SERVER_IP = "127.0.0.1";
constexpr int PORT = 54000;
constexpr int BUFFER_SIZE = 1024;

// DeviceClient class handles communication with the server
class DeviceClient {
private:
    SOCKET socket_;
    bool isConnected_;
    std::vector<std::string> actionLogs_;

public:
    // Constructor: Initializes Winsock and sets up the client
    DeviceClient() : socket_(INVALID_SOCKET), isConnected_(false) {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }

    // Destructor: Cleans up resources when the client is destroyed
    ~DeviceClient() {
        shutdownConnection();
        WSACleanup();
    }

    // Displays the main menu for user interaction
    void displayMainMenu() {
        int choice;
        do {
            std::cout << "\nMain Menu:\n";
            std::cout << "1. Connect\n2. Send Device Info\n3. Receive Device Info\n";
            std::cout << "4. Generate Report\n5. Disconnect\n0. Exit\nChoose an option: ";
            std::cin >> choice;

            // Process user selection
            switch (choice) {
                case 1: connectToServer(); break;
                case 2: sendDeviceInformation(); break;
                case 3: receiveDeviceInformation(); break;
                case 4: createReport(); break;
                case 5: shutdownConnection(); break;
                case 0: std::cout << "Exiting...\n"; break;
                default: std::cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 0); // Continue until the user exits
    }

private:
    // Connects to the server
    void connectToServer() {
        socket_ = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT);
        inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);

        // Attempt to connect to the server
        if (connect(socket_, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
            logAction("Connection failed.");
            return;
        }

        isConnected_ = true;
        logAction("Successfully connected to server.");
    }

    // Menu for sending device information
    void sendDeviceInformation() {
        int choice;
        do {
            std::cout << "\nSend Device Information Menu:\n";
            std::cout << "1. Add Device\n2. Control Device\n3. Back to Main Menu\nChoose an option: ";
            std::cin >> choice;

            // Process user's choice
            switch (choice) {
                case 1: addDevice(); break;
                case 2: controlDevice(); break;
                case 3: return;
                default: std::cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 3);
    }

    // Initiates adding a device to the server
    void addDevice() {
        std::string deviceType, deviceStatus;
        std::cout << "Enter device type: ";
        std::cin >> deviceType;
        std::cout << "Enter device status: ";
        std::cin >> deviceStatus;

        // Prepare JSON request to add device
        const json request = { {"action", "add_device"}, {"type", deviceType}, {"status", deviceStatus} };
        json response;
        processRequest(request, response);
        std::cout << response.dump(4) << std::endl;
        logAction("Added device: " + deviceType + " with status: " + deviceStatus);
    }

    // Controls an existing device by updating its status
    void controlDevice() {
        std::string deviceName, newStatus;
        std::cout << "Enter device name: ";
        std::cin >> deviceName;
        std::cout << "Enter new status: ";
        std::cin >> newStatus;

        // Prepare JSON request to control device
        const json request = { {"action", "control_device"}, {"name", deviceName}, {"status", newStatus} };
        json response;
        processRequest(request, response);
        std::cout << response.dump(4) << std::endl;
        logAction("Controlled device: " + deviceName + " to status: " + newStatus);
    }

    // Retrieves and displays device information from the server
    void receiveDeviceInformation() {
        const json request = { {"action", "get_devices"} };
        json response;
        processRequest(request, response);
        std::cout << "Devices:\n" << response.dump(4) << std::endl;
        logAction("Received devices information.");
    }

    // Generates a report of logged actions and saves to a file
    void createReport() {
        std::ofstream reportFile("report.json");
        if (reportFile.is_open()) {
            const json reportJson = { {"logs", actionLogs_} };
            reportFile << reportJson.dump(4);
            reportFile.close(); // Close the file
            std::cout << "Report generated: report.json\n";
        } else {
            std::cerr << "Unable to create report file." << std::endl;
        }
    }

    // Closes the connection to the server
    void shutdownConnection() {
        if (isConnected_) {
            closesocket(socket_);
            isConnected_ = false;
            logAction("Disconnected from server.");
            std::cout << "Connection closed.\n";
        } else {
            std::cout << "Already disconnected.\n";
        }
    }

    // Sends a JSON request to the server and receives a response
    void processRequest(const json& request, json& response) {
        const std::string requestString = request.dump();
        send(socket_, requestString.c_str(), static_cast<int>(requestString.size()), 0); // Send request to server

        char buffer[BUFFER_SIZE] = {0};
        const int bytesRead = recv(socket_, buffer, BUFFER_SIZE, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response = json::parse(buffer);
            logAction("Received Response: " + response.dump(4));
        } else {
            logAction("Failed to receive response from server.");
        }
    }

    // Logs actions performed by the client
    void logAction(const std::string& action) {
        actionLogs_.push_back(action);
    }
};


int main() {
    DeviceClient deviceClient;
    deviceClient.displayMainMenu();
    return 0; // End of the program
}