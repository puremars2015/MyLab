//+------------------------------------------------------------------+
//|                                              gimmy_pionex_v1.mq4 |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

#define TRADE_PAIR "XAUUSD."
/*
  printf(StringConcatenate(
      TimeYear(TimeLocal()), "-", 
      TimeMonth(TimeLocal()), "-", 
      TimeDay(TimeLocal()), " ", 
      TimeHour(TimeLocal()), ":",
      TimeMinute(TimeLocal()), ":",
      TimeSeconds(TimeLocal()),".csv"));
      */
// 外部輸入參數
input int MAGIC_NUMBER;

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

// 賣價(Ask)：金融機構賣給投資人的價格 <<投資人要買債券看Ask
// 買價(Bid)：金融機構跟投資人買的價格 <<投資人要賣債券看Bid

double 單網格寬度 = 3;
double 小單網格寬度 = 2;

double Bid紀錄 = 0;
double Ask紀錄 = 0;

double 最低做多價 = 0;
double 最高做空價 = 0;

double 買單數量 = 0;
double 賣單數量 = 0;

string LOG檔名 = "";
int LOG檔案 = 0;

double 讀取多單數量()
{
    return 買單數量;
}

double 讀取空單數量()
{
    return 賣單數量;
}

void 加入多單數量(double 數量)
{
    買單數量 += 數量;

    string 訊息 = StringConcatenate("[OP:BUY]", "[Lots:", 數量, "]", "[Time:", TimeCurrent(), "]");
    紀錄LOG(訊息);
}

void 加入空單數量(double 數量)
{
    賣單數量 += 數量;

    string 訊息 = StringConcatenate("[OP:SELL]", "[Lots:", 數量, "]", "[Time:", TimeCurrent(), "]");
    紀錄LOG(訊息);
}

void 更新價位()
{
    Bid紀錄 = Bid;
    Ask紀錄 = Ask;
}

bool 多單平倉(int ticket, double lots, double price, int slippage, color arrow_color)
{
    bool isClosed = OrderClose(ticket, lots, price, slippage, arrow_color);
    if (isClosed)
    {
        加入多單數量(-lots);
    }
    return isClosed;
}

bool 空單平倉(int ticket, double lots, double price, int slippage, color arrow_color)
{
    bool isClosed = OrderClose(ticket, lots, price, slippage, arrow_color);
    if (isClosed)
    {
        加入空單數量(-lots);
    }
    return isClosed;
}

void 平獲利多單()
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER && Bid > OrderOpenPrice())
            {
                double lots = OrderLots();
                double price = Bid;
                多單平倉(OrderTicket(), lots, price, 0, clrNONE);
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
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER && OrderOpenPrice() > Ask)
            {
                double lots = OrderLots();
                double price = Ask;
                空單平倉(OrderTicket(), lots, price, 0, clrNONE);
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

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[0].價格 - Bid;
}

double 第二遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[1].價格 - Bid;
}

double 第三遠多單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

    多單掛單資訊陣列排序(掛單資訊陣列, length);

    return 掛單資訊陣列[2].價格 - Bid;
}

double 最遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[0].價格;
}

double 第二遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[1].價格;
}

double 第三遠空單損失()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    return Ask - 掛單資訊陣列[2].價格;
}

double 平最遠多單()
{
    掛單資訊 掛單資訊陣列[18];

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_BUY) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

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

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

    空單掛單資訊陣列排序(掛單資訊陣列, length);

    double 平倉價 = Ask;

    if (OrderSelect(掛單資訊陣列[2].單號, SELECT_BY_TICKET))
    {
        空單平倉(OrderTicket(), OrderLots(), 平倉價, 0, clrNONE);
    }

    return 平倉價 - 掛單資訊陣列[2].價格;
}

