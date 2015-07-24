using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TicTacToeClient
{

    public partial class GameBoard : Form
    {
        LoginDialog loginForm = new LoginDialog();
        ComposeMail composeMail;
        MatchRequest matchRequest;
        delegate void SetTextCallback(string text);

        public GameBoard()
        {
            InitializeComponent();
        }

        #region ui control management

        private void lbChat_MeasureItem(object sender, MeasureItemEventArgs e)
        {
            e.ItemHeight = (int)e.Graphics.MeasureString(lbChat.Items[e.Index].ToString(), lbChat.Font, lbChat.Width).Height;
        }

        private void lbChat_DrawItem(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();
            e.DrawFocusRectangle();
            e.Graphics.DrawString(lbChat.Items[e.Index].ToString(), e.Font, new SolidBrush(e.ForeColor), e.Bounds);
        }

         private void SetlblChatWrapping()
        {
            lbChat.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
            lbChat.MeasureItem += lbChat_MeasureItem;
            lbChat.DrawItem += lbChat_DrawItem;
        }

         private void GameBoard_HelpButtonClicked(object sender, CancelEventArgs e)
         {
             SendHelp();
         }
        #endregion

        Socket client;

        private void GameBoard_Load(object sender, EventArgs e)
        {
            //Application.Run(new Form1());
            loginForm.ShowDialog();
            Login();
            
            //set the listbox wrapping and other buttons
            SetlblChatWrapping();
            this.AcceptButton = btnDummy;
            this.Text += "(" + loginForm.UsernameValue + ")";
            this.HelpButton = true;
            this.HelpButtonClicked += GameBoard_HelpButtonClicked;
            this.MinimizeBox = false;
            this.MaximizeBox = false;
            // Receive the response from the remote device.
            Receive(client);
            //receiveDone..WaitOne();

            SendWho();
            SendGame();
            Thread.Sleep(100);
            SendListMail();
        }

        

        

        
        #region asynchronized gameboard
        // ManualResetEvent instances signal completion.
        private  ManualResetEvent connectDone =
            new ManualResetEvent(false);
        private ManualResetEvent sendDone =
            new ManualResetEvent(false);
        private ManualResetEvent receiveDone =
            new ManualResetEvent(false);

        // The response from the remote device.
        private String response = String.Empty;

        private void Receive(Socket client)
        {
            try
            {
                // Create the state object.
                StateObject state = new StateObject();
                state.workSocket = client;

                // Begin receiving the data from the remote device.
                client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                    new AsyncCallback(ReceiveCallback), state);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private  void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                // Retrieve the state object and the client socket 
                // from the asynchronous state object.
                StateObject state = (StateObject)ar.AsyncState;
                Socket client = state.workSocket;

                state.sb.Clear();

                // Read data from the remote device.
                int bytesRead = client.EndReceive(ar);

                if (bytesRead > 0)
                {
                    // There might be more data, so store the data received so far.
                    state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));

                    // Get the rest of the data.
                    client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                        new AsyncCallback(ReceiveCallback), state);
                }
                //else
                //{
                //    // All the data has arrived; put it in response.
                //    if (state.sb.Length > 1)
                //    {
                //        response = state.sb.ToString();
                //    }
                //    // Signal that all bytes have been received.
                //    receiveDone.Set();
                //}

                if (state.sb.Length > 1)
                {
                    response = state.sb.ToString();

                    if(!response.StartsWith("<"+loginForm.UsernameValue))
                    {
                        SetTextCallback responseCallBack = new SetTextCallback(HandleResponse);
                        this.Invoke(responseCallBack, new object[] { response });
                    }
                      

                }

                
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void HandleResponse(string response)
        {
            if (response.StartsWith("\n!shout!") || response.StartsWith("!shout!"))
            {
                lbChat.Items.Add(response);
            }
            else if (response.StartsWith("\n!private!") || response.StartsWith("!private!"))
            {
                lbChat.Items.Add(response);
            }
            else if (response.StartsWith("\nKibitz*") || response.StartsWith("Kibitz*"))
            {
                lbChat.Items.Add(response);
            }
            else if (response.Contains("Total") && response.Contains("user(s)") && response.Contains("online"))
            {
                PopulateUserList(response);
            }
            else if (response.Contains("Total") && response.Contains("game(s)"))
            {
                PopulateGameList(response);
            }
            else if (response.Contains("You have been kicked out due to multiple login"))
            {
                MessageBox.Show(response);
                this.Close();
            }
            else if (response.StartsWith("Please input mail body"))
            { 
                //ignore this line
            }
            else if(response.StartsWith("!MAILBOX!"))
            {
                PopulateMailList(response);
            }
            else if (response.Contains("You have received a new message"))
            {
                MessageBox.Show(response);
                SendListMail();
            }
            else if (response.StartsWith("!OBSERVE!") || response.StartsWith("\n!OBSERVE!"))
            {
            
                PopulateObservedGames(response);
            }
            else if (response.StartsWith("!GAME!") || response.StartsWith("\n!GAME!"))
            {

                PopulateYourGames(response);
            }
            else if (response.Contains("invites your for a game"))
            {
                string[] splitresponse = System.Text.RegularExpressions.Regex.Split(response, "[ \n]");
                matchRequest = new MatchRequest();
                matchRequest.requestText = response;
                matchRequest.pieceText = splitresponse[9];
                matchRequest.timerText = splitresponse[10];
                matchRequest.SendToUserName = splitresponse[1];
                matchRequest.VisibleChanged += matchRequest_VisibleChanged;
                matchRequest.ShowDialog();
            }
            else
                MessageBox.Show(response);
        }

        private void PopulateObservedGames(string response)
        {
            txtObservedGames.Text = response.Substring(response.IndexOf('\n', response.IndexOf('\n') + 1), response.LastIndexOf('\n') - response.IndexOf('\n', response.IndexOf('\n') + 1) + 1).Replace("\n",Environment.NewLine);

        }

        private void PopulateYourGames(string response)
        {
            txtYourMatch.Text = response.Substring(response.IndexOf('\n', response.IndexOf('\n') + 1), response.LastIndexOf('\n') - response.IndexOf('\n', response.IndexOf('\n') + 1) + 1).Replace("\n", Environment.NewLine);

        }

        private void PopulateUserList(string response)
        {
            string userstring = response.Substring(response.IndexOf("\n") + 1, response.LastIndexOf("\n") - response.IndexOf("\n") - 1);
            string[] userarray = userstring.Split('\t');
            lbUsers.Items.Clear();
            for (int i = 0; i < userarray.Length; i++)
            {
                lbUsers.Items.Add(userarray[i]);
            }
        }

        private void PopulateGameList(string response)
        {
            if (!response.StartsWith("Total 0 game(s):")) { 
            string userstring = response.Substring(response.IndexOf("\n") + 1, response.LastIndexOf("\n") - response.IndexOf("\n") - 1);
            string[] userarray = userstring.Split('\n');
            lbGames.Items.Clear();
            for (int i = 0; i < userarray.Length; i++)
            {
                lbGames.Items.Add(userarray[i]);
            }
            }
        }

        private void PopulateMailList(string response)
        {
            
                string mailstring = response.Substring(response.IndexOf("\n") + 1, response.LastIndexOf("\n") - response.IndexOf("\n") - 1);
                string[] mailarray = mailstring.Split('\n');
                lbMail.Items.Clear();
                for (int i = 0; i < mailarray.Length; i++)
                {
                    lbMail.Items.Add(mailarray[i]);
                }
            
        }
        #endregion

        #region synchronized login section

        // Data buffer for incoming data.
        byte[] bytes = new byte[2048];
        byte[] msgToSend;
        string strbytesRec = string.Empty;
        int bytesSent = 0;
        int bytesRec = 0;
        IPAddress ipAddress;

        private void Login()
        {
            try
            {
                if (string.IsNullOrEmpty(loginForm.ServerValue))
                {
                    ShowMessageBoxAndQuit("Server Cannot be empty");
                    return;
                }
                if (string.IsNullOrEmpty(loginForm.PortValue))
                {
                    ShowMessageBoxAndQuit("Port Cannot be empty");
                    return;
                }

                if (string.IsNullOrEmpty(loginForm.UsernameValue))
                {
                    ShowMessageBoxAndQuit("UserName Cannot be empty");
                    return;
                }
                if (string.IsNullOrEmpty(loginForm.PasswordValue))
                {
                    ShowMessageBoxAndQuit("Password Cannot be empty");
                    return;
                }

                bool isnumberip = IPAddress.TryParse(loginForm.ServerValue, out ipAddress);

                if (!isnumberip)
                {
                    try
                    {
                        IPHostEntry ipHostInfo = Dns.GetHostEntry(loginForm.ServerValue);
                        ipAddress = ipHostInfo.AddressList[0];
                    }
                    catch (Exception ex)
                    {
                        ShowMessageBoxAndQuit("Server could not be found.");
                        return;
                    }
                }

                IPEndPoint remoteEP = new IPEndPoint(ipAddress, int.Parse(loginForm.PortValue));

                // Create a TCP/IP  socket.
                client = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream, ProtocolType.Tcp);

                // Connect the socket to the remote endpoint. Catch any errors.

                client.Connect(remoteEP);
                UpdateToolStrip("Socket connected to " + client.RemoteEndPoint.ToString());

                SyncReceiveFromServer(client);
                if (string.Compare(strbytesRec, "<username>:", true, System.Globalization.CultureInfo.InvariantCulture) == 0)
                {
                    // Encode the data string into a byte array.
                    msgToSend = Encoding.ASCII.GetBytes(loginForm.UsernameValue);

                    // Send the data through the socket.
                    bytesSent = client.Send(msgToSend);

                    SyncReceiveFromServer(client);

                    if (strbytesRec.Contains("password"))
                    {
                        msgToSend = Encoding.ASCII.GetBytes(loginForm.PasswordValue);

                        // Send the data through the socket.
                        bytesSent = client.Send(msgToSend);

                        SyncReceiveFromServer(client);
                        if (strbytesRec.Contains("failed"))
                        {
                            ShowMessageBoxAndQuit("Login Failed!");
                        }
                        if (strbytesRec.Contains(loginForm.UsernameValue))
                        {
                            strbytesRec = "Login Successful!";
                            UpdateToolStrip(strbytesRec);
                        }
                    }
                    else
                        ShowMessageBoxAndQuit("Login Failed!");
                }
                else
                    ShowMessageBoxAndQuit("Login Failed!");
            }
            catch (Exception e)
            {
                ShowMessageBoxAndQuit("Unable to connect to server");
            }
            finally
            {
                statusStrip.Refresh();
            }
        }

        /// <summary>
        /// this function populates the global var strbytesRec with the data sent by server
        /// </summary>
        /// <param name="sender"></param>
        private void SyncReceiveFromServer(Socket sender)
        {
            // Receive the username response from server 
            bytesRec = sender.Receive(bytes);
            strbytesRec = Encoding.ASCII.GetString(bytes, 0, bytesRec);
        }

        private void UpdateToolStripAndQuit(Socket sender)
        {
            byte[] msg = Encoding.ASCII.GetBytes("quit");
            // Send the data through the socket.
            int bytesSent = sender.Send(msg);

            // Release the socket.
            sender.Shutdown(SocketShutdown.Both);
            sender.Close();

            UpdateToolStrip(strbytesRec);
        }

        private void UpdateToolStrip(string message)
        {
            if (message.Length > 200)
                toolStripStatusLabel1.Text = message.Substring(0, 199);
            else
                toolStripStatusLabel1.Text = message;
            statusStrip.Refresh();
        }

        private void ShowMessageBoxAndQuit(string message)
        {
            MessageBox.Show(this, message, "Error");
            this.Close();
        }
        #endregion

        private void GameBoard_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (client != null && client.Connected)
            {
                byte[] msg = Encoding.ASCII.GetBytes("quit");
                // Send the data through the socket.
                int bytesSent = client.Send(msg);

                // Release the socket.
                client.Shutdown(SocketShutdown.Both);
                client.Close();
            }
        }

        private void txtChatResponse_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if(rbShout.Checked)
                {
                    SendShout();
                    txtChatResponse.Text = string.Empty;
                }
                else if (rbKibitz.Checked)
                {
                    SendKibitz();
                    txtChatResponse.Text = string.Empty;
                }
                else if(rbTell.Checked)
                {
                    if (lbUsers.SelectedIndex != -1)
                    {
                        SendTell(lbUsers.SelectedItem.ToString());
                        txtChatResponse.Text = string.Empty;
                    }
                    else
                    {
                        MessageBox.Show("Select a user from user list to send private message");
                    }
                }
                e.Handled = true;
            }
            
        }

        private void SendMove(string move)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes(move);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendResign()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("resign");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        

        private void SendTell(string user)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("tell " + user +" " + txtChatResponse.Text);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);

            lbChat.Items.Add( loginForm.UsernameValue + " <private> to "+ user+" : " + txtChatResponse.Text);
        }

        private void SendMatchRequest(string userToSend, string piece, string timer)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("match " + userToSend + " " + piece + " " + timer);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
            MessageBox.Show("Match Request Sent.");
        }

        private void SendMail(string user,string subject, string mail)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("mail " + user + " " + subject + "\r\n" );

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
            Thread.Sleep(200);
            string[] mailtoSendArray = System.Text.RegularExpressions.Regex.Split(mail, "\n");
            
            for (int i = 0; i < mailtoSendArray.Length; i++)
            {
                msgToSend = Encoding.ASCII.GetBytes(mailtoSendArray[i]);

                // Send the data through the socket.
                bytesSent = client.Send(msgToSend);
                Thread.Sleep(200);
            }

            //msgToSend = Encoding.ASCII.GetBytes("\r\n");

            //// Send the data through the socket.
            //bytesSent = client.Send(msgToSend);
            Thread.Sleep(200);
            msgToSend = Encoding.ASCII.GetBytes(".");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendBlock(string user)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("block " + user);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }


        private void SendUnBlock(string user)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("unblock " + user);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendStats(string user)
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("stats " + user);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendShout()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("shout " + txtChatResponse.Text);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendKibitz()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("kibitz " + txtChatResponse.Text);

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendWho()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("who");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendListMail()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("listmail");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendHelp()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("help");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendGame()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("game");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendQuiet()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("quiet");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendNonQuiet()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("nonquiet");

            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendInfo()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("info " + txtInfo.Text);
            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);

            MessageBox.Show("Information changes sent!");
        }

        private void SendPasswd()
        {
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("passwd " + txtPassword.Text);
            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
            MessageBox.Show("Password changes sent!");
        }

        private void btnRefreshUsers_Click(object sender, EventArgs e)
        {
            SendWho();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //dummy buttom for form bug
        }

        private void btnRefreshGames_Click(object sender, EventArgs e)
        {
            SendGame();
        }

        private void btnUnblock_Click(object sender, EventArgs e)
        {
            if (lbUsers.SelectedIndex != -1)
            {
                SendUnBlock(lbUsers.SelectedItem.ToString());

            }
            else
            {
                MessageBox.Show("Select a user from user list to unblock");
            }
        }

        private void btnBlock_Click(object sender, EventArgs e)
        {
            if (lbUsers.SelectedIndex != -1)
            {
                SendBlock(lbUsers.SelectedItem.ToString());

            }
            else
            {
                MessageBox.Show("Select a user from user list to block");
            }
        }

        private void btnStats_Click(object sender, EventArgs e)
        {
            if (lbUsers.SelectedIndex != -1)
            {
                SendStats(lbUsers.SelectedItem.ToString());

            }
            else
            {
                MessageBox.Show("Select a user from user list to block");
            }
        }

        private void btnQuiet_Click(object sender, EventArgs e)
        {
            SendQuiet();
        }

        private void btnNonQuiet_Click(object sender, EventArgs e)
        {
            SendNonQuiet();
        }

        private void btnChangeInfo_Click(object sender, EventArgs e)
        {
            SendInfo();
        }

        private void btnChangePassword_Click(object sender, EventArgs e)
        {
            SendPasswd();
        }

        private void btnReadMail_Click(object sender, EventArgs e)
        {
            if (lbMail.SelectedIndex != -1)
            {
                if (!lbMail.SelectedItem.ToString().Contains("You have no messages"))
                    SendReadMail(lbMail.SelectedItem.ToString());
            }
            else
            {
                MessageBox.Show("Select a mail to read");
            }
        }

        private void btnSendMail_Click(object sender, EventArgs e)
        {
            composeMail = new ComposeMail();
            composeMail.VisibleChanged += composeMail_VisibleChanged;
            composeMail.ShowDialog();
        }

        void composeMail_VisibleChanged(object sender, EventArgs e)
        {
            if (composeMail!=null && composeMail.Visible == false && composeMail.sendmailpressed)
            {
                SendMail(composeMail.receipientText, composeMail.subjectText, composeMail.mailText);
            }
        }

        void matchRequest_VisibleChanged(object sender, EventArgs e)
        {
            if (matchRequest != null && matchRequest.Visible == false && matchRequest.sendrequestPressed)
            {
                SendMatchRequest(matchRequest.SendToUserName,matchRequest.pieceText,matchRequest.timerText);
            }
        }

        private void btnDeleteMail_Click(object sender, EventArgs e)
        {
            if (lbMail.SelectedIndex != -1)
            {
                if (!lbMail.SelectedItem.ToString().Contains("You have no messages"))
                    SendDeleteMail(lbMail.SelectedItem.ToString());
            }
            else
            {
                MessageBox.Show("Select a mail to delete");
            }
        }

        private void SendDeleteMail(string mailItem)
        {
            string[] mailItemsplit = mailItem.Split(' ');
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("deletemail " + mailItemsplit[1]);
            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
            SendListMail();
        }

        private void SendReadMail(string mailItem)
        {
            string[] mailItemsplit = mailItem.Split(' ');
            // sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("readmail " + mailItemsplit[1]);
            // Send the data through the socket.
            bytesSent = client.Send(msgToSend);
            SendListMail();
        }

        private void SendObserveGame(string gameItem)
        {

            string gameIndex = gameItem.Substring(gameItem.IndexOf('(') + 1, gameItem.IndexOf(')') - gameItem.IndexOf('(') - 1);

            //// sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("observe " + gameIndex);
            //// Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void SendUnObserveGame(string gameItem)
        {

            string gameIndex = gameItem.Substring(gameItem.IndexOf('(') + 1, gameItem.IndexOf(')') - gameItem.IndexOf('(') - 1);

            //// sending synchronously
            msgToSend = Encoding.ASCII.GetBytes("unobserve " + gameIndex);
            //// Send the data through the socket.
            bytesSent = client.Send(msgToSend);
        }

        private void btnRefreshMailBox_Click(object sender, EventArgs e)
        {
            SendListMail();
        }

        private void btnObserve_Click(object sender, EventArgs e)
        {
            if (lbGames.SelectedIndex != -1)
            {
                    SendObserveGame(lbGames.SelectedItem.ToString());
            }
            else
            {
                MessageBox.Show("Select a game to observe");
            }
        }



        private void btnUnobserve_Click(object sender, EventArgs e)
        {
            if (lbGames.SelectedIndex != -1)
            {
                SendUnObserveGame(lbGames.SelectedItem.ToString());
            }
            else
            {
                MessageBox.Show("Select a game to observe");
            }
        }

        private void btnMatch_Click(object sender, EventArgs e)
        {
            if (lbUsers.SelectedIndex != -1)
            {
                matchRequest = new MatchRequest();
                matchRequest.SendToUserName = lbUsers.SelectedItem.ToString();
                matchRequest.VisibleChanged += matchRequest_VisibleChanged;
                matchRequest.ShowDialog();
            }
            else
            {
                MessageBox.Show("Select a user from user list to play match.");
            }
        }

        private void btna1_Click(object sender, EventArgs e)
        {
            Button b = (Button)sender;
            SendMove(b.Text);
        }

        private void btnResign_Click(object sender, EventArgs e)
        {
            SendResign();
        }
    }

    // State object for receiving data from remote device.
    public class StateObject
    {
        // Client socket.
        public Socket workSocket = null;
        // Size of receive buffer.
        public const int BufferSize = 2048;
        // Receive buffer.
        public byte[] buffer = new byte[BufferSize];
        // Received data string.
        public StringBuilder sb = new StringBuilder();
    }
}
