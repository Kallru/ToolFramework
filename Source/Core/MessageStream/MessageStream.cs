using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Serialization.Formatters;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace Core
{
    public class MessageStream
    {
        private MemoryStream m_buffer = new MemoryStream();

        public string Action { get; private set; }
        public byte[] Data { get { return m_buffer.GetBuffer(); } }

        public MessageStream( string action )
        {
            Action = action;
            // WriteString( action );
        }

        public MessageStream( byte[] data )
        {
            m_buffer.Write( data, 0, data.Length );
            m_buffer.Seek( 0, SeekOrigin.Begin );
        }

        public void WriteBool( bool value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 1 );
        }

        public void WriteU8( byte value )
        {
            m_buffer.WriteByte( value );
        }

        public void WriteU16( ushort value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 2 );
        }

        public void WriteU32( uint value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 4 );
        }

        public void WriteU64( ulong value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 8 );
        }

        public void WriteS8( sbyte value )
        {
            m_buffer.WriteByte( ( byte )value );
        }

        public void WriteS16( short value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 2 );
        }

        public void WriteS32( int value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 4 );
        }

        public void WriteS64( long value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 8 );
        }

        public void WriteF32( float value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 4 );
        }

        public void WriteF64( double value )
        {
            m_buffer.Write( BitConverter.GetBytes( value ), 0, 8 );
        }

        public void WriteString( string value )
        {
            WriteS32( value.Length );
            m_buffer.Write( Encoding.Default.GetBytes( value ), 0, value.Length );

        }

        public void WriteString( string value, Encoding encoding )
        {
            WriteS32( value.Length );
            m_buffer.Write( encoding.GetBytes( value ), 0, value.Length );
        }

        public bool ReadBool()
        {
            return Convert.ToBoolean( m_buffer.ReadByte() );
        }

        public byte ReadU8()
        {
            return ( byte )m_buffer.ReadByte();
        }

        public ushort ReadU16()
        {
            byte[] value = new byte[2];
            m_buffer.Read( value, 0, 2 );
            return BitConverter.ToUInt16( value, 0 );
        }

        public uint ReadU32()
        {
            byte[] value = new byte[4];
            m_buffer.Read( value, 0, 4 );
            return BitConverter.ToUInt32( value, 0 );
        }

        public ulong ReadU64()
        {
            byte[] value = new byte[8];
            m_buffer.Read( value, 0, 8 );
            return BitConverter.ToUInt64( value, 0 );
        }

        public sbyte ReadS8()
        {
            return ( sbyte )m_buffer.ReadByte();
        }

        public short ReadS16()
        {
            byte[] value = new byte[2];
            m_buffer.Read( value, 0, 2 );
            return BitConverter.ToInt16( value, 0 );
        }

        public int ReadS32()
        {
            byte[] value = new byte[4];
            m_buffer.Read( value, 0, 4 );
            return BitConverter.ToInt32( value, 0 );
        }

        public long ReadS64()
        {
            byte[] value = new byte[8];
            m_buffer.Read( value, 0, 8 );
            return BitConverter.ToInt32( value, 0 );
        }

        public float ReadF32()
        {
            byte[] value = new byte[4];
            m_buffer.Read( value, 0, 4 );
            return BitConverter.ToSingle( value, 0 );
        }

        public double ReadF64()
        {
            byte[] value = new byte[8];
            m_buffer.Read( value, 0, 8 );
            return BitConverter.ToDouble( value, 0 );
        }

        public string ReadString()
        {
            int size = ReadS32();

            byte[] value = new byte[size];
            m_buffer.Read( value, 0, size );

            return Encoding.Default.GetString( value );
        }

        public string ReadString( Encoding encoding )
        {
            int size = ReadS32();

            byte[] value = new byte[size];
            m_buffer.Read( value, 0, size );
            return encoding.GetString( value );
        }
    }
}
