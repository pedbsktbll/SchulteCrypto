#pragma once
#include "RSAKey.h"

class RSAPrivateKey : public RSAKey
{
public:
	RSAPrivateKey();
	~RSAPrivateKey();

	virtual bool setKey( const char* buff, DWORD buffSize );
};

