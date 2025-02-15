//+------------------------------------------------------------------+
//|                                           DynamicDeclaration.mq4 |
//|                        Copyright 2021, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2021, MetaQuotes Software Corp."
#property link "https://www.mql5.com"
#property version "1.00"
#property strict

const int MAGIC_NUMBER = 56789;

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

//+------------------------------------------------------------------+
//| Script program start function                                    |
//+------------------------------------------------------------------+
void OnStart()
{
    //---
    掛單資訊 掛單資訊陣列[];

    int 空單數量 = 讀取空單數量();

    ArrayResize(掛單資訊陣列, 空單數量);

    int length = 掛單陣列給值(掛單資訊陣列, OP_SELL) + 1;

    for (int i = 0; i < length; i++)
    {
        Print(StringConcatenate(掛單資訊陣列[i].單號));
    }
}
//+------------------------------------------------------------------+
