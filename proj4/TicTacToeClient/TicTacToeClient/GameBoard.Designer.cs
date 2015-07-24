namespace TicTacToeClient
{
    partial class GameBoard
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.gbGameBox = new System.Windows.Forms.GroupBox();
            this.btnResign = new System.Windows.Forms.Button();
            this.btnc3 = new System.Windows.Forms.Button();
            this.btnc2 = new System.Windows.Forms.Button();
            this.btnc1 = new System.Windows.Forms.Button();
            this.btnb3 = new System.Windows.Forms.Button();
            this.btnb2 = new System.Windows.Forms.Button();
            this.btnb1 = new System.Windows.Forms.Button();
            this.btna3 = new System.Windows.Forms.Button();
            this.btna2 = new System.Windows.Forms.Button();
            this.btna1 = new System.Windows.Forms.Button();
            this.lblYourGame = new System.Windows.Forms.Label();
            this.txtYourMatch = new System.Windows.Forms.TextBox();
            this.lblObservedGames = new System.Windows.Forms.Label();
            this.txtObservedGames = new System.Windows.Forms.TextBox();
            this.gbChatWindow = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.rbKibitz = new System.Windows.Forms.RadioButton();
            this.btnDummy = new System.Windows.Forms.Button();
            this.rbTell = new System.Windows.Forms.RadioButton();
            this.rbShout = new System.Windows.Forms.RadioButton();
            this.txtChatResponse = new System.Windows.Forms.TextBox();
            this.lbChat = new System.Windows.Forms.ListBox();
            this.gbUsers = new System.Windows.Forms.GroupBox();
            this.btnMatch = new System.Windows.Forms.Button();
            this.btnStats = new System.Windows.Forms.Button();
            this.btnBlock = new System.Windows.Forms.Button();
            this.btnUnblock = new System.Windows.Forms.Button();
            this.btnRefreshUsers = new System.Windows.Forms.Button();
            this.lbUsers = new System.Windows.Forms.ListBox();
            this.gbActiveGames = new System.Windows.Forms.GroupBox();
            this.btnUnobserve = new System.Windows.Forms.Button();
            this.btnObserve = new System.Windows.Forms.Button();
            this.btnRefreshGames = new System.Windows.Forms.Button();
            this.lbGames = new System.Windows.Forms.ListBox();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.gbMySettings = new System.Windows.Forms.GroupBox();
            this.btnChangePassword = new System.Windows.Forms.Button();
            this.btnChangeInfo = new System.Windows.Forms.Button();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.txtInfo = new System.Windows.Forms.TextBox();
            this.btnNonQuiet = new System.Windows.Forms.Button();
            this.btnQuiet = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnRefreshMailBox = new System.Windows.Forms.Button();
            this.btnDeleteMail = new System.Windows.Forms.Button();
            this.btnComposeMail = new System.Windows.Forms.Button();
            this.btnReadMail = new System.Windows.Forms.Button();
            this.lbMail = new System.Windows.Forms.ListBox();
            this.gbGameBox.SuspendLayout();
            this.gbChatWindow.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.gbUsers.SuspendLayout();
            this.gbActiveGames.SuspendLayout();
            this.statusStrip.SuspendLayout();
            this.gbMySettings.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbGameBox
            // 
            this.gbGameBox.Controls.Add(this.btnResign);
            this.gbGameBox.Controls.Add(this.btnc3);
            this.gbGameBox.Controls.Add(this.btnc2);
            this.gbGameBox.Controls.Add(this.btnc1);
            this.gbGameBox.Controls.Add(this.btnb3);
            this.gbGameBox.Controls.Add(this.btnb2);
            this.gbGameBox.Controls.Add(this.btnb1);
            this.gbGameBox.Controls.Add(this.btna3);
            this.gbGameBox.Controls.Add(this.btna2);
            this.gbGameBox.Controls.Add(this.btna1);
            this.gbGameBox.Controls.Add(this.lblYourGame);
            this.gbGameBox.Controls.Add(this.txtYourMatch);
            this.gbGameBox.Controls.Add(this.lblObservedGames);
            this.gbGameBox.Controls.Add(this.txtObservedGames);
            this.gbGameBox.Location = new System.Drawing.Point(12, 12);
            this.gbGameBox.Name = "gbGameBox";
            this.gbGameBox.Size = new System.Drawing.Size(538, 252);
            this.gbGameBox.TabIndex = 0;
            this.gbGameBox.TabStop = false;
            this.gbGameBox.Text = "GameBox";
            // 
            // btnResign
            // 
            this.btnResign.Location = new System.Drawing.Point(193, 77);
            this.btnResign.Name = "btnResign";
            this.btnResign.Size = new System.Drawing.Size(82, 23);
            this.btnResign.TabIndex = 13;
            this.btnResign.Text = "Resign";
            this.btnResign.UseVisualStyleBackColor = true;
            this.btnResign.Click += new System.EventHandler(this.btnResign_Click);
            // 
            // btnc3
            // 
            this.btnc3.Location = new System.Drawing.Point(148, 78);
            this.btnc3.Name = "btnc3";
            this.btnc3.Size = new System.Drawing.Size(29, 23);
            this.btnc3.TabIndex = 12;
            this.btnc3.Text = "c3";
            this.btnc3.UseVisualStyleBackColor = true;
            this.btnc3.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btnc2
            // 
            this.btnc2.Location = new System.Drawing.Point(113, 78);
            this.btnc2.Name = "btnc2";
            this.btnc2.Size = new System.Drawing.Size(29, 23);
            this.btnc2.TabIndex = 11;
            this.btnc2.Text = "c2";
            this.btnc2.UseVisualStyleBackColor = true;
            this.btnc2.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btnc1
            // 
            this.btnc1.Location = new System.Drawing.Point(78, 78);
            this.btnc1.Name = "btnc1";
            this.btnc1.Size = new System.Drawing.Size(29, 23);
            this.btnc1.TabIndex = 10;
            this.btnc1.Text = "c1";
            this.btnc1.UseVisualStyleBackColor = true;
            this.btnc1.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btnb3
            // 
            this.btnb3.Location = new System.Drawing.Point(148, 49);
            this.btnb3.Name = "btnb3";
            this.btnb3.Size = new System.Drawing.Size(29, 23);
            this.btnb3.TabIndex = 9;
            this.btnb3.Text = "b3";
            this.btnb3.UseVisualStyleBackColor = true;
            this.btnb3.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btnb2
            // 
            this.btnb2.Location = new System.Drawing.Point(113, 49);
            this.btnb2.Name = "btnb2";
            this.btnb2.Size = new System.Drawing.Size(29, 23);
            this.btnb2.TabIndex = 8;
            this.btnb2.Text = "b2";
            this.btnb2.UseVisualStyleBackColor = true;
            this.btnb2.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btnb1
            // 
            this.btnb1.Location = new System.Drawing.Point(78, 49);
            this.btnb1.Name = "btnb1";
            this.btnb1.Size = new System.Drawing.Size(29, 23);
            this.btnb1.TabIndex = 7;
            this.btnb1.Text = "b1";
            this.btnb1.UseVisualStyleBackColor = true;
            this.btnb1.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btna3
            // 
            this.btna3.Location = new System.Drawing.Point(148, 20);
            this.btna3.Name = "btna3";
            this.btna3.Size = new System.Drawing.Size(29, 23);
            this.btna3.TabIndex = 6;
            this.btna3.Text = "a3";
            this.btna3.UseVisualStyleBackColor = true;
            this.btna3.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btna2
            // 
            this.btna2.Location = new System.Drawing.Point(113, 20);
            this.btna2.Name = "btna2";
            this.btna2.Size = new System.Drawing.Size(29, 23);
            this.btna2.TabIndex = 5;
            this.btna2.Text = "a2";
            this.btna2.UseVisualStyleBackColor = true;
            this.btna2.Click += new System.EventHandler(this.btna1_Click);
            // 
            // btna1
            // 
            this.btna1.Location = new System.Drawing.Point(78, 20);
            this.btna1.Name = "btna1";
            this.btna1.Size = new System.Drawing.Size(29, 23);
            this.btna1.TabIndex = 4;
            this.btna1.Text = "a1";
            this.btna1.UseVisualStyleBackColor = true;
            this.btna1.Click += new System.EventHandler(this.btna1_Click);
            // 
            // lblYourGame
            // 
            this.lblYourGame.AutoSize = true;
            this.lblYourGame.Location = new System.Drawing.Point(9, 16);
            this.lblYourGame.Name = "lblYourGame";
            this.lblYourGame.Size = new System.Drawing.Size(63, 13);
            this.lblYourGame.TabIndex = 3;
            this.lblYourGame.Text = "Your Game:";
            // 
            // txtYourMatch
            // 
            this.txtYourMatch.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtYourMatch.Location = new System.Drawing.Point(8, 111);
            this.txtYourMatch.Multiline = true;
            this.txtYourMatch.Name = "txtYourMatch";
            this.txtYourMatch.ReadOnly = true;
            this.txtYourMatch.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtYourMatch.Size = new System.Drawing.Size(267, 124);
            this.txtYourMatch.TabIndex = 2;
            // 
            // lblObservedGames
            // 
            this.lblObservedGames.AutoSize = true;
            this.lblObservedGames.Location = new System.Drawing.Point(260, 16);
            this.lblObservedGames.Name = "lblObservedGames";
            this.lblObservedGames.Size = new System.Drawing.Size(92, 13);
            this.lblObservedGames.TabIndex = 1;
            this.lblObservedGames.Text = "Observed Games:";
            // 
            // txtObservedGames
            // 
            this.txtObservedGames.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtObservedGames.Location = new System.Drawing.Point(281, 49);
            this.txtObservedGames.Multiline = true;
            this.txtObservedGames.Name = "txtObservedGames";
            this.txtObservedGames.ReadOnly = true;
            this.txtObservedGames.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtObservedGames.Size = new System.Drawing.Size(250, 186);
            this.txtObservedGames.TabIndex = 0;
            // 
            // gbChatWindow
            // 
            this.gbChatWindow.Controls.Add(this.groupBox2);
            this.gbChatWindow.Controls.Add(this.txtChatResponse);
            this.gbChatWindow.Controls.Add(this.lbChat);
            this.gbChatWindow.Location = new System.Drawing.Point(772, 12);
            this.gbChatWindow.Name = "gbChatWindow";
            this.gbChatWindow.Size = new System.Drawing.Size(265, 481);
            this.gbChatWindow.TabIndex = 1;
            this.gbChatWindow.TabStop = false;
            this.gbChatWindow.Text = "Chat Window";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbKibitz);
            this.groupBox2.Controls.Add(this.btnDummy);
            this.groupBox2.Controls.Add(this.rbTell);
            this.groupBox2.Controls.Add(this.rbShout);
            this.groupBox2.Location = new System.Drawing.Point(7, 432);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(250, 43);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Chat Options";
            // 
            // rbKibitz
            // 
            this.rbKibitz.AutoSize = true;
            this.rbKibitz.Location = new System.Drawing.Point(115, 19);
            this.rbKibitz.Name = "rbKibitz";
            this.rbKibitz.Size = new System.Drawing.Size(50, 17);
            this.rbKibitz.TabIndex = 3;
            this.rbKibitz.TabStop = true;
            this.rbKibitz.Text = "Kibitz";
            this.rbKibitz.UseVisualStyleBackColor = true;
            // 
            // btnDummy
            // 
            this.btnDummy.Location = new System.Drawing.Point(157, 0);
            this.btnDummy.Name = "btnDummy";
            this.btnDummy.Size = new System.Drawing.Size(75, 23);
            this.btnDummy.TabIndex = 2;
            this.btnDummy.Text = "btnDummy";
            this.btnDummy.UseVisualStyleBackColor = true;
            this.btnDummy.Visible = false;
            this.btnDummy.Click += new System.EventHandler(this.button1_Click);
            // 
            // rbTell
            // 
            this.rbTell.AutoSize = true;
            this.rbTell.Location = new System.Drawing.Point(66, 19);
            this.rbTell.Name = "rbTell";
            this.rbTell.Size = new System.Drawing.Size(42, 17);
            this.rbTell.TabIndex = 1;
            this.rbTell.TabStop = true;
            this.rbTell.Text = "Tell";
            this.rbTell.UseVisualStyleBackColor = true;
            // 
            // rbShout
            // 
            this.rbShout.AutoSize = true;
            this.rbShout.Checked = true;
            this.rbShout.Location = new System.Drawing.Point(6, 19);
            this.rbShout.Name = "rbShout";
            this.rbShout.Size = new System.Drawing.Size(53, 17);
            this.rbShout.TabIndex = 0;
            this.rbShout.TabStop = true;
            this.rbShout.Text = "Shout";
            this.rbShout.UseVisualStyleBackColor = true;
            // 
            // txtChatResponse
            // 
            this.txtChatResponse.Location = new System.Drawing.Point(6, 406);
            this.txtChatResponse.Name = "txtChatResponse";
            this.txtChatResponse.Size = new System.Drawing.Size(251, 20);
            this.txtChatResponse.TabIndex = 1;
            this.txtChatResponse.KeyUp += new System.Windows.Forms.KeyEventHandler(this.txtChatResponse_KeyUp);
            // 
            // lbChat
            // 
            this.lbChat.FormattingEnabled = true;
            this.lbChat.Location = new System.Drawing.Point(7, 20);
            this.lbChat.Name = "lbChat";
            this.lbChat.Size = new System.Drawing.Size(250, 381);
            this.lbChat.TabIndex = 0;
            // 
            // gbUsers
            // 
            this.gbUsers.Controls.Add(this.btnMatch);
            this.gbUsers.Controls.Add(this.btnStats);
            this.gbUsers.Controls.Add(this.btnBlock);
            this.gbUsers.Controls.Add(this.btnUnblock);
            this.gbUsers.Controls.Add(this.btnRefreshUsers);
            this.gbUsers.Controls.Add(this.lbUsers);
            this.gbUsers.Location = new System.Drawing.Point(556, 266);
            this.gbUsers.Name = "gbUsers";
            this.gbUsers.Size = new System.Drawing.Size(200, 263);
            this.gbUsers.TabIndex = 2;
            this.gbUsers.TabStop = false;
            this.gbUsers.Text = "Online Users";
            // 
            // btnMatch
            // 
            this.btnMatch.Location = new System.Drawing.Point(7, 232);
            this.btnMatch.Name = "btnMatch";
            this.btnMatch.Size = new System.Drawing.Size(187, 23);
            this.btnMatch.TabIndex = 5;
            this.btnMatch.Text = "Match";
            this.btnMatch.UseVisualStyleBackColor = true;
            this.btnMatch.Click += new System.EventHandler(this.btnMatch_Click);
            // 
            // btnStats
            // 
            this.btnStats.Location = new System.Drawing.Point(7, 173);
            this.btnStats.Name = "btnStats";
            this.btnStats.Size = new System.Drawing.Size(75, 23);
            this.btnStats.TabIndex = 4;
            this.btnStats.Text = "Stats";
            this.btnStats.UseVisualStyleBackColor = true;
            this.btnStats.Click += new System.EventHandler(this.btnStats_Click);
            // 
            // btnBlock
            // 
            this.btnBlock.Location = new System.Drawing.Point(7, 203);
            this.btnBlock.Name = "btnBlock";
            this.btnBlock.Size = new System.Drawing.Size(75, 23);
            this.btnBlock.TabIndex = 3;
            this.btnBlock.Text = "Block";
            this.btnBlock.UseVisualStyleBackColor = true;
            this.btnBlock.Click += new System.EventHandler(this.btnBlock_Click);
            // 
            // btnUnblock
            // 
            this.btnUnblock.Location = new System.Drawing.Point(119, 203);
            this.btnUnblock.Name = "btnUnblock";
            this.btnUnblock.Size = new System.Drawing.Size(75, 23);
            this.btnUnblock.TabIndex = 2;
            this.btnUnblock.Text = "Unblock";
            this.btnUnblock.UseVisualStyleBackColor = true;
            this.btnUnblock.Click += new System.EventHandler(this.btnUnblock_Click);
            // 
            // btnRefreshUsers
            // 
            this.btnRefreshUsers.Location = new System.Drawing.Point(119, 173);
            this.btnRefreshUsers.Name = "btnRefreshUsers";
            this.btnRefreshUsers.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshUsers.TabIndex = 1;
            this.btnRefreshUsers.Text = "Refresh";
            this.btnRefreshUsers.UseVisualStyleBackColor = true;
            this.btnRefreshUsers.Click += new System.EventHandler(this.btnRefreshUsers_Click);
            // 
            // lbUsers
            // 
            this.lbUsers.FormattingEnabled = true;
            this.lbUsers.Location = new System.Drawing.Point(7, 20);
            this.lbUsers.Name = "lbUsers";
            this.lbUsers.Size = new System.Drawing.Size(187, 147);
            this.lbUsers.TabIndex = 0;
            // 
            // gbActiveGames
            // 
            this.gbActiveGames.Controls.Add(this.btnUnobserve);
            this.gbActiveGames.Controls.Add(this.btnObserve);
            this.gbActiveGames.Controls.Add(this.btnRefreshGames);
            this.gbActiveGames.Controls.Add(this.lbGames);
            this.gbActiveGames.Location = new System.Drawing.Point(556, 12);
            this.gbActiveGames.Name = "gbActiveGames";
            this.gbActiveGames.Size = new System.Drawing.Size(200, 235);
            this.gbActiveGames.TabIndex = 3;
            this.gbActiveGames.TabStop = false;
            this.gbActiveGames.Text = "Current Active Games";
            // 
            // btnUnobserve
            // 
            this.btnUnobserve.Location = new System.Drawing.Point(7, 204);
            this.btnUnobserve.Name = "btnUnobserve";
            this.btnUnobserve.Size = new System.Drawing.Size(75, 23);
            this.btnUnobserve.TabIndex = 3;
            this.btnUnobserve.Text = "Unobserve";
            this.btnUnobserve.UseVisualStyleBackColor = true;
            this.btnUnobserve.Click += new System.EventHandler(this.btnUnobserve_Click);
            // 
            // btnObserve
            // 
            this.btnObserve.Location = new System.Drawing.Point(7, 174);
            this.btnObserve.Name = "btnObserve";
            this.btnObserve.Size = new System.Drawing.Size(75, 23);
            this.btnObserve.TabIndex = 2;
            this.btnObserve.Text = "Observe";
            this.btnObserve.UseVisualStyleBackColor = true;
            this.btnObserve.Click += new System.EventHandler(this.btnObserve_Click);
            // 
            // btnRefreshGames
            // 
            this.btnRefreshGames.Location = new System.Drawing.Point(119, 174);
            this.btnRefreshGames.Name = "btnRefreshGames";
            this.btnRefreshGames.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshGames.TabIndex = 1;
            this.btnRefreshGames.Text = "Refresh";
            this.btnRefreshGames.UseVisualStyleBackColor = true;
            this.btnRefreshGames.Click += new System.EventHandler(this.btnRefreshGames_Click);
            // 
            // lbGames
            // 
            this.lbGames.FormattingEnabled = true;
            this.lbGames.Location = new System.Drawing.Point(7, 20);
            this.lbGames.Name = "lbGames";
            this.lbGames.Size = new System.Drawing.Size(187, 147);
            this.lbGames.TabIndex = 0;
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.statusStrip.Location = new System.Drawing.Point(0, 532);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(1047, 22);
            this.statusStrip.TabIndex = 4;
            this.statusStrip.Text = "statusStrip";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 17);
            // 
            // gbMySettings
            // 
            this.gbMySettings.Controls.Add(this.btnChangePassword);
            this.gbMySettings.Controls.Add(this.btnChangeInfo);
            this.gbMySettings.Controls.Add(this.txtPassword);
            this.gbMySettings.Controls.Add(this.txtInfo);
            this.gbMySettings.Controls.Add(this.btnNonQuiet);
            this.gbMySettings.Controls.Add(this.btnQuiet);
            this.gbMySettings.Location = new System.Drawing.Point(12, 422);
            this.gbMySettings.Name = "gbMySettings";
            this.gbMySettings.Size = new System.Drawing.Size(538, 100);
            this.gbMySettings.TabIndex = 5;
            this.gbMySettings.TabStop = false;
            this.gbMySettings.Text = "My Settings";
            // 
            // btnChangePassword
            // 
            this.btnChangePassword.Location = new System.Drawing.Point(424, 48);
            this.btnChangePassword.Name = "btnChangePassword";
            this.btnChangePassword.Size = new System.Drawing.Size(107, 23);
            this.btnChangePassword.TabIndex = 5;
            this.btnChangePassword.Text = "Change Password";
            this.btnChangePassword.UseVisualStyleBackColor = true;
            this.btnChangePassword.Click += new System.EventHandler(this.btnChangePassword_Click);
            // 
            // btnChangeInfo
            // 
            this.btnChangeInfo.Location = new System.Drawing.Point(424, 17);
            this.btnChangeInfo.Name = "btnChangeInfo";
            this.btnChangeInfo.Size = new System.Drawing.Size(75, 23);
            this.btnChangeInfo.TabIndex = 4;
            this.btnChangeInfo.Text = "Change Info";
            this.btnChangeInfo.UseVisualStyleBackColor = true;
            this.btnChangeInfo.Click += new System.EventHandler(this.btnChangeInfo_Click);
            // 
            // txtPassword
            // 
            this.txtPassword.Location = new System.Drawing.Point(99, 53);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.PasswordChar = '*';
            this.txtPassword.Size = new System.Drawing.Size(308, 20);
            this.txtPassword.TabIndex = 3;
            // 
            // txtInfo
            // 
            this.txtInfo.Location = new System.Drawing.Point(99, 20);
            this.txtInfo.Name = "txtInfo";
            this.txtInfo.Size = new System.Drawing.Size(308, 20);
            this.txtInfo.TabIndex = 2;
            // 
            // btnNonQuiet
            // 
            this.btnNonQuiet.Location = new System.Drawing.Point(7, 50);
            this.btnNonQuiet.Name = "btnNonQuiet";
            this.btnNonQuiet.Size = new System.Drawing.Size(75, 23);
            this.btnNonQuiet.TabIndex = 1;
            this.btnNonQuiet.Text = "Nonquiet";
            this.btnNonQuiet.UseVisualStyleBackColor = true;
            this.btnNonQuiet.Click += new System.EventHandler(this.btnNonQuiet_Click);
            // 
            // btnQuiet
            // 
            this.btnQuiet.Location = new System.Drawing.Point(7, 20);
            this.btnQuiet.Name = "btnQuiet";
            this.btnQuiet.Size = new System.Drawing.Size(75, 23);
            this.btnQuiet.TabIndex = 0;
            this.btnQuiet.Text = "Quiet";
            this.btnQuiet.UseVisualStyleBackColor = true;
            this.btnQuiet.Click += new System.EventHandler(this.btnQuiet_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnRefreshMailBox);
            this.groupBox3.Controls.Add(this.btnDeleteMail);
            this.groupBox3.Controls.Add(this.btnComposeMail);
            this.groupBox3.Controls.Add(this.btnReadMail);
            this.groupBox3.Controls.Add(this.lbMail);
            this.groupBox3.Location = new System.Drawing.Point(18, 270);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(532, 143);
            this.groupBox3.TabIndex = 6;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Mail";
            // 
            // btnRefreshMailBox
            // 
            this.btnRefreshMailBox.Location = new System.Drawing.Point(288, 107);
            this.btnRefreshMailBox.Name = "btnRefreshMailBox";
            this.btnRefreshMailBox.Size = new System.Drawing.Size(75, 23);
            this.btnRefreshMailBox.TabIndex = 7;
            this.btnRefreshMailBox.Text = "Refresh";
            this.btnRefreshMailBox.UseVisualStyleBackColor = true;
            this.btnRefreshMailBox.Click += new System.EventHandler(this.btnRefreshMailBox_Click);
            // 
            // btnDeleteMail
            // 
            this.btnDeleteMail.Location = new System.Drawing.Point(369, 107);
            this.btnDeleteMail.Name = "btnDeleteMail";
            this.btnDeleteMail.Size = new System.Drawing.Size(75, 23);
            this.btnDeleteMail.TabIndex = 6;
            this.btnDeleteMail.Text = "Delete";
            this.btnDeleteMail.UseVisualStyleBackColor = true;
            this.btnDeleteMail.Click += new System.EventHandler(this.btnDeleteMail_Click);
            // 
            // btnComposeMail
            // 
            this.btnComposeMail.Location = new System.Drawing.Point(6, 107);
            this.btnComposeMail.Name = "btnComposeMail";
            this.btnComposeMail.Size = new System.Drawing.Size(75, 23);
            this.btnComposeMail.TabIndex = 5;
            this.btnComposeMail.Text = "Compose";
            this.btnComposeMail.UseVisualStyleBackColor = true;
            this.btnComposeMail.Click += new System.EventHandler(this.btnSendMail_Click);
            // 
            // btnReadMail
            // 
            this.btnReadMail.Location = new System.Drawing.Point(450, 107);
            this.btnReadMail.Name = "btnReadMail";
            this.btnReadMail.Size = new System.Drawing.Size(75, 23);
            this.btnReadMail.TabIndex = 1;
            this.btnReadMail.Text = "Read";
            this.btnReadMail.UseVisualStyleBackColor = true;
            this.btnReadMail.Click += new System.EventHandler(this.btnReadMail_Click);
            // 
            // lbMail
            // 
            this.lbMail.FormattingEnabled = true;
            this.lbMail.Location = new System.Drawing.Point(6, 19);
            this.lbMail.Name = "lbMail";
            this.lbMail.ScrollAlwaysVisible = true;
            this.lbMail.Size = new System.Drawing.Size(519, 82);
            this.lbMail.TabIndex = 0;
            // 
            // GameBoard
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1047, 554);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.gbMySettings);
            this.Controls.Add(this.statusStrip);
            this.Controls.Add(this.gbActiveGames);
            this.Controls.Add(this.gbUsers);
            this.Controls.Add(this.gbChatWindow);
            this.Controls.Add(this.gbGameBox);
            this.Name = "GameBoard";
            this.Text = "Tic Tac Toe";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.GameBoard_FormClosed);
            this.Load += new System.EventHandler(this.GameBoard_Load);
            this.gbGameBox.ResumeLayout(false);
            this.gbGameBox.PerformLayout();
            this.gbChatWindow.ResumeLayout(false);
            this.gbChatWindow.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.gbUsers.ResumeLayout(false);
            this.gbActiveGames.ResumeLayout(false);
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.gbMySettings.ResumeLayout(false);
            this.gbMySettings.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox gbGameBox;
        private System.Windows.Forms.GroupBox gbChatWindow;
        private System.Windows.Forms.ListBox lbChat;
        private System.Windows.Forms.GroupBox gbUsers;
        private System.Windows.Forms.ListBox lbUsers;
        private System.Windows.Forms.GroupBox gbActiveGames;
        private System.Windows.Forms.TextBox txtChatResponse;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton rbShout;
        private System.Windows.Forms.Button btnRefreshUsers;
        private System.Windows.Forms.Button btnRefreshGames;
        private System.Windows.Forms.ListBox lbGames;
        private System.Windows.Forms.RadioButton rbTell;
        private System.Windows.Forms.Button btnDummy;
        private System.Windows.Forms.Button btnBlock;
        private System.Windows.Forms.Button btnUnblock;
        private System.Windows.Forms.Button btnStats;
        private System.Windows.Forms.GroupBox gbMySettings;
        private System.Windows.Forms.Button btnChangePassword;
        private System.Windows.Forms.Button btnChangeInfo;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.TextBox txtInfo;
        private System.Windows.Forms.Button btnNonQuiet;
        private System.Windows.Forms.Button btnQuiet;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ListBox lbMail;
        private System.Windows.Forms.Button btnReadMail;
        private System.Windows.Forms.Button btnComposeMail;
        private System.Windows.Forms.Button btnDeleteMail;
        private System.Windows.Forms.Button btnRefreshMailBox;
        private System.Windows.Forms.Button btnUnobserve;
        private System.Windows.Forms.Button btnObserve;
        private System.Windows.Forms.Label lblObservedGames;
        private System.Windows.Forms.TextBox txtObservedGames;
        private System.Windows.Forms.RadioButton rbKibitz;
        private System.Windows.Forms.Button btnMatch;
        private System.Windows.Forms.TextBox txtYourMatch;
        private System.Windows.Forms.Label lblYourGame;
        private System.Windows.Forms.Button btnResign;
        private System.Windows.Forms.Button btnc3;
        private System.Windows.Forms.Button btnc2;
        private System.Windows.Forms.Button btnc1;
        private System.Windows.Forms.Button btnb3;
        private System.Windows.Forms.Button btnb2;
        private System.Windows.Forms.Button btnb1;
        private System.Windows.Forms.Button btna3;
        private System.Windows.Forms.Button btna2;
        private System.Windows.Forms.Button btna1;
    }
}