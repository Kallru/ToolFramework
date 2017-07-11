using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Core;

namespace RenderingTool
{
    /// <summary>
    /// MainWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            MessageStream ms = new MessageStream( "aa" );
            
            ms.WriteString( "fefefefdsvvwq" );

            ms.WriteU64( 553434 );
            ms.WriteS8( -123 );

            ms.WriteF64( 34.56 );
            ms.WriteF32( 12.0f );

            MessageStream rs = new MessageStream( ms.Data );
            string a = rs.ReadString();
            ulong b = rs.ReadU64();
            sbyte c = rs.ReadS8();
            double d = rs.ReadF64();
            float e = rs.ReadF32();
        }
    }
}
