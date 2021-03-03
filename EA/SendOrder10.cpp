//+------------------------------------------------------------------+
//|                                                  SendOrder10.mq4 |
//|                        Copyright 2021, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

#define TRADE_PAIR "XAUUSD"

bool 下單(
    string symbol,              // symbol
    int cmd,                    // operation
    double volume,              // volume
    double price,               // price
    int slippage,               // slippage
    double stoploss,            // stop loss
    double takeprofit,          // take profit
    string comment = NULL,      // comment
    int magic = 0,              // magic number
    datetime expiration = 0,    // pending order expiration
    color arrow_color = clrNONE // color
)
{
    int count = 0;

    int ticketNumber = -1;

    if (cmd == OP_BUY)
    {
        while ((ticketNumber == -1) && (count < 100))
        {
            RefreshRates();
            ticketNumber = OrderSend(symbol, cmd, volume, Ask, 10, stoploss, takeprofit, comment, magic, expiration, arrow_color);
            count++;
        }
    }
    else if (cmd == OP_SELL)
    {
        while ((ticketNumber == -1) && (count < 100))
        {
            RefreshRates();
            ticketNumber = OrderSend(symbol, cmd, volume, Bid, 10, stoploss, takeprofit, comment, magic, expiration, arrow_color);
            count++;
        }
    }

    return !(ticketNumber < 0);
}

//+------------------------------------------------------------------+
//| Script program start function                                    |
//+------------------------------------------------------------------+
void OnStart()
{
    //---
    for (int i = 0; i < 10; i++)
    {

        if (下單(TRADE_PAIR, OP_BUY, 0.01, Ask, 1, 0, 0, "", 56789, 0, clrNONE))
        {
            Print(StringConcatenate("下單:", i));
        }
        else
        {
            Print(StringConcatenate("下單失敗", GetLastError()));
        }
    }
}
//+------------------------------------------------------------------+
