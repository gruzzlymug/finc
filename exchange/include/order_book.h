#pragma once

namespace finc
{
    template<typename order_t>
    class order_book
    {
    private:
        order_t bids_[1024];
        order_t asks_[1024];

    public:
        void add_order();
    };

    template<typename order_t>
    void order_book<order_t>::add_order()
    {
        // does nothing
    }
}
