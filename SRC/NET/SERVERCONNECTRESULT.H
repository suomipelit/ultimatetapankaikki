#ifndef __SERVERCONNECTRESULT_H__
#define __SERVERCONNECTRESULT_H__

namespace tk_port
{
namespace net
{
enum ServerConnectResult
{
    SUCCESS = 0,
    UNKNOWN_ERROR = 1,
    UNKNOWN_RESPONSE = 2,
    UNKOWN_REJECTION_REASON = 3,
    GAME_IS_FULL = 4,
    WRONG_PASSWORD = 5,
};
}
}

#endif
