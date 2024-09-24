#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json; // Alias for JSON library

// Structure to represent a device
struct Device {
    std::string id;
    std::string status;
};

// Function to handle incoming requests
json handleRequest(const json& request);

// Function to start the server
void startServer();

#endif // SERVER_H