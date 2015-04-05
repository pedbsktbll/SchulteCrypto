#pragma once
#include <Windows.h>

// All numbers are positive
// Stored LEAST SIGNIFICANT bit first (So reverse-order)
class BigNum
{
public:
	BigNum();
	BigNum( DWORD numDigits, BYTE* num = NULL, bool reverseOrder = true );
	BigNum( char* num ); // Must be in hexadecimal
	BigNum( ULONGLONG );
	BigNum( const BigNum& other );
	~BigNum();

	void initialize( DWORD numDigits, BYTE* num = NULL, bool reverseOrder = true );
	void initialize( char* num );

	// C++ Overloaded Functions
	bool operator>(BigNum& other);
	bool operator<(BigNum& other);
	bool operator==(BigNum& other);
	bool operator==(ULONGLONG other);
	bool operator>=(BigNum& other);
	bool operator<=(BigNum& other);
	BigNum& operator=(BigNum& other);
	BigNum operator+(BigNum& other);
	BigNum& operator+=(BigNum& other);
	BigNum operator-(BigNum& other);
	BigNum& operator-=(BigNum& other);
	BigNum operator*(BigNum& other);
	BigNum operator/(BigNum& other);
	BigNum operator%(BigNum& other);
	BigNum operator^(BigNum& other);
	BigNum operator<<(DWORD other); // AKA multiply by 16 ^ x
	BigNum operator>>(DWORD other); // AKA divide by 16 ^ x

	// Classical Algorithms
	void left_shift( DWORD numShifts, BigNum* retVal );
	void right_shift( DWORD numShifts, BigNum* retVal );
	void classicalAddition( BigNum& other, BigNum* retVal );
	void classicalSubtract( BigNum& other, BigNum* retVal );
	void classicalMultiply( BigNum& other, BigNum& retVal );
	void classicalDivision( BigNum& other, BigNum& quotient, BigNum& remainder );
	BigNum classicalExponent( BigNum& exponent );
	ULONGLONG modInverse_bruteForce( bool negative = true );

	// Advanced Algorithms
	BigNum pow_modulo( BigNum& power, BigNum& modulo );
	BigNum karatsubaMultiply( BigNum& other );
	BigNum slidingWindowExp( BigNum& e ); //TODO: unfinished
	BigNum modInverse_fermat( BigNum& other );

	// Montgomery Reduction Algorithms
	BigNum montgomeryReduction( BigNum &m, ULONGLONG *m_invPtr = NULL );
	BigNum montgomeryMultiply( BigNum &y, BigNum &m, ULONGLONG* pm_inv = NULL );
	BigNum montgomeryExponent( BigNum &e, BigNum &m );
	BigNum montSlidingWindowExp(BigNum &e, BigNum& N);

	// User Functions
	void clear();
	bool toArray(char* array, DWORD& len);
	ULONGLONG toULL();
	

//	static BigNum ULLToBigNum( ULONGLONG num );

//protected:
	DWORD numDigits; // Total number of digits starting with 1. May include zeros at the beginning
	DWORD allocatedBytes; // Total number of actual bytes allocated for the number in "num"
	BYTE* num;
	static const unsigned short base = 16;

	// Internal Functions
	void validateNumDigits();
	void increaseCapacity( DWORD totalBytes );
	void padDigits( DWORD totalDigits );

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

};

