#include "NET/TRANSPORT/TRANSPORT.H"
#include <cstdio>
#include <CONFIG.H>

// Implemented in TRANSPORT.JS
extern "C" {
    extern int webrtc_init(const char *lobby_host, unsigned int max_peers);
    extern void webrtc_deinit();
    extern int webrtc_create_server();
    extern int webrtc_is_server();
    extern int webrtc_connect_to_server(const char *game_id);
    extern int webrtc_is_connected();
    extern int webrtc_has_peer(unsigned int peer_index);
    extern void webrtc_reset();
    extern int webrtc_send(unsigned int index, const char *buffer, int buffer_size);
    extern int webrtc_receive(unsigned int index, char *buffer, int buffer_size);
}

namespace tk_port
{
namespace net
{
namespace transport
{

namespace
{
    static size_t max_peers;
    static size_t max_buffer_size;
}

bool init(unsigned int max_connections, size_t max_packet_size)
{
    max_peers = max_connections;
    max_buffer_size = max_packet_size;
    return webrtc_init(TK_LOBBY_HOST, max_connections);
}

void deinit()
{
    webrtc_deinit();
}

void broadcast(const Buffer& buffer, DisconnectCallback disconnectedCb)
{
    for (size_t i = 0; i < max_peers; ++i)
    {
        if (!send(i, buffer) && disconnectedCb)
        {
            disconnectedCb(i);
        }
    }
}

bool send(Identifier identifier, const Buffer& buffer)
{
    if (identifier >= max_peers) {
        return false;
    }

    if (webrtc_send(identifier, buffer.data(), (int)buffer.size()) != (int)buffer.size()) {
        return false;
    }

    return true;
}

int server::create_server(Port port)
{
    return webrtc_create_server();
}

bool is_server()
{
    return webrtc_is_server();
}

int client::connect_to_server(const char* server_address, Port port)
{
    return webrtc_connect_to_server(server_address);
}

void close_connections()
{
    webrtc_reset();
}

void server::accept_connections()
{
}

bool is_connected()
{
    return is_server() || webrtc_is_connected();
}

void disconnect(Identifier index)
{
}

Received receive(Identifier index)
{
    if (index >= max_peers || !webrtc_has_peer(index)) return { NOT_CONNECTED };

    Buffer buffer(max_buffer_size);
    const int len = webrtc_receive(index, buffer.data(), (int)buffer.size());

    if (len > 0)
    {
        buffer.resize(len);
        return { NEW_DATA, std::move(buffer) };
    }
    else if (len == 0)
    {
        return { NO_DATA };
    }
    else
    {
        return { DISCONNECTED };
    }
}
}
}
}
