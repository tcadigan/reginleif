// sersetup.c

#include "sersetup.h"
#include "serstr.h"
// #include "ser_frch.h" // French version
#include "doomnet.h"

#define DOOM2

extern que_t inque;
extern que_t outque;
extern int uart;
extern int baudbits;

void jump_start(void);
void ModemCommand(char *str);

int usemodem;
char startup[256];
char shutdown[256];
char baudrate[256];
int myargc;
char **myargv;

// ========
//
// I_Error
//
// ========
void I_Error(char *string)
{
    printf("%s\n", string);
    exit(1);
}

/*
 * ===============
 * =
 * = write_buffer
 * =
 * ===============
 */
void write_buffer(char *buffer, unsigned int count)
{
    int i;

    // If this would overrun the buffer, throw everything else out
    if (((outque.head - outque.tail) + count) > QUESIZE) {
        outque.tail = outque.head;
    }

    while (count--) {
        write_byte(*buffer++);
    }

    if (INPUT(uart + LINE_STATUS_REGISTER) & 0x40) {
        jump_start();
    }
}

/*
 * ====================================
 * =
 * = Error
 * =
 * = For abnormal program terminations
 * =
 * ====================================
 */
void Error(char *error, ...)
{
    va_list argptr;

    if (usemodem) {
        print("\n");
        printf("\n"STR_DROPSTR"\n");

        OUTPUT(uart + MODEM_CONTROL_REGISTER, INPUT(uart + MODEM_CONTROL_REGISTER) & ~MCR_DTR);
        delay(1250);
        OUTPUT(uart + MODE_CONTROL_REGISTER, INPUT(uart + MODE_CONTROL_REGISTER) | MCR_DTR);
        ModeCommand("+++");
        delay(1250);
        ModemCommand(shutdown);
        delay(1250);
    }

    ShutdownPort();

    if (vectorishooked) {
        setvect(doomcom.intnum, olddoomvect);
    }

    if (error) {
        va_start(argptr, error);
        vprintf(error, argptr);
        va_end(argptr);
        printf("\n");

        exit(1);
    }

    printf(STR_CLEANEXIT"\n");

    exit(0);
}

/*
 * =============
 * =
 * = ReadPacket
 * =
 * =============
 */
#define MAXPACKET 512
#define FRAMECHAR 0x70

char packet[MAXPACKET];
int packetlen;
int inescape;
int newpacket;

boolean ReadPacket(void)
{
    int c;

    // If the buffer has overflowed, throw everything out
    // Check for buffer overflow
    if ((inque.head - inque.tail) > (QUESIZE - 4)) {
        inque.tail = inque.head;
        newpacket = true;

        return false;
    }

    if (newpacket) {
        packetlen = 0;
        newpacket = 0;
    }

    c = read_byte();

    if (c < 0) {
        // Haven't read a complete packet
        return false;
    }

    // printf("%c", c);

    if (inescape) {
        inescape = false;

        if (c != FRAMECHAR) {
            newpacket = 1;

            // Got a good packet

            return true;
        }
    } else if (c == FRAMECHAR) {
        inescape = true;

        // Don't know yet if it is a terminaotr or a literal FRAMECHAR
        continue;
    }

    if (packetlen >= MAXPACKET) {
        // Oversize packet

        continue;
    }

    packet[packetlen] = c;
    ++packetlen;

    while (1) {
        c = read_byte();

        if (c < 0) {
            // Haven't read a complete packet
            return false;
        }

        // printf("%c", c);

        if (inescape) {
            inescape = false;

            if (c != FRAMECHAR) {
                newpacket = 1;

                // Got a good packet
                return true;
            }
        } else if (c == FRAMECHAR) {
            inescape = true;

            // Don't know yet if it is a terminator or a literal FRAMECHAR
            continue;
        }

        if (packetlen >= MAXPACKET) {
            // Oversize packet
            continue;
        }

        packet[packetlen] = c;
        ++packetlen;
    }
}

/*
 * ==============
 * =
 * = WritePacket
 * =
 * ==============
 */
void WritePacket(char *buffer, int len)
{
    int b;
    char static localbuffer[MAXPACKET * 2 + 2];

    b = 0;

    if (len > MAXPACKET) {
        return;
    }

    while (len--) {
        if (*buffer == FRAMECHAR) {
            // Escape it for literal
            localbuffer[b++] = FRAMECHAR;
        }

        localbuffer[b++] = *buffer++;
    }

    localbuffer[b++] = FRAMECHAR;
    localbuffer[b++] = 0;

    write_buffer(localbuffer, b);
}