bool 所有空單價格距離賣價紀錄大於價差(double 價差)
{
    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
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
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
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

void 初始設定LOG檔()
{
    LOG檔名 = StringConcatenate(Year(), "-", Month(), "-", Day(), "-log-", MAGIC_NUMBER, ".csv");
    LOG檔案 = FileOpen(LOG檔名, FILE_CSV | FILE_READ | FILE_WRITE);
}

void 檢查LOG檔()
{
    string LOG新檔名 = StringConcatenate(Year(), "-", Month(), "-", Day(), "-log-", MAGIC_NUMBER, ".csv");
    if (LOG檔名 != LOG新檔名)
    {
        if (LOG檔案 != INVALID_HANDLE)
        {
            FileClose(LOG檔案);
        }
        else
        {
            Print("LOG檔名檢查更新失敗", GetLastError());
        }

        LOG檔案 = FileOpen(LOG新檔名, FILE_CSV | FILE_READ | FILE_WRITE);
    }
}

void 紀錄LOG(string 訊息)
{
    if (LOG檔案 != INVALID_HANDLE)
    {
        FileSeek(LOG檔案, 0, SEEK_END);
        FileWrite(LOG檔案, 訊息);
    }
    else
    {
        Print("紀錄LOG失敗!!", GetLastError());
    }
}

void 關閉LOG檔()
{
    if (LOG檔案 != INVALID_HANDLE)
    {
        FileClose(LOG檔案);
    }
    else
    {
        Print("關閉LOG檔失敗!!", GetLastError());
    }
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

    初始設定LOG檔();

    double longLots = 0;
    double shortLots = 0;

    for (int i = 0; i < OrdersTotal(); i++)
    {
        if (OrderSelect(i, SELECT_BY_POS))
        {
            if (OrderType() == OP_BUY && OrderMagicNumber() == MAGIC_NUMBER)
            {
                加入多單數量(OrderLots());
                longLots += OrderLots();
            }

            if (OrderType() == OP_SELL && OrderMagicNumber() == MAGIC_NUMBER)
            {
                加入空單數量(OrderLots());
                shortLots += OrderLots();
            }
        }
    }

    string 訊息 = StringConcatenate("程式啟動時，多單倉數:", longLots, "、", "空單倉數:", shortLots, "。");
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
    關閉LOG檔();
    //---
}

char status = ' ';

//https://www.youtube.com/watch?v=tva-XMSbsLc&vl=en
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
    檢查LOG檔();

    if (讀取多單數量() == 0 && 讀取空單數量() == 0)
    {
        if (status != 'A')
        {
            更新價位();
            status = 'A';

            string 訊息 = StringConcatenate("移動區間到A，時間:", TimeCurrent());
            紀錄LOG(訊息);
        }
    }
    else if (讀取多單數量() < (9 * 單位手數) && 讀取空單數量() < (9 * 單位手數))
    {
        if (status != 'B')
        {
            更新價位();
            status = 'B';

            string 訊息 = StringConcatenate("移動區間到B，時間:", TimeCurrent());
            紀錄LOG(訊息);
        }
    }
    else if (讀取多單數量() == (9 * 單位手數) && 所有多單虧損() && 讀取空單數量() < (18 * 單位手數))
    {
        if (status != 'C')
        {
            更新價位();
            status = 'C';

            string 訊息 = StringConcatenate("移動區間到C，時間:", TimeCurrent());
            紀錄LOG(訊息);
        }
    }
    else if (讀取多單數量() == (9 * 單位手數) && 所有多單虧損() && 讀取空單數量() == (18 * 單位手數))
    {
        if (status != 'D')
        {
            更新價位();
            status = 'D';

            string 訊息 = StringConcatenate("移動區間到D，時間:", TimeCurrent());
            紀錄LOG(訊息);
        }
    }
    else if (讀取空單數量() == (9 * 單位手數) && 所有空單虧損() && 讀取多單數量() < (18 * 單位手數))
    {
        if (status != 'E')
        {
            更新價位();
            status = 'E';

            string 訊息 = StringConcatenate("移動區間到E，時間:", TimeCurrent());
            紀錄LOG(訊息);
        }
    }
    else if (讀取空單數量() == (9 * 單位手數) && 所有空單虧損() && 讀取多單數量() == (18 * 單位手數))
    {
        if (status != 'F')
        {
            更新價位();
            status = 'F';

            string 訊息 = StringConcatenate("移動區間到F，時間:", TimeCurrent());
            紀錄LOG(訊息);
        }
    }

    if (status == 'A')
    {
        OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
        OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);

        加入多單數量(單位手數);
        加入空單數量(單位手數);
    }
    else if (status == 'B')
    {
        if ((Bid - Ask紀錄) > 單網格寬度)
        {
            string 訊息 = StringConcatenate("B區間，發生上漲，時間：", TimeCurrent());
            紀錄LOG(訊息);

            平獲利多單();
            OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
            加入空單數量(單位手數);
            更新價位();
        }

        if ((Bid紀錄 - Ask) > 單網格寬度)
        {
            string 訊息 = StringConcatenate("B區間，發生下跌，時間：", TimeCurrent());
            紀錄LOG(訊息);

            平獲利空單();
            OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
            加入多單數量(單位手數);
            更新價位();
        }
    }
    else if (status == 'C')
    {

        if (Bid - Ask紀錄 > 單網格寬度)
        {
            string 訊息 = StringConcatenate("C區間，發生上漲，時間：", TimeCurrent());
            紀錄LOG(訊息);

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                string 訊息 = StringConcatenate("C區間，發生上漲，有獲利多單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利多單();
                if (讀取空單數量() < (9 * 單位手數))
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入空單數量(單位手數);
                    更新價位();
                }
                else if (讀取空單數量() > (9 * 單位手數))
                {
                    平損失最多的空單直到空單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {

                string 訊息 = StringConcatenate("C區間，發生上漲，有獲利空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利空單();
                更新價位();

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
                    OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入多單數量(單位手數);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                string 訊息 = StringConcatenate("C區間，發生上漲，沒有獲利多空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                if (Bid > (空單最高價() + 單網格寬度))
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入空單數量(單位手數);
                }
                更新價位();
            }
        }

        //單網格寬度改為2
        if (Bid紀錄 - Ask > 小單網格寬度)
        {
            string 訊息 = StringConcatenate("C區間，繼續下跌，時間：", TimeCurrent());
            紀錄LOG(訊息);

            更新價位();

            if (所有空單價格距離賣價紀錄大於價差(小單網格寬度))
            {
                OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入空單數量(單位手數);
            }
        }
    }
    else if (status == 'D')
    {
        if (Bid - Ask紀錄 > 單網格寬度)
        {
            string 訊息 = StringConcatenate("D區間，發生上漲，時間：", TimeCurrent());
            紀錄LOG(訊息);

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                string 訊息 = StringConcatenate("D區間，發生上漲，有獲利多單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利多單();
                if (讀取空單數量() < (9 * 單位手數))
                {
                    OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入空單數量(單位手數);
                    更新價位();
                }
                else if (讀取空單數量() > (9 * 單位手數))
                {
                    平損失最多的空單直到空單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                string 訊息 = StringConcatenate("D區間，發生上漲，有獲利空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利空單();
                更新價位();

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
                    OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入多單數量(單位手數);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                string 訊息 = StringConcatenate("D區間，發生上漲，沒有獲利多空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                if (讀取空單數量() > (9 * 單位手數))
                {
                    平損失最多的空單直到空單剩下九張();
                }
                更新價位();
            }
        }

        //單網格寬度2
        if (Bid紀錄 - Ask > 小單網格寬度)
        {
            string 訊息 = StringConcatenate("D區間，發生下跌，時間：", TimeCurrent());
            紀錄LOG(訊息);

            平獲利空單();
            更新價位();

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
                OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入多單數量(單位手數);
            }
        }
    }
    else if (status == 'E')
    {
        if (Bid紀錄 - Ask > 單網格寬度)
        {
            string 訊息 = StringConcatenate("E區間，發生下跌，時間：", TimeCurrent());
            紀錄LOG(訊息);

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                string 訊息 = StringConcatenate("E區間，發生下跌，有獲利空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利空單();
                if (讀取多單數量() < (9 * 單位手數))
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入多單數量(單位手數);
                    更新價位();
                }
                else if (讀取多單數量() > (9 * 單位手數))
                {
                    平損失最多的多單直到多單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                string 訊息 = StringConcatenate("E區間，發生下跌，有獲利多單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利多單();
                更新價位();

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
                    OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入空單數量(單位手數);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                string 訊息 = StringConcatenate("E區間，發生下跌，沒有獲利多空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                if (多單最低價() > (Ask + 單網格寬度))
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入多單數量(單位手數);
                }
                更新價位();
            }
        }

        //單網格寬度2
        if (Bid - Ask紀錄 > 小單網格寬度)
        {
            string 訊息 = StringConcatenate("E區間，發生上漲，時間：", TimeCurrent());
            紀錄LOG(訊息);

            更新價位();

            if (所有多單價格距離買價紀錄大於價差(小單網格寬度))
            {
                OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入多單數量(單位手數);
            }
        }
    }
    else if (status == 'F')
    {
        if (Bid紀錄 - Ask > 單網格寬度)
        {
            string 訊息 = StringConcatenate("F區間，發生下跌，時間：", TimeCurrent());
            紀錄LOG(訊息);

            bool 是否有獲利空單 = 有獲利空單();

            if (是否有獲利空單)
            {
                string 訊息 = StringConcatenate("F區間，發生下跌，有獲利空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利空單();
                if (讀取多單數量() < (9 * 單位手數))
                {
                    OrderSend(TRADE_PAIR, OP_BUY, 單位手數, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入多單數量(單位手數);
                    更新價位();
                }
                else if (讀取多單數量() > (9 * 單位手數))
                {
                    平損失最多的多單直到多單剩下九張();
                    更新價位();
                }
            }

            bool 是否有獲利多單 = 有獲利多單();

            if (是否有獲利多單)
            {
                string 訊息 = StringConcatenate("F區間，發生下跌，有獲利多單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                平獲利多單();
                更新價位();

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
                    OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                    加入空單數量(單位手數);
                }
            }

            if (!是否有獲利多單 && !是否有獲利空單)
            {
                string 訊息 = StringConcatenate("F區間，發生下跌，沒有獲利多空單，時間：", TimeCurrent());
                紀錄LOG(訊息);

                if (讀取多單數量() > (9 * 單位手數))
                {
                    平損失最多的多單直到多單剩下九張();
                }
                更新價位();
            }
        }

        //單網格寬度2
        if (Bid - Ask紀錄 > 小單網格寬度)
        {
            string 訊息 = StringConcatenate("F區間，發生上漲，時間：", TimeCurrent());
            紀錄LOG(訊息);

            平獲利多單();
            更新價位();

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
                OrderSend(TRADE_PAIR, OP_SELL, 單位手數, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                加入空單數量(單位手數);
            }
        }
    }
}
//+------------------------------------------------------------------+
