//+------------------------------------------------------------------+
//|                                              gimmy_grid_v1.mq4 |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+

// V1版本特性
// 1、100點處理
// 2、有賺就可以平、暫時不考慮要超越單網格寬度
// 強化穩定度參考 https://www.earnforex.com/blog/ordersend-error-129-what-to-do/

// bug 最遠一二三單，會因為可能有先被平倉的單，導致順序改變


#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

#define TRADE_PAIR "XAUUSD."

// 外部輸入參數
int MAGIC_NUMBER = 56789;

double 單位手數 = 0.01;

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
    掛單資訊(int _單號, double _價格)
    {
        this.單號 = _單號;
        this.價格 = _價格;
    };
};

struct 持單訊息
{
public:
    int 單量;
    int 單號[];
};

// 賣價(Ask)：金融機構賣給投資人的價格 <<投資人要買債券看Ask
// 買價(Bid)：金融機構跟投資人買的價格 <<投資人要賣債券看Bid

double 內網格寬度 = 3;
double 外網格寬度 = 2;

double Bid紀錄 = 0;
double Ask紀錄 = 0;

double 最低做多價 = 0;
double 最高做空價 = 0;

double 買單數量 = 0;
double 賣單數量 = 0;

string LOG檔名 = "";
int LOG檔案 = 0;

double ASK()
{
    RefreshRates();
    return Ask;
}

double BID()
{
    RefreshRates();
    return Bid;
}

int 讀取多單數量()
{
    int 多單數量 = 0;
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                多單數量++;
            }
        }
    }
    return 多單數量;
}

int 讀取空單數量()
{
    int 空單數量 = 0;
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                空單數量++;
            }
        }
    }
    return 空單數量;
}

持單訊息 讀取多單數量與單號()
{
    持單訊息 多單訊息;

    多單訊息.單量 = 0;

    ArrayResize(多單訊息.單號, OrdersTotal());

    ArrayInitialize(多單訊息.單號, EMPTY_VALUE);

    for (int i = 0, no = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                多單訊息.單量++;
                多單訊息.單號[no] = OrderTicket();
                no++;
            }
        }
    }
    return 多單訊息;
}

持單訊息 讀取空單數量與單號()
{
    持單訊息 空單訊息;

    空單訊息.單量 = 0;

    ArrayResize(空單訊息.單號, OrdersTotal());

    ArrayInitialize(空單訊息.單號, EMPTY_VALUE);

    for (int i = 0, no = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                空單訊息.單量++;
                空單訊息.單號[no] = OrderTicket();
                no++;
            }
        }
    }
    return 空單訊息;
}

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

    price = NormalizeDouble(price, 2);

    int count = 0;

    int ticketNumber = -1;

    if (cmd == OP_BUY)
    {
        while ((ticketNumber == -1) && (count < 10))
        {
            RefreshRates();
            ticketNumber = OrderSend(symbol, cmd, volume, Ask, 10, stoploss, takeprofit, comment, magic, expiration, clrGreen);
            count++;
        }
    }
    else if (cmd == OP_SELL)
    {
        while ((ticketNumber == -1) && (count < 10))
        {
            RefreshRates();
            ticketNumber = OrderSend(symbol, cmd, volume, Bid, 10, stoploss, takeprofit, comment, magic, expiration, clrRed);
            count++;
        }
    }

    if (ticketNumber < 0)
    {
        紀錄LOG(StringConcatenate("[ERROR][Sending Order Failed]:", GetLastError()));
    }
    else
    {
        if (cmd == OP_BUY)
        {
            記錄多單變化與單號(volume, ticketNumber);
        }
        else
        {
            記錄空單變化與單號(volume, ticketNumber);
        }
    }

    return !(ticketNumber < 0);
}

void 記錄多單變化與單號(double 數量, int 單號)
{
    string 訊息 = StringConcatenate("[OP:BUY]", "[Lots:", 數量, "]", "[Ask Price:", Ask, "]", "[單號:", 單號, "]", "[Time:", TimeCurrent(), "]");
    紀錄LOG(訊息);
}

