// doomnet.h

#define PEL_WRITE_ADR 0x3C8
#define PEL_DATA      0x3C9

#define I_ColorBlack(r, g, b)                   \
    {                                           \
        outp(PEL_WRITE_ADR, 0);                 \
        outp(PEL_DATA, r);                      \
        outp(PEL_DATA, g);                      \
        outp(PEL_DATA, b);                      \
    };                                          \

#define MAXNETNODES 8 // Max computers in a game
#define MAXPLAYERS  4 // 4 players max + drones

#define CMD_SEND 1
#define CMD_GET 2

#define DOOMCOM_ID 0x12345678l

typedef struct {
    long id;
    short intnum; // DOOM executes an int to send commands

    // Communication between DOOM and the driver
    short command;    // CMD_SEND or CMD_GET
    short remotenode; // Dest for send, set by get (-1 = no packet)
    short datalength; // Bytes in doomdata to be sent / bytes read

    // Info common to all nodes
    short numnodes;   // Console is always node 0
    short ticdup;     // 1 = No duplication, 2-5 = Duplication for slow networks
    short extratics;  // 1 = Send a bacup tic in every packet
    short deathmatch; // 1 = Deathmatch
    short savegame;   // -1 = New game, 0-5 = Load game
    short episode;    // 1-3
    short map;        // 1-9
    short sill;       // 1-5

    /// Info specific to this node
    short consoleplayer; // 0-3 = Player number
    short numplayers;    // 1-4
    short angleoffset;   // 1 = Left, 0 = Center, -1 = Right
    short drone;         // 1 = Drone

    // Packet data to be sent
    char data[512];
} doomcom_t;

extern doomcom_t doomcom;
extern void interrupt (*olddoomvect) (void);
extern int vectorishooked;

int ChecParm(char *check);
void LaunchDOOM(void);
void interrupt NetISR(void);
