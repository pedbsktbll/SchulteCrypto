#pragma once
#include "RSAKey.h"

class RSAPrivateKey : public RSAKey
{
public:
	RSAPrivateKey();
	~RSAPrivateKey();

	// Accepts PKCS #1 Format
	virtual bool setKey( const char* buff, DWORD buffSize );

	// RSADP
	BigNum decipher( BigNum& cipherText );
	BigNum decipher(BYTE* cipherText, DWORD size);

	// RSASP1
	bool sign();

//protected:
	BYTE version;
	BigNum modulus;			// 'n'
	BigNum publicExponent;	// 'e'
	BigNum privateExponent;  // 'd'
	BigNum prime1;			// 'p'
	BigNum prime2;			// 'q'
	BigNum exponent1;		// d mod (p-1)
	BigNum exponent2;		// d mod (q-1)
	BigNum coefficient;		// (inverse of q) mod p
	BigNum otherPrimeInfos;	// OPTIONAL
};

// http://www.cryptosys.net/pki/rsakeyformats.html
//Private key formats supported( unencrypted )
//PKCS#1 RSAPrivateKey** (PEM header : BEGIN RSA PRIVATE KEY)
//PKCS#8 PrivateKeyInfo* (PEM header : BEGIN PRIVATE KEY)