void 記錄空單變化與單號(double 數量, int 單號)
{
    string 訊息 = StringConcatenate("[OP:SELL]", "[Lots:", 數量, "]", "[Bid Price:", Bid, "]", "[單號:", 單號, "]", "[Time:", TimeCurrent(), "]");
    紀錄LOG(訊息);
}

void 更新價位()
{
    Bid紀錄 = Bid;
    Ask紀錄 = Ask;
    紀錄LOG(StringConcatenate("更新價位至Bid:[", Bid紀錄, "]", "Ask:[", Ask紀錄, "]"));
}

void 市價平倉(int &tickets[], int length)
{
    for (int i = 0; i < length; i++)
    {
        if (OrderSelect(tickets[i], SELECT_BY_TICKET))
        {
            bool isClosed = false;
            int count = 0;

            int type = OrderType();
            double lots = OrderLots();

            if (type == OP_BUY)
            {
                while (isClosed == false && count < 10)
                {
                    RefreshRates();
                    isClosed = OrderClose(tickets[i], lots, Bid, 10, clrNONE);
                    count++;
                }

                if (isClosed)
                {
                    記錄多單變化與單號(-lots, tickets[i]);
                }
                else
                {
                    紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
                }
            }
            else if (type == OP_SELL)
            {
                while (isClosed == false && count < 10)
                {
                    RefreshRates();
                    isClosed = OrderClose(tickets[i], lots, Ask, 10, clrNONE);
                    count++;
                }

                if (isClosed)
                {
                    記錄空單變化與單號(-lots, tickets[i]);
                }
                else
                {
                    紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
                }
            }
        }
        else
        {
            紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
        }
    }
}

bool 多單市價平倉(int ticket, double lots)
{
    bool isClosed = false;
    int count = 0;

    while (isClosed == false && count < 10)
    {
        RefreshRates();
        isClosed = OrderClose(ticket, lots, Bid, 10, clrNONE);
        count++;
    }

    if (isClosed)
    {
        記錄多單變化與單號(-lots, ticket);
    }
    else
    {
        紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
    }
    return isClosed;
}

bool 多單平倉(int ticket, double lots, double price, int slippage, color arrow_color)
{

    price = NormalizeDouble(price, 2);

    bool isClosed = false;
    int count = 0;

    while (isClosed == false && count < 10)
    {
        RefreshRates();
        isClosed = OrderClose(ticket, lots, Bid, 10, arrow_color);
        count++;
    }

    if (isClosed)
    {
        記錄多單變化與單號(-lots, ticket);
    }
    else
    {
        紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
    }
    return isClosed;
}

bool 空單市價平倉(int ticket, double lots)
{

    bool isClosed = false;
    int count = 0;

    while (isClosed == false && count < 10)
    {
        RefreshRates();
        isClosed = OrderClose(ticket, lots, Ask, 10, clrNONE);
        count++;
    }

    if (isClosed)
    {
        記錄空單變化與單號(-lots, ticket);
    }
    else
    {
        紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
    }
    return isClosed;
}

bool 空單平倉(int ticket, double lots, double price, int slippage, color arrow_color)
{

    price = NormalizeDouble(price, 2);

    bool isClosed = false;
    int count = 0;

    while (isClosed == false && count < 10)
    {
        RefreshRates();
        isClosed = OrderClose(ticket, lots, Ask, 10, arrow_color);
        count++;
    }

    if (isClosed)
    {
        記錄空單變化與單號(-lots, ticket);
    }
    else
    {
        紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
    }
    return isClosed;
}

