
#include "bytesbuffer.h"

#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CBytesBuffer::CBytesBuffer(const int len /* = INITSIZE */)
:_bufferSize( 0 ) , _dataSize( 0 )
{
	_buffer = static_cast<char*>(malloc( len ));
	if( _buffer != NULL )
	{
		_bufferSize = len;
	}
}

CBytesBuffer::~CBytesBuffer()
{
	if ( _buffer != NULL )
	{
		free( _buffer );
	}
	_buffer = NULL;
}

void CBytesBuffer::checkOverflow(int addLength)
{
	addLength += _dataSize;
	if ( _bufferSize > addLength )
	{
		return;
	}
	int incsize = _bufferSize;
	while ( incsize < addLength )
	{
		incsize += ( _bufferSize >> 4 );
	}
	void* ptmp = static_cast<char*>(realloc(_buffer,incsize));
	if( ptmp == NULL )
	{
		fprintf(stderr, "%s, %d realloc fail!!\n", __FUNCTION__, __LINE__);
		exit(-1);
	}
	_buffer = static_cast<char*>(ptmp);
	_bufferSize = incsize;
}

int CBytesBuffer::getDataSize() const
{
	return _dataSize;
}

const char* CBytesBuffer::getData() const
{
	return _buffer;
}

void CBytesBuffer::clear()
{
	_dataSize = 0;
}

void CBytesBuffer::popData(int length)
{
	if( length <= 0 )
	{
		return;
	}
	if ( length >= _dataSize )
	{
		clear();
		return ;
	}
	_dataSize -= length;
	::memmove( _buffer, _buffer + length,_dataSize);
}

void CBytesBuffer::append(const void* data ,int dataSize )
{
	checkOverflow(dataSize);
	memcpy( _buffer + _dataSize , data , dataSize );
	_dataSize += dataSize;
}

void CBytesBuffer::resize(unsigned int size )
{
	if( size < static_cast<size_t>(_bufferSize) )
	{
		_dataSize = static_cast<int>(size);
		return;
	}
	checkOverflow( static_cast<int>(size) - _dataSize );
	_dataSize = static_cast<int>(size);
}