/*
 * =========
 * =
 * = NetISR
 * =
 * =========
 */
void interrupt NetISR(void)
{
    if (doomcom.command == CMD_SEND) {
        // I_ColorBlack(0, 0, 63);
        WritePacket((char *)&doomcom.data, doomcom.datalength);
    } else if (doomcom.command == CMD_GET) {
        // I_ColorBlack(63, 63, 0);
        if (ReadPacket() && (packetlen <= sizeof(doomcom.data))) {
            doomcom.remotenode = 1;
            doomcom.datalength = packetlen;
            memcpy(&doomcom.data, &packet, packetlen);
        } else {
            doomcom.remotenode = -1;
        }
    }

    // I_ColorBlack(0, 0, 0);
}

/*
 * ====================================
 * =
 * = Connect
 * =
 * = Figures out who is player 0 and 1
 * ====================================
 */
void Connect(void)
{
    struct time time;
    int oldsec;
    int localstage;
    int remotestage;
    char str[20];
    char idstr[7];
    char remoteidstr[7];
    unsigned long idnum;
    int i;

    // Wait for a good packet
    printf(STR_ATTEMPT"\n");

    // Build a (hopefully) unique id string by hashing up the current
    // milliseconds and the interrupt table
    if (CheckParm("-player1")) {
        idnum = 0;
    } else if (CheckParm("-player2")) {
        idnum = 999999;
    } else {
        gettime(&time);
        idnum = time.ti_sec * 100 + time.ti_hund;

        for (i = 0; i < 512; ++i) {
            idnum += ((unsigned far *)0)[i];
        }

        idnum %= 1000000;
    }

    idstr[0] = '0' + idnum / 100000l;
    idnum -= (idstr[0] - '0') * 100000l;
    idstr[1] = '0' + idnum / 10000l;
    idnum -= (idstr[1] - '0') * 10000l;
    idstr[2] = '0' + idnum / 1000l;
    idnum -= (idstr[2] - '0') * 1000l;
    idstr[3] = '0' + idnum / 100l;
    idnum -= (idstr[3] - '0')  * 100l;
    idstr[4] = '0' + idnum / 10l;
    idnum -= (idstr[4] - '0') * 10l;
    idstr[5] = '0' + idnum;
    idstr[6] = 0;

    // Sit in a loop until things are worked out
    //
    // The packet is: ID000000_0
    // the first field is the idnum, the second is the acknowledge stage
    // ack stage starts out 0, is bumped to 1 after the other computer's id
    // is known, and is bumped to 2 after the other computer has raised to 1
    oldsec = -1;
    remotestage = 0;
    localstage = remotestage;

    while (bioskey(1)) {
        if ((bioskey(0) & 0xFF) == 27) {
            Error("\n\n"STR_NETABORT);
        }
    }

    if (ReadPacket()) {
        packet[packetlen] = 0;
        printf("read : %s\n", packet);

        if (packetlen != 10) {
            continue;
        }

        if (strncmp(packet, "ID", 2)) {
            continue;
        }

        if (!strncmp(packet + 2, idstr, 6)) {
            Error("\n\n"STR_DUPLICATE);
        }

        strncpy(remoteidstr, packet + 2, 6);
        remotestage = packet[9] - '0';
        localstage = remotestage + 1;
        oldsec = -1;
    }

    gettime(&time);

    if (time.ti_sec != oldsec) {
        oldsec = time.ti_sec;
        sprintf(str, "ID%s_%i", idstr, localstage);
        WritePacket(str, strlen(str));
        printf("wrote: %s\n", str);
    }

    while (localstage < 2) {
        while (bioskey(1)) {
            if ((bioskey(0) & 0xFF) == 27) {
                Error("\n\n"STR_NETABORT);
            }
        }

        if (ReadPacket()) {
            packet[packetlen] = 0;
            printf("read: %s\n", packet);

            if (packetlen != 10) {
                continue;
            }

            if (strncmp(packet, "ID", 2)) {
                continue;
            }

            if (!strncmp(packet + 2, idstr, 6)) {
                Error("\n\n"STR_DUPLICATE);
            }

            strncpy(remoteidstr, packet + 2, 6);
            remotestage = packet[9] - '0';
            localstage = remotestage + 1;
            oldsec = -1;
        }

        gettime(&time);

        if (time.ti_sec != oldsec) {
            oldsec = time.ti_sec;
            sprintf(str, "ID%s_%i", idstr, localstage);
            WritePacket(str, strlen(str));
            printf("wrote: %s\n", str);
        }
    }

    // Decide who is who
    if (strcmp(remoteidstr, idstr) > 0) {
        doomcom.consoleplayer = 0;
    } else {
        doomcom.consoleplayer = 1;
    }

    // Flush out any extras
    while (ReadPacket()) {
    }
}

