#include <iostream>

#include "zhelpers.h"
#include "zmq.h"

using namespace std;

void send_message()
{
    cout << "Connecting to bus…\n";
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://localhost:5556");
    assert(rc == 0);

    // subscribe to a zipcode
    //char *filter = (argc > 1) ? argv[1] : "10001";
    char *filter = "10001";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(rc == 0);

    int seq_num = 0;
    long total_temp = 0;
    while (true) {
        char *string = s_recv(subscriber);

        int zipcode;
        int temperature;
        int rel_humidity;
        sscanf(string, "%d %d %d", &zipcode, &temperature, &rel_humidity);
        total_temp += temperature;
        free(string);

        cout << "Received message " << seq_num << "\n";
        ++seq_num;
        if (seq_num > 100) break;
    }
    printf("Average temperature for zipcode %s was %dF\n", filter, (int) (total_temp / seq_num));

    zmq_close(subscriber);
    zmq_ctx_destroy(context);
}

int main()
{
    cout << "Starting FIX Gateway\n";

    send_message();

    return 0;
}
