using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;


namespace TicTacToeClient
{
    public partial class LoginDialog : Form
    {

        public string ServerValue
        {
            get
            {
                return txtServer.Text;
            }
        }

        public string PortValue
        {
            get 
            {
                return txtPort.Text;
            }
        }

        public string UsernameValue
        {
            get {
                return txtUsername.Text; 
            }
        }

        public string PasswordValue
        {
            get {
                return txtPassword.Text;
            }
        }

        public LoginDialog()
        {
            InitializeComponent();
        }

        private void btnLogin_Click(object sender, EventArgs e)
        {
            this.Visible = false;    
            //StartClient();
        }

        // Data buffer for incoming data.
        byte[] bytes = new byte[2048];
        byte[] msgToSend;
        string strbytesRec = string.Empty;
        int bytesSent = 0;
        int bytesRec = 0;

        public void RegisterUser()
        {

            // Connect to a remote device.
            try
            {
                // Establish the remote endpoint for the socket.
                IPAddress ipAddress;

                if(string.IsNullOrEmpty(this.ServerValue))
                {
                    UpdateToolStrip("Server Cannot be empty");
                    return;
                }
                if(string.IsNullOrEmpty(this.PortValue))
                {
                    UpdateToolStrip("Port Cannot be empty");
                    return;
                }

                if (string.IsNullOrEmpty(this.UsernameValue))
                {
                    UpdateToolStrip("UserName Cannot be empty");
                    return;
                }
                if (string.IsNullOrEmpty(this.PasswordValue))
                {
                    UpdateToolStrip("Password Cannot be empty");
                    return;
                }

                bool isnumberip = IPAddress.TryParse(this.ServerValue,out ipAddress);

                if (!isnumberip)
                {
                    try
                    {
                        IPHostEntry ipHostInfo = Dns.GetHostEntry(this.ServerValue);
                        ipAddress = ipHostInfo.AddressList[0];
                    }
                    catch (Exception ex)
                    {
                        UpdateToolStrip("Server could not be found.");   
                        return;
                    }
                }
                IPEndPoint remoteEP = new IPEndPoint(ipAddress, int.Parse(this.PortValue));

                // Create a TCP/IP  socket.
                Socket sender = new Socket(AddressFamily.InterNetwork,
                    SocketType.Stream, ProtocolType.Tcp);

                // Connect the socket to the remote endpoint. Catch any errors.
                
                sender.Connect(remoteEP);
                UpdateToolStrip("Socket connected to " + sender.RemoteEndPoint.ToString());

                ReceiveFromServer(sender);
                if(string.Compare(strbytesRec,"<username>:",true,System.Globalization.CultureInfo.InvariantCulture)==0)
                {
                    // Encode the data string into a byte array.
                    msgToSend = Encoding.ASCII.GetBytes("\n");

                    // Send the data through the socket.
                    bytesSent = sender.Send(msgToSend);

                    ReceiveFromServer(sender);

                    if (strbytesRec.Contains("guest"))
                    {
                        msgToSend = Encoding.ASCII.GetBytes("register " + UsernameValue + " " + PasswordValue);

                        // Send the data through the socket.
                        bytesSent = sender.Send(msgToSend);

                        ReceiveFromServer(sender);
                        if (strbytesRec.Contains("already exists"))
                        {
                            // do nothing just print output
                        }
                        else
                        if (strbytesRec.Contains("guest") || strbytesRec.Contains("registered"))
                        {
                            strbytesRec = "User Registered Successfully.";
                        }
                    }
                }
                UpdateToolStripAndQuit(sender);
                
                //// Release the socket.
                //sender.Shutdown(SocketShutdown.Both);
                //sender.Close();
            }
            catch (Exception e)
            {
                UpdateToolStrip("Unable to connect to server");
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
        private void ReceiveFromServer(Socket sender)
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
            if(message.Length>200)
                toolStripStatusLabel1.Text = message.Substring(0,199);
            else
                toolStripStatusLabel1.Text = message;
            statusStrip.Refresh();
        }

        private void btnRegister_Click(object sender, EventArgs e)
        {
            RegisterUser();
        }

        private void LoginDialog_Load(object sender, EventArgs e)
        {
            this.AcceptButton = btnLogin;
        }

        //}
        //catch (ArgumentNullException ane)
        //{
        //    statusStrip.Text = "ArgumentNullException : " + ane.ToString();
        //}
        //catch (SocketException se)
        //{
        //    statusStrip.Text = "SocketException : " + se.ToString();
        //}
        //catch (Exception e)
        //{
        //    statusStrip.Text = "Unexpected exception : " + e.ToString();
        //}

     
    }
}
