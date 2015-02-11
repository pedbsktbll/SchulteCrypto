#include "RSAPublicKey.h"


RSAPublicKey::RSAPublicKey(){}


RSAPublicKey::~RSAPublicKey(){}

bool RSAPublicKey::setKey( const char* buff, DWORD buffSize )
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