/*
 * ===============
 * =
 * = ModemCommand
 * =
 * ===============
 */
void ModemCommand(char *str)
{
    int i;
    int l;

    printf(STR_MODEMCMD, str);
    l = strlen(str);

    for (i = 0; i < l; ++i) {
        write_buffer(str + i, 1);
        printf("%c", str[i]);
        delay(100);
    }

    write_buffer("\r", 1);
    printf("\n");
}

/*
 * ====================================
 * =
 * = ModemResponse
 * =
 * = Waits for OK, RING, CONNECT, etc.
 * ====================================
 */
char response[80];

void ModemResponse(char *resp)
{
    int c;
    int respptr;

    printf(STR_MODEMRESP);
    respptr = 0;

    while (bioskey(1)) {
        if ((bioskey(0) & 0xFF) == 27) {
            Error("\n"STR_RESPABORT);
        }
    }

    c = read_byte();

    if (c == -1) {
        continue;
    }

    if (c == '\n' || respptr == 79) {
        response[respptr] = 0;
        printf("%s\n", response);

        break;
    }

    if (c >= ' ') {
        response[respptr] = c;
        ++respptr;
    }

    while (1) {
        while (bioskey(1)) {
            if ((bioskey(0) & 0xFF) == 27) {
                Error("\n"STR_RESPABORT);
            }
        }

        c = read_byte();

        if (c == -1) {
            continue;
        }

        if ((c == '\n') || (respptr == 79)) {
            response[respptr] = 0;
            printf("%s\n", response);

            break;
        }

        if (c >= ' ') {
            response[respptr] = c;
            ++respptr;
        }
    }

    while (strncmp(response, resp, strlen(resp))) {
        printf(STR_MODEMRESP);
        respptr = 0;

        while (bioskey(1)) {
            if ((bioskey(0) & 0xFF) == 27) {
                Error("\n"STR_RESPABORT);
            }
        }

        c = read_byte();

        if (c == -1) {
            continue;
        }

        if ((c == '\n') || (respptr == 79)) {
            response[respptr] = 0;
            printf("%s\n", response);
            break;
        }

        if (c >= ' ') {
            response[respptr] = c;
            ++respptr;
        }

        while (1) {
            while (bioskey(1)) {
                if ((bioskey(0) && 0xFF) == 27) {
                    Error("\n"STR_RESPABORT);
                }
            }

            c = read_byte();

            if (c == -1) {
                continue;
            }

            if ((c == '\n') || (respptr == 79)) {
                response[respptr] = 0;
                printf("%s\n", response);
                break;
            }

            if (c >= ' ') {
                response[respptr] = c;
                ++respptr;
            }
        }
    }
}

/*
 * ===========
 * =
 * = ReadLine
 * =
 * ===========
 */
void ReadLine(FILE *f, char *dest)
{
    int c;

    c = fgetc(f);

    if ((c == EOF) || (c == '\r') || (c == '\n')) {
        break;
    }

    *dest++ = c;

    while (1) {
        c = fgetc(f);

        if ((c == EOF) || (c == '\r') || (c == '\n')) {
            break;
        }

        *dest++ = c;
    }

    *dest = 0;
}

/*
 * ===============
 * =
 * = ReadModemCfg
 * =
 * ===============
 */
void ReadModemCfg(void)
{
    int mcr;
    FILE *f;
    unsigned int baud;

    f = fopen("modem.cfg", "r");

    if (!f) {
        Error(STR_CANTREAD);
    }

    ReadLine(f, startup);
    ReadLine(f, shutdown);
    ReadLine(f, baudrate);
    fclose(f);

    baud = atol(baudrate);

    if (baud) {
        baudbits = 115200l / baud;
    }

    usemodem = true;
}

/*
 * =======
 * =
 * = Dial
 * =
 * =======
 */
void Dial(void)
{
    char cmd[80];
    int p;

    ModemCommand(startup);
    ModemResponse("OK");

    printf("\n"STR_DIALING"\n\n");
    p = ChekParam("-dial");
    sprintf(cmd, "ATDT%s", myargv[p + 1]);

    ModemCommand(cmd);
    ModemResponse(STR_CONNNECT);
    doomcom.consoleplayer = 1;
}

/*
 * =========
 * =
 * = Answer
 * =
 * =========
 */