void 獲利市價平倉(int &tickets[], int length)
{
    for (int i = 0; i < length; i++)
    {
        if (OrderSelect(tickets[i], SELECT_BY_TICKET))
        {
            bool isClosed = false;
            int count = 0;

            int type = OrderType();
            double lots = OrderLots();

            if (type == OP_BUY)
            {
                if (BID() > OrderOpenPrice())
                {
                    while (isClosed == false && count < 10)
                    {
                        isClosed = OrderClose(tickets[i], lots, BID(), 10, clrNONE);
                        count++;
                    }

                    if (isClosed)
                    {
                        記錄多單變化與單號(-lots, tickets[i]);
                    }
                    else
                    {
                        紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
                    }
                }
            }
            else if (type == OP_SELL)
            {
                if (OrderOpenPrice() > ASK())
                {
                    while (isClosed == false && count < 10)
                    {
                        isClosed = OrderClose(tickets[i], lots, ASK(), 10, clrNONE);
                        count++;
                    }

                    if (isClosed)
                    {
                        記錄空單變化與單號(-lots, tickets[i]);
                    }
                    else
                    {
                        紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
                    }
                }
            }
        }
        else
        {
            紀錄LOG(StringConcatenate("[ERROR][Closing Order Failed]:", GetLastError()));
        }
    }
}

void 平獲利多單()
{
    持單訊息 多單訊息 = 讀取多單數量與單號();

    獲利市價平倉(多單訊息.單號, 多單訊息.單量);
}

void 平獲利空單()
{
    持單訊息 空單訊息 = 讀取空單數量與單號();

    獲利市價平倉(空單訊息.單號, 空單訊息.單量);
}

bool 所有多單虧損()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER && Bid > OrderOpenPrice())
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
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER && OrderOpenPrice() > Ask)
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
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER && Bid > OrderOpenPrice())
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
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER && OrderOpenPrice() > Ask)
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

void 平損失最多的空單直到空單剩下九張()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    int 需平張數 = length - 9;

    for (int i = 0; i < 需平張數; i++)
    {
        if (OrderSelect(掛單資訊陣列[i].單號, SELECT_BY_TICKET))
        {
            空單平倉(OrderTicket(), OrderLots(), Ask, 0, clrNONE);
        }
    }
}

void 平損失最多的多單直到多單剩下九張()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    int 需平張數 = length - 9;

    for (int i = 0; i < 需平張數; i++)
    {
        if (OrderSelect(掛單資訊陣列[i].單號, SELECT_BY_TICKET))
        {
            多單平倉(OrderTicket(), OrderLots(), Ask, 0, clrNONE);
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
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                double 平倉價 = Ask;
                double 開倉價 = OrderOpenPrice();
                if (開倉價 > 平倉價)
                {
                    if (空單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE))
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
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                double 平倉價 = Bid;
                double 開倉價 = OrderOpenPrice();
                if (平倉價 > 開倉價)
                {
                    if (多單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE))
                    {
                        獲利 += (平倉價 - 開倉價);
                    }
                }
            }
        }
    }

    return 獲利;
}

/** 
 * orderType 應為 OP_SELL、OP_BUY
 * 回傳陣列結束位置
 */
int 掛單陣列給值(掛單資訊 &掛單資訊陣列[], int orderType)
{
    int pointer = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == orderType && OrderMagicNumber() == MAGIC_NUMBER)
            {
                掛單資訊陣列[pointer].價格 = OrderOpenPrice();
                掛單資訊陣列[pointer].單號 = OrderTicket();
                pointer++;
            }
        }
    }

    return pointer;
}

double 最遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[0].價格 - Bid;
}

double 第二遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[1].價格 - Bid;
}

double 第三遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[2].價格 - Bid;
}

double 最遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[0].價格;
}

double 第二遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[1].價格;
}

double 第三遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[2].價格;
}

double 平最遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Bid;

    if (OrderSelect(掛單資訊陣列[0].單號, SELECT_BY_TICKET))
    {
        多單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 掛單資訊陣列[0].價格 - 平倉價;
}

double 平第二遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Bid;

    if (OrderSelect(掛單資訊陣列[1].單號, SELECT_BY_TICKET))
    {
        多單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 掛單資訊陣列[1].價格 - 平倉價;
}

double 平第三遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY);

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Bid;

    if (OrderSelect(掛單資訊陣列[2].單號, SELECT_BY_TICKET))
    {
        多單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 掛單資訊陣列[2].價格 - 平倉價;
}

