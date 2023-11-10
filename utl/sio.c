#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// NAME
//     sio -- serial i/o device server
//
// SYNOPSIS
//     sio port
//
// ARGUMENTS
//     port    -- TCP port number to service
//
// DESCRIPTION
//     This program accepts a single TCP client on the specfied port,
//     then copies any data from the network to stdout and any data
//     from stdin to the network port.
//
//     If any error occurs, a message is printed to stdout and the
//     server terminates.

// FIN macro: handle the most common error reporting mechanism. Used
// for functions that report errors by returning a negative value
// (with errno set), while zero and positive are success. This macro
// reports errors and aborts, and should be used when recovery from
// the function failing is not appropriate.

#define FIN(var, fn, ...)                                               \
    int var = fn(__VA_ARGS__);                                          \
    if (var >= 0) { } else { perror(#fn); abort(); }

int main(int argc, char **argv)
{
    assert(argc == 2);
    const int           port = atoi(argv[1]);

    // Create the socket
    FIN(server, socket, AF_INET, SOCK_STREAM, 0);

    // Bind the socket to the server port
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);       // htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    FIN(bind_rv, bind, server, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Allow our listening port to be re-used immediately after we
    // close it.
    int                 enable = 1;
    FIN(opt_rv, setsockopt, server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

    // Listen for a client to connect to us. While we only ever accept
    // one connection, it is not likely that reducing the backlog
    // capacity here would have any benefit.
    const int           backlog = 10;
    FIN(rv, listen, server, backlog);

    fprintf(stderr, "sio ready to connect on port %d\r\n", port);

    // Accept connection from our client.
    struct sockaddr_in  client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t           len = sizeof(client_addr);
    FIN(conn, accept, server, (struct sockaddr *)&client_addr, &len);

    // socklen_t           sz = sizeof(client_addr);
    // FIN(rv, getsockname, conn, (struct sockaddr *)&client_addr, &sz);

    fprintf(stderr, "accepted connection on port %d from port %d\r\n",
            port, ntohs(client_addr.sin_port));

    // in parallel:
    //   copy data from stdin to conn
    //   copy data from conn to stdout

    char                stdin_to_conn[1];
    int                 stdin_to_conn_pend = 0;

    char                conn_to_stdout[1];
    int                 conn_to_stdout_pend = 0;

    while (1) {

        int                 nfds = conn + 1;

        fd_set              readfds[1];
        FD_ZERO(readfds);
        if (!stdin_to_conn_pend)
            FD_SET(0, readfds);
        if (!conn_to_stdout_pend)
            FD_SET(conn, readfds);

        fd_set              writefds[1];
        FD_ZERO(writefds);
        if (conn_to_stdout_pend)
            FD_SET(1, writefds);
        if (stdin_to_conn_pend)
            FD_SET(conn, writefds);

        struct timeval      tv[1];
        tv->tv_sec = 0;
        tv->tv_usec = 1000;

        FIN(sel, select, nfds, readfds, writefds, NULL, tv);

        if ((sel > 0) && FD_ISSET(0, readfds)) {
            FIN(rsz, read, 0, stdin_to_conn, 1);
            if (rsz < 1)
                break;
            stdin_to_conn_pend = 1;
        }

        if ((sel > 0) && FD_ISSET(conn, readfds)) {
            FIN(rsz, read, conn, conn_to_stdout, 1);
            if (rsz < 1)
                break;
            conn_to_stdout_pend = 1;
        }

        if ((sel > 0) && FD_ISSET(1, writefds)) {
            FIN(wsz, write, 1, conn_to_stdout, 1);
            if (wsz < 1)
                break;
            conn_to_stdout_pend = 0;

            // if we just sent a backspace,
            // send a blank and another backspace
            // to erase what was on the screen.

            if (conn_to_stdout[0] == '\b') {
                FIN(ws1, write, 1, " ", 1);
                if (ws1 < 1)
                    break;
                FIN(ws2, write, 1, conn_to_stdout, 1);
                if (ws2 < 1)
                    break;
            }

        }

        if ((sel > 0) && FD_ISSET(conn, writefds)) {
            FIN(wsz, write, conn, stdin_to_conn, 1);
            if (wsz < 1)
                break;
            stdin_to_conn_pend = 0;
        }
    }

    fprintf(stderr, "\n\r\n\n\nsession from port %d terminated\r\n",
            ntohs(client_addr.sin_port));

    close(conn);
    close(server);
    return 0;
}
