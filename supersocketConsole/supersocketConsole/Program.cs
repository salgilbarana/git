using SuperSocket.SocketBase;
using SuperWebSocket;
using System;


namespace supersocketConsole
{
    class Program
    {
        private static WebSocketServer wsServer;

        static void Main(string[] args)
        {
            wsServer = new WebSocketServer();
            int port = 8088;
            wsServer.Setup(port);
            wsServer.NewSessionConnected += wsServer_NewSessionConnected;
            wsServer.NewMessageReceived += wsServer_NewMessageReceived;
            wsServer.NewDataReceived += wsServer_NewDataReceived;
            wsServer.SessionClosed += wsServer_SessionClosed;
            wsServer.Start();
            Console.WriteLine("Server is running on port" + port + ". Press to exit...");
            Console.ReadKey();
            wsServer.Stop();
        }

        private static void wsServer_SessionClosed(WebSocketSession session, CloseReason value)
        {
            Console.WriteLine("SessionClosed");
        }

        private static void wsServer_NewDataReceived(WebSocketSession session, byte[] value)
        {
            Console.WriteLine("NewDataReceived");

        }

        private static void wsServer_NewMessageReceived(WebSocketSession session, string value)
        {
            Console.WriteLine("NewMessageReceived " + value);
            if (value == "Hello server")
            {
                session.Send("hello client");
            }
        }

        private static void wsServer_NewSessionConnected(WebSocketSession session)
        {
            Console.WriteLine("NewSessionConnected");
        }
    }
}
