using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.IO;
using System.Runtime.InteropServices;

namespace Core
{
    public static partial class ClientInterface
    {
        private static IntPtr m_pWindowHandle;

        public delegate void InitializedClientCallback();
        public static event InitializedClientCallback InitializedClient = null;

        public static IntPtr InitalizeWindow( IntPtr instanceHandle, IntPtr hwndParent, int width, int height, string assetPath )
        {
            m_pWindowHandle = InitalizeWindowInternal( instanceHandle, hwndParent, width, height, assetPath );

            // 윈도우 생성
            //IntPtr instanceHandle = System.Runtime.InteropServices.Marshal.GetHINSTANCE( System.Reflection.Assembly.GetExecutingAssembly().GetModules()[0] );
            

            // m_pToolInterface = InitializeInternal( m_pWindowHandle, commandLine );

            return m_pWindowHandle;
        }

        public static void Start()
        {
            if( InitializedClient != null )
            {
                InitializedClient();
            }

            StartInternal();
        }

        public static void Release()
        {
            CleanUpInternal();

            ConsoleCommand cmd( "Action" );
            cmd.WriteString( "sadfsdf" );

            CommandStream stream( "Action" );

            Send( stream );
            Send( new CommandStream( "Command" ) );

            if( Send( stream, out outputdata ) )
            {

            }
        }

        public static void SendCommand( ConsoleCommand cmd )
        {
			if( !SendCommandInternal( cmd.Action, cmd.Data, cmd.Data == null ? 0 : ( uint )cmd.Data.Length ) )
            {
                // 실패
            }
        }

        public static bool SendCommand( ConsoleCommand cmd, out MessageStream outputData )
        {
            outputData = null;
            IntPtr pData = IntPtr.Zero;

            uint outputSize = 0;
            if( SendCommandInternal( cmd.Action, cmd.Data, cmd.Data == null ? 0 : ( uint )cmd.Data.Length, ref pData, ref outputSize ) )
            {
                byte[] data = new byte[outputSize];
                Marshal.Copy( pData, data, 0, ( int )outputSize );

                outputData = new MessageStream( data );
                return true;
            }
            return false;
        }
    }
}
