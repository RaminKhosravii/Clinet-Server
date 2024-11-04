#include <Client.h>

using json = nlohmann::json;

Client::Client() : sock(-1), server(), is_connected(false) {
}

void Client::connect_Server(const std::string &server_address, int port) {
    if (is_connected) {
        std::cout << "Already connected to the server!" << std::endl;
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, server_address.c_str(), &server.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0) { // Connect to server
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
// Update connection status
    is_connected = true;
    std::cout << "Connected to server!" << std::endl;
}

void Client::close_Connection() {
    if (is_connected) {
        close(sock);
        is_connected = false;
        std::cout << "Connection closed!" << std::endl;
    } else {
        std::cout << "No connection to close!" << std::endl;
    }
}

void Client::send_Data(const json &data) const {
    std::string data_str = data.dump();
    send(sock, data_str.c_str(), data_str.size(), 0);
}

json Client::receive_Data() const {
    char buffer[1024] = {0};
    const int valread = read(sock, buffer, 1024);
    return json::parse(std::string(buffer, valread));
}

void Client::show_Menu() {
    int choice;
    std::string server_address;
    int port;

    do {
        std::cout << "\nMain Menu:\n";
        std::cout << "1. Connect to Server\n";
        std::cout << "2. Close Connection\n";
        std::cout << "3. Send Data Menu\n";
        std::cout << "4. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Enter server IP address: ";
                std::cin >> server_address;
                std::cout << "Enter server port: ";
                std::cin >> port;
                connect_Server(server_address, port); // Connect to the server
                break;

            case 2: close_Connection(); break;
            case 3: show_Send_Data_Menu(); break;
            case 4:
                close_Connection(); // Close the connection
                std::cout << "Exiting..." << std::endl;
                return;

            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    } while (true);
}

void Client::show_Send_Data_Menu() {
    int choice;
    do {
        std::cout << "\nSend Data Menu:\n";
        std::cout << "1. Create Device\n";
        std::cout << "2. Get Device Status\n";
        std::cout << "3. Back to Main Menu\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1: create_device(); break;
            case 2: get_device_status(); break;
            case 3: return; // Back to main menu
            default:
                std::cout << "Invalid choice!" << std::endl;
        }
    } while (true);
}

void Client::create_device() {
    json json_request;
    json_request["action"] = "create_device";
    json_request["device_name"] = "Device_1";
    send_Data(json_request);
    json response = receive_Data();
    std::cout << "Server Response: " << response.dump() << std::endl;
}

void Client::get_device_status() {
    json json_request;
    json_request["action"] = "get_device_status";
    send_Data(json_request);
    json response = receive_Data();
    std::cout << "Server Response: " << response.dump() << std::endl;
}