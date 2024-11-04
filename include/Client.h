#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <psdk_inc/_ip_types.h>
#include <winsock2.h>
#include <ws2tcpip.h>

class Client {
private:
    int sock;
    struct sockaddr_in server;
    bool is_connected;

public:
    Client();
    void connect_Server(const std::string &server_address, int port);
    void close_Connection();
    void send_Data(const nlohmann::json &data) const;
    nlohmann::json receive_Data() const;
    void show_Menu();
    void show_Send_Data_Menu();
    void create_device();
    void get_device_status();
};

#endif // CLIENT_H