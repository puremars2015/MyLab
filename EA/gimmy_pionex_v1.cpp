//+------------------------------------------------------------------+
//|                                              gimmy_pionex_v1.mq4 |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

#define MAGIC_NUMBER 294381
#define TRADE_PAIR "XAUUSD"

// 賣價(Ask)：金融機構賣給投資人的價格 <<投資人要買債券看Ask
// 買價(Bid)：金融機構跟投資人買的價格 <<投資人要賣債券看Bid

double Bid紀錄 = 0;
double Ask紀錄 = 0;

double 最低做多價 = 0;
double 最高做空價 = 0;

double 買單數量 = 0;
double 賣單數量 = 0;

double 讀取多單數量()
{
    return 買單數量;
}

double 讀取空單數量()
{
    return 賣單數量;
}

void 加入買單數量(double 數量)
{
    買單數量 += 數量;
}

void 加入賣單數量(double 數量)
{
    賣單數量 += 數量;
}

void 紀錄多空價格()
{
    Bid紀錄 = Bid;
    Ask紀錄 = Ask;
}

void 平獲利多單()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && Bid > OrderOpenPrice())
            {
                double lots = OrderLots();
                double price = Bid;
                bool isClosed = OrderClose(OrderTicket(), lots, price, 0, clrNONE);

                if (isClosed)
                {
                    加入買單數量(-lots);
                }
            }
        }
    }
}

void 平獲利空單()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderOpenPrice() > Ask)
            {
                double lots = OrderLots();
                double price = Ask;
                bool isClosed = OrderClose(OrderTicket(), lots, price, 0, clrNONE);

                if (isClosed)
                {
                    加入賣單數量(-lots);
                }
            }
        }
    }
}

bool 所有多單虧損()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && Bid > OrderOpenPrice())
            {
                return false;
            }
        }
    }

    return true;
}

bool 所有空單虧損()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderOpenPrice() > Ask)
            {
                return false;
            }
        }
    }

    return true;
}

bool 有獲利多單()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && Bid > OrderOpenPrice())
            {
                return true;
            }
        }
    }

    return false;
}

bool 有獲利空單()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderOpenPrice() > Ask)
            {
                return true;
            }
        }
    }

    return false;
}

void 平損失最多的空單直到空單剩下九張()
{
}

void 平損失最多的多單直到多單剩下九張()
{
}

double 計算本次空單獲利()
{
    return 0;
}

double 計算本次多單獲利()
{
    return 0;
}

double 最遠多單損失()
{
    return 0;
}

double 第二遠多單損失()
{
    return 0;
}

double 第三遠多單損失()
{
    return 0;
}

double 最遠空單損失()
{
    return 0;
}

double 第二遠空單損失()
{
    return 0;
}

double 第三遠空單損失()
{
    return 0;
}

double 平最遠多單()
{
    return 0;
}

double 平第二遠多單()
{
    return 0;
}

double 平第三遠多單()
{
    return 0;
}

double 平最遠空單()
{
    return 0;
}

double 平第二遠空單()
{
    return 0;
}

double 平第三遠空單()
{
    return 0;
}

bool 所有空單價格大於賣價紀錄(double 價差)
{
    return true;
}

bool 所有多單價格大於買價紀錄(double 價差)
{
    return true;
}

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
    //---

    //---
    return (INIT_SUCCEEDED);
}
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
    //---
}

char status = '';

