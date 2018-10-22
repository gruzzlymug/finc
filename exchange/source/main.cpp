#include "order_book.h"

#include <zmq.h>

#include <iostream>
#include <thread>

using namespace finc;
using namespace std;

class order
{
private:
    int quantity_;
    int price_;
    bool side_;
};

void listen()
{
    cout << "Connecting to bus…\n";
    void *context = zmq_ctx_new();
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
        if (seq_num > 128) break;
    }
    zmq_close(requester);
    zmq_ctx_destroy(context);
}

int main()
{
    cout << "Order Book Test ][\n";

    order_book<order> book;
    book.add_order();

    thread listener(listen);

    listener.join();
}
