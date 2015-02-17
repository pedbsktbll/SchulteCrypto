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
	this->numDigits = this->allocatedBytes = 0;
	this->num = NULL;
	initialize( numDigits, num, reverseOrder );
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

void BigNum::initialize( DWORD numDigits, BYTE* num /*= NULL*/, bool reverseOrder /*= true */ )
{
	this->numDigits = numDigits;
	this->allocatedBytes = 0;
	if( this->num != NULL )
		free( this->num );

	if( numDigits > 0 )
	{
		this->allocatedBytes = (numDigits + 1) / 2;
		this->num = (BYTE*) calloc( allocatedBytes, 1 );
	}
	else
		this->num = NULL;

	if( num != NULL && numDigits > 0 )
	{
//		this->numDigits = numDigits;
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

bool BigNum::operator>(BigNum& other)
{
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits > other.numDigits )
		return true;
	else if( this->numDigits < other.numDigits )
		return false;
	else if( this->numDigits == 0 && other.numDigits == 0 )
		return false;

// 	// Same number of digits, so let's see which number is bigger
// 	for( int i = this->numDigits; i > 0; i-- )
// 	{
// 		BYTE thisNib = (i % 2 == 0 ? this->num[i / 2] & 0x0F : (this->num[i / 2] & 0xF0) << 4);
// 		BYTE otherNib = (i % 2 == 0 ? this->num[i / 2] & 0x0F : (this->num[i / 2] & 0xF0) << 4);
// 		if( thisNib > otherNib )
// 			return true;
// 		else if( thisNib < otherNib )
// 			return false;
// 	}
	for( int i = (this->numDigits - 1) / 2; i >= 0; i-- )
	{
		nibble* thisNib = (nibble*) &this->num[i];
		nibble* otherNib = (nibble*) &other.num[i];
		if( thisNib->nibLeast > otherNib->nibLeast )
			return true;
		else if( thisNib->nibLeast < otherNib->nibLeast )
			return false;
		else if( thisNib->nibMost > otherNib->nibMost )
			return true;
		else if( thisNib->nibMost < otherNib->nibMost )
			return false;
	}
	return false;
}

bool BigNum::operator<(BigNum& other)
{
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits < other.numDigits )
		return true;
	else if( this->numDigits > other.numDigits )
		return false;
	else if( this->numDigits == 0 && other.numDigits == 0 )
		return false;

	// Same number of digits, so let's see which number is bigger
	for( int i = (this->numDigits - 1) / 2; i >= 0; i-- )
	{
		nibble* thisNib = (nibble*) &this->num[i];
		nibble* otherNib = (nibble*) &other.num[i];
		if( thisNib->nibLeast < otherNib->nibLeast )
			return true;
		else if( thisNib->nibLeast > otherNib->nibLeast )
			return false;
		else if( thisNib->nibMost < otherNib->nibMost )
			return true;
		else if( thisNib->nibMost > otherNib->nibMost )
			return false;
	}
	return false;
}

bool BigNum::operator==(BigNum& other)
{
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits != other.numDigits )
		return false;
	else if( this->numDigits == 0 && other.numDigits == 0 )
		return true;

	// Same number of digits, so let's see which number is bigger
	for( int i = (this->numDigits - 1) / 2; i >= 0; i-- )
	{
		if( this->num[i] != other.num[i] )
			return false;
	}
	return true;
}

bool BigNum::operator>=(BigNum& other)
{
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits > other.numDigits )
		return true;
	else if( this->numDigits < other.numDigits )
		return false;
	else if( this->numDigits == 0 && other.numDigits == 0 )
		return true;

	// Same number of digits, so let's see which number is bigger
	for( int i = (this->numDigits - 1) / 2; i >= 0; i-- )
	{
		nibble* thisNib = (nibble*) &this->num[i];
		nibble* otherNib = (nibble*) &other.num[i];
		if( thisNib->nibLeast > otherNib->nibLeast )
			return true;
		else if( thisNib->nibLeast < otherNib->nibLeast )
			return false;
		else if( thisNib->nibMost > otherNib->nibMost )
			return true;
		else if( thisNib->nibMost < otherNib->nibMost )
			return false;
	}
	return true;
}

