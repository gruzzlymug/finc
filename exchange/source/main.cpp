#include "order_book.h"

#include "zmq.h"
#include "zhelpers.h"

#include <cassert>
#include <iostream>
#include <thread>

#include <cstring>

using namespace finc;
using namespace std;

class order
{
private:
    int quantity_;
    int price_;
    bool side_;
};

void listen(void *context)
{
    // Socket to talk to clients
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://localhost:5556");
    assert(rc == 0);

    char *filter = "94965";
    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, filter, strlen(filter));
    assert(rc == 0);

    int seq_num = 0;
    long total_temp = 0;
    while (true) {
        char *string = s_recv(subscriber);
        cout << "Received message " << seq_num << "\n";

        int zipcode, temperature, rel_humidity;
        sscanf(string, "%d %d %d", &zipcode, &temperature, &rel_humidity);
        total_temp += temperature;
        free(string);

        ++seq_num;
        if (seq_num > 100) break;
    }
    cout << "Average temperature for zipcode " << filter
         << " was " << static_cast<int>(total_temp / seq_num) << "F\n";
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
}

void send(void *context)
{
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    char buffer[10];
    int seq_num = 0;
    while (true) {
        cout << "Sending message " << seq_num << "…\n";
        zmq_send(requester, "Hello\0", 6, 0);
        zmq_recv(requester, buffer, 10, 0);
        cout << "Received reply " << seq_num << "\n";
        ++seq_num;
        if (seq_num > 8) break;
    }
    zmq_close(requester);
}

int main()
{
    cout << "Starting Exchange\n";

    order_book<order> book;
    book.add_order();

    cout << "Connecting to bus\n";
    void *context = zmq_ctx_new();
    thread listener(listen, context);
    listener.join();
    zmq_ctx_destroy(context);
}
