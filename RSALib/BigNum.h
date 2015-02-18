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
	void initialize( DWORD numDigits, BYTE* num = NULL, bool reverseOrder = true );//, bool positive = true );
	void initialize( char* num );

	bool operator>(BigNum& other);
	bool operator<(BigNum& other);
	bool operator==(BigNum& other);
	bool operator==(ULONGLONG& other);
	bool operator>=(BigNum& other);
	bool operator<=(BigNum& other);
	BigNum& operator=(const BigNum& other);
	BigNum operator+(const BigNum& other);
	BigNum& operator+=(const BigNum& other);
	BigNum operator-(BigNum& other);
	BigNum& operator-=(BigNum& other);
	BigNum operator*(const BigNum& other);
	BigNum operator/(BigNum& other);
	BigNum operator%(BigNum& other);
	BigNum operator^(BigNum& other);

	void clear();
	ULONGLONG toULL();
	void increaseCapacity( DWORD totalBytes );

protected:
	DWORD numDigits; // Total number of digits starting with 1. May include zeros at the beginning
	DWORD allocatedBytes; // Total number of actual bytes allocated for the number in "num"
//	bool positive;
	BYTE* num;

#pragma pack(push, 1)
	union nibble
	{
		BYTE fullByte;
		struct
		{
			BYTE nibLeast : 4;
			BYTE nibMost : 4;
		};
		
	};
#pragma pack(pop)

	const unsigned short base = 16;

	void validateNumDigits();
	BigNum classicalMultiply( const BigNum& other );
	void classicalDivide( BigNum& other, BigNum& quotient, BigNum& remainder );

};

