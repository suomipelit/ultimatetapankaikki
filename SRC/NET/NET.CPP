#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NET.H"
#include "../ERROR/ERROR.H"
#include "NETDEFS.H"
#include "DEFINES.H"
#include "TRANSPORT/TRANSPORT.H"
#include "PORT.H"

#include <algorithm>
#include <vector>

namespace
{
// Add few extra storages in case clients try to join when server is full. Game
// logic rejects such join attempts
static const int NUMBER_OF_STORAGES = MAX_PLAYERS + 3;
static std::vector<std::vector<char>> storages(NUMBER_OF_STORAGES);

static const int PACKET_HEADER_SIZE = 2; // 2 length bytes (16-bit) at the beginning
static const int PACKET_CHECKSUM_SIZE = 1; // 1 checksum byte at the and
static const int PACKET_OVERHEAD_SIZE = PACKET_HEADER_SIZE + PACKET_CHECKSUM_SIZE;
}

namespace tk_port
{
namespace net
{

bool init()
{
    return transport::init(NUMBER_OF_STORAGES, MAXDATASIZE);
}

void deinit()
{
    transport::deinit();
}

int create_server(const char *server_name, unsigned int max_players, const unsigned int port)
{
    return transport::server::create_server(server_name, max_players, port);
}

int connect_server(const char* server_address, const unsigned int port)
{
    return transport::client::connect_to_server(server_address, port);
}

void close_connections()
{
    for (auto& storage : storages)
    {
        storage.clear();
    }

    return transport::close_connections();
}

void accept_connections()
{
    return transport::server::accept_connections();
}

bool active_connections_exist()
{
    return transport::is_connected();
}

void disconnect(struct nodeaddr destnode)
{
    // Disconnect is not propagated to game logic but
    // server relies on ALIVE messages. It then propagates
    // disconnects to clients with PLAYERDISCONNECTED message.

    storages[destnode.node].clear();

    return transport::disconnect(destnode.node);
}

// Very simple checksum for packet sanity check
static char checksum(const char* data, word len)
{
    char c = (char)0xAA;
    for (int i = 0; i < len; ++i)
    {
        c ^= data[i];
    }
    return c;
}

static std::vector<char> get_packet_envelope(const char* data, word len)
{
    std::vector<char> packet;
    packet.push_back((char)(len >> 8));
    packet.push_back((char)(len));
    for (int i = 0; i < len; ++i)
    {
        packet.push_back(data[i]);
    }
    packet.push_back(checksum(data, len));
    return packet;
}

static int get_payload_length(const char* data)
{
    return ((int)data[0] << 8) + (data[1] & 0xFF);
}

static void receive_to_storage(const unsigned int index)
{
    const auto received = transport::receive(index);

    switch (received.status)
    {
    case transport::NEW_DATA:
    {
        auto& storage = storages[index];
        storage.insert(storage.end(), received.buffer.begin(), received.buffer.end());
        break;
    }
    case transport::DISCONNECTED:
        disconnect({ index });
        break;
    default:
        break;
    }
}

static bool find_next_packet_from_storage(struct packet *buffer)
{
    // Clients always have just one socket to server
    for (unsigned int i = 0; i < (transport::is_server() ? storages.size() : 1); ++i)
    {
        auto& storage = storages[i];

        // Check if there is enough data for packet consisting
        // of 1 or more payload bytes
        if (storage.size() >= PACKET_OVERHEAD_SIZE + 1)
        {
            const unsigned int payload_length = get_payload_length(storage.data());

            if (storage.size() < (payload_length + PACKET_OVERHEAD_SIZE))
            {
                // Not enough data in the storage, i.e. packet not yet fully received
                continue;
            }

            const char received_checksum = storage.data()[PACKET_HEADER_SIZE + payload_length];
            const char calculated_checksum = checksum(storage.data() + PACKET_HEADER_SIZE, payload_length);

            if (received_checksum == calculated_checksum)
            {
                // Packet fully received, return it to the app
                memcpy(buffer->data, storage.data() + PACKET_HEADER_SIZE, payload_length);
                buffer->header.src.node = i;

                // Remove packet from storage
                storage.erase(storage.begin(), storage.begin() + payload_length + PACKET_OVERHEAD_SIZE);

                return true;
            }
            else
            {
                warn( "Corrupted packet\n" );
                storage.clear();
            }
        }
    }

    return false;
}

static unsigned int get_next_storage_to_receive()
{
    if (transport::is_server())
    {
        static unsigned int storage_index = 0;
        // Loops 0 to NUMBER_OF_STORAGES - 1
        return storage_index++ % NUMBER_OF_STORAGES;
    }
    else
    {
        return 0;
    }
}

bool receive(struct packet *buffer)
{
    // Receive data evenly from sockets and store any received
    // data to storage
    receive_to_storage(get_next_storage_to_receive());

    // Look through the socket storages to see if there are
    // any fully received packets. Return one if finding
    // any.
    return find_next_packet_from_storage(buffer);
}

struct nodeaddr BROADCAST()
{
    return nodeaddr{ BROADCAST_ID };
}

void sendpacket(struct nodeaddr destnode, struct packet *packetet, word len)
{
    const int datalength = len - 1;
    auto packet = get_packet_envelope(packetet->data, datalength);

    if (destnode.node == BROADCAST_ID)
    {
        transport::broadcast(packet, [](transport::Identifier index) { disconnect({ index }); });
    }
    else if (!transport::send(destnode.node, packet))
    {
        disconnect(destnode);
    }
}
}
}
