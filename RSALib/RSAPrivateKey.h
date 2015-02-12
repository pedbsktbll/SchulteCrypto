#pragma once
#include "RSAKey.h"

class RSAPrivateKey : public RSAKey
{
public:
	RSAPrivateKey();
	~RSAPrivateKey();

	virtual bool setKey( const char* buff, DWORD buffSize );

protected:
	BYTE version;
	BYTE* modulus;			// 'n'
	BYTE* publicExponent;	// 'e'
	BYTE* privateExponent;  // 'd'
	BYTE* prime1;			// 'p'
	BYTE* prime2;			// 'q'
	BYTE* exponent1;		// d mod (p-1)
	BYTE* exponent2;		// d mod (q-1)
	BYTE* coefficient;		// (inverse of q) mod p
	BYTE* otherPrimeInfos;	// OPTIONAL

	DWORD modulusSize;
	DWORD publicExponentSize;
	DWORD privateExponentSize;
	DWORD prime1Size;
	DWORD prime2Size;
	DWORD exponent1Size;
	DWORD exponent2Size;
	DWORD coefficientSize;
	DWORD otherPrimeInfosSize;
};

// http://www.cryptosys.net/pki/rsakeyformats.html
//Private key formats supported( unencrypted )
//PKCS#1 RSAPrivateKey** (PEM header : BEGIN RSA PRIVATE KEY)
//PKCS#8 PrivateKeyInfo* (PEM header : BEGIN PRIVATE KEY)
