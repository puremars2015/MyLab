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

class 掛單資訊
{
public:
    int 單號;
    double 價格;
    掛單資訊()
    {
        this.單號 = 0;
        this.價格 = 0;
    };
    掛單資訊(int 單號, double 價格)
    {
        this.單號 = 單號;
        this.價格 = 價格;
    };
};

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

void 空單掛單資訊陣列排序(掛單資訊 &掛單資訊陣列[], int 陣列長度)
{
    int i, j;

    掛單資訊 temp;

    bool exchanged = true;

    for (i = 0; exchanged && i < 陣列長度 - 1; i++)
    { /* 外迴圈為排序趟數，len個數進行len-1趟,只有交換過,exchanged值為true才有執行迴圈的必要,否則exchanged值為false不執行迴圈 */
        exchanged = false;
        for (j = 0; j < 陣列長度 - 1 - i; j++)
        { /* 內迴圈為每趟比較的次數，第i趟比較陣列長度-i次  */
            if (掛單資訊陣列[j].價格 < 掛單資訊陣列[j + 1].價格)
            { /* 相鄰元素比較，若逆序則互換（升序為左大於右，逆序反之） */
                temp.價格 = 掛單資訊陣列[j].價格;
                temp.單號 = 掛單資訊陣列[j].單號;
                掛單資訊陣列[j].價格 = 掛單資訊陣列[j + 1].價格;
                掛單資訊陣列[j].單號 = 掛單資訊陣列[j + 1].單號;
                掛單資訊陣列[j + 1].價格 = temp.價格;

                掛單資訊陣列[j + 1].單號 = temp.單號;
                exchanged = true; /*只有數值互換過, exchanged才會從false變成true,否則數列已經排序完成,exchanged值仍然為false,沒必要排序 */
            }
        }
    }
}

void 多單掛單資訊陣列排序(掛單資訊 &掛單資訊陣列[], int 陣列長度)
{
    int i, j;

    掛單資訊 temp;

    bool exchanged = true;

    for (i = 0; exchanged && i < 陣列長度 - 1; i++)
    { /* 外迴圈為排序趟數，len個數進行len-1趟,只有交換過,exchanged值為true才有執行迴圈的必要,否則exchanged值為false不執行迴圈 */
        exchanged = false;
        for (j = 0; j < 陣列長度 - 1 - i; j++)
        { /* 內迴圈為每趟比較的次數，第i趟比較陣列長度-i次  */
            if (掛單資訊陣列[j].價格 > 掛單資訊陣列[j + 1].價格)
            { /* 相鄰元素比較，若逆序則互換（升序為左大於右，逆序反之） */
                temp.價格 = 掛單資訊陣列[j].價格;
                temp.單號 = 掛單資訊陣列[j].單號;
                掛單資訊陣列[j].價格 = 掛單資訊陣列[j + 1].價格;
                掛單資訊陣列[j].單號 = 掛單資訊陣列[j + 1].單號;
                掛單資訊陣列[j + 1].價格 = temp.價格;

                掛單資訊陣列[j + 1].單號 = temp.單號;
                exchanged = true; /*只有數值互換過, exchanged才會從false變成true,否則數列已經排序完成,exchanged值仍然為false,沒必要排序 */
            }
        }
    }
}

void 平損失最多的空單直到空單剩下九張()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    int 需平張數 = length - 9;

    for (int i = 0; i < 需平張數; i++)
    {
        if (OrderSelect(掛單資訊陣列[i].單號, SELECT_BY_TICKET))
        {
            OrderClose(OrderTicket(), OrderLots(), Ask, 0, clrNONE);
        }
    }
}

void 平損失最多的多單直到多單剩下九張()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    int 需平張數 = length - 9;

    for (int i = 0; i < 需平張數; i++)
    {
        if (OrderSelect(掛單資訊陣列[i].單號, SELECT_BY_TICKET))
        {
            OrderClose(OrderTicket(), OrderLots(), Ask, 0, clrNONE);
        }
    }
}

double 計算本次空單獲利()
{
    double 獲利 = 0;
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                double 平倉價 = Ask;
                double 開倉價 = OrderOpenPrice();
                if (開倉價 > 平倉價)
                {
                    if (OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE))
                    {
                        獲利 += (開倉價 - 平倉價);
                    }
                }
            }
        }
    }

    return 獲利;
}

double 計算本次多單獲利()
{
    double 獲利 = 0;
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                double 平倉價 = Bid;
                double 開倉價 = OrderOpenPrice();
                if (平倉價 > 開倉價)
                {
                    if (OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE))
                    {
                        獲利 += (平倉價 - 開倉價);
                    }
                }
            }
        }
    }

    return 獲利;
}

double 最遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[0].價格 - Bid;
}

double 第二遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[1].價格 - Bid;
}

double 第三遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[2].價格 - Bid;
}

double 最遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[0].價格;
}

double 第二遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[1].價格;
}

double 第三遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[2].價格;
}

double 平最遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);


    double 平倉價 = Bid;

    if (OrderSelect(掛單資訊陣列[0].單號, SELECT_BY_TICKET))
    {
        OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 掛單資訊陣列[0].價格 - 平倉價;
}

double 平第二遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);


    double 平倉價 = Bid;

    if (OrderSelect(掛單資訊陣列[1].單號, SELECT_BY_TICKET))
    {
        OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 掛單資訊陣列[1].價格 - 平倉價;
}

double 平第三遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    多單掛單資訊陣列排序(掛單資訊陣列, length);


    double 平倉價 = Bid;

    if (OrderSelect(掛單資訊陣列[2].單號, SELECT_BY_TICKET))
    {
        OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 掛單資訊陣列[2].價格 - 平倉價;
}

double 平最遠空單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[0].單號, SELECT_BY_TICKET))
    {
        OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[0].價格;
}

double 平第二遠空單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[1].單號, SELECT_BY_TICKET))
    {
        OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[1].價格;
}

double 平第三遠空單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                bool isSaved = false;

                for (int p = length; p < 18 && !isSaved; p++)
                {
                    if (掛單資訊陣列[p].單號 == 0)
                    {
                        掛單資訊陣列[p].價格 = OrderOpenPrice();
                        掛單資訊陣列[p].單號 = OrderTicket();
                        isSaved = true;
                        length++; //掛單資訊儲存進度
                    }
                }

                if (!isSaved)
                {
                    // throw __EXCEPTIONS;
                }
            }
        }
    }

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[2].單號, SELECT_BY_TICKET))
    {
        OrderClose(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[2].價格;
}

bool 所有空單價格距離賣價紀錄大於價差(double 價差)
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL)
            {
                if (Abs(OrderOpenPrice() - Bid紀錄) < 價差)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool 所有多單價格距離買價紀錄大於價差(double 價差)
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY)
            {
                if (Abs(OrderOpenPrice() - Ask紀錄) < 價差)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

double Abs(double value)
{
    return value > 0 ? value : 0 - value;
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

char status = ' ';

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

            if (所有空單價格距離賣價紀錄大於價差(6))
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

            if (所有多單價格距離買價紀錄大於價差(6))
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
