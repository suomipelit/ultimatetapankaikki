#include "NET/TRANSPORT/TRANSPORT.H"

#include "SDL_net.h"

#include <algorithm>

namespace tk_port
{
namespace net
{
namespace transport
{

namespace
{
static TCPsocket server_socket;
static SDLNet_SocketSet set;
static std::vector<TCPsocket> sockets;
static size_t max_buffer_size;
}

bool init(unsigned int max_connections, size_t max_packet_size)
{
    sockets.resize(max_connections, nullptr);

    if (SDLNet_Init() == -1)
    {
        printf("Error initializing net API: %s\n", SDLNet_GetError());
        return false;
    }

    set = SDLNet_AllocSocketSet(max_connections);

    if (!set)
    {
        printf("Error allocating socket set: %s\n", SDLNet_GetError());
        return false;
    }

    max_buffer_size = max_packet_size;

    return true;
}

void deinit()
{
    if (set)
    {
        SDLNet_FreeSocketSet(set);
        set = NULL;
    }

    SDLNet_Quit();
}

void broadcast(const Buffer& buffer, DisconnectCallback disconnectedCb)
{
    for (unsigned int i = 0; i < sockets.size(); ++i)
    {
        if (!send(i, buffer) && disconnectedCb)
        {
            disconnectedCb(i);
        }
    }
}

bool send(Identifier identifier, const Buffer& buffer)
{
    if (sockets[identifier])
    {
        if ((size_t)SDLNet_TCP_Send(sockets[identifier], buffer.data(), (int)buffer.size()) != buffer.size())
        {
            return false;
        }
    }

    return true;
}

int server::create_server(const char *server_name, unsigned int max_players, Port port, const char*)
{
    IPaddress ip;

    if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
    {
        printf("Error resolving host: %s\n", SDLNet_GetError());
        return 1;
    }

    server_socket = SDLNet_TCP_Open(&ip);

    if (server_socket == NULL)
    {
        printf("Error opening socket: %s\n", SDLNet_GetError());
        return 2;
    }

    return 0;
}

bool is_server()
{
    return server_socket != nullptr;
}

ServerConnectResult client::connect_to_server(const char* server_address, Port port, const char*)
{
    IPaddress ip;

    if (SDLNet_ResolveHost(&ip, server_address, port) == -1)
    {
        printf("Error resolving host: %s\n", SDLNet_GetError());
        return ServerConnectResult::UNKNOWN_ERROR;
    }

    TCPsocket socket = SDLNet_TCP_Open(&ip);

    if (!socket)
    {
        printf("Error opening socket: %s\n", SDLNet_GetError());
        return ServerConnectResult::UNKNOWN_ERROR;
    }

    // Client always has just one socket to server
    sockets[0] = socket;
    SDLNet_TCP_AddSocket(set, socket);

    return ServerConnectResult::SUCCESS;
}

void close_connections()
{
    if (server_socket)
    {
        SDLNet_TCP_Close(server_socket);
        server_socket = NULL;
    }

    for (auto socket : sockets)
    {
        if (socket != nullptr)
        {
            SDLNet_TCP_Close(socket);
            SDLNet_TCP_DelSocket(set, socket);
        }
    }

    std::fill(sockets.begin(), sockets.end(), nullptr);
}

void server::accept_connections()
{
    TCPsocket newClient = SDLNet_TCP_Accept(server_socket);

    if (newClient)
    {
        auto empty_slot = std::find(sockets.begin(), sockets.end(), nullptr);

        if (empty_slot != sockets.end())
        {
            *empty_slot = newClient;
            SDLNet_TCP_AddSocket(set, newClient);
        }
        else
        {
            SDLNet_TCP_Close(newClient);
            printf("Out of sockets\n");
        }
    }
}

bool is_connected()
{
    const auto doesSocketExist = [](const TCPsocket& socket) {
        return socket != nullptr;
    };

    // Find if there exists one or more active sockets
    return std::find_if(sockets.begin(), sockets.end(), doesSocketExist) != sockets.end();
}

void disconnect(Identifier index)
{
    SDLNet_TCP_Close(sockets[index]);
    SDLNet_TCP_DelSocket(set, sockets[index]);
    sockets[index] = nullptr;
}

Received receive(Identifier index)
{
    if (index >= sockets.size() || sockets[index] == nullptr) return { NOT_CONNECTED };

    if (SDLNet_CheckSockets(set, 0))
    {
        if (SDLNet_SocketReady(sockets[index]))
        {
            Buffer buffer(max_buffer_size);
            const int len = SDLNet_TCP_Recv(sockets[index], buffer.data(), (int)buffer.size());

            if (len > 0)
            {
                buffer.resize(len);
                return { NEW_DATA, std::move(buffer) };
            }
            else
            {
                return { DISCONNECTED };
            }
        }
    }

    return { NO_DATA };
}
}
}
}
