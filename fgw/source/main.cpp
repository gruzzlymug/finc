#include <iostream>

#include <zmq.h>

using namespace std;

void send_message()
{
    cout << "Connecting to bus…\n";
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    char buffer[10];
    int seq_num = 0;
    while (true) {
        cout << "Sending message " << seq_num << "…\n";
        zmq_send(requester, "35=D|\0", 6, 0);
        zmq_recv(requester, buffer, 10, 0);
        cout << "Received reply " << seq_num << "\n";
        ++seq_num;
        if (seq_num > 0) break;
    }
    zmq_close(requester);
    zmq_ctx_destroy(context);
}

int main()
{
    cout << "Starting FIX Gateway\n";

    send_message();

    return 0;
}
