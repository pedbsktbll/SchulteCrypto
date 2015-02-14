#pragma once
#include <Windows.h>

// All numbers are positive
// Stored LEAST SIGNIFICANT bit first (So reverse-order)
class BigNum
{
public:
	BigNum();
	BigNum( DWORD numDigits, BYTE* num = NULL, bool reverseOrder = true );//, bool positive = true );
	BigNum( char* num ); // Must be in hexadecimal
	BigNum( const BigNum& other );
	~BigNum();

	BigNum operator+(const BigNum& other);
	BigNum operator*(const BigNum& other);
	BigNum operator^(const BigNum& other);

protected:
	DWORD numDigits;
	DWORD allocatedBytes;
//	bool positive;
	BYTE* num;

	const unsigned short base = 16;

	BigNum gradeSchoolMultiply( const BigNum& other );
};

