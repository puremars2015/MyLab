using System;
using System.Collections.Generic;
using System.Linq;

namespace labgogo
{

    class color : Object
    {

    }

    
    // 賣價(Ask)：金融機構賣給投資人的價格 <<投資人要買債券看Ask
    // 買價(Bid)：金融機構跟投資人買的價格 <<投資人要賣債券看Bid

    class Program
    {


        static void Main(string[] args)
        {
           

          
        }
    }

    class LinkedList
    {
        public const color clrNONE = null;

        public LinkNode ZeroNode;

        public LinkNode CurrentNode;

        public int CurrentLength;

        public double TotalLots;

        public LinkedList()
        {
            ZeroNode = new LinkNode();
        }

        /// <summary>
        /// 新增獲利單
        /// </summary>
        /// <param name="type"></param>
        /// <param name="price"></param>
        public void Add(string type, double price, double amount)
        {
            if (CurrentNode == null)
            {
                ZeroNode.nextNode = new LinkNode();

                CurrentNode = ZeroNode.nextNode;
            }
            else
            {
                CurrentNode.nextNode = new LinkNode();

                CurrentNode = CurrentNode.nextNode;
            }

            CurrentNode.Price = price;
            CurrentNode.Lots = amount;
            CurrentNode.Type = type;
        }


        /// <summary>
        /// 平倉掛勾
        /// </summary>
        /// <param name="ticket"></param>
        /// <param name="lots"></param>
        /// <param name="price"></param>
        /// <param name="slippage"></param>
        /// <param name="arrow_color"></param>
        /// <returns></returns>
        public bool OrderClose(int ticket, double lots, double price, int slippage, color arrow_color)
        {
            return true;
        }

        public int OrderTicket() => new Random().Next();


        /// <summary>
        /// 平獲利buy單，或平獲利sell單
        /// </summary>
        /// <param name="type"></param>
        /// <param name="price"></param>
        public void CoverByType(string type, double price)
        {

            var node = ZeroNode;

            while (node.nextNode != null)
            {
                node = node.nextNode;

                if (type == LinkNode.LONG)
                {
                    if (price > node.Price)
                    {
                        OrderClose(OrderTicket(), node.Lots, price, 0, clrNONE);
                    }
                }
                else
                {
                    if (price < node.Price)
                    {
                        OrderClose(OrderTicket(), node.Lots, price, 0, clrNONE);
                    }
                }

            }
        }
    }
    

    class LinkNode
    {
        public const string LONG = "LONG";

        public const string SHORT = "SHORT";

        /// <summary>
        /// 下單價格
        /// </summary>
        public double Price;

        /// <summary>
        /// 下單數量/手
        /// </summary>
        public double Lots;

        /// <summary>
        /// 多/空
        /// </summary>
        public string Type;

        /// <summary>
        /// 單號
        /// </summary>
        public int TicketNo;

        /// <summary>
        /// 下個資料節點
        /// </summary>
        public LinkNode nextNode;

        public LinkNode()
        {

        }
    }
}
