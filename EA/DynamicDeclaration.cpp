//+------------------------------------------------------------------+
//|                                           DynamicDeclaration.mq4 |
//|                        Copyright 2021, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

const int MAGIC_NUMBER = 0;

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


void 掛單資訊陣列由低至高排序(掛單資訊 &掛單資訊陣列[], int 陣列長度)
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

void 掛單資訊陣列由高至低排序(掛單資訊 &掛單資訊陣列[], int 陣列長度)
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

//+------------------------------------------------------------------+
//| Script program start function                                    |
//+------------------------------------------------------------------+
void OnStart()
{
    //---
    掛單資訊 掛單資訊陣列[];

    int 空單數量 = 讀取空單數量();

    ArrayResize(掛單資訊陣列, 空單數量);

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL);

    掛單資訊陣列由低至高排序(掛單資訊陣列, length);

    for (int i = 0; i < length; i++)
    {
        Print(StringConcatenate(i, " Ticket:", 掛單資訊陣列[i].單號, "  Price:", 掛單資訊陣列[i].價格));
    }
}
//+------------------------------------------------------------------+
