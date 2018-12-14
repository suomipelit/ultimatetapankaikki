#ifndef __NET_H__
#define __NET_H__

#define RECEIVERS MAX_PLAYERS
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

namespace Net
{
bool init();
void deinit();

int create_server(const unsigned int port);
int connect_server(const char*, const unsigned int port);

void close_connections();

void accept_connections();

bool receive(struct packet *buffer, const unsigned int index);
void sendpacket(struct nodeaddr destnode, struct packet *packetet, word len);

struct nodeaddr BROADCAST();
}

#endif