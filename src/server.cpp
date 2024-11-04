#include <Server.h>

using json = nlohmann::json;

Server::Server(const int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create socket for server
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) { // Set socket options
        perror("Set socket options failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started, waiting for connections..." << std::endl;
    start_Accept(); // Start accepting client connections
}

void Server::start_Accept() {
    new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address), (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    handle_Client(); // Handle communication with the client
}

void Server::handle_Client() const {
    while (true) {
        char buffer[1024] = {0};
        const int valread = read(new_socket, buffer, 1024);

        if (valread <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break; // Exit loop
        }

        json message = json::parse(std::string(buffer, valread));
        std::cout << "Received: " << message.dump() << std::endl;

        json response; // Prepare a response message based on the action
        if (message["action"] == "create_device") {
            response["response"] = "Device created successfully!";
        } else if (message["action"] == "get_device_status") {
            response["response"] = "Device is operational.";
        } else {
            response["response"] = "Unknown action.";
        }

        send(new_socket, response.dump().c_str(), response.dump().size(), 0);
    }
    close(new_socket); // Close the connection with the client
}
