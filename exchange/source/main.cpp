#include "order_book.h"

#include <iostream>

using namespace finc;
using namespace std;

class order
{
private:
    bool side_;
    int price_;
};

int main()
{
    cout << "Order Book Test ][\n";

    order_book<order> book;
    book.add_order();
}
