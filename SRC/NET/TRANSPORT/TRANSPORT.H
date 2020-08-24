#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include <functional>
#include <stdint.h>
#include <vector>

namespace tk_port
{
namespace net
{
namespace transport
{
using Identifier = const unsigned int;

using Buffer = std::vector<char>;

enum ReceiveResult
{
    NOT_CONNECTED,
    NO_DATA,
    NEW_DATA,
    DISCONNECTED
};

struct Received
{
    const ReceiveResult status;
    const Buffer buffer;
};

using DisconnectCallback = std::function<void(Identifier index)>;

using Port = const uint16_t;

bool init(unsigned int max_connections, size_t max_packet_size);

void deinit();

bool is_server();

void close_connections();

bool is_connected();

void disconnect(Identifier index);

void broadcast(const Buffer& buffer, DisconnectCallback disconnectedCb);

bool send(Identifier index, const Buffer& buffer);

Received receive(Identifier index);

namespace server
{
int create_server(const char *server_name, unsigned int max_players, Port port);

void accept_connections();
}

namespace client
{
int connect_to_server(const char* server_address, Port port);
}
}
}
}

#endif