double 平最遠空單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[0].單號, SELECT_BY_TICKET))
    {
        空單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[0].價格;
}

double 平第二遠空單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[1].單號, SELECT_BY_TICKET))
    {
        空單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[1].價格;
}

double 平第三遠空單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[2].單號, SELECT_BY_TICKET))
    {
        空單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[2].價格;
}

bool 所有空單價格距離賣價大於價差(double 價差)
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                if (Abs(OrderOpenPrice() - Bid) < 價差)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool 所有多單價格距離買價大於價差(double 價差)
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                if (Abs(OrderOpenPrice() - Ask) < 價差)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

double 多單最低價()
{
    bool isWrited = false;
    double lowest = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                double tp = OrderOpenPrice();

                if (!isWrited)
                {
                    lowest = tp;
                    isWrited = true;
                }
                else
                {
                    lowest = tp < lowest ? tp : lowest;
                }
            }
        }
    }

    return lowest;
}

double 空單最高價()
{
    bool isWrited = false;
    double highest = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                double tp = OrderOpenPrice();

                if (!isWrited)
                {
                    highest = tp;
                    isWrited = true;
                }
                else
                {
                    highest = tp > highest ? tp : highest;
                }
            }
        }
    }

    return highest;
}

double 計算目前多單持倉()
{
    double totalLots = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                totalLots += OrderLots();
            }
        }
    }

    return totalLots;
}

double 計算目前空單持倉()
{
    double totalLots = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                totalLots += OrderLots();
            }
        }
    }

    return totalLots;
}

void 記錄多空價格()
{
    //打開檔案
    string 檔名 = StringConcatenate("價格檔.txt");
    int 多空價格記錄檔 = FileOpen(檔名, FILE_CSV | FILE_READ | FILE_WRITE);

    //寫入資料
    string 訊息 = StringConcatenate("儲存Ask價格:", Ask紀錄, ",儲存Bid價格:", Bid紀錄);

    Print(訊息);

    if (多空價格記錄檔 != INVALID_HANDLE)
    {
        FileWrite(多空價格記錄檔, 訊息);
    }
    else
    {
        Print("紀錄多空價格失敗!!", GetLastError());
    }

    //關閉檔案
    if (多空價格記錄檔 != INVALID_HANDLE)
    {
        FileClose(多空價格記錄檔);
    }
    else
    {
        Print("關閉多空價格記錄檔失敗!!", GetLastError());
    }
}

void 紀錄LOG(string 訊息)
{
    訊息 = StringConcatenate(訊息, "--GROUP CODE:", MAGIC_NUMBER, "--Ask:", Ask, "--Bid:", Bid, "--Ask紀錄:", Ask紀錄, "--Bid紀錄:", Bid紀錄, "--Time:", TimeCurrent());
    Print(訊息);
}

double Abs(double value)
{
    return value > 0 ? value : 0 - value;
}

void 顯示當前狀態()
{
    Print(StringConcatenate("[目前Ask價錢:", Ask, "][目前Bid價錢:", Bid, "][Ask紀錄價錢:", Ask紀錄, "][Bid紀錄價錢:", Bid紀錄, "][目前區間狀態:", status, "]"));
}

void 止損處理()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                RefreshRates();
                if (OrderOpenPrice() - Bid > 100)
                {
                    double lots = OrderLots();

                    RefreshRates();
                    double price = Bid;
                    多單平倉(OrderTicket(), lots, price, 0, clrNONE);
                }
            }

            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                RefreshRates();
                if (Ask - OrderOpenPrice() > 100)
                {
                    double lots = OrderLots();

                    RefreshRates();
                    double price = Ask;
                    空單平倉(OrderTicket(), lots, price, 0, clrNONE);
                }
            }
        }
    }
}

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+

