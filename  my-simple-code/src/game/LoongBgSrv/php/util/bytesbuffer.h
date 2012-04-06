#ifndef BYTES_BUFFER_H_
#define BYTES_BUFFER_H_

#ifndef BYTES_BUFFER_INIT_SIZE
#define BYTES_BUFFER_INIT_SIZE ( 1024 * 2 )
#endif
#ifndef INC_BUFFER_SIZE
#define INC_BUFFER_SIZE  512
#endif

class CBytesBuffer
{
public:
	/**
	* to init bytesbuffer
	*/
	CBytesBuffer(const int len = BYTES_BUFFER_INIT_SIZE );

	~CBytesBuffer();

	/**
	* return data size of the buffer
	*/
	int getDataSize() const;

	/**
	* to pop data of the buffer
	*/
	void popData(int len );

	/**
	* to clear data of the buffer
	*/
	void clear();

	/**
	* to append data of the buffer
	*/
	void append(const void* data,int dataSize );

	/**
	* to get data of the buffer
	*/
	const char* getData() const;

	/**
	* to resize buffer and set data length
	*/
	void resize( unsigned int size );

private:
	char* _buffer;
	int _bufferSize;
	int _dataSize;

private:
	void checkOverflow(int addLength);

	CBytesBuffer( const CBytesBuffer& other );
	CBytesBuffer& operator=( const CBytesBuffer& other );
};

#endif
