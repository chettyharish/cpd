using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace TicTacToeClient
{
    public sealed class ServerConnect
    {
        private static ServerConnect instance = null;
        private static readonly object padlock = new object();

        ServerConnect()
        { }

        public static ServerConnect Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (padlock)
                    {
                        if (instance == null)
                        {
                            instance = new ServerConnect();
                        }
                    }
                }
                return instance;
            }
        }
    }
}
