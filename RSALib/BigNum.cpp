#include "BigNum.h"
#include <Math.h>

BigNum::BigNum()
{
	numDigits = 0;
	positive = true;
	num = NULL;
}

BigNum::BigNum( DWORD numDigits, BYTE* num /*= NULL*/, bool positive /*= true */ )
{
	this->numDigits = numDigits;
	this->positive = positive;
	this->num = new BYTE[numDigits];

	if( num != NULL )
		memcpy( this->num, num, numDigits );
}

BigNum::~BigNum()
{
	if( num != NULL )
		delete[] num;
}

BigNum BigNum::operator*(const BigNum& other)
{
	return gradeSchoolMultiply( other );
}

/****************** Elementary method for multiplication ******************
			Method is O(n^2)
   43241
     621
   ----- *
   43241 <--   1 * 43241
  864820 <--  20 * 43241, basically do 2 * 43241 and append a zero
25944600 <-- 600 * 43241, basically do 6 * 43241 and append two zeroes
-------- +
26852661 <-- Add up results, remember to carry
 **************************************************************************/
BigNum BigNum::gradeSchoolMultiply( const BigNum& other )
{
	// We need a BigNum for each and every digit of the smaller number of digits number
	const BigNum* bot, *top;
	if( this->numDigits >= other.numDigits )
	{
		bot = &other;
		top = this;
	}
	else
	{
		bot = this;
		top = &other;
	}

	DWORD numDigitsPerSingle = top->numDigits + 1;//log10( (double) top->numDigits );
//	BigNum* arr = new BigNum[bot->numDigits * numDigitsPerSingle];
//	BigNum** resultArr = (BigNum**) arr;
	BYTE** resultArr = (BYTE**) calloc(bot->numDigits, numDigitsPerSingle);
	
	// Multiplies everything out
	int result = 0, carry = 0;
//	BYTE* b = bot->num[]
	for( int i = 0, b = bot->num[i]; i < (int) bot->numDigits; i++, b = bot->num[i] )
	{
		for( int j = 0, t = top->num[j]; j < (int) top->numDigits; j++, t = top->num[j] )
		{
			result = (t + carry) * b;
			carry = result % base;
			result /= base;
			resultArr[i][top->numDigits - j] = result;
		}
		resultArr[i][0] = carry;
	}

	// Now time to add them all!
	BigNum retVal( bot->numDigits + top->numDigits );

	// Iterate over EVERY digit
	for( int k = 0, sum = 0, carry = 0; k < (int) numDigitsPerSingle; k++ )
	{
		for( int l = 0; l <= k; l++ )
			sum += resultArr[l][k - l];
		carry = sum % base;
		sum /= base;
		retVal.num[retVal.numDigits - 1 - k] = sum;
	}

	free( resultArr );

	return retVal;
}
