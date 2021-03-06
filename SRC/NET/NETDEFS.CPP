#include "NET/NET.H"
#include "NETDEFS.H"
#include "CLASSES.H"

int IPXMSGLEN[] =
{
    1,  // END
    1,  // SERVERSEARCH
    20, // TEXTMESSAGE (text)
    13, // SERVERANSWER (name)
    1,  // SERVERSHUTDOWN
    1,  // JOINREQUEST
    2,  // JOINACCEPT (1/0)
    1,  // SENDLEVELINFO
    3,  // SENDLEVEL (short packetno)
    4+sizeof(General_level_info),  // LEVELINFO
    0, // TRANSMITLEVEL
    0, // GAMEDATA
    0, // IMJOININGNOW
    10,// PLYXY
    5, // PLYRANGLE
    1,  // ALIVE
    5, // PLYANIM
    2, // SENDCRATES
    0, // UPDCRATE
    0, // UPDKILLS
    0, //  NEWBULLET
    0, // Bööning
    0, // messsssssaaaaaaaaaaage
    0, // saund
    0, // NEWPLAYERDATA
    0, // PLAYERDISCONNECTED
    0, // SENDLEVELEFFECTS
    0, // LEVELEFFECTS
};
