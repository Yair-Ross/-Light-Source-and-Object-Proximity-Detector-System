using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using System.IO.Ports;
using System.Windows.Documents;
using Microsoft.Win32;
using System.IO;
using System.Text.RegularExpressions;
using System.Collections.Specialized;

namespace DCSProject.Net
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region General Declaration

        private delegate void AddGraphics();
        private delegate void connectionParser(string data);
        private delegate void updateScript(ListBoxItem item, string filename);
        SerialPort comPort = new SerialPort();
        public static double lineLength;
        private Thread trdBGRunner;
        Thread updateIO;
        private int radarAngle;
        private double distance = 0, distance2 = 0;
        private int radarMask = 50;
        bool turnLeft = true, ComMode = false;

        List<Label> lblDistanceList = new List<Label>(), lblAngleList = new List<Label>();
        List<Line> staticLineList = new List<Line>();
        List<System.Windows.Shapes.Path> staticPathList = new List<System.Windows.Shapes.Path>();
        List<Line> radarRadius = new List<Line>();
        List<int> angleList = new List<int>();

        #endregion

        #region Auto Generated Code

        public MainWindow()
        {
            InitializeComponent();
            radarAngle = 0;
            //this.Icon =(System.Drawing.Icon)Properties.Resources.icon;
        }

        #endregion

        #region Thread's Functions/Procedures

        public void Run()
        {
            trdBGRunner = new Thread(new ThreadStart(CursorRunner));
            trdBGRunner.IsBackground = true;
            trdBGRunner.Start();
            this.Dispatcher.Invoke(new AddGraphics(radarCursorAngleChange));
            updateIO = new Thread(new ThreadStart(UpdateSerial));
            updateIO.IsBackground = true;
            updateIO.Start();
        }

        private void UpdateScript(ListBoxItem item, string filename)
        {
            try
            {
                item.Content = filename;
            }
            catch { }
        }

        private void UpdateSerial()
        {
            string received = string.Empty;
            string[] receivedData;
            int opcode;
            string strAngle = string.Empty, strDistance = string.Empty;
            while (true)
            {
                try
                {
                    if (comPort.IsOpen)
                    {
                        txtConnectionStatus.Dispatcher.Invoke(new connectionParser(display_status), "Connected");
                        received = comPort.ReadLine();
                        receivedData = received.Split('.');
                        if(receivedData.Length > 0 && int.TryParse(receivedData[0], out opcode))
                        {
                            switch(opcode)
                            {
                                case 1:
                                case 2:
                                case 3:
                                case 4:
                                    if (receivedData.Length >= 3)
                                    {
                                        distance = 0;
                                        distance2 = 0;
                                        txtSerialInput.Dispatcher.Invoke(new connectionParser(display_input), received);
                                        comPort.DiscardOutBuffer();
                                        strAngle = receivedData[1];
                                        radarAngle = Convert.ToInt32(strAngle, 16);
                                        if (opcode == 1 || opcode == 2 || opcode == 4)
                                        {
                                            strDistance = receivedData[2];
                                            distance = 0.01 * (Convert.ToInt32(strDistance, 16) / 58);
                                        }
                                        if(opcode == 3 || opcode == 4)
                                        {
                                            strDistance = receivedData[opcode - 1];
                                            distance2 = 0.01 * (Convert.ToInt32(strDistance, 16));
                                        }
                                        received = string.Empty;

                                    }
                                    else
                                    {

                                        txtSerialInput.Dispatcher.Invoke(new connectionParser(display_input), "Bad Data!");
                                    }
                                    break;
                                case 6:
                                    string[] filenames = { "/NOSCRIPT", "/NOSCRIPT", "/NOSCRIPT" };
                                    for(int i=1; i<receivedData.Length-1; i+=2)
                                    {
                                        int filenum;
                                        if (int.TryParse(receivedData[i], out filenum) && filenum >= 0 && filenum < filenames.Length)
                                            filenames[filenum] = receivedData[i + 1];
                                    }
                                    list_script0item.Dispatcher.Invoke(new updateScript(UpdateScript), list_script0item, filenames[0]);
                                    list_script1item.Dispatcher.Invoke(new updateScript(UpdateScript), list_script1item, filenames[1]);
                                    list_script2item.Dispatcher.Invoke(new updateScript(UpdateScript), list_script2item, filenames[2]);
                                    break;
                                default:
                                    break;
                            }
                        }

                    }
                    else
                    {
                        txtSerialInput.Dispatcher.Invoke(new connectionParser(display_input), "Connection N/A");
                        //txtConnectionStatus.Dispatcher.Invoke(new connectionParser(display_status), "Disconnected");
                    }
                    Thread.Sleep(25);
                    ComMode = (comPort.IsOpen);
                }
                catch (Exception)
                {
                    //throw;
                }
            }
        }

        private void CursorRunner()
        {
            try
            {
                while (true)
                {
                    this.Dispatcher.Invoke(new AddGraphics(radarCursorAngleChange));
                    Thread.Sleep(50);
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        #endregion

        #region Drawing Functions

        private void drawArcPath()
        {
            foreach (System.Windows.Shapes.Path path in staticPathList)
            {
                lock (this)
                {
                    this.DrawCanvas.Children.Add(path);
                }
            }
            foreach (Label lbl in lblDistanceList)
            {
                lock (this)
                {
                    this.DrawCanvas.Children.Add(lbl);
                }
            }

        }

        private void drawStaticLines()
        {
            foreach (Line line1 in staticLineList)
            {
                lock (this)
                {
                    this.DrawCanvas.Children.Add(line1);
                }
            }
            foreach (Label lbl in lblAngleList)
            {
                lock (this)
                {
                    this.DrawCanvas.Children.Add(lbl);
                }
            }
        }

        public void drawStaticGraphics()
        {
            lock (this)
            {
                this.DrawCanvas.Children.Clear();
            }

            drawArcPath();
            drawStaticLines();
        }

        private void drawRadarRadius()
        {
            switch (ComMode)
            {
                case false:
                    if (radarAngle < 180 && turnLeft) radarAngle++;
                    else if (radarAngle >= 180 || !turnLeft)
                    {
                        turnLeft = (radarAngle <= 1);
                        radarAngle--;
                    }
                    break;
                default:
                    break;
            }
            angleList[0] = radarAngle;
            for (int i = 30; i >= 1; i--)
            {
                angleList[i] = angleList[i - 1];
            }
            for (int i = 0; i <= 30; i++)
            {
                lock (this)
                {
                    if (radarRadius[i] != null)
                    {
                        this.DrawCanvas.Children.Remove(radarRadius[i]);
                    }

                    radarRadius[i].Opacity = 0.7 - 0.02 * i;
                    radarRadius[i].X2 = lineLength + Math.Cos(angleList[i] * Math.PI / 180) * (lineLength - 30);
                    radarRadius[i].Y2 = lineLength - Math.Sin(angleList[i] * Math.PI / 180) * (lineLength - 30);

                    this.DrawCanvas.Children.Add(radarRadius[i]);
                }
            }
            lblAngle.Content = "Radar Angle : " + angleList[0] + "°";
            lblDistance.Content = "Object Distance : " + distance + " m";
            lblDistance2.Content = "Light Distance : " + distance2 + " m";
        }

        private void printAtia(double dist, int angle, Brush color)
        {
            if (dist != 0)
            {
                double scaleDistance;
                Point p = new Point();
                scaleDistance = map(0, radarMask / 100.0, 0, lineLength - 30, dist);
                p.X = lineLength + Math.Cos(angle * Math.PI / 180) * scaleDistance;
                p.Y = lineLength - Math.Sin(angle * Math.PI / 180) * scaleDistance;
                Atia atia = new Atia(this, p.X, p.Y, color);
                atia.Run(true);
            }
        }

        private void printAtiaIfColizionExist()
        {
            if (!ComMode)
            {
                Point p = Mouse.GetPosition(DrawCanvas);

                if (Math.Round(((p.Y - DrawCanvas.ActualHeight) / (p.X - DrawCanvas.ActualHeight)), 1) ==
                    Math.Round(((radarRadius[0].Y2 - DrawCanvas.ActualHeight) / (radarRadius[0].X2 - DrawCanvas.ActualHeight)), 1)
                    || ((int)p.X == (int)DrawCanvas.ActualHeight && (int)radarRadius[0].X2 == (int)DrawCanvas.ActualHeight))
                {
                    Atia atia = new Atia(this, p.X, p.Y, Brushes.Blue);
                    atia.Run(false);
                }
            }
            else
            {

            }

        }

        #endregion

        #region Lists init

        private void radarRadiusInit()
        {
            for (int i = 0; i <= 30; i++)
            {
                radarRadius.Add(new Line());
                radarRadius[i].SnapsToDevicePixels = true;
                radarRadius[i].SetValue(RenderOptions.EdgeModeProperty, EdgeMode.Aliased);
                radarRadius[i].Visibility = Visibility.Visible;
                radarRadius[i].StrokeThickness = 5;
                radarRadius[i].Stroke = Brushes.LimeGreen;
                radarRadius[i].X1 = DrawCanvas.ActualWidth / 2;
                radarRadius[i].Y1 = DrawCanvas.ActualHeight;
                radarRadius[i].X2 = lineLength + Math.Cos(radarAngle * Math.PI / 180) * (lineLength - 20);
                radarRadius[i].Y2 = lineLength - Math.Sin(radarAngle * Math.PI / 180) * (lineLength - 20);
                angleList.Add(0);
            }
        }

        private void staticGraphInit()
        {
            double dblArcSizeDiv = lineLength / 10;
            double actualAngle = 0, outerArcRadius = lineLength + 2;
            radarMask = (int)slider_range.Value;
            int intDistanceCounter = radarMask, intLblCounter = 0;
            int radarRadiusDelta = intDistanceCounter / 10;
            PathFigure ArcFigure = new PathFigure();

            if (DrawCanvas.ActualHeight > 0)
            {
                for (int i = 0; i <= 180; i++)
                {
                    staticLineList.Add(new Line());
                    staticLineList[i].SnapsToDevicePixels = true;
                    staticLineList[i].SetValue(RenderOptions.EdgeModeProperty, EdgeMode.Aliased);
                    staticLineList[i].Visibility = Visibility.Visible;
                    if (i == 0 || i == 90 || i == 180)
                    {
                        staticLineList[i].StrokeThickness = 5;
                        staticLineList[i].Opacity = 0.7;
                        staticLineList[i].X1 = DrawCanvas.ActualWidth / 2;
                        staticLineList[i].Y1 = DrawCanvas.ActualHeight;
                    }
                    else if (i % 10 == 0)
                    {
                        staticLineList[i].X1 = DrawCanvas.ActualWidth / 2;
                        staticLineList[i].Y1 = DrawCanvas.ActualHeight;
                        staticLineList[i].StrokeThickness = 2;
                        staticLineList[i].Opacity = 0.5;
                    }
                    else if (i % 5 == 0)
                    {
                        staticLineList[i].X1 = lineLength + Math.Cos(actualAngle * Math.PI / 180) * (lineLength - 30);
                        staticLineList[i].Y1 = lineLength - Math.Sin(actualAngle * Math.PI / 180) * (lineLength - 30);
                        staticLineList[i].StrokeThickness = 2;
                        staticLineList[i].Opacity = 0.5;
                    }
                    else
                    {
                        staticLineList[i].X1 = lineLength + Math.Cos(actualAngle * Math.PI / 180) * (lineLength - 20);
                        staticLineList[i].Y1 = lineLength - Math.Sin(actualAngle * Math.PI / 180) * (lineLength - 20);
                        staticLineList[i].StrokeThickness = 1;
                        staticLineList[i].Opacity = 0.5;
                    }
                    staticLineList[i].Stroke = Brushes.LightGreen;
                    staticLineList[i].X2 = lineLength + Math.Cos(actualAngle * Math.PI / 180) * lineLength;
                    staticLineList[i].Y2 = lineLength - Math.Sin(actualAngle * Math.PI / 180) * lineLength;
                    if (i % 10 == 0)
                    {
                        lblAngleList.Add(new Label());
                        lblAngleList[intLblCounter].Content = actualAngle.ToString() + "°";
                        lblAngleList[intLblCounter].Foreground = new SolidColorBrush(Colors.White);
                        lblAngleList[intLblCounter].Background = new SolidColorBrush(Colors.Black);
                        if (actualAngle > 90)
                        {
                            lblAngleList[intLblCounter].Margin = new Thickness(staticLineList[i].X2 - lblAngleList[intLblCounter].ActualWidth - 5,
                                staticLineList[i].Y2 - lblAngleList[intLblCounter].ActualHeight, 0, 0);
                        }
                        else if (actualAngle == 90)
                        {
                            lblAngleList[intLblCounter].Margin = new Thickness(staticLineList[i].X2 - lblAngleList[intLblCounter].ActualWidth / 2,
                                staticLineList[i].Y2 - lblAngleList[intLblCounter].ActualHeight, 0, 0);
                        }
                        else
                        {
                            lblAngleList[intLblCounter].Margin = new Thickness(staticLineList[i].X2 + 5,
                                staticLineList[i].Y2 - lblAngleList[intLblCounter].ActualHeight, 0, 0);
                        }
                        intLblCounter++;
                    }

                    actualAngle += 1;
                }


                for (int i = 0; i <= 19; i++)
                {
                    lblDistanceList.Add(new Label());
                }

                for (int i = 0; i <= 9; i++)
                {
                    ArcFigure = new PathFigure();
                    dblArcSizeDiv = (lineLength) / 10 * i;
                    ArcFigure.StartPoint = new Point((DrawCanvas.ActualWidth - dblArcSizeDiv - 30), DrawCanvas.ActualHeight);
                    ArcFigure.Segments.Add(
                        new ArcSegment(
                            new Point(dblArcSizeDiv + 30, lineLength),
                                new Size((lineLength - dblArcSizeDiv - 30), (lineLength - dblArcSizeDiv - 30)),
                                90,
                                false,
                                SweepDirection.Counterclockwise,
                                    true));
                    PathGeometry ArcPath = new PathGeometry();
                    ArcPath.Figures.Add(ArcFigure);
                    staticPathList.Add(new System.Windows.Shapes.Path());
                    staticPathList[i].Data = ArcPath;
                    staticPathList[i].SnapsToDevicePixels = true;
                    staticPathList[i].SetValue(RenderOptions.EdgeModeProperty, EdgeMode.Aliased);
                    staticPathList[i].Visibility = Visibility.Visible;
                    staticPathList[i].Stroke = Brushes.LightGreen;
                    if (i == 0)
                    {
                        staticPathList[i].Opacity = 0.7;
                        staticPathList[i].StrokeThickness = 5;
                    }
                    else
                    {
                        staticPathList[i].Opacity = 0.3;
                        staticPathList[i].StrokeThickness = 2;
                    }

                    lblDistanceList[i].Content = intDistanceCounter.ToString() + " cm";
                    lblDistanceList[i].Foreground = new SolidColorBrush(Colors.White);
                    lblDistanceList[i].Background = new SolidColorBrush(Colors.Black);
                    lblDistanceList[i].Margin = new Thickness(ArcFigure.StartPoint.X - 20, ArcFigure.StartPoint.Y, 0, 0);
                    lblDistanceList[19 - i].Content = intDistanceCounter.ToString() + " cm";
                    lblDistanceList[19 - i].Foreground = new SolidColorBrush(Colors.White);
                    lblDistanceList[19 - i].Background = new SolidColorBrush(Colors.Black);
                    lblDistanceList[19 - i].Margin = new Thickness(dblArcSizeDiv+10, lineLength, 0, 0);
                    intDistanceCounter -= radarRadiusDelta;

                }
            }


        }

        #endregion

        #region Functions / procedures

        private void handleConnection()
        {
            try
            {
                if (!comPort.IsOpen)
                {
                    comPort.PortName = txtSerialPort.Text;
                    comPort.BaudRate = 9600;
                    comPort.DataBits = 8;
                    comPort.StopBits = StopBits.One;
                    comPort.Parity = Parity.None;
                    comPort.Open();
                    txtConnectionStatus.Text = "Connected";
                    btnConnect.Content = "Disconnect";
                    txtConnectionStatus.Background = Brushes.Green;
                    txtConnectionStatus.BorderBrush = Brushes.Green;
                    LoadFiles();
                }
                else
                {
                    comPort.Close();
                    txtConnectionStatus.Text = "Disconnected";
                    btnConnect.Content = "Connect";
                    txtConnectionStatus.Background = Brushes.Red;
                    txtConnectionStatus.BorderBrush = Brushes.Red;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void radarCursorAngleChange()
        {
            if (this != null)
            {
                lineLength = DrawCanvas.ActualHeight;
                if (lineLength != 0)
                {
                    drawRadarRadius();
                    printAtiaIfColizionExist();
                    if (distance < radarMask / 100.0 && ComMode) printAtia(distance, radarAngle, Brushes.OrangeRed);
                    if (distance2 < Math.Min(radarMask / 100.0, 0.5) && ComMode) printAtia(distance2, radarAngle, Brushes.Yellow);
                }
            }
        }


        private double map(
            double originalStart, double originalEnd, // original range
            double newStart, double newEnd, // desired range
            double value) // value to convert
        {
            double scale = (double)(newEnd - newStart) / (originalEnd - originalStart);
            return (newStart + ((value - originalStart) * scale));
        }



        private void display_status(string state)
        {
            txtConnectionStatus.Background = (state == "Disconnected") ? Brushes.Red : Brushes.Green;
            txtConnectionStatus.Text = state;
        }

        private void display_input(string input)
        {
            txtSerialInput.IsEnabled = !(input == "Connection N/A");
            txtSerialInput.Text = input;
            txtSerialInput.Background = (input == "Bad Data!") ? Brushes.Red : Brushes.White;
        }

        #endregion

        #region Event Handlers

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //drawStaticGraphics();
            lineLength = DrawCanvas.ActualHeight;
            staticGraphInit();
            drawStaticGraphics();
            Run();
        }

        private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            DrawCanvas.Height = DrawCanvas.Width / 2;
            lineLength = DrawCanvas.ActualHeight;
            staticGraphInit();
            radarRadiusInit();
            drawStaticGraphics();
        }

        private void button_telemeter_Click(object sender, RoutedEventArgs e)
        {
            int angle;
            if (int.TryParse(textBox_telemeter.Text, out angle) && angle >= 0 && angle <= 180)
            {
                comPort.WriteLine("2");
                comPort.WriteLine(angle.ToString());
            }
            else
                MessageBox.Show("Telemeter angle is invalid. Choose a whole value between 0 and 180");
        }

        private void button_sweep_Click(object sender, RoutedEventArgs e)
        {
            string cmd = "";
            bool objectsweep = (bool)checkBox_objectsweep.IsChecked, lightsweep = (bool)checkBox_lightsweep.IsChecked;
            if (objectsweep && !lightsweep)
                cmd = "1";
            else if (!objectsweep && lightsweep)
                cmd = "3";
            else if (objectsweep && lightsweep)
                cmd = "4";
            else
            {
                MessageBox.Show("Select at least one type of sweep.");
                return;
            }
            comPort.WriteLine(cmd);
        }

        private void button_runscript_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                int filenum = listBox_scripts.SelectedIndex;
                if (filenum >= 0 && filenum <= 2)
                {
                    if ((string)((ListBoxItem)listBox_scripts.Items[filenum]).Content != "/NOSCRIPT")
                    {
                        comPort.WriteLine("5");
                        comPort.WriteLine(filenum.ToString());
                    }
                    else
                        MessageBox.Show("You tried running a script that does not exist.");
                }
                else
                    MessageBox.Show("Error occurred running the script.");
            }
            catch { }
        }

        private void button_uploadscript_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                int filenum = listBox_scripts.SelectedIndex;
                if(filenum < 0 || filenum > 2)
                {
                    MessageBox.Show("Chosen file is illegal.");
                    return;
                }
                OpenFileDialog ofd = new OpenFileDialog();
                ofd.Filter = "Script|*.txt";
                if (ofd.ShowDialog() == true)
                {
                    NameValueCollection cmds_oneoperand = new NameValueCollection
                    {
                        { "inc_lcd", "\x01" },
                        { "dec_lcd", "\x02" },
                        { "rra_lcd", "\x03" },
                        { "set_delay", "\x04" },
                        { "servo_deg", "\x06" },
                    };
                    NameValueCollection cmds_twooperands = new NameValueCollection
                    {
                        { "servo_scan", "\x07" }
                    };
                    NameValueCollection cmds_nooperand = new NameValueCollection
                    {
                        { "clear_lcd", "\x05" },
                        { "sleep", "\x08" }
                    };
                    string[] fLines = File.ReadAllLines(ofd.FileName).Where(s => s.Trim() != string.Empty).ToArray();
                    Regex regex = new Regex("[ ]{2,}");
                    string EncodedCode = "";
                    for(int i=0; i<fLines.Length; i++)
                    {
                        fLines[i] = regex.Replace(fLines[i], " ");
                        string[] command = fLines[i].Trim().ToLower().Replace(Environment.NewLine, "").Split(new char[] { ' ', ',' });
                        int operand1, operand2;
                        if(command.Length > 0 )
                        {
                            string Encode = "";
                            if (cmds_nooperand[command[0]] != null && command.Length == 1)
                            {
                                Encode += cmds_nooperand[command[0]];
                            }
                            else if (cmds_oneoperand[command[0]] != null && command.Length == 2)
                            {
                                Encode += cmds_oneoperand[command[0]];
                                if (int.TryParse(command[1], out operand1))
                                    Encode += (char)Convert.ToByte(command[1]);
                                else
                                    continue;
                            }
                            else if (cmds_twooperands[command[0]] != null && command.Length == 3)
                            {
                                Encode += cmds_twooperands[command[0]];
                                if (int.TryParse(command[1], out operand1) && int.TryParse(command[2], out operand2))
                                {
                                    Encode += (char)Convert.ToByte(command[1]);
                                    Encode += (char)Convert.ToByte(command[2]);
                                }
                                else
                                    continue;
                            }
                            EncodedCode += Encode;
                        }
                    }
                    string filename = System.IO.Path.GetFileNameWithoutExtension(ofd.FileName);
                    EncodedCode = $"{filenum}:{(char)EncodedCode.Length}:{EncodedCode}:{filename.Substring(0, Math.Min(filename.Length, 10))}\xFF";
                    //byte[] my_bytes = System.Text.Encoding.Unicode.GetBytes(EncodedCode).Where(b => b != 0).ToArray();
                    byte[] my_bytes = System.Text.Encoding.Unicode.GetBytes(EncodedCode);
                    int j = 1;
                    for(int i=2; i<my_bytes.Length; i+=2)
                        my_bytes[j++] = my_bytes[i];
                    comPort.WriteLine("7");
                    comPort.Write(my_bytes, 0, j);
                    //comPort.Write("\xFF");
                }
            }
            catch
            {
                MessageBox.Show("Error occurred uploading the script.");
            }
        }

        private void textBox_range_TextChanged(object sender, TextChangedEventArgs e)
        {
            //Window_SizeChanged(null, null);
            staticGraphInit();
            //radarRadiusInit();
            //drawStaticGraphics();
            ///MessageBox.Show("abc");
        }

        private void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            handleConnection();
        }

        private void LoadFiles()
        {
            comPort.WriteLine("6");
        }


        private void DrawCanvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            Point p = Mouse.GetPosition(DrawCanvas);
            Atia atia = new Atia(this, p.X, p.Y, Brushes.OrangeRed);
            atia.Run(false);
        }

        #endregion
    }
}
