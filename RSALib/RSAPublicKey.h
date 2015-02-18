#pragma once
#include "RSAKey.h"

class RSAPublicKey : public RSAKey
{
public:
	RSAPublicKey();
	~RSAPublicKey();

	// Accepts PKCS #1 Format
	virtual bool setKey( const char* buff, DWORD buffSize );

	// RSAEP
	BigNum cipher(BigNum& plainText);

	// RSAVP1
	bool verify();

//protected:
	BigNum modulus; // 'n'
	BigNum publicExponent; // 'e'
};