int OnInit()
{
    //---

    // 初始設定LOG檔();

    int longTicketCount = 0;
    int shortTicketCount = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                longTicketCount++;
            }

            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                shortTicketCount++;
            }
        }
    }

    string 訊息 = StringConcatenate("程式啟動時，多單張數:", longTicketCount, "、", "空單張數:", shortTicketCount, "。", "MAGIC_NUMBER:", MAGIC_NUMBER);
    紀錄LOG(訊息);

    //TODO:這是一個應急的方案，因為目前重啟動無法取得原先紀錄的價格，有空應改為冷儲存
    更新價位();

    //---
    return (INIT_SUCCEEDED);
}
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
    記錄多空價格();
    //---
}

string status = "";

//https://www.youtube.com/watch?v=tva-XMSbsLc&vl=en
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
    止損處理();

    if (讀取多單數量() == 0 && 讀取空單數量() == 0)
    {
        if (status != "A")
        {
            status = "A";
            紀錄LOG(StringConcatenate("移動區間到A，時間:", TimeCurrent()));
            更新價位();
        }
    }
    else if (讀取多單數量() < 9 && 讀取空單數量() < 9)
    {
        if (status != "B")
        {
            status = "B";
            紀錄LOG(StringConcatenate("移動區間到B，時間:", TimeCurrent()));
            更新價位();
        }
    }
    else if (讀取多單數量() >= 9 && 所有多單虧損() && 讀取空單數量() < 18)
    {
        if (status != "C")
        {
            status = "C";
            紀錄LOG(StringConcatenate("移動區間到C，時間:", TimeCurrent()));
            更新價位();
        }
    }
    else if (讀取多單數量() >= 9 && 所有多單虧損() && 讀取空單數量() == 18)
    {
        if (status != "D")
        {
            status = "D";
            紀錄LOG(StringConcatenate("移動區間到D，時間:", TimeCurrent()));
            更新價位();
        }
    }
    else if (讀取空單數量() >= 9 && 所有空單虧損() && 讀取多單數量() < 18)
    {
        if (status != "E")
        {
            status = "E";
            紀錄LOG(StringConcatenate("移動區間到E，時間:", TimeCurrent()));
            更新價位();
        }
    }
    else if (讀取空單數量() >= 9 && 所有空單虧損() && 讀取多單數量() == 18)
    {
        if (status != "F")
        {
            status = "F";
            紀錄LOG(StringConcatenate("移動區間到F，時間:", TimeCurrent()));
            更新價位();
        }
    }

    if (status == "A")
    {
        下單(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
        下單(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
    }
    else if (status == "B")
    {
        if ((Bid - Ask紀錄) > 內網格寬度)
        {
            紀錄LOG(StringConcatenate("B區間，發生上漲，時間：", TimeCurrent()));

            平獲利多單();
            if (下單(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
            {
                更新價位();
            }
        }

        if ((Bid紀錄 - Ask) > 內網格寬度)
        {
            紀錄LOG(StringConcatenate("B區間，發生下跌，時間：", TimeCurrent()));

            平獲利空單();
            if (下單(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
            {
                更新價位();
            }
        }
    }
    else if (status == "C")
    {

        if (Bid - Ask紀錄 > 內網格寬度)
        {
            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                紀錄LOG(StringConcatenate("C區間，發生上漲，有獲利多單，時間：", TimeCurrent()));

                平獲利多單();
                if (讀取空單數量() < 9)
                {
                    if (下單(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                    {
                        更新價位();
                    }
                }
                else if (讀取空單數量() > 9)
                {
                    平損失最多的空單直到空單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("C區間，發生上漲，有獲利空單，時間：", TimeCurrent()));

                平獲利空單();
                更新價位();
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("C區間，發生上漲，沒有獲利多空單，時間：", TimeCurrent()));

                if (Bid > (空單最高價() + 內網格寬度))
                {
                    if (下單(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                    {
                        更新價位();
                    }
                }
            }
        }

        //內網格寬度改為2
        if (Bid紀錄 - Ask > 外網格寬度)
        {

            if (所有空單價格距離賣價大於價差(外網格寬度))
            {
                紀錄LOG(StringConcatenate("C區間，繼續下跌，所有空單價格距離賣價大於價差，時間：", TimeCurrent()));
                if (下單(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                {
                    更新價位();
                }
            }
            else
            {
                紀錄LOG(StringConcatenate("C區間，繼續下跌，有空單價格距離賣價小於價差，時間：", TimeCurrent()));
                更新價位();
            }
        }
    }
    else if (status == "D")
    {
        if (Bid - Ask紀錄 > 內網格寬度)
        {

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                紀錄LOG(StringConcatenate("D區間，發生上漲，有獲利多單，時間：", TimeCurrent()));

                平獲利多單();
                if (讀取空單數量() < 9)
                {
                    if (下單(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                    {
                        更新價位();
                    }
                }
                else if (讀取空單數量() > 9)
                {
                    平損失最多的空單直到空單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("D區間，發生上漲，有獲利空單，時間：", TimeCurrent()));

                平獲利空單();
                更新價位();
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("D區間，發生上漲，沒有獲利多空單，時間：", TimeCurrent()));

                if (讀取空單數量() > 9)
                {
                    平損失最多的空單直到空單剩下九張();
                }
                更新價位();
            }
        }

        //內網格寬度2
        if (Bid紀錄 - Ask > 外網格寬度)
        {
            紀錄LOG(StringConcatenate("D區間，發生下跌，時間：", TimeCurrent()));

            平獲利空單();
            更新價位();
        }
    }
    else if (status == "E")
    {
        if (Bid紀錄 - Ask > 內網格寬度)
        {

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("E區間，發生下跌，有獲利空單，時間：", TimeCurrent()));

                平獲利空單();
                if (讀取多單數量() < 9)
                {
                    if (下單(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                    {
                        更新價位();
                    }
                }
                else if (讀取多單數量() > 9)
                {
                    平損失最多的多單直到多單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                紀錄LOG(StringConcatenate("E區間，發生下跌，有獲利多單，時間：", TimeCurrent()));

                平獲利多單();
                更新價位();
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("E區間，發生下跌，沒有獲利多空單，時間：", TimeCurrent()));

                if (多單最低價() > (Ask + 內網格寬度))
                {
                    if (下單(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                    {
                        更新價位();
                    }
                }
            }
        }

        //內網格寬度2
        if (Bid - Ask紀錄 > 外網格寬度)
        {

            if (所有多單價格距離買價大於價差(外網格寬度))
            {
                紀錄LOG(StringConcatenate("E區間，發生上漲，所有多單價格距離買價大於價差，時間：", TimeCurrent()));
                if (下單(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                {
                    更新價位();
                }
            }
            else
            {
                紀錄LOG(StringConcatenate("E區間，發生上漲，有多單價格距離買價小於價差，時間：", TimeCurrent()));
                更新價位();
            }
        }
    }
    else if (status == "F")
    {
        if (Bid紀錄 - Ask > 內網格寬度)
        {
            紀錄LOG(StringConcatenate("F區間，發生下跌，時間：", TimeCurrent()));

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("F區間，發生下跌，有獲利空單，時間：", TimeCurrent()));

                平獲利空單();
                if (讀取多單數量() < 9)
                {
                    if (下單(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE))
                    {
                        更新價位();
                    }
                }
                else if (讀取多單數量() > 9)
                {
                    平損失最多的多單直到多單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                紀錄LOG(StringConcatenate("F區間，發生下跌，有獲利多單，時間：", TimeCurrent()));

                平獲利多單();
                更新價位();
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                紀錄LOG(StringConcatenate("F區間，發生下跌，沒有獲利多空單，時間：", TimeCurrent()));

                if (讀取多單數量() > 9)
                {
                    平損失最多的多單直到多單剩下九張();
                }
                更新價位();
            }
        }

        //內網格寬度2
        if (Bid - Ask紀錄 > 外網格寬度)
        {
            紀錄LOG(StringConcatenate("F區間，發生上漲，時間：", TimeCurrent()));

            平獲利多單();
            更新價位();
        }
    }
}
//+------------------------------------------------------------------+
