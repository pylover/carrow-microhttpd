// Copyright 2023 Vahid Mardani
#include <stdlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <microhttpd.h>
#include <clog.h>
#include <carrow.h>


typedef struct carrow_microhttpd {
    struct MHD_Daemon *daemon;
} carrow_microhttpd;


#undef CARROW_ENTITY
#define CARROW_ENTITY carrow_microhttpd
#include <carrow_generic.h>
#include <carrow_generic.c>


static enum MHD_Result
request_handler(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **req_cls
) {
    struct MHD_Response *response;
    enum MHD_Result ret;

    response = MHD_create_response_from_buffer(
        strlen(url),
        (void *) url,
        MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(
        connection,
        MHD_HTTP_OK,
        response);

    MHD_destroy_response(response);
    return ret;
}


static void
httpserverA(
    struct carrow_microhttpd_coro *self,
    struct carrow_microhttpd *state
) {
    fd_set rs;
    fd_set ws;
    fd_set es;
    int i;

    CORO_START;

    while (true) {
        FD_ZERO(&rs);
        FD_ZERO(&ws);
        FD_ZERO(&es);

        if (MHD_get_fdset(state->daemon, &rs, &ws, &es, NULL) != MHD_YES) {
            CORO_REJECT("MHD_get_fdset failed.");
        }

        for (i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &rs)) {
                CORO_WAIT(i, CIN);

                FD_ZERO(&rs);
                FD_SET(i, &rs);

                if (MHD_run(state->daemon) != MHD_YES) {
                    CORO_REJECT("MHD_run failed.");
                }

                break;  // only one fd we are interested in
            }
        }
    }

    CORO_FINALLY;
    MHD_stop_daemon(state->daemon);
    CORO_END;
}


int
main() {
    struct MHD_Daemon *d;
    int port;

    port = 8080;
    clog_verbosity = CLOG_DEBUG;

    d = MHD_start_daemon(
        MHD_USE_DEBUG,
        port,
        NULL,
        NULL,
        &request_handler,
        NULL,
        MHD_OPTION_END);
    if (d == NULL) {
        ERROR("Could not start daemon on port %d.\n", port);
        return 1;
    }

    INFO("Listening on port %d...\n", port);

    struct carrow_microhttpd state = {
        .daemon = d
    };

    return carrow_microhttpd_forever(httpserverA, &state, NULL);
}
