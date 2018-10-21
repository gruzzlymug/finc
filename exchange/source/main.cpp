#include "order_book.h"

#include <zmq.h>

#include <iostream>

using namespace finc;
using namespace std;

class order
{
private:
    bool side_;
    int price_;
};

void thing()
{
    cout << "Connecting to hello world server…\n";
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer[10];
        cout << "Sending Hello " << request_nbr << "…\n";
        zmq_send(requester, "Hello", 5, 0);
        zmq_recv(requester, buffer, 10, 0);
        cout << "Received World " << request_nbr << "\n";
    }
    zmq_close(requester);
    zmq_ctx_destroy(context);
}

int main()
{
    cout << "Order Book Test ][\n";

    order_book<order> book;
    book.add_order();

    thing();
}
