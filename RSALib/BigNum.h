#pragma once
#include <Windows.h>

class BigNum
{
public:
	BigNum();
	BigNum( DWORD numDigits, BYTE* num = NULL, bool positive = true );
	~BigNum();

	BigNum* operator*(const BigNum& other);


protected:
	DWORD numDigits;
	bool positive;
	BYTE* num;

	const unsigned short base = 16;

	BigNum* gradeSchoolMultiply( const BigNum& other );
};

