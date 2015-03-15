#include "RSAPrivateKey.h"


RSAPrivateKey::RSAPrivateKey()
{
	version = 0;
/*	modulus = publicExponent = privateExponent = prime1 = prime2 = exponent1 = exponent2 = coefficient = otherPrimeInfos = NULL;*/
}


RSAPrivateKey::~RSAPrivateKey()
{
// 	if( modulus != NULL )
// 		delete[] modulus;
// 	modulusSize = 0;
// 
// 	if( modulus != NULL )
// 		delete[] publicExponent;
// 	publicExponentSize = 0;
// 
// 	if( privateExponent != NULL )
// 		delete[] privateExponent;
// 	privateExponentSize = 0;
// 
// 	if( prime1 != NULL )
// 		delete[] prime1;
// 	prime1Size = 0;
// 
// 	if( prime2 != NULL )
// 		delete[] prime2;
// 	prime2Size = 0;
// 
// 	if( exponent1 != NULL )
// 		delete[] exponent1;
// 	exponent1Size = 0;
// 
// 	if( exponent2 != NULL )
// 		delete[] exponent2;
// 	exponent2Size = 0;
// 
// 	if( coefficient != NULL )
// 		delete[] coefficient;
// 	coefficientSize = 0;
// 
// 	if( otherPrimeInfos != NULL )
// 		delete[] otherPrimeInfos;
// 	otherPrimeInfos = 0;
}

/****** BEGIN RSA PRIVATE KEY Format***********
RSAPrivateKey ::= SEQUENCE {
  version           Version,
  modulus           INTEGER,  -- n
  publicExponent    INTEGER,  -- e
  privateExponent   INTEGER,  -- d
  prime1            INTEGER,  -- p
  prime2            INTEGER,  -- q
  exponent1         INTEGER,  -- d mod (p-1)
  exponent2         INTEGER,  -- d mod (q-1)
  coefficient       INTEGER,  -- (inverse of q) mod p
  otherPrimeInfos   OtherPrimeInfos OPTIONAL
}
*/
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

	BYTE* dataPtr = decodedBuffer;
	BYTE* endPtr = decodedBuffer + decodedBuffSize;
	DER_TLV der;

	// Expect SEQUENCE to start
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != SEQUENCE || der.length + (dataPtr - decodedBuffer) != decodedBuffSize )
		return false;

	// Expect VERSION
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
	this->version = *der.value;
	dataPtr += der.length;

	// Expect modulus
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, modulus, modulusSize );
	modulus.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect public exponent
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, publicExponent, publicExponentSize );
	publicExponent.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect private exponent
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, privateExponent, privateExponentSize );
	privateExponent.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect prime1
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, prime1, prime1Size );
	prime1.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect prime2
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, prime2, prime2Size );
	prime2.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect exponent1
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, exponent1, exponent1Size );
	exponent1.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect exponent2
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, exponent2, exponent2Size );
	exponent2.initialize( der.length, der.value );
	dataPtr += der.length;

	// Expect coefficient
	if( !readDER_TLV( dataPtr, der ) || der.tag.tag != INTEGER )
		return false;
//	setDER_Values( der, coefficient, coefficientSize );
	coefficient.initialize( der.length, der.value );
	dataPtr += der.length;

	return true;
}

BigNum RSAPrivateKey::decipher( BigNum& cipherText )
{
// //	return (cipherText ^ privateExponent) % modulus;
// 	BigNum a = cipherText ^ privateExponent;
// 
// // 	DWORD test = a.numDigits + 1;
// // 	char* arr = new char[test];
// // 	a.toArray( arr, test );
// 
// 	BigNum b = a % modulus;
// 	return b;
//	return cipherText.pow_modulo( privateExponent, modulus );

	// inefficient....
	// Use chinese remainder theorem optimization instead!
	// m1 = c ^ dp mod p
	// m2 = c ^ dq mod q
	// h = qinv(m1 - m2) (mod p)
	// m = m2 + hq;
	BigNum one( "1" );

	BigNum m1 = cipherText.pow_modulo(exponent1, prime1);
	BigNum m2 = cipherText.pow_modulo(exponent2, prime2);

	BigNum h = m2 < m1 ? (coefficient * (m1 - m2)) % prime1 : coefficient * ((m1 + (((prime2 / prime1) + one) * prime1)) - m2) % prime1;
	BigNum m = m2 + h * prime2;
	return m;
}
