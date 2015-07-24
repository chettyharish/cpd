using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TicTacToeClient
{
    public partial class MatchRequest : Form
    {
        private string sendToUserName = string.Empty;
        public string SendToUserName
        {
            get
            {
                return sendToUserName;
            }
            set {
                sendToUserName = value;
            }

        }

        public string pieceText
        {
            get
            {
                return txtPiece.Text;
            }
            set {
                txtPiece.Text = value;
            }
        }

        public string timerText
        {
            get
            {
                return txtTimer.Text;
            }
            set
            {
                txtTimer.Text = value;
            }
        }

        public string requestText
        {
            set {
                lblRequestText.Text = value;
            }
        }
        public bool sendrequestPressed =false;
        
        public MatchRequest()
        {
            InitializeComponent();
        }

        private void btnSendRequest_Click(object sender, EventArgs e)
        {
            sendrequestPressed = true;
            this.Visible = false;
        }
    }
}
