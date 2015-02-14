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
	this->numDigits = 0;
}

BigNum* BigNum::operator*(const BigNum& other)
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
BigNum* BigNum::gradeSchoolMultiply( const BigNum& other )
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
	BYTE* resultArr = (BYTE*) calloc(bot->numDigits, numDigitsPerSingle);
	
	// Multiplies everything out
	int result = 0, carry = 0, arrOffset = 0;
	for( int i = bot->numDigits - 1, b = bot->num[i]; i >= 0; i--, b = bot->num[i], arrOffset++ )
//	for( BYTE* b = bot->num + bot->numDigits - 1; b >= bot->num; b-- )
	{
		carry = 0;
		for( int j = top->numDigits - 1, t = top->num[j]; j >= 0; j--, t = top->num[j] )
//		for( BYTE* t = top->num + top->numDigits - 1; t >= top->num; t-- )
		{
			result = t * b + carry;
			carry = result / base;
			result %= base;
			resultArr[(arrOffset * numDigitsPerSingle) + (j + 1)] = result;
		}
		resultArr[arrOffset * numDigitsPerSingle] = carry;
	}

	// Now time to add them all!
	BigNum* retVal = new BigNum( bot->numDigits + top->numDigits );

// 	// Iterate over EVERY digit
// 	for( int k = 0, sum = 0, carry = 0; k < (int) numDigitsPerSingle; k++ )
// 	{
// 		for( int l = 0; l <= k; l++ )
// 			sum += resultArr[(l * numDigitsPerSingle) + (numDigitsPerSingle - k - l)];
// 		carry = sum % base;
// 		sum /= base;
// 		retVal.num[retVal.numDigits - 1 - k] = sum;
// 	}

	carry = 0;
	for( int k = 0; k < (int) retVal->numDigits; k++ )
	{
		result = carry;
		for( int l = 0; l <= k && l < 3; l++ )
		{
			if( ((int) numDigitsPerSingle - 1 - (k - l)) < 0 )
				continue;
			result += resultArr[(l * numDigitsPerSingle) + (numDigitsPerSingle - 1 - (k - l))];
		}
		carry = result / base;
		result %= base;
		retVal->num[retVal->numDigits - 1 - k] = result;
	}

	free( resultArr );

	return retVal;
}
