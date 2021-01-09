//+------------------------------------------------------------------+
//|                                              gimmy_pionex_v1.mq4 |
//|                        Copyright 2020, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2020, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict

#define MAGIC_NUMBER 294381
#define TRADE_PAIR "XAUUSD"


    // 賣價(Ask)：金融機構賣給投資人的價格 <<投資人要買債券看Ask
    // 買價(Bid)：金融機構跟投資人買的價格 <<投資人要賣債券看Bid

double 賣價紀錄 = 0;
double 買價紀錄 = 0;

double 最低做多價 = 0;
double 最高做空價 = 0;

double 買單數量 = 0;
double 賣單數量 = 0;

double 讀取買單數量()
{
   return 買單數量;
}

double 讀取賣單數量()
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

void 紀錄買賣價格()
{
   賣價紀錄 = Bid;
   買價紀錄 = Ask;
}

void 平獲利多單()
{
   for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS))
      {
         if(OrderType() == OP_BUY && Bid > OrderOpenPrice())
         {
            double lots = OrderLots();
            double price = Bid;
            bool isClosed = OrderClose(OrderTicket(), lots, price, 0, clrNONE);
            
           if(isClosed)
           {
            加入買單數量(-lots);
           }
         }
      }
   }
}

void 平獲利空單()
{
   for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS))
      {
         if(OrderType() == OP_SELL && OrderOpenPrice() > Ask)
         {
            double lots = OrderLots();
            double price = Ask;
            bool isClosed = OrderClose(OrderTicket(), lots, price, 0, clrNONE);
            
           if(isClosed)
           {
            加入賣單數量(-lots);
           }
         }
      }
   }
}

bool 所有多單虧損()
{
   for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS))
      {
         if(OrderType() == OP_BUY && Bid > OrderOpenPrice())
         {
            return false;
         }
      }
   }
   
   return true;
}

bool 所有空單虧損()
{
   for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS))
      {
         if(OrderType() == OP_SELL && OrderOpenPrice() > Ask)
         {
            return false;
         }
      }
   }
   
   return true;
}

bool 有獲利多單()
{
 for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS))
      {
         if(OrderType() == OP_BUY && Bid > OrderOpenPrice())
         {
            return true;
         }
      }
   }
   
   return false;
}

bool 有獲利空單()
{
 for(int i = 0; i < OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS))
      {
         if(OrderType() == OP_SELL && OrderOpenPrice() > Ask)
         {
            return true;
         }
      }
   }
   
   return false;
}

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//---
  
   
//---
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//---
   
  }
  
  //https://www.youtube.com/watch?v=tva-XMSbsLc&vl=en
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---
    if(讀取買單數量() == 0 && 讀取賣單數量() == 0)
   {
      OrderSend(TRADE_PAIR, OP_BUY, 0.01, Ask, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
      OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
      
      加入買單數量(0.01);
      加入賣單數量(0.01);
      
      紀錄買賣價格();
   }
   
   if(讀取買單數量() < 0.09 && 讀取賣單數量() < 0.09)
   {
      if ((Bid - 買價紀錄) > 600)
      {
         平獲利多單();
         OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
         加入賣單數量(0.01);
         紀錄買賣價格();
      }

      if ((賣價紀錄 - Ask) > 600)
      {
         平獲利空單();
         OrderSend(TRADE_PAIR, OP_BUY, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
         加入買單數量(0.01);
         紀錄買賣價格();
      }
   }
   
    if(讀取買單數量() == 0.09 && 所有多單虧損() && 讀取賣單數量() < 0.18)
      {
         if ((Bid - 買價紀錄) > 600)
         {
            if (有獲利多單())
            {
               平獲利多單();
               if (讀取賣單數量() < 0.09)
               {
                   OrderSend(TRADE_PAIR, OP_SELL, 0.01, Bid, 1, 0, 0, "", MAGIC_NUMBER, 0, clrNONE);
                   加入賣單數量(0.01);
                   紀錄買賣價格();
               }
               else if (讀取賣單數量() > 0.09)
               {
                   
               }
            }
         }
      }

   
  }
//+------------------------------------------------------------------+
