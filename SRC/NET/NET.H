#ifndef __NET_H__
#define __NET_H__

#include <stddef.h>
#include "SERVERCONNECTRESULT.H"

#ifdef TK_PORT_EMCC
#include "SERVERINFO.H"
#endif

#define RECEIVERS 16 // Defines how many packets can be handled during a frame
#define MAXDATASIZE 1024
#define BROADCAST_ID 0xFF

#define word unsigned short

struct nodeaddr
{
  // Socket index is used to identify connections
  unsigned int node;
};

struct header
{
  struct nodeaddr src;
};

struct packet
{
  struct header header;
  char data[MAXDATASIZE];
};

namespace tk_port
{
namespace net
{
bool init();
void deinit();

#ifdef TK_PORT_EMCC
int connect_to_lobby();
ServerInfos get_server_list();
int update_server(const char *server_name, unsigned int player_amount,
                  unsigned int max_players, const char* password = NULL);
#endif

int create_server(const char *server_name, unsigned int max_players,
                  const unsigned int port, const char* password = NULL);

ServerConnectResult connect_server(const char* server_name, const unsigned int port,
                                   const char* password = NULL);

void close_connections();

void accept_connections();

bool active_connections_exist();

void disconnect(struct nodeaddr destnode);

bool receive(struct packet *buffer);
void sendpacket(struct nodeaddr destnode, struct packet *packetet, word len);

struct nodeaddr BROADCAST();
}
}

#endif
