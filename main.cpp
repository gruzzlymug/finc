#include "order_book.h"

using namespace finc;

class order
{
private:
    bool side_;
    int price_;
};

int main()
{
    order_book<order> book;
    book.add_order();
}
