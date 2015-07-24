namespace TicTacToeClient
{
    partial class MatchRequest
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
            this.lblPiece = new System.Windows.Forms.Label();
            this.txtPiece = new System.Windows.Forms.TextBox();
            this.lblTimer = new System.Windows.Forms.Label();
            this.txtTimer = new System.Windows.Forms.TextBox();
            this.btnSendRequest = new System.Windows.Forms.Button();
            this.lblRequestText = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lblPiece
            // 
            this.lblPiece.AutoSize = true;
            this.lblPiece.Location = new System.Drawing.Point(96, 49);
            this.lblPiece.Name = "lblPiece";
            this.lblPiece.Size = new System.Drawing.Size(62, 13);
            this.lblPiece.TabIndex = 1;
            this.lblPiece.Text = "Piece(b/w):";
            // 
            // txtPiece
            // 
            this.txtPiece.Location = new System.Drawing.Point(176, 46);
            this.txtPiece.Name = "txtPiece";
            this.txtPiece.Size = new System.Drawing.Size(44, 20);
            this.txtPiece.TabIndex = 2;
            this.txtPiece.Text = "b";
            // 
            // lblTimer
            // 
            this.lblTimer.AutoSize = true;
            this.lblTimer.Location = new System.Drawing.Point(96, 82);
            this.lblTimer.Name = "lblTimer";
            this.lblTimer.Size = new System.Drawing.Size(59, 13);
            this.lblTimer.TabIndex = 3;
            this.lblTimer.Text = "Timer(sec):";
            // 
            // txtTimer
            // 
            this.txtTimer.Location = new System.Drawing.Point(176, 82);
            this.txtTimer.Name = "txtTimer";
            this.txtTimer.Size = new System.Drawing.Size(44, 20);
            this.txtTimer.TabIndex = 4;
            this.txtTimer.Text = "600";
            // 
            // btnSendRequest
            // 
            this.btnSendRequest.Location = new System.Drawing.Point(99, 116);
            this.btnSendRequest.Name = "btnSendRequest";
            this.btnSendRequest.Size = new System.Drawing.Size(121, 23);
            this.btnSendRequest.TabIndex = 5;
            this.btnSendRequest.Text = "Send Request";
            this.btnSendRequest.UseVisualStyleBackColor = true;
            this.btnSendRequest.Click += new System.EventHandler(this.btnSendRequest_Click);
            // 
            // lblRequestText
            // 
            this.lblRequestText.AutoSize = true;
            this.lblRequestText.Location = new System.Drawing.Point(27, 13);
            this.lblRequestText.Name = "lblRequestText";
            this.lblRequestText.Size = new System.Drawing.Size(0, 13);
            this.lblRequestText.TabIndex = 6;
            // 
            // MatchRequest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(314, 161);
            this.Controls.Add(this.lblRequestText);
            this.Controls.Add(this.btnSendRequest);
            this.Controls.Add(this.txtTimer);
            this.Controls.Add(this.lblTimer);
            this.Controls.Add(this.txtPiece);
            this.Controls.Add(this.lblPiece);
            this.Name = "MatchRequest";
            this.Text = "MatchRequest";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblPiece;
        private System.Windows.Forms.TextBox txtPiece;
        private System.Windows.Forms.Label lblTimer;
        private System.Windows.Forms.TextBox txtTimer;
        private System.Windows.Forms.Button btnSendRequest;
        private System.Windows.Forms.Label lblRequestText;
    }
}