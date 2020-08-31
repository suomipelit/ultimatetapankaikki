#ifndef __SERVERINFO_H__
#define __SERVERINFO_H__

#include <stdint.h>
#include <string>
#include <vector>

namespace tk_port
{
namespace net
{
struct ServerInfo
{
  ServerInfo(const char* serialized);
  bool operator==(const ServerInfo& rhs) const;
  bool operator!=(const ServerInfo& rhs) const;

  std::string gameId;
  std::string serverName;
  uint8_t playerAmount;
  uint8_t maxPlayers;
  bool requiresPassword;
};

using ServerInfos = std::vector<ServerInfo>;
}
}

#endif