bool BigNum::operator<=(BigNum& other)
{
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits < other.numDigits )
		return true;
	else if( this->numDigits > other.numDigits )
		return false;
	else if( this->numDigits == 0 && other.numDigits == 0 )
		return true;

	// Same number of digits, so let's see which number is bigger
	for( int i = (this->numDigits - 1) / 2; i >= 0; i-- )
	{
		nibble* thisNib = (nibble*) &this->num[i];
		nibble* otherNib = (nibble*) &other.num[i];
		if( thisNib->nibLeast < otherNib->nibLeast )
			return true;
		else if( thisNib->nibLeast > otherNib->nibLeast )
			return false;
		else if( thisNib->nibMost < otherNib->nibMost )
			return true;
		else if( thisNib->nibMost > otherNib->nibMost )
			return false;
	}
	return true;
}

BigNum& BigNum::operator=(const BigNum& other)
{
	this->allocatedBytes = other.allocatedBytes;
	this->numDigits = other.numDigits;
	if( this->num != NULL )
		free( this->num );
	this->num = (BYTE*) malloc( this->allocatedBytes );
	memcpy( this->num, other.num, this->allocatedBytes );
	return *this;
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

BigNum& BigNum::operator+=(const BigNum& other)
{
	BigNum retVal = *this + other;
	*this = retVal;
	return *this;
}

BigNum BigNum::operator-(BigNum& other)
{
	BigNum me( *this );
	BigNum retVal( me.numDigits );
	
	if( other >= me )
		return retVal;

	bool firstNibble = true;
	for( int result = 0, i = 0, byteOffset = 0; i < (int) retVal.numDigits;
		i++, firstNibble = !firstNibble, firstNibble ? byteOffset++ : 0 )
	{
		BYTE a = me.num[byteOffset];
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

		// We gotsta borrow! FUCK!
		if( a < b )
		{
			// Find me some digits to use!
			for( int j = i + 1; j < (int) me.numDigits; j++ )
			{
				nibble* myNibs = (nibble*) &me.num[j / 2];
				if( j % 2 == 0 )
				{
					if( myNibs->nibMost != 0 )
					{
						myNibs->nibMost--;
						break;
					}
					else
						myNibs->nibMost = base - 1;
				}
				else
				{
					if( myNibs->nibLeast != 0 )
					{
						myNibs->nibLeast--;
						break;
					}
					else
						myNibs->nibLeast = base - 1;
				}
			}
			a += base;
		}

		result = a - b;
		result = (firstNibble ? result << 4 : result );
		retVal.num[byteOffset] |= result;
	}
	return retVal;
}

BigNum& BigNum::operator-=(BigNum& other)
{
	BigNum retVal = *this - other;
	*this = retVal;
	return *this;
}

BigNum BigNum::operator*(const BigNum& other)
{
	return classicalMultiply( other );
}

BigNum BigNum::operator/(BigNum& other)
{
	BigNum quotient, remainder;
	classicalDivide( other, quotient, remainder );
	return quotient;
}

BigNum BigNum::operator%(BigNum& other)
{
	BigNum quotient, remainder;
	classicalDivide( other, quotient, remainder );
	return remainder;
}

/****************** Exponentiation by squaring: O((n*log(x))^k) ******************
x^15 = (x^7)*(x^7)*x
x^7 = (x^3)*(x^3)*x
x^3 = x*x*x
**************************************************************************/
// BigNum BigNum::operator^(const BigNum& other)
// {
// 	if( other.numDigits == 0 )
// 		return 1;
// 	if( other.numDigits == 1 && other.num[0] == 1 )
// 		return *this;
// 	if( other % 2 == 0 )
// 		return (*this * *this) ^ other / 2;
// 	else
// 
// }

/****************** Elementary method for multiplication: O(n^2) ******************
   43241
     621
   ----- *
   43241 <--   1 * 43241
  864820 <--  20 * 43241, basically do 2 * 43241 and append a zero
25944600 <-- 600 * 43241, basically do 6 * 43241 and append two zeroes
-------- +
26852661 <-- Add up results, remember to carry
 **************************************************************************/
BigNum BigNum::classicalMultiply( const BigNum& other )
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

void BigNum::classicalDivide( BigNum& other, BigNum& quotient, BigNum& remainder )
{
	quotient.initialize( this->numDigits );

	if( other > *this )
		return;
	else if( other == *this )
	{
		quotient.num[0] = 1;
		quotient.numDigits = 1;
		return;
	}

	nibble* nextNumDigit = (nibble*) (this->num + (this->numDigits - 1) / 2);
	bool nextNumDigitLeast = this->numDigits % 2 == 0 ? true : false;

 	// For EACH digit in the numerator/dividend...
	BigNum dividendWorkingSet( this->numDigits );
	BigNum multiple;
	for( int i = 0, k = this->numDigits - 1; i < this->numDigits; i++, k-- )
	{
		// Let's bring the next digit to the dividendWorkingSet:
		dividendWorkingSet.validateNumDigits();
		if( dividendWorkingSet.numDigits == 0 )
		{
			dividendWorkingSet.num[0] = (nextNumDigitLeast ? nextNumDigit->nibLeast : nextNumDigit->nibMost) << 4;
			dividendWorkingSet.numDigits = 1;
			nextNumDigitLeast = !nextNumDigitLeast;
		}
		else
		{
			// This is no joke, we need to move everything down one nibble so I can set the most significant nibble >.>
			BYTE nextDigit = nextNumDigitLeast ? nextNumDigit->nibLeast : nextNumDigit->nibMost;
			for( int j = 0; j <= (dividendWorkingSet.numDigits + 1) / 2; j++ )
			{
				nibble* next = (nibble*)&dividendWorkingSet.num[j];
				BYTE prev = next->nibMost;
				next->nibMost = nextDigit;
				nextDigit = next->nibLeast;
				next->nibLeast = prev;
			}
			dividendWorkingSet.numDigits++;
			nextNumDigitLeast = !nextNumDigitLeast;
			if( nextNumDigitLeast == true )
				nextNumDigit--;
		}

		if( other > dividendWorkingSet )
			continue;
		else if( dividendWorkingSet == other )
		{
			quotient.num[k / 2] |= ((k % 2 == 0) ? 1 << 4 : 1);
			dividendWorkingSet.numDigits = 0;
		}
		else
		{
			multiple = other;
			for( int j = 1; j < base; j++, multiple += other )
			{
				if( multiple + other > dividendWorkingSet )
				{
					quotient.num[k / 2] |= ((k % 2 == 0) ? j << 4 : j );
					dividendWorkingSet -= multiple;
					break;
				}
			}
		}
	}
	remainder = dividendWorkingSet;

// 	// For EACH digit in the numerator/dividend...
// 	// BUT, let's just skip ahead and grab the same number of digits denominator/divisor
// 	remainder = *this;
// 	BigNum dividendWorkingSet( this->numDigits );
// 	BigNum multiple;
// 	for( int k = this->numDigits; remainder > other; dividendWorkingSet.numDigits = 0 )
// 	{
// 		// Let's get the working set larger than the remainder
// 		for( int i = other.numDigits; i < (int) remainder.numDigits && other > dividendWorkingSet; i++ )
// 		{
// 			dividendWorkingSet.numDigits = i;
// 			memcpy( dividendWorkingSet.num, remainder.num, (i / 2) + 1 );
// 		}
// 
// 		multiple = other;
// 		for( int j = 1; j < base; j++, multiple+=multiple )
// 		{
// 			if( multiple + other > dividendWorkingSet )
// 			{
// 				quotient.num[k / 2] |= ((k % 2 == 0) ? j << 4 : j );
// 				remainder -= multiple;
// 				k++;
// 				break;
// 			}
// 		}
// 	}
}

void BigNum::validateNumDigits()
{
	// ensures numDigits is correct
	for( int i = numDigits - 1; i >= 0; i-- )
	{
		if( ((i % 2) == 0 ? (num[i / 2] & 0xF0) >> 4 : num[i / 2] & 0x0F) == 0 )
			numDigits--;
		else
			break;
	}

	// Should we also clear the rest that isn't in the numDigits?
	if( numDigits % 2 != 0 )
		num[numDigits / 2] &= 0xF0;
	unsigned short extraClears = allocatedBytes - ((numDigits + 1) / 2);
	if( extraClears > 0 )
		SecureZeroMemory( num + ((numDigits + 1) / 2), extraClears );
}

void BigNum::clear()
{
	allocatedBytes = numDigits = 0;
	if( num != NULL )
		free( num );
	num = NULL;
}
