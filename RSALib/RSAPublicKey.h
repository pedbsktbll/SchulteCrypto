#pragma once
#include "RSAKey.h"

class RSAPublicKey : public RSAKey
{
public:
	RSAPublicKey();
	~RSAPublicKey();

	virtual bool setKey( const char* buff, DWORD buffSize );

protected:
	DWORD modulus; // 'n'
	DWORD publicExponent; // 'e'
};