//https://www.youtube.com/watch?v=tva-XMSbsLc&vl=en
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{

    if (讀取多單數量() == 0 && 讀取空單數量() == 0)
    {
        if (status != 'A')
        {
            紀錄多空價格();
            status = 'A';
        }
    }
    else if (讀取多單數量() < 0.09 && 讀取空單數量() < 0.09)
    {
        if (status != 'B')
        {
            紀錄多空價格();
            status = 'B';
        }
    }
    else if (讀取多單數量() == 0.09 && 所有多單虧損() && 讀取空單數量() < 0.18)
    {
        if (status != 'C')
        {
            紀錄多空價格();
            status = 'C';
        }
    }
    else if (讀取多單數量() == 0.09 && 所有多單虧損() && 讀取空單數量() == 0.18)
    {
        if (status != 'D')
        {
            紀錄多空價格();
            status = 'D';
        }
    }
    else if (讀取空單數量() == 0.09 && 所有空單虧損() && 讀取多單數量() < 0.18)
    {
        if (status != 'E')
        {
            紀錄多空價格();
            status = 'E';
        }
    }
    else if (讀取空單數量() == 0.09 && 所有空單虧損() && 讀取多單數量() == 0.18)
    {
        if (status != 'F')
        {
            紀錄多空價格();
            status = 'F';
        }
    }

    if (status == 'A')
    {
        OrderSend(TRADE_PAIR, OP_BUY, 0.01, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
        OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);

        加入買單數量(0.01);
        加入賣單數量(0.01);
    }
    else if (status == 'B')
    {
        if ((Bid - Ask紀錄) > 6)
        {
            平獲利多單();
            OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
            加入賣單數量(0.01);
            紀錄多空價格();
        }

        if ((Bid紀錄 - Ask) > 6)
        {
            平獲利空單();
            OrderSend(TRADE_PAIR, OP_BUY, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
            加入買單數量(0.01);
            紀錄多空價格();
        }
    }
    else if (status == 'C')
    {
        if (Bid - Ask紀錄 > 6)
        {
            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                平獲利多單();
                if (讀取空單數量() < 0.09)
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入賣單數量(0.01);
                    紀錄多空價格();
                }
                else if (讀取空單數量() > 0.09)
                {
                    平損失最多的空單直到空單剩下九張();
                    紀錄多空價格();
                }
            }

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                平獲利空單();
                紀錄多空價格();

                double 獲利 = 計算本次空單獲利();

                bool 有平多單 = false;

                if (獲利 > 最遠多單損失())
                {
                    double 損失 = 平最遠多單();
                    獲利 = 獲利 - 損失;
                    有平多單 = true;
                }

                if (獲利 > 第二遠多單損失())
                {
                    double 損失 = 平第二遠多單();
                    獲利 = 獲利 - 損失;
                    有平多單 = true;
                }

                if (獲利 > 第三遠多單損失())
                {
                    double 損失 = 平第三遠多單();
                    獲利 = 獲利 - 損失;
                    有平多單 = true;
                }

                if (有平多單)
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入買單數量(0.01);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄多空價格();
            }
        }

        if (Bid紀錄 - Ask > 6)
        {
            紀錄多空價格();

            if (所有空單價格大於賣價紀錄(6))
            {
                OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入賣單數量(0.01);
            }
        }
    }
    else if (status == 'D')
    {
        if (Bid - Ask紀錄 > 6)
        {
            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                平獲利多單();
                if (讀取空單數量() < 0.09)
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入賣單數量(0.01);
                    紀錄多空價格();
                }
                else if (讀取空單數量() > 0.09)
                {
                    平損失最多的空單直到空單剩下九張();
                    紀錄多空價格();
                }
            }

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                平獲利空單();
                紀錄多空價格();

                double 獲利 = 計算本次空單獲利();

                bool 有平多單 = false;

                if (獲利 > 最遠多單損失())
                {
                    double 損失 = 平最遠多單();
                    獲利 = 獲利 - 損失;
                    有平多單 = true;
                }

                if (獲利 > 第二遠多單損失())
                {
                    double 損失 = 平第二遠多單();
                    獲利 = 獲利 - 損失;
                    有平多單 = true;
                }

                if (獲利 > 第三遠多單損失())
                {
                    double 損失 = 平第三遠多單();
                    獲利 = 獲利 - 損失;
                    有平多單 = true;
                }

                if (有平多單)
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入買單數量(0.01);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄多空價格();
            }
        }

        if (Bid紀錄 - Ask > 6)
        {
            平獲利空單();
            紀錄多空價格();

            double 獲利 = 計算本次空單獲利();

            bool 有平多單 = false;

            if (獲利 > 最遠多單損失())
            {
                double 損失 = 平最遠多單();
                獲利 = 獲利 - 損失;
                有平多單 = true;
            }

            if (獲利 > 第二遠多單損失())
            {
                double 損失 = 平第二遠多單();
                獲利 = 獲利 - 損失;
                有平多單 = true;
            }

            if (獲利 > 第三遠多單損失())
            {
                double 損失 = 平第三遠多單();
                獲利 = 獲利 - 損失;
                有平多單 = true;
            }

            if (有平多單)
            {
                OrderSend(TRADE_PAIR, OP_BUY, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入買單數量(0.01);
            }
        }
    }
    else if (status == 'E')
    {
        if (Bid紀錄 - Ask > 6)
        {
            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                平獲利空單();
                if (讀取多單數量() < 0.09)
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 0.01, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入買單數量(0.01);
                    紀錄多空價格();
                }
                else if (讀取多單數量() > 0.09)
                {
                    平損失最多的多單直到多單剩下九張();
                    紀錄多空價格();
                }
            }

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                平獲利多單();
                紀錄多空價格();

                double 獲利 = 計算本次多單獲利();

                bool 有平空單 = false;

                if (獲利 > 最遠空單損失())
                {
                    double 損失 = 平最遠空單();
                    獲利 = 獲利 - 損失;
                    有平空單 = true;
                }

                if (獲利 > 第二遠空單損失())
                {
                    double 損失 = 平第二遠空單();
                    獲利 = 獲利 - 損失;
                    有平空單 = true;
                }

                if (獲利 > 第三遠空單損失())
                {
                    double 損失 = 平第三遠空單();
                    獲利 = 獲利 - 損失;
                    有平空單 = true;
                }

                if (有平空單)
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入賣單數量(0.01);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄多空價格();
            }
        }

        if (Bid - Ask紀錄 > 6)
        {
            紀錄多空價格();

            if (所有多單價格大於買價紀錄(6))
            {
                OrderSend(TRADE_PAIR, OP_BUY, 0.01, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入買單數量(0.01);
            }
        }
    }
    else if (status == 'F')
    {
        if (Bid紀錄 - Ask > 6)
        {
            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                平獲利空單();
                if (讀取多單數量() < 0.09)
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 0.01, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入買單數量(0.01);
                    紀錄多空價格();
                }
                else if (讀取多單數量() > 0.09)
                {
                    平損失最多的多單直到多單剩下九張();
                    紀錄多空價格();
                }
            }

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                平獲利多單();
                紀錄多空價格();

                double 獲利 = 計算本次多單獲利();

                bool 有平空單 = false;

                if (獲利 > 最遠空單損失())
                {
                    double 損失 = 平最遠空單();
                    獲利 = 獲利 - 損失;
                    有平空單 = true;
                }

                if (獲利 > 第二遠空單損失())
                {
                    double 損失 = 平第二遠空單();
                    獲利 = 獲利 - 損失;
                    有平空單 = true;
                }

                if (獲利 > 第三遠空單損失())
                {
                    double 損失 = 平第三遠空單();
                    獲利 = 獲利 - 損失;
                    有平空單 = true;
                }

                if (有平空單)
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入賣單數量(0.01);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄多空價格();
            }
        }

        if (Bid - Ask紀錄 > 6)
        {
            平獲利多單();
            紀錄多空價格();

            double 獲利 = 計算本次多單獲利();

            bool 有平空單 = false;

            if (獲利 > 最遠空單損失())
            {
                double 損失 = 平最遠空單();
                獲利 = 獲利 - 損失;
                有平空單 = true;
            }

            if (獲利 > 第二遠空單損失())
            {
                double 損失 = 平第二遠空單();
                獲利 = 獲利 - 損失;
                有平空單 = true;
            }

            if (獲利 > 第三遠空單損失())
            {
                double 損失 = 平第三遠空單();
                獲利 = 獲利 - 損失;
                有平空單 = true;
            }

            if (有平空單)
            {
                OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入賣單數量(0.01);
            }
        }
    }
}
//+------------------------------------------------------------------+
