//+------------------------------------------------------------------+
//|                                                CloseAllOrder.mq4 |
//|                        Copyright 2021, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

bool 平倉(int cmd, int ticket, double lots)
{
    bool isClosed = false;
    int count = 0;

    if (cmd == OP_BUY)
    {
        while (isClosed == false && count < 10)
        {
            RefreshRates();
            isClosed = OrderClose(ticket, lots, MarketInfo(OrderSymbol(), MODE_BID), 10, clrNONE);
            count++;
        }
    }
    else
    {
        while (isClosed == false && count < 10)
        {
            RefreshRates();
            isClosed = OrderClose(ticket, lots, MarketInfo(OrderSymbol(), MODE_ASK), 10, clrNONE);
            count++;
        }
    }

    return isClosed;
}

void close_all()
{
    int get_out = 0, hstTotal = OrdersHistoryTotal();
    if (hstTotal < 1)
        get_out = 1;
    while (get_out == 0)
    {
        if (!OrderSelect(0, SELECT_BY_POS, MODE_TRADES))
            get_out = 1;
        else
        {
            if (OrderType() == OP_BUY)
            {
                //you can add here any specific if condition
                OrderClose(OrderTicket(), OrderLots(), Bid, 3);
            }

            if (OrderType() == OP_SELL)
            {
                //you can add here any specific if condition
                OrderClose(OrderTicket(), OrderLots(), Ask, 3);
            }
        }
        hstTotal = OrdersHistoryTotal();
        if (hstTotal < 1)
            get_out = 1;
    }
}

//+------------------------------------------------------------------+
//| Script program start function                                    |
//+------------------------------------------------------------------+
void OnStart()
{
    //---

    int total = OrdersTotal();

    int tickets[];
    
    ArrayResize(tickets, total);
    
    ArrayInitialize(tickets,EMPTY_VALUE);

    Print(StringConcatenate("平倉總數量:", total));

    for (int i = 0; i < total; i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            tickets[i] = OrderTicket();
        }
    }

    for (int i = 0; i < total; i++)
    {
        if (OrderSelect(tickets[i], SELECT_BY_TICKET))
        {

            bool result = 平倉(OrderType(), OrderTicket(), OrderLots());

            if (!result)
            { 
                Alert("Order ", OrderTicket(), " failed to close. Error:", GetLastError());
                Sleep(3000);
            }
        }
    }
}
//+------------------------------------------------------------------+
