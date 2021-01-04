using System;
using System.Collections.Generic;
using System.Linq;

namespace labgogo
{
  

    class Program
    {
        const string LONG = "LONG";

        const string SHORT = "SHORT";

        static void Main(string[] args)
        {
           

          
        }
    }

    class LinkedList
    {
        public LinkNode ZeroNode;

        public LinkNode CurrentNode;

        public LinkedList()
        {
            ZeroNode = new LinkNode();
        }


        /// <summary>
        /// 新增獲利單
        /// </summary>
        /// <param name="type"></param>
        /// <param name="price"></param>
        public void Add(string type, double price)
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
            CurrentNode.Type = type;
        }

        public void Get
    }
    

    class LinkNode
    {
        public double Price;

        public string Type;

        public LinkNode nextNode;

        public LinkNode()
        {

        }
    }
}
