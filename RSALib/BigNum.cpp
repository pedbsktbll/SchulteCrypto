#include "BigNum.h"
#include <Math.h>

BigNum::BigNum()
{
	numDigits = 0;
	allocatedBytes = 0;
	num = NULL;
}

BigNum::BigNum( DWORD numDigits, BYTE* num /*= NULL*/, bool reverseOrder /*= true*/ )
{
	this->numDigits = numDigits;
	this->allocatedBytes = (numDigits + 1) / 2;

	if( numDigits > 0 )
		this->num = (BYTE*) calloc( allocatedBytes, 1 );
	else
		this->num = NULL;
	
	if( num != NULL && numDigits > 0 )
	{
		if( reverseOrder )
			memcpy( this->num, num, numDigits );
		else
		{
			for( DWORD i = 0; i < allocatedBytes; i++ )
			{
				BYTE b = num[allocatedBytes - 1];
				this->num[i] = ((b & 0x0F) << 4) | ((b & 0xF0) >> 4);
			}
		}
	}
}

BigNum::BigNum( const BigNum& other )
{
	this->numDigits = this->allocatedBytes = 0;
	this->num = NULL;

	if( other.allocatedBytes > 0 && other.num != NULL )
	{
		this->num = (BYTE*) malloc( this->allocatedBytes = other.allocatedBytes );
		memcpy( num, other.num, allocatedBytes );
		this->numDigits = other.numDigits;
	}	
}

BigNum::BigNum( char* num )
{
	this->numDigits = strlen( num );
	this->allocatedBytes = (numDigits + 1) / 2;
	this->num = (BYTE*) calloc( allocatedBytes, 1 );
	for( int i = numDigits - 1, j = 0; i >= 0; i--, j++ )
	{
		BYTE b = isalpha( num[i] ) ? tolower( num[i] ) - 'a' + 10 : num[i] - '0';
		this->num[j / 2] |= (j % 2 == 0 ? b << 4 : b);
	}
}

BigNum::~BigNum()
{
	if( num != NULL )
		free( num );
	this->numDigits = 0;
	this->allocatedBytes = 0;
}

BigNum BigNum::operator+(const BigNum& other)
{
	BigNum retVal( (this->numDigits > other.numDigits ? this->numDigits : other.numDigits) + 1 );
	
	bool firstNibble = true;
	for( int carry = 0, result = 0, i = 0, byteOffset = 0; i < (int) retVal.numDigits - 1;
		i++, firstNibble = !firstNibble, firstNibble ? byteOffset++ : 0 )
	{
		BYTE a = i < (int) this->numDigits ? this->num[byteOffset] : 0;
		BYTE b = i < (int) other.numDigits ? other.num[byteOffset] : 0;
		if( firstNibble )
		{
			// Then we're doing the first nibble (most significant bit)
			a = (a & 0xF0) >> 4;
			b = (b & 0xF0) >> 4;
		}
		else
		{
			// Then we're doing the second nibble (least significant bit)
			a &= 0x0F;
			b &= 0x0F;
		}

		result = carry + a + b;
		carry = result / base;
		result = ( firstNibble ? (result % base) << 4 : (result % base) );
		retVal.num[byteOffset] |= result;
	}
	return retVal;
}

BigNum BigNum::operator*(const BigNum& other)
{
	return gradeSchoolMultiply( other );
}

// BigNum BigNum::operator^(const BigNum& other)
// {
// 
// }

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
	DWORD numBytesPerSingle = (numDigitsPerSingle + 1) / 2;
	BYTE* resultArr = (BYTE*) calloc( bot->numDigits, numBytesPerSingle );
	
	// Multiplies everything out
	for( int i = 0, bByteOffset = 0, carry = 0, bFirstNibble = true; i < (int) bot->numDigits;
		i++, bFirstNibble = !bFirstNibble, bFirstNibble ? bByteOffset++ : 0 )
	{
		int b = (bFirstNibble ? (bot->num[bByteOffset] & 0xF0) >> 4 : bot->num[bByteOffset] & 0x0F);
		carry = 0;
		for( int j = 0, tByteOffset = 0, result = 0, tFirstNibble = true; j < (int) top->numDigits;
			j++, tFirstNibble = !tFirstNibble, tFirstNibble ? tByteOffset++ : 0 )
		{
			int t = (tFirstNibble ? (top->num[tByteOffset] & 0xF0) >> 4 : top->num[tByteOffset] & 0x0F);
			result = t * b + carry;
			carry = result / base;
			result = ( tFirstNibble ? (result % base) << 4 : (result % base));
			resultArr[i * numBytesPerSingle + tByteOffset] |= result;
		}
		// Hmm, I could maybe group this into the for-loop...
		carry = ((top->numDigits % 2 == 0 ) ? (carry % base) << 4 : (carry % base));
		resultArr[i * numBytesPerSingle + numBytesPerSingle - 1] |= carry;
	}

	// Now time to add them all! This is an optimized add which is why we dont use BigNum+
	BigNum retVal( bot->numDigits + top->numDigits );
	for( int k = 0, result = 0, carry = 0; k < (int) retVal.numDigits; k++ )
	{
		result = carry;
		for( int l = 0, bitNum = k - l; l < (int) bot->numDigits && l <= k; l++, bitNum = k - l )
		{
			if( bitNum > (int) numDigitsPerSingle )
				continue;
			result += ((bitNum % 2 == 0) ? (resultArr[l * numBytesPerSingle + bitNum / 2] & 0xF0) >> 4 :
				(resultArr[l * numBytesPerSingle + bitNum / 2] & 0x0F));
		}
		carry = result / base;
		result = ((k % 2 == 0) ? (result % base) << 4 : (result % base));
		retVal.num[k / 2] |= result;
	}

	free( resultArr );
	return retVal;
}