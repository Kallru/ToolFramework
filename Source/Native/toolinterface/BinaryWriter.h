#pragma once

class BinaryWriter
{
public:
	BinaryWriter()
	{
		Clear();
	}

	~BinaryWriter()
	{}

	void Clear()
	{
		m_memory.clear();
		m_seek = 0;
	}

	void Write( const void* pData, unsigned int size )
	{
		if( ( m_seek + size ) > m_memory.size() )
		{
			m_memory.resize( m_seek + size + 1000 );
		}

		::memcpy( &m_memory[m_seek], pData, size );
		m_seek += size;
	}

	template<typename T>
	void Write( const T& data )
	{
		Write( &data, sizeof(T) );
	}

	template<>
	void Write( const bool& data )
	{
		Write<unsigned char>( static_cast<unsigned char>( data ) );
	}

	template<>
	void Write( const std::string& data )
	{
		Write<unsigned int>( data.size() );
		Write( data.c_str(), data.size() );
	}

	void* GetStream()
	{
		return &m_memory[0];
	}

	unsigned GetSize()
	{
		return m_memory.size();
	}

private:
	std::vector<char> m_memory;
	unsigned int m_seek;
};