#include "zhelpers.h"
#include "zmq.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main(void)
{
    printf("Starting Bus\n");

    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf("Using ZeroMQ v%d.%d.%d\n", major, minor, patch);

    // Socket to talk to clients
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publisher, "tcp://*:5556");
    assert(rc == 0);

    srandom((unsigned) time(NULL));
    while (1) {
        int zipcode = randof(100000);
        int temperature = randof(215) - 80;
        int rel_humidity = randof(50) + 10;

        // send to subscribers
        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, rel_humidity);
        s_send(publisher, update);
        printf("Published '%s'\n", update);
    }
    zmq_close(publisher);
    zmq_ctx_destroy(context);
    return 0;
}
