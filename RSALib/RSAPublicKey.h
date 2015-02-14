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
	bool cipher();

	// RSAVP1
	bool verify();

protected:
	BYTE* modulus; // 'n'
	BYTE* publicExponent; // 'e'

	DWORD modulusSize;
	DWORD publicExponentSize;
};

