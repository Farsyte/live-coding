#include "common/cdev.h"
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

typedef void       *pthread_fn(void *);

static pthread_fn   cdev_service;

static void         cdev_rdd(Cdev d);
static void         cdev_rdc(Cdev d);
static void         cdev_wrd(Cdev d);
static void         cdev_wrc(Cdev d);

// TODO add comments

void cdev_init(Cdev d, Cstr name)
{
    assert(d);
    assert(name);
    assert(name[0]);

    d->name = name;
    bring_init(d->rx, format("%s:RX", name));
    bring_init(d->tx, format("%s:TX", name));

    edge_init(d->rd[0], format("%s:/RDD", name), 1);
    edge_init(d->rd[1], format("%s:/RDC", name), 1);
    edge_init(d->wr[0], format("%s:/WRD", name), 1);
    edge_init(d->wr[1], format("%s:/WRC", name), 1);

    d->DATA = NULL;

    d->conn = -1;
}

void cdev_close(Cdev d)
{
    int                 conn = d->conn;
    if (conn >= 0) {
        d->conn = -1;

        void               *rv[1];
        rv[0] = NULL;
        pthread_join(d->thread, rv);
        close(conn);
    }
}

void cdev_invar(Cdev d)
{
    assert(d);
    assert(d->name);
    assert(d->name[0]);

    bring_invar(d->rx);
    bring_invar(d->tx);

    edge_invar(d->rd[0]);
    edge_invar(d->rd[1]);
    edge_invar(d->wr[0]);
    edge_invar(d->wr[1]);

    assert(NULL != d->DATA);
}

void cdev_linked(Cdev d)
{
    cdev_invar(d);

    EDGE_ON_FALL(d->rd[0], cdev_rdd, d);
    EDGE_ON_FALL(d->rd[1], cdev_rdc, d);
    EDGE_ON_FALL(d->wr[0], cdev_wrd, d);
    EDGE_ON_FALL(d->wr[1], cdev_wrc, d);
}

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

static void cdev_rdd(Cdev d)
{
    cdev_invar(d);

    if (cdev_can_rx(d)) {
        Byte                b = cdev_rx(d);
        data_to(d->DATA, b);
    } else {
        data_to(d->DATA, 0xFF);
    }
}
static void cdev_rdc(Cdev d)
{
    cdev_invar(d);

    Byte                b = 0;
    if (d->conn >= 0)
        b |= CDEV_STATUS_CONN;
    if (cdev_can_rx(d))
        b |= CDEV_STATUS_RXRDY;
    if (cdev_can_tx(d))
        b |= CDEV_STATUS_TXRDY;
    data_to(d->DATA, b);
}
static void cdev_wrd(Cdev d)
{
    cdev_invar(d);

    if (cdev_can_tx(d)) {
        Byte                b = d->DATA->value;
        cdev_tx(d, b);
    }
}
static void cdev_wrc(Cdev d)
{
    cdev_invar(d);

    Byte                b = d->DATA->value;
    STUB("wr ctrl %02X --> UNDEFINED BEHAVIOR", b);
}

int cdev_can_rx(Cdev d)
{
    cdev_invar(d);

    return !bring_empty(d->rx);
}

Byte cdev_rx(Cdev d)
{
    cdev_invar(d);

    assert(cdev_can_rx(d));
    return bring_cons(d->rx);
}

int cdev_can_tx(Cdev d)
{
    cdev_invar(d);

    return !bring_full(d->tx);
}

void cdev_tx(Cdev d, Byte b)
{
    cdev_invar(d);

    assert(cdev_can_tx(d));
    bring_prod(d->tx, b);
}

void cdev_conn(Cdev d, int tcp_port)
{
    cdev_invar(d);

    assert(tcp_port > 0);
    assert(tcp_port < 65536);

    FIN(conn, socket, AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in  sa[1];
    memset(sa, 0, sizeof(sa));
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = inet_addr("127.0.0.1");
    sa->sin_port = htons(tcp_port);

    FIN(connected, connect, conn, (struct sockaddr *)sa, sizeof(sa));

    d->port = tcp_port;
    d->conn = conn;     // turn the connection over to the cdev

    FIN(started, pthread_create, &d->thread, NULL, (pthread_fn *) cdev_service, (void *)d);
}

static void        *cdev_service(void *a)
{
    fd_set              readfds[1];
    fd_set              writefds[1];
    struct timeval      tv[1];
    Byte                cb[1];
    int                 conn;

    volatile pCdev      d = (pCdev)a;

    for (;;) {
        conn = d->conn;
        if (conn < 0)
            return NULL;

        int                 nfds = conn + 1;

        FD_ZERO(readfds);
        if (!bring_full(d->rx))
            FD_SET(conn, readfds);

        FD_ZERO(writefds);
        if (!bring_empty(d->tx))
            FD_SET(conn, writefds);

        tv->tv_sec = 0;
        tv->tv_usec = 1000;

        FIN(sel, select, nfds, readfds, writefds, NULL, tv);

        if ((sel > 0) && FD_ISSET(conn, readfds)) {

            FIN(rs, read, conn, cb, sizeof cb);
            if (rs < 1) {
                STUB("cdev %s ended: read(%d) returned %d", d->name, conn, rs);
                break;
            }

            bring_prod(d->rx, cb[0]);
            sel--;
        }

        if ((sel > 0) && FD_ISSET(conn, writefds)) {

            cb[0] = bring_cons(d->tx);

            FIN(ws, write, conn, cb, sizeof cb);
            if (ws < 1) {
                STUB("cdev %s ended: write(%d) returned %d", d->name, conn, ws);
                break;
            }
            sel--;
        }
    }
    d->conn = -1;
    return 0;
}
