#include "RSAPrivateKey.h"


RSAPrivateKey::RSAPrivateKey(){}


RSAPrivateKey::~RSAPrivateKey(){}

bool RSAPrivateKey::setKey( const char* buff, DWORD buffSize )
{
	if( buff == NULL || buffSize == 0 )
		return false;

	if( !eliminateHeader( buff, buffSize ) )
		return false;

	BYTE* decodedBuffer;
	DWORD decodedBuffSize;
	if( !base64Decode( buff, buffSize, decodedBuffer, decodedBuffSize ) )
		return false;
	return true;
}
