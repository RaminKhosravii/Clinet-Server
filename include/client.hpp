#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json; // Alias for JSON library

// Function to connect to the server
void connectToServer(const std::string& serverIP, int port);

// Function to send a request to the server
void sendRequest(const json& request);

// Function to receive a response from the server
json receiveResponse();

#endif // CLIENT_H