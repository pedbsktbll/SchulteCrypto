#include "RSAPublicKey.h"
#include <time.h>

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
	// 	//	return (plainText ^ this->publicExponent) % this->modulus;
//	BigNum a = plainText ^ publicExponent;
//	BigNum b = a % modulus;
//	return b;
//	return plainText.pow_modulo( publicExponent, modulus );
	return plainText.montgomeryExponent( publicExponent, modulus );
}

bool RSAPublicKey::cipher( BYTE* buffer, DWORD& bufferSize, DWORD& commitSize )
{
	DWORD k = modulus.numDigits;
	DWORD padSize = k - 3 - bufferSize;
	if( bufferSize > k - 11 || commitSize < modulus.numDigits )
		return false;
	memmove( buffer + 3 + padSize, buffer, bufferSize );

	srand( (unsigned int)time( NULL ) );
	pkcs15_padding_encryptionBlock* eb = (pkcs15_padding_encryptionBlock*) buffer;
	eb->blockType = 2;
	for( DWORD i = 0; i < padSize; i++ )
		eb->paddingString[i] = (rand() % 0xFE) + 1; // This must be non-zero
	eb->paddingString[padSize] = 0;

	// Now convert octet-string to integer (BigNum). First octet of EB has the most significance:
	BigNum plainText;
	plainText.initialize( k, buffer );
//	plainText = (plainText ^ publicExponent) % modulus;
//	plainText.pow_modulo( publicExponent, modulus );
	plainText.montgomeryExponent( publicExponent, modulus );

	// PlainText is now RSA-encrypted. Now let's convert back to an octet-string:
	return true;
}
