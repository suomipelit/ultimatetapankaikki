#include "SERVERINFO.H"

#include <string.h>

namespace tk_port
{
namespace net
{
ServerInfo::ServerInfo(const char* serialized)
{
    char* copy = strdup(serialized);
    const char* delimiter = ";";
    char *end_token;
    size_t context = 0;
    char* split_ptr = strtok_r(copy, delimiter, &end_token);

    while (split_ptr != NULL)
    {
        switch (context++)
        {
            case 0:
            gameId = split_ptr;
            break;

            case 1:
            serverName = split_ptr;
            break;

            case 2:
            playerAmount = strtol(split_ptr, NULL, 10);
            break;

            case 3:
            maxPlayers = strtol(split_ptr, NULL, 10);;
            break;

            default:
            break;
        }

        split_ptr = strtok_r(NULL, delimiter, &end_token);
    }
    free(copy);
}

bool ServerInfo::operator==(const ServerInfo& rhs) const
{
    return gameId == rhs.gameId &&
        serverName == rhs.serverName &&
        playerAmount == rhs.playerAmount &&
        maxPlayers == rhs.maxPlayers;
}

bool ServerInfo::operator!=(const ServerInfo& rhs) const
{
    return !operator==(rhs);
}
}
}
