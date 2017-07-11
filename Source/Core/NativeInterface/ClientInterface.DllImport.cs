using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Core
{
 	partial class ClientInterface
	{
        const string DllFileName = "EntityToolInterface.dll";

        [DllImport( DllFileName, EntryPoint = "InitalizeEntityToolInterface", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl )]
        private static extern IntPtr InitalizeWindowInternal( IntPtr applicationInstance, IntPtr hWndParent, int width, int height, string assetPath );

        [DllImport( DllFileName, EntryPoint = "StartEntityToolInterface", CallingConvention = CallingConvention.Cdecl )]
        private static extern void StartInternal();

        [DllImport( DllFileName, EntryPoint = "CleanUpEntityToolInterface", CallingConvention = CallingConvention.Cdecl )]
        private static extern void CleanUpInternal();

        [DllImport( DllFileName, EntryPoint = "SendCommandInterface", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl )]
        private static extern bool SendCommandInternal(string action, byte [] data, uint dataSize );

        [DllImport( DllFileName, EntryPoint = "SendCommandOutputInterface", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl )]
        [return: MarshalAs( UnmanagedType.I1 )]
        private static extern bool SendCommandInternal( string action, byte[] inputData, uint inputDataSize, ref IntPtr pOutputData, ref uint outputSize );

        [DllImport( DllFileName, EntryPoint = "ReadPoolInterface", CallingConvention = CallingConvention.Cdecl )]
        private static extern bool ReadPoolInternal( byte[] outputData );      
	}
}
