using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows;

namespace Core
{
    public class ClientHwndHost : HwndHost
    {
        private string m_commandLine = "";
        private MouseButton? m_previewMouseButtonDown = null;

        protected override HandleRef BuildWindowCore( HandleRef hwndParent )
        {
            IntPtr windowHandle = ClientInterface.InitalizeWindow( hwndParent.Handle );
            return new HandleRef( this, windowHandle );
        }

        protected override void DestroyWindowCore( HandleRef hwnd )
        {
            Stop();
        }

        public ClientHwndHost( string commandLine )
        {
            m_commandLine = commandLine;
        }

        public void Run()
        {
            ClientInterface.Start( m_commandLine );
        }

        public void Stop()
        {
            ClientInterface.Release();
        }

        protected override void OnGotFocus( System.Windows.RoutedEventArgs e )
        {
            base.OnGotFocus( e );
        }

        protected override void OnLostFocus( System.Windows.RoutedEventArgs e )
        {
            base.OnLostFocus( e );
            ClientInterface.SendCommand( new ConsoleCommand( "LostFocus" ) );
        }

        private void SetFocus()
        {
            this.Focus();
            ClientInterface.SendCommand( new ConsoleCommand( "GotFocus" ) );
        }      

        protected override void OnKeyDown( KeyEventArgs e )
        {
            if( UITransaction.UIController != null )
            {
                UITransaction.UIController.OnKeyDown( e );
            }
        }

        protected override void OnKeyUp( KeyEventArgs e )
        {
            if( UITransaction.UIController != null )
            {
                UITransaction.UIController.OnKeyUp( e );
            }
        }

        protected override void OnInitialized( EventArgs e )
        {
            base.OnInitialized( e );
        }

        protected void OnCustumMouseDown( MouseButton button, Point position )
        {
            SetFocus();
            if( m_previewMouseButtonDown == button )
            {
                return;
            }

            m_previewMouseButtonDown = button;

            if( UITransaction.UIController != null )
            {
                UITransaction.UIController.OnMouseDown( button, position );
            }
        }

        protected void OnCustumMouseUp( MouseButton button, Point position )
        {
            if( m_previewMouseButtonDown == button )
            {
                m_previewMouseButtonDown = null;
            }

            if( UITransaction.UIController != null )
            {
                UITransaction.UIController.OnMouseUp( button, position );
            }

            switch( button )
            {
                case MouseButton.Middle:
                case MouseButton.Right:
                    ReleaseMouseCapture();
                    break;
            }
        }
        protected void OnCustumMouseMove( Point position )
        {
            if( UITransaction.UIController != null )
            {
                UITransaction.UIController.OnMouseMove( position );
            }
        }

        // 마우스 캡처를 하면, 이쪽으로 들어온다
        protected override void OnMouseUp( MouseButtonEventArgs e )
        {
            OnCustumMouseUp( e.ChangedButton, e.GetPosition( this ) );
        }
        protected override void OnMouseDown( MouseButtonEventArgs e )
        {
            OnCustumMouseDown( e.ChangedButton, e.GetPosition( this ) );
        }
        protected override void OnMouseMove( MouseEventArgs e )
        {
            OnCustumMouseMove( e.GetPosition( this ) );
        }
        protected override void OnMouseWheel( MouseWheelEventArgs e )
        {
            base.OnMouseWheel( e );
        }

        protected override IntPtr WndProc( IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled )
        {
            handled = false;
            switch( msg )
            {
                case Constants.WM_LBUTTONDOWN:
                    OnCustumMouseDown( MouseButton.Left, new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;
                case Constants.WM_RBUTTONDOWN:
                    OnCustumMouseDown( MouseButton.Right, new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;
                case Constants.WM_MBUTTONDOWN:
                    OnCustumMouseDown( MouseButton.Middle, new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;
                case Constants.WM_LBUTTONUP:
                    OnCustumMouseUp( MouseButton.Left, new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;
                case Constants.WM_RBUTTONUP:
                    OnCustumMouseUp( MouseButton.Right, new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;
                case Constants.WM_MBUTTONUP:
                    OnCustumMouseUp( MouseButton.Middle, new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;               
                case Constants.WM_MOUSEMOVE:
                    OnCustumMouseMove( new Point( Macro.GET_X_LPARAM( lParam ), Macro.GET_Y_LPARAM( lParam ) ) );
                    break;
                case Constants.WM_KEYDOWN:
                    break;
            }
            return base.WndProc( hwnd, msg, wParam, lParam, ref handled );
        }
    }
}
