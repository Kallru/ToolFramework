#pragma once

class BinaryReader
{
public:
	BinaryReader( void* pMemroy, unsigned int size )
		: m_memory( size )
		, m_seek( 0 )
	{
		::memcpy( &m_memory[0], pMemroy, size );
	}

	~BinaryReader()
	{
	}

	BinaryReader( const BinaryReader& rhw )
	{
		m_memory = rhw.m_memory;
		m_seek = rhw.m_seek;
	}

	bool IsEnd()
	{
		return m_seek == m_memory.size();
	}

	void Read( void* pData, unsigned int size )
	{
		unsigned int ableSize = size;
		if( ( m_seek + size ) > m_memory.size() )
		{
			ableSize = ( m_seek + size ) - m_memory.size();
		}

		::memcpy( pData, &m_memory[m_seek], ableSize );
		m_seek += ableSize;
	}

	template<typename T>
	T Read()
	{
		T data;
		Read( &data, sizeof(T) );
		return data;
	}

	template<>
	bool Read()
	{
		return static_cast<bool>( Read<unsigned char>() );
	}
	
	template<>
	std::string Read()
	{
		unsigned int lenght = Read<unsigned int>();

		char* pBuffer = new char[ lenght + 1 ];
		::ZeroMemory( pBuffer, lenght + 1 );
		Read( pBuffer, lenght );

		std::string data( pBuffer );
		
		delete pBuffer;
		pBuffer = nullptr;

		return data;
	}

private:
	std::vector<char> m_memory;
	unsigned int m_seek;
};