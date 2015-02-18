#include "RSAPublicKey.h"


RSAPublicKey::RSAPublicKey()
{
// 	modulus = publicExponent = NULL;
// 	modulusSize = publicExponentSize = 0;
}


RSAPublicKey::~RSAPublicKey()
{
// 	if( modulus != NULL )
// 		delete[] modulus;
// 	modulusSize = 0;
// 
// 	if( publicExponent != NULL )
// 		delete[] publicExponent;
// 	publicExponentSize = 0;
}

/****** BEGIN RSA PUBLIC KEY Format***********
RSAPublicKey :: = SEQUENCE{
	modulus           INTEGER, --n
	publicExponent    INTEGER   -- e
}
*/
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

	BYTE* dataPtr = decodedBuffer;
	BYTE* endPtr = decodedBuffer + decodedBuffSize;
	DER_TLV der;

	// Expect SEQUENCE to start
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != SEQUENCE || der.length + (dataPtr - decodedBuffer) != decodedBuffSize )
		return false;

	// Expect modulus
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, modulus, modulusSize );
	modulus.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect publicExponent
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, publicExponent, publicExponentSize );
	publicExponent.initialize( der.length, der.value );
	dataPtr += der.length;

	return true;
}

BigNum RSAPublicKey::cipher( BigNum& plainText )
{
//	return (plainText ^ this->publicExponent) % this->modulus;
	BigNum a = plainText ^ publicExponent;
	BigNum b = a % modulus;
	return b;
}
