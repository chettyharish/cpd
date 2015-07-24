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
    public partial class ComposeMail : Form
    {
        public ComposeMail()
        {
            InitializeComponent();
        }

        public string mailText {
            get
            {
                return txtMail.Text;
            }
        }

        public string receipientText
        {
            get
            {
                return txtReceipient.Text;
            }
        }

        public string subjectText
        {
            get
            {
                return txtSubject.Text;
            }
        }

        public bool sendmailpressed = false;

        private void ComposeMail_Load(object sender, EventArgs e)
        {
            txtMail.Text = string.Empty;
            txtReceipient.Text = string.Empty;
            txtSubject.Text = string.Empty;
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            sendmailpressed = true;
            this.Visible = false;
        }
    }
}
