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
            mainRun();

          
        }

        static void bubbleSort(int[] arr, int len)
        {

            int i, j, temp;
            Boolean exchanged = true;

            for (i = 0; exchanged && i < len - 1; i++)
            { /* 外迴圈為排序趟數，len個數進行len-1趟,只有交換過,exchanged值為true才有執行迴圈的必要,否則exchanged值為false不執行迴圈 */
                exchanged = false;
                for (j = 0; j < len - 1 - i; j++)
                { /* 內迴圈為每趟比較的次數，第i趟比較len-i次  */
                    if (arr[j] > arr[j + 1])
                    { /* 相鄰元素比較，若逆序則互換（升序為左大於右，逆序反之） */
                        temp = arr[j];
                        arr[j] = arr[j + 1];
                        arr[j + 1] = temp;
                        exchanged = true; /*只有數值互換過, exchanged才會從false變成true,否則數列已經排序完成,exchanged值仍然為false,沒必要排序 */
                    }
                }
            }
        }

        static int mainRun()
        {
            int[] arr = new int[] { 3, 5, 1, -7, 4, 9, -6, 8, 10, 4 };

            int len = 10;
            int i;

            bubbleSort(arr, len);

            for (i = 0; i < len; i++)
                Console.WriteLine(arr[i]);
         

            return 0;
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
