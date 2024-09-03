#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#ifdef WITH_SSL
#include <websocketpp/config/asio_client.hpp>
#endif

class WebSocketClient {
public:
    WebSocketClient();
    void run(const std::string& uri);
    void send_message(const std::string& message);
    void close();
private:
    void on_open(websocketpp::connection_hdl h);
    void on_message(websocketpp::connection_hdl h, websocketpp::client<websocketpp::config::asio_client>::message_ptr msg);
    void on_close(websocketpp::connection_hdl h);
    void on_fail(websocketpp::connection_hdl h);

    websocketpp::client<websocketpp::config::asio_client> c;
    websocketpp::connection_hdl hdl;
};

#endif // WEBSOCKET_CLIENT_H



//src/websocket_client.cpp


#include "websocket_client.h"
#include <iostream>

WebSocketClient::WebSocketClient() {
    c.init_asio();
    c.set_open_handler(websocketpp::lib::bind(&WebSocketClient::on_open, this, websocketpp::lib::placeholders::_1));
    c.set_message_handler(websocketpp::lib::bind(&WebSocketClient::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
    c.set_close_handler(websocketpp::lib::bind(&WebSocketClient::on_close, this, websocketpp::lib::placeholders::_1));
    c.set_fail_handler(websocketpp::lib::bind(&WebSocketClient::on_fail, this, websocketpp::lib::placeholders::_1));
}

void WebSocketClient::run(const std::string& uri) {
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(uri, ec);

    if (ec) {
        std::cout << "Could not create connection because: " << ec.message() << std::endl;
        return;
    }

    hdl = con->get_handle();
    c.connect(con);
    c.run();
}

void WebSocketClient::send_message(const std::string& message) {
    websocketpp::lib::error_code ec;
    c.send(hdl, message, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cout << "Error sending message: " << ec.message() << std::endl;
    }
}

void WebSocketClient::close() {
    websocketpp::lib::error_code ec;
    c.close(hdl, websocketpp::close::status::normal, "Client closed connection", ec);
    if (ec) {
        std::cout << "Error closing connection: " << ec.message() << std::endl;
    }
}

void WebSocketClient::on_open(websocketpp::connection_hdl h) {
    std::cout << "Connection opened!" << std::endl;
}

void WebSocketClient::on_message(websocketpp::connection_hdl h, websocketpp::client<websocketpp::config::asio_client>::message_ptr msg) {
    std::cout << "Received: " << msg->get_payload() << std::endl;
}

void WebSocketClient::on_close(websocketpp::connection_hdl h) {
    std::cout << "Connection closed!" << std::endl;
}

void WebSocketClient::on_fail(websocketpp::connection_hdl h) {
    std::cout << "Connection failed!" << std::endl;
}


//src/main.cpp

#include "websocket_client.h"
#include <iostream>

int main() {
    WebSocketClient ws_client;

    std::string uri = "wss://echo.websocket.events/.ws";

    std::cout << "Connecting to " << uri << "..." << std::endl;
    ws_client.run(uri);

    std::string input;
    while (true) {
        std::cout << "Enter a message to send (or type 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            ws_client.close();
            break;
        }

        ws_client.send_message(input);
    }

    return 0;
}


//Unit Tests (tests/websocket_client_test.cpp)

#include "websocket_client.h"
#include <gtest/gtest.h>

TEST(WebSocketClientTest, ConnectionTest) {
    WebSocketClient ws_client;
    std::string uri = "wss://echo.websocket.events/.ws";
    ASSERT_NO_THROW(ws_client.run(uri));
}

TEST(WebSocketClientTest, MessageTest) {
    WebSocketClient ws_client;
    std::string uri = "wss://echo.websocket.events/.ws";
    ws_client.run(uri);

    ASSERT_NO_THROW(ws_client.send_message("Hello, WebSocket!"));
    ws_client.close();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