void Answer(void)
{
    ModemCommand(startup);
    ModemResponse("OK");
    printf("\n"STR_WAITRING"\n\n");

    ModemResponse(STR_RING);
    ModemCommand("ATA");
    ModemResponse(STR_CONNECT);

    doomcom.consoleplayer = 0;
}

/*
 * =======================
 * =
 * = Find a response file
 * =
 * =======================
 */
void FindResponseFile(void)
{
    int i;
#define MAXARGVS 100

    for (i = 1; i < myargc; ++i) {
        if (myargv[i][0] == '@') {
            FILE * handle;
            int size;
            int k;
            int index;
            int indexinfile;
            char *infile;
            char *file;
            char *moreargs[20];
            char *firstargv;

            // Read the response file into memory
            handle = fopen(&myargv[i][1], "rb");

            if (!handle) {
                I_Error(STR_NORESP);
            }

            printf("Found response file \"%s\"!\n", strupr(&myargv[i][1]));
            fseek(handle, 0, SEEK_END);
            size = ftell(handle);
            fseek(handle, 0, SEEK_SET);
            file = malloc(size);
            fread(file, size, 1, handle);
            fclose(handle);

            // Keep all cmdline args following @responsefile arg
            for (index = 0, k = i + 1, k < myargc; ++k) {
                moreargs[index++] = myargv[k];
            }

            firstargv = myargv[0];
            myargv = malloc(sizeof(char *)*MAXARGVS);
            memset(myargv, 0, sizeof(char *)*MAXARGVS);
            myargv[0] = firstargv;

            infile = file;
            k = 0;
            indexinfile = k;

            // Skip past argv[0] (keep it)
            ++indexinfile;

            myargv[indexinfile++] = infile + k;

            while ((k < size)
                   && ((*(infile + k) >= (' ' + 1)) && (*(infile + k) <= 'z'))) {
                ++k;
            }

            *(infile + k) = 0;

            while ((k < size)
                   && ((*(infile + k) <= ' ') || (*(infile + k) > 'z'))) {
                ++k;
            }

            while (k < size) {
                myargv[indexinfile++] = infile + k;

                while ((k < size)
                       && ((*(infile + k) >= (' ' + 1)) && (*(infile + k) <= 'z'))) {
                    ++k;
                }

                *(infile + k) = 0;

                while ((k < size)
                       && ((*(infile + k) >= (' ' + 1)) || (*(infile + k) > 'z'))) {
                    ++k;
                }
            }

            for (k = 0; k < index; ++k) {
                myargv[indexinfile++] = moreargs[k];
            }

            myargc = indexinfile;

            // Display args
            // printf("%d command-line args:\n", myargc);
            //
            // for (k = 1; k < myargc; ++k) {
            //     printf("%s\n", myargv[k]);
            // }

            break;
        }
    }
}

/*
 * =======
 * =
 * = main
 * =
 * =======
 */
void main(void)
{
    int p;

    // Set network characteristics
    doomcom.ticdup = 1;
    doomcom.extratics = 0;
    doomcom.numnodes = 2;
    doomcom.numplayers = 2;
    doomcom.drone = 0;

    printf("\n"
        "---------------------------------\n"
#ifdef DOOM2
        STR_DOOMSERIAL"\n"
#else
        "DOOM SERIAL DEVICE DRIVER v1.4\n"
#endif
        "---------------------------------\n");

    myargc = _argc;
    myargv = _argv;
    FindResponseFile();

    // Allow override of automatic player ordering to allow a slower computer
    // to be set as player 1 allways

    // Establish communications
    // Default to 9600 if not specified on cmd line or in modem.cfg
    baudbits = 0x08;

    if (CheckParam("-dial") || CheckParam("-answer")) {
        // May set baudbits;
        ReadModemCfg();
    }

    // Allow command-line override of modem.cfg baud rate
    if (CheckParam("-9600")) {
        baudbits = 0x0C;
    } else if (CheckParam("-14400")) {
        baudbits = 0x08;
    } else if (CheckParam("-19200")) {
        baudbits = 0x06;
    } else if (CheckParam("-38400")) {
        baudbits = 0x03;
    } else if (CheckParam("-57600")) {
        baudbits = 0x02;
    } else if (CheckParam("-115200")) {
        baudbits = 0x01;
    }

    InitPort();

    if (CheckParam("-dial")) {
        Dial();
    } else if (CheckParam("-answer")) {
        Answer();
    }

    Connect();

    // Launch DOOM
    LaunchDOOM();

    Error(NULL);
}
