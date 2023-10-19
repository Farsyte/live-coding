#pragma once
#include "common/bring.h"
#include "common/support.h"
#include "common/data.h"
#include "common/edge.h"

#define CDEV_SILO_CAP		BRING_CAP

#define CDEV_STATUS_CONN	0x01
#define CDEV_STATUS_RXRDY	0x02
#define CDEV_STATUS_TXRDY	0x04

typedef struct sCdev {
    Cstr                name;
    Bring               rx;
    Bring               tx;

    // cdev is an I/O device that has the following ports:
    //
    // RD[0] - read the data port.
    //    returns the next byte of data from the input ring,
    //    or 0xFF if the input ring is empty.
    // WR[0] - write the data port.
    //    stores the data to the output ring if it is not empty.
    //
    // RD[1] - read the status port.
    //    bit 0: true if the device is connected
    //    bit 1: true if the rx ring has data
    //    bit 2: true if the tx ring can accept data
    // WR[1] - write the control port.
    //    data is a command byte
    //    00: disconnect (UNIMP)
    //    01: reconnect (UNIMP)
    //    behavior is undefined for all commands not matched above.

    Edge                rd[2];                  // read strobes, active low
    Edge                wr[2];                  // write strobes, active low

    pData               DATA;                   // system data bus

    // state information for the service thread.
    int                 port;                   // TCP port number of server
    int                 conn;                   // data socket
    pthread_t           thread;                 // service thread

}                  *pCdev, Cdev[1];

extern void         cdev_init(Cdev, Cstr name);
extern void         cdev_linked(Cdev);

extern void         cdev_close(Cdev);

extern void         cdev_conn(Cdev, int tcp_port);
extern void         cdev_invar(Cdev);

extern int          cdev_can_rx(Cdev);
extern Byte         cdev_rx(Cdev);
extern int          cdev_can_tx(Cdev);
extern void         cdev_tx(Cdev, Byte);

extern void         cdev_bist();

// This test is not self contained: the TTY server must be running.
// This test is not automated: operator must type and observe at the TTY server.
extern int          cdev_test(int argc, char **argv);
