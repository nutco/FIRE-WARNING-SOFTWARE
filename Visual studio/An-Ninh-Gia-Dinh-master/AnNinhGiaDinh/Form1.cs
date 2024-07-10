//using WebSocketSharp;
using Newtonsoft.Json.Linq;
using WebSocketSharp;

namespace AnNinhGiaDinh
{
    public partial class Form1 : Form
    {
        bool isBtnTurnOnBuzzer = false;
        bool isGasSensor1 = false;
        bool isGasSensor2 = false;
        bool isBuzzerCry = false;
        bool isbtnSystemOn = true;

        string url = "127.0.0.1:4000";
        WebSocket ws;

        public Form1()
        {
            InitializeComponent();
            ws = new WebSocket($"ws://{this.url}/ws");
            ws.OnOpen += Ws_OnOpen;
            ws.OnMessage += Ws_OnMessage;
            ws.Connect();
            //ws.Send("Hello World!");
            ChangeText();
        }

        public void ChangeText()
        {
            if (isBtnTurnOnBuzzer)
            {
                lbliTurnOnBuzzer.Text = "Cảnh Báo: có cháy";
                btnTurnOnBuzzer.ForeColor = Color.Yellow;
                btnTurnOnBuzzer.BackColor = Color.Red;
                lbliTurnOnBuzzer.ForeColor = Color.Red;
            }
            else
            {
                lbliTurnOnBuzzer.Text = "nút cảnh báo cháy đang tắt";
                btnTurnOnBuzzer.ForeColor = Color.Black;
                btnTurnOnBuzzer.BackColor = Color.LimeGreen;
                lbliTurnOnBuzzer.ForeColor = Color.Green;
            }

            if (isGasSensor1)
            {
                lblGasSensor1.Text = "ối dồi ôi cháy phòng 1";
                lblGasSensor1.ForeColor = Color.Red;
            }
            else
            {
                lblGasSensor1.Text = "phòng 1 không khí vẫn trong lành";
                lblGasSensor1.ForeColor = Color.Green;
            }
            if (isGasSensor2)
            {
                lblGasSensor2.Text = "ối dồi ôi cháy phòng 2";
                lblGasSensor2.ForeColor = Color.Red;
            }
            else
            {
                lblGasSensor2.Text = "phòng 2 không khí vẫn trong lành";
                lblGasSensor2.ForeColor = Color.Green;
            }
            if (isBuzzerCry)
            {
                lblbuzzerCry.Text = "KÊU";
                lblbuzzerCry.ForeColor = Color.Red;

            }
            else
            {
                lblbuzzerCry.Text = "IM";
                lblbuzzerCry.ForeColor = Color.Green;

            }

            if (isbtnSystemOn)
            {
                lblSystemOn.Text = "hệ thống đang được bật";
                lblSystemOn.ForeColor = Color.Green;

            }
            else
            {
                lblSystemOn.Text = "hệ thống đang được bật";
                lblSystemOn.ForeColor = Color.Red;
            }

        }

        private void Ws_OnOpen(object? sender, EventArgs e)
        {
            MessageBox.Show("Connection");
        }
        void showcontent(string abc)
        {
            lblContent.Text = abc;
        }
        private void Ws_OnMessage(object? sender, MessageEventArgs e)
        {
            //MessageBox.Show(e.Data);

            Invoke(() => showcontent(e.Data));
            JObject jsonObject = JObject.Parse(e.Data);

            if (jsonObject["command"] != null)
            {
                string content = jsonObject["command"].ToString();
                if (content == "SWITCH")
                {
                    isBtnTurnOnBuzzer = !isBtnTurnOnBuzzer;

                }
            }
            else
            {
                //JObject jsonObject = JObject.Parse(e.Data);

                if (jsonObject != null)
                {
                    isBtnTurnOnBuzzer = (bool)jsonObject["isBtnTurnOnBuzzer"];
                    isGasSensor1 = (bool)jsonObject["isGasSensor1"];
                    isGasSensor2 = (bool)jsonObject["isGasSensor1"];
                    isBuzzerCry = (bool)jsonObject["isBuzzerCry"];
                    isbtnSystemOn = (bool)jsonObject["btnSystemOn"];

                }
            }
            Invoke(() => ChangeText());

        }
        int count_changecolor = 0;
        private void btnTurnOnBuzzer_Click_1(object sender, EventArgs e)
        {
            if (count_changecolor == 0)
            {
                btnTurnOnBuzzer.BackColor = Color.Red;
                count_changecolor++;
            }
            else
            {
                btnTurnOnBuzzer.BackColor = Color.LimeGreen;
                count_changecolor = 0;
            }
            ws = new WebSocket($"ws://{this.url}/ws");

            ws.OnOpen += (sender, args) =>
            {
                ws.Send("SWITCH");
            };

            ws.Connect();
        }
    }
}