#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#else
//#define _CrtMemState
#pragma warning(disable : 4005)
#define _CrtMemCheckpoint(...)
#define _CrtMemDifference(...) false
#define _CrtMemDumpStatistics(...)
#define _CrtDumpMemoryLeaks()
#define _CrtSetDbgFlag(...)
#endif

#include "BigNum.h"
#include <Math.h>

#define SET_NIBBLE(arr, digitNum, val) arr[(digitNum) / 2] = ((digitNum) % 2 == 0 ? arr[(digitNum) / 2] & 0x0F | ( (val) << 4 ) : arr[(digitNum) / 2] & 0xF0 | (val))
#define GET_NIBBLE(arr, digitNum) ((digitNum) % 2 == 0 ? (arr[(digitNum) / 2] & 0xF0) >> 4 : (arr[(digitNum) / 2] & 0x0F))

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
	this->numDigits = this->allocatedBytes = 0;
	this->num = NULL;
	initialize( num );
}

//template<typename t> BigNum::BigNum( T t );
BigNum::BigNum(ULONGLONG u)
{
	this->allocatedBytes = sizeof( u );
	this->numDigits = this->allocatedBytes * 2;
	this->num = (BYTE*) malloc( this->allocatedBytes );

	BYTE* b = (BYTE*) &u;
	for( DWORD i = 0; i < allocatedBytes; i++ )
		this->num[i] = ((b[i] & 0xF0) >> 4) | ((b[i] & 0x0F) << 4);
	this->validateNumDigits();
}

BigNum::~BigNum()
{
	if( num != NULL )
		free( num );
	this->num = NULL;
	this->numDigits = 0;
	this->allocatedBytes = 0;
}

void BigNum::initialize( DWORD numDigits, BYTE* num /*= NULL*/, bool reverseOrder /*= true */ )
{
	this->numDigits = numDigits;
	this->allocatedBytes = 0;
	if( this->num != NULL )
		free( this->num );
	this->num = NULL;

	if( numDigits > 0 )
	{
		this->allocatedBytes = (numDigits + 1) / 2;
		this->num = (BYTE*) calloc( allocatedBytes, 1 );
	}

	if( num != NULL && numDigits > 0 )
	{
//		this->numDigits = numDigits;
		if( reverseOrder )
			memcpy( this->num, num, numDigits );
		else
		{
			for( DWORD i = 0; i < allocatedBytes; i++ )
			{
				BYTE b = num[allocatedBytes - 1 - i];
				this->num[i] = ((b & 0x0F) << 4) | ((b & 0xF0) >> 4);
			}
		}
	}
}

void BigNum::initialize( char* num )
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

bool BigNum::operator==(ULONGLONG other)
{
	return this->toULL() == other ? true : false;
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

BigNum& BigNum::operator=(BigNum& other)
{
	this->allocatedBytes = other.allocatedBytes;
	this->numDigits = other.numDigits;
	if( this->num != NULL )
		free( this->num );
	this->num = (BYTE*) malloc( this->allocatedBytes );
	memcpy( this->num, other.num, this->allocatedBytes );
	return *this;
}

BigNum BigNum::operator+(BigNum& other)
{
	static BigNum retVal; // Efficiency? only creates object on stack once?
	this->classicalAddition( other, &retVal );
	return retVal;
}

BigNum& BigNum::operator+=(BigNum& other)
{
	this->classicalAddition( other, NULL );
	return *this;
}

BigNum BigNum::operator-(BigNum& other)
{
	static BigNum retVal; // Efficiency? only creates object on stack once?
	this->classicalSubtract( other, &retVal );
	return retVal;
}

BigNum& BigNum::operator-=(BigNum& other)
{
	this->classicalSubtract( other, NULL );
	return *this;
}

BigNum BigNum::operator*(BigNum& other)
{
//	return classicalMultiply( other );
	return karatsubaMultiply( other );
}

BigNum BigNum::operator/(BigNum& other)
{
	static BigNum quotient, remainder;
	classicalDivision( other, quotient, remainder );
	return quotient;
}

BigNum BigNum::operator%(BigNum& other)
{
	static BigNum quotient, remainder;
	classicalDivision( other, quotient, remainder );
	return remainder;
}

BigNum BigNum::operator^(BigNum& other)
{
	return classicalExponent( other );
}

BigNum BigNum::operator<<(DWORD numZeros)
{
	BigNum retVal( *this );
	DWORD newSize = ((retVal.numDigits + 1) / 2) + ((numZeros + 1) / 2);
	if( retVal.allocatedBytes < newSize )
		retVal.increaseCapacity( newSize );

	memmove( retVal.num + (numZeros + 1) / 2, retVal.num, (retVal.numDigits + 1) / 2 );
	SecureZeroMemory( retVal.num, (numZeros + 1) / 2 );

	// If we added an odd number of zeros, now we have to move everything over by a fucking nibble!
	if( numZeros % 2 != 0 )
	{
		BYTE nextByte = 0, newByte = 0;
		for( DWORD i = 0; i <= retVal.numDigits / 2; i++ )
			retVal.num[numZeros / 2 + i] = ((retVal.num[numZeros / 2 + i] & 0x0F) << 4) | ((retVal.num[numZeros / 2 + i + 1] & 0xF0) >> 4);
		if( retVal.numDigits % 2 != 0 )
			retVal.num[newSize - 1] = 0;
		else
			retVal.num[newSize - 1] &= 0xF0;
	}
	retVal.numDigits += numZeros;
	return retVal;
}

BigNum BigNum::operator>>(DWORD other)
{
	BigNum retVal( *this );
	memmove( retVal.num, retVal.num + other / 2, (retVal.numDigits - other + 1) / 2 );
	SecureZeroMemory( retVal.num + (retVal.numDigits - other + 1) / 2, other / 2 );

	// If we added an odd number of zeros, now we have to move everything over by a fucking nibble!
	if( other % 2 != 0 )
	{
		BYTE nextByte = 0, newByte = 0;
		for( DWORD i = 0; i < retVal.numDigits / 2; i++ )
			retVal.num[i] = ((retVal.num[i] & 0x0F) << 4) | ((i + 1 >= retVal.allocatedBytes) ? 0 : ((retVal.num[i + 1] & 0xF0) >> 4));
	}
	retVal.numDigits -= other;
	return retVal;
}

void BigNum::classicalAddition( BigNum& other, BigNum* retVal )
{
	DWORD sumNumDigits = (this->numDigits > other.numDigits ? this->numDigits : other.numDigits) + 1;
	if( retVal == NULL )
	{
		retVal = this;
		retVal->padDigits( sumNumDigits );
	}
	else
		retVal->initialize( sumNumDigits );

	bool firstNibble = true;
	for( int carry = 0, result = 0, i = 0, byteOffset = 0; i < (int) sumNumDigits;
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
//		result = (firstNibble ? (result % base) << 4 : (result % base));
//		retVal->num[byteOffset] |= result;
		retVal->num[byteOffset] = (firstNibble ? ((result % base) << 4) | (retVal->num[byteOffset] & 0x0F) : (result % base) | (retVal->num[byteOffset] & 0xF0));
	}
	retVal->validateNumDigits();
}

void BigNum::classicalSubtract( BigNum& other, BigNum* retVal )
{
//	BigNum me( *this );
	DWORD differenceNumDigits = this->numDigits;
	if( retVal == NULL )
		retVal = this;
	else
		//		retVal->initialize( this->numDigits );
		*retVal = *this;

	if( other >= *this )
	{
		retVal->numDigits = 0;
		return;
	}
	
	
	bool firstNibble = true;
	for( int result = 0, i = 0, byteOffset = 0; i < (int) differenceNumDigits;
		i++, firstNibble = !firstNibble, firstNibble ? byteOffset++ : 0 )
	{
		BYTE a = retVal->num[byteOffset];
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
			for( int j = i + 1; j < (int) retVal->numDigits; j++ )
			{
				nibble* myNibs = (nibble*) &retVal->num[j / 2];
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
//		result = (firstNibble ? result << 4 : result);
//		retVal.num[byteOffset] |= result;
		retVal->num[byteOffset] = (firstNibble ? (result << 4) | (retVal->num[byteOffset] & 0x0F) : result | (retVal->num[byteOffset] & 0xF0));
	}
	retVal->validateNumDigits();
}

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
void BigNum::classicalMultiply( BigNum& other, BigNum& retVal )
{
	const BigNum *bot, *top;
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

	// Multiplies everything out
//	retVal.padDigits( bot->numDigits + top->numDigits );
	retVal.initialize( bot->numDigits + top->numDigits );
	for( int i = 0, bByteOffset = 0, carry = 0, retValIndx = 0, bFirstNibble = true; i < (int) bot->numDigits;
		i++, bFirstNibble = !bFirstNibble, bFirstNibble ? bByteOffset++ : 0 )
	{
		int b = (bFirstNibble ? (bot->num[bByteOffset] & 0xF0) >> 4 : bot->num[bByteOffset] & 0x0F);
		carry = 0;
		retValIndx = i;
		for( int j = 0, tByteOffset = 0, result = 0, tFirstNibble = true; j < (int) top->numDigits;
			j++, retValIndx++, tFirstNibble = !tFirstNibble, tFirstNibble ? tByteOffset++ : 0 )
		{
			int t = (tFirstNibble ? (top->num[tByteOffset] & 0xF0) >> 4 : top->num[tByteOffset] & 0x0F);
			result = t * b + carry +
				((retValIndx  % 2 == 0) ? (retVal.num[retValIndx / 2] & 0xF0) >> 4 : (retVal.num[retValIndx / 2] & 0x0F));
			carry = result / base;
			result = ((retValIndx % 2 == 0) ? ((result % base) << 4) | (retVal.num[retValIndx / 2] & 0x0F) : (result % base) | (retVal.num[retValIndx / 2] & 0xF0));
			retVal.num[retValIndx / 2] = result;
		}
		// Hmm, I could maybe group this into the for-loop...
		retVal.num[retValIndx / 2] |= ((retValIndx % 2 == 0) ? (carry % base) << 4 : (carry % base));
	}
	retVal.validateNumDigits();
}

void BigNum::classicalDivision( BigNum& other, BigNum& quotient, BigNum& remainder )
{
	this->validateNumDigits();
	other.validateNumDigits();
	quotient.initialize( this->numDigits );

	if( other > *this )
	{
		remainder = *this;
		return;
	}
	else if( other == *this )
	{
		quotient.num[0] = 1 << 4;
		quotient.numDigits = 1;
		return;
	}

	nibble* nextNumDigit = (nibble*) (this->num + (this->numDigits - 1) / 2);
	bool nextNumDigitLeast = this->numDigits % 2 == 0 ? true : false;

 	// For EACH digit in the numerator/dividend...
	BigNum dividendWorkingSet( this->numDigits );
	BigNum multiple;
	for( int i = 0, k = this->numDigits - 1; i < (int) this->numDigits; i++, k-- )
	{
		// Let's bring the next digit to the dividendWorkingSet:
		dividendWorkingSet.validateNumDigits();
		if( dividendWorkingSet.numDigits == 0 )
		{
			dividendWorkingSet.num[0] = (nextNumDigitLeast ? nextNumDigit->nibLeast : nextNumDigit->nibMost) << 4;
			dividendWorkingSet.numDigits = 1;
		}
		else
		{
			// This is no joke, we need to move everything down one nibble so I can set the most significant nibble >.>
			BYTE nextDigit = nextNumDigitLeast ? nextNumDigit->nibLeast : nextNumDigit->nibMost;
			for( int j = 0; j <= (int) ((dividendWorkingSet.numDigits) / 2); j++ )
			{
				nibble* next = (nibble*)&dividendWorkingSet.num[j];
				BYTE prev = next->nibMost;
				next->nibMost = nextDigit;
				nextDigit = next->nibLeast;
				next->nibLeast = prev;
			}
			dividendWorkingSet.numDigits++;
		}
		nextNumDigitLeast = !nextNumDigitLeast;
		if( nextNumDigitLeast == true )
			nextNumDigit--;

		int j = 0;
		for( ; j < base && dividendWorkingSet >= other; j++ )
				dividendWorkingSet.classicalSubtract( other, NULL );

		if( j > 0 )
			quotient.num[k / 2] |= ((k % 2 == 0) ? j << 4 : j);
	}
	remainder = dividendWorkingSet;
	remainder.validateNumDigits();
	quotient.validateNumDigits();
}

// void BigNum::appliedCryptoDivision( BigNum& other, BigNum& quotient, BigNum& remainder )
// {
// 	this->validateNumDigits();
// 	other.validateNumDigits();
// 	BigNum x = *this;
// 	BigNum y = other;
// 
// 	DWORD n = this->numDigits - 1;
// 	DWORD t = other.numDigits - 1;
// 	DWORD q = n - t;
// 	quotient.initialize( q );
// 
// 	if( other > *this )
// 	{
// 		remainder = *this;
// 		return;
// 	}
// 	else if( other == *this )
// 	{
// 		quotient.num[0] = 1 << 4;
// 		quotient.numDigits = 1;
// 		return;
// 	}
// 
// 	BigNum bnt(base ^ q);
// 	BigNum multiple = y.karatsubaMultiply( bnt );
// 	nibble* mostSignificant = (nibble*) &quotient.num[q / 2];
// 	while( x >= multiple )
// 	{
// 		if( q % 2 == 0 )
// 			mostSignificant->nibMost++;
// 		else
// 			mostSignificant->nibLeast++;
// 		x -= multiple;
// 	}
// 
// //	nibble* yt = (nibble*)&y.num[t / 2];
// 	BYTE yt = (t % 2 == 0 ? (y.num[t / 2] & 0xF0) >> 4 : (y.num[t / 2] & 0x0F));
// 	for( int i = n; i >= t + 1; i-- )
// 	{
// 		BYTE xi = GET_NIBBLE( x.num, i );
// 		BYTE xi_1 = GET_NIBBLE( x.num, i - 1 );
// 		BYTE xi_2 = GET_NIBBLE( x.num, i - 2 );
// 		BYTE yt_1 = GET_NIBBLE( y.num, t - 1 );
// 		BYTE q_i_t_1 = GET_NIBBLE( quotient.num, i - t - 1 );
// 		if( xi == yt )
// 			SET_NIBBLE( quotient.num, i - t - 1, base - 1 );
// 		else
// 			SET_NIBBLE( quotient.num, i - t - 1, (xi * base + xi_1) / yt );
// 
// 		while( q_i_t_1 * (yt * base + yt_1) > xi * base * base + xi_1 * base + xi_2 )
// 		{
// 			SET_NIBBLE( q.num, i - t - 1, --q_i_t_1 );
// 		}
// 		BigNum fuckm( (ULONGLONG) q_i_t_1 * (ULONGLONG) y * (ULONGLONG) base ^ (i - t - 1) );
// 	}
// }

/****************** Exponentiation by squaring: O((n*log(x))^k) ******************
x^15 = (x^7)*(x^7)*x
x^7 = (x^3)*(x^3)*x
x^3 = x*x*x
**************************************************************************/
BigNum BigNum::classicalExponent( BigNum& exponent )
{
	BigNum retVal, halved, two;
	if( exponent.numDigits == 0 || exponent == 0 )
		return (ULONGLONG) 1;
	if( exponent == 1 )
		return *this;

	exponent.right_shift( 1, &two );
	halved = classicalExponent( two );

	//	DWORD d = halved.numDigits + 1;
	// 	char* arr = new char[d];
	// 	halved.toArray( arr, d );

	halved.classicalMultiply( halved, retVal );
	if( ((exponent.num[0] & 0xF0) >> 4) % 2 != 0 )
	{
		halved = retVal;
		halved.classicalMultiply( *this, retVal );
	}
	return retVal;
}

BigNum BigNum::pow_modulo( BigNum& power, BigNum& modulo )
{
	BigNum two;
	if( power.numDigits == 0 || power == 0 )
		return (ULONGLONG) 1;
	if( power == 1 )
		return *this;

	power.right_shift( 1, &two );
	BigNum halved = this->pow_modulo( two, modulo );
	// 	BigNum testHalved = this->classicalPow( other / two );
	// 	if( !(halved == testHalved) )
	// 	{
	// 		DebugBreak();
	// 	}

	//	DWORD d = halved.numDigits + 1;
	// 	char* arr = new char[d];
	// 	halved.toArray( arr, d );

	//	BigNum retVal = power.num[0] % 2 == 0 ? halved * halved : halved * halved * *this;
	return ((power.num[0] & 0xF0) >> 4) % 2 == 0 ? (halved * halved) % modulo : (halved * halved * *this) % modulo;
}

BigNum BigNum::karatsubaMultiply( BigNum& other )
{
	BigNum retVal;
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits == 0 || other.numDigits == 0 )
		return (ULONGLONG) 0;

	if( numDigits <= 7 && other.numDigits <= 7 )
		return this->toULL() * other.toULL();

	if( this->numDigits <= 2 || other.numDigits <= 2 )
	{
		this->classicalMultiply( other, retVal );
		return retVal;
	}

	DWORD totalDigits = this->numDigits > other.numDigits ? this->numDigits : other.numDigits;
	DWORD m = (totalDigits + 1) / 2;

	// m must be less than both string lenths
	if( this->numDigits < m || other.numDigits < m )
	{
		this->classicalMultiply( other, retVal );
		return retVal;
	}

	this->padDigits( totalDigits );
	other.padDigits( totalDigits );

	// Get the lower half and upper half of each of the numbers to multiply
	BigNum thisLow( totalDigits ), otherLow( totalDigits ), thisHigh( totalDigits ), otherHigh( totalDigits ), thisSum( totalDigits ), otherSum( totalDigits );

	// From offset (totalDigits - m) / 2, we want (m + 1) / 2 BYTES for our low
	memcpy( thisLow.num, this->num, (m + 1) / 2 );
	memcpy( otherLow.num, other.num, (m + 1) / 2 );

	// From offset 0, we want total digits - m DIGITS for our high
	memcpy( thisHigh.num, &this->num[m / 2], ((totalDigits + 1) / 2) - m / 2 );
	memcpy( otherHigh.num, &other.num[m / 2], ((totalDigits + 1) / 2) - m / 2 );
	//	if( totalDigits > 3 && totalDigits % 2 != 0 )
	if( m % 2 != 0 )
	{
		thisLow.num[m / 2] &= 0xF0;
		thisHigh = thisHigh >> 1;
		otherLow.num[m / 2] &= 0xF0;
		otherHigh = otherHigh >> 1;
	}
	thisSum = thisLow + thisHigh;
	otherSum = otherLow + otherHigh;

	// [ this * other ] = [ x * y ] = [(x1 * B^m + x0) * (y1 * B^m + y0)] = (x1 * B^m + x0) * (y1 * B^m + y0) = [z2 * B^2m + z1 * B^m + z0]
	BigNum z2 = thisHigh.karatsubaMultiply( otherHigh );
	BigNum z0 = thisLow.karatsubaMultiply( otherLow );
	BigNum z1 = thisSum.karatsubaMultiply( otherSum ) - z2 - z0;

	//	m = totalDigits - m;
	//	z2.addZeros( 2 * m ); // z2 = z2 * 16 ^ ( 2 * 2 )
	z2 = z2 << (2 * m);
	//	z1.addZeros( m ); // z1 = z1 * 16 ^ (2 )
	z1 = z1 << m;
	retVal = z2 + z1 + z0;
	return retVal;
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
	DWORD extraClears = allocatedBytes - ((numDigits + 1) / 2);
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

void BigNum::increaseCapacity( DWORD totalBytes )
{
	if( this->allocatedBytes >= totalBytes )
		return;

	this->num = (BYTE*) realloc( this->num, totalBytes );
	this->allocatedBytes = totalBytes;
}

void BigNum::padDigits( DWORD totalDigits )
{
	if( this->numDigits >= totalDigits )
		return;

	DWORD newSize = (totalDigits + 1) / 2;
	if( this->allocatedBytes < newSize )
		increaseCapacity( newSize );

	SecureZeroMemory( this->num + (this->numDigits + 1) / 2, (totalDigits + 1) / 2 - ((this->numDigits + 1) / 2) );
	this->numDigits = totalDigits;
}

bool BigNum::toArray( char* array, DWORD& len )
{
	if( array == NULL || len < (this->numDigits + 1) / 2 )
		return false;

	DWORD indx = 0;
	BYTE b = 0;
	len = (this->numDigits + 1) / 2;
	if( this->numDigits % 2 != 0 )
	{
		b = (num[len - 1] & 0xF0) >> 4;
		array[indx++] = (b < 10 ? b + '0' : b - 10 + 'A');
	}
	
	for( DWORD i = indx; i < len; i++ )
	{
		b = num[len - 1 - i];
		array[indx++] = ((b & 0x0F) < 10 ? (b & 0x0F) + '0' : (b & 0x0F) - 10 + 'A');
		array[indx++] = (((b & 0xF0) >> 4) < 10 ? ((b & 0xF0) >> 4) + '0' : ((b & 0xF0) >> 4) - 10 + 'A');
	}
	array[indx++] = '\0';
	len = indx;
	return true;
}

ULONGLONG BigNum::toULL()
{
	if( numDigits > 16 )
		return -1;

	ULONGLONG retVal = 0;
	BYTE* b = (BYTE*) &retVal;
	for( DWORD i = 0; i < /*allocatedBytes*/(numDigits + 1) / 2; i++ )
		b[i] = ((this->num[i] & 0xF0) >> 4) | ((this->num[i] & 0x0F) << 4);
	return retVal;
}

BigNum BigNum::modInverse( BigNum& other )
{
	BigNum two( "2" );
	return pow_modulo(other - two, other);
}

void BigNum::left_shift( DWORD numShifts, BigNum* retVal )
{
	if( retVal == NULL )
		retVal = this;
	else
		*retVal = *this;

	DWORD newSize = (retVal->numDigits * 4 + numShifts + 7) / 8; 
	if( retVal->allocatedBytes < newSize )
		retVal->increaseCapacity( newSize );

	// Move (numShifts + 7) / 8... This *may* shift us too far...
	memmove( retVal->num + (numShifts + 7) / 8, retVal->num, (retVal->numDigits + 1) / 2 );
	SecureZeroMemory( retVal->num, (numShifts + 7) / 8 );
//	retVal->numDigits += (numShifts + 3) / 4;
	retVal->numDigits += ((numShifts + 7) / 8) * 2;

	// We may have shifted too far! If so, we right shift back 8 - numShifts % 8 ! :D Hopefully we don't right shift too far
	DWORD lftShiftRemainder = numShifts % 8;
	if( lftShiftRemainder != 0 )
		retVal->right_shift( 8 - lftShiftRemainder, NULL );
}

void BigNum::right_shift( DWORD numShifts, BigNum* retVal )
{
	if( retVal == NULL )
		retVal = this;
	else
		*retVal = *this;

	if( numShifts / 8 >= (retVal->numDigits + 1) / 2 )
	{
		retVal->numDigits = 0;
		SecureZeroMemory( retVal->num, retVal->allocatedBytes );
		return;
	}

	// Let's actually right shift by the % 8 per byte first, THEN move the block
	int rtShiftRemainder = numShifts % 8;
	if( rtShiftRemainder != 0 )
	{
		BYTE nextByte = 0x00;
		for( int i = ((this->numDigits + 1) / 2) - 1; i >= (int) (numShifts / 8); i-- )
		{
			BYTE thisByte = ((retVal->num[i] & 0xF0) >> 4) | ((retVal->num[i] & 0x0F) << 4); // Flip the nibbles, as this is how it's interpreted...
			BYTE thisLeftover = (thisByte & (0xFF >> (8 - rtShiftRemainder))) << (8 - rtShiftRemainder);
			thisByte = (thisByte >> rtShiftRemainder) | nextByte;
			retVal->num[i] = ((thisByte & 0xF0) >> 4) | ((thisByte & 0x0F) << 4); // Flip it back...
			nextByte = thisLeftover;
		}
	}

	if( numShifts / 8 > 0 )
	{
		// Right shift by numShifts / 8... We may not have shifted far enough
		memmove( retVal->num, retVal->num + numShifts / 8, ((retVal->numDigits + 1) / 2) - numShifts / 8 );
		SecureZeroMemory( retVal->num + ((retVal->numDigits + 1) / 2) - numShifts / 8, numShifts / 8 );
	}

	retVal->numDigits -= numShifts / 4;
	if( numShifts % 4 != 0 )
		retVal->validateNumDigits();
}

// Let M be a positive integer, and R and T integers such that R > m, gcd(m,R) = 1, and 0 <= T < mR.
// T * R ^ -1 mod m
// BigNum BigNum::montgomeryExponent( BigNum& other, BigNum& exponent )
// {
// 	BigNum x = *this;
// 	BigNum y = other;
// 	BigNum m = exponent;
// 	// We want to calculate x ^ y mod m
// 
// 	// Pre-compute R^2 mod m
// 	BigNum tempQuotient, tempRemainder;
// 	BigNum R( "1" );
// 	R.left_shift( m.numDigits * 2, NULL );
// 	R.classicalDivision( m, tempQuotient, tempRemainder );
// 	R = tempRemainder;
// 
// 	// W[1] = A * R^2 * R^-1 mod N = A * R mod N
// 	x.classicalDivision( m, tempQuotient, tempRemainder );
// 	BigNum w = tempRemainder;
// 
// 	// Montgomery multiplication: A = A * B * R^-1 mod N  (HAC 14.36)
// 	BigNum temp = w.karatsubaMultiply( R );
// 
// 
// 	// Montgomery Reduction: A = A * R^-1 * mod N
// 
// 
// 	for( int i = 0; )
// }

// Montgomery Multiplication: x * y mod m |||| this * B mod N |||| HAC 14.36
// Where 0 <= x, y < m, R = b ^ n, GCD(m,b) = 1, and m' = -m ^ -1 mod b
BigNum BigNum::montgomeryMultiply( BigNum &B, BigNum &N, ULONGLONG n)
{
// 	// -N ^ -1
// 	BigNum bnBase( "10" );
// 	BigNum n_inv = N.modInverse( bnBase );
// 
// 	BigNum tempQuotient, tempRemainder, temp;
// 
// 	// a_reduc = a * R mod N
// 	BigNum a_reduc = this->karatsubaMultiply( R );
// 	a_reduc.classicalDivision( N, tempQuotient, tempRemainder );
// 	a_reduc = tempRemainder;
// 
// 	// b_reduc = b * R mod N
// 	BigNum b_reduc = B.karatsubaMultiply( R );
// 	b_reduc.classicalDivision( N, tempQuotient, tempRemainder );
// 	b_reduc = tempRemainder;
// 
// 	// c_reduc = a_reduc * b_reduc * R ^ -1
// 	BigNum c_reduc = a_reduc.karatsubaMultiply( b_reduc );
// 	c_reduc = c_reduc.montgomeryReduction( N, R, &n_inv );
// 
// 	// c = Reduc( c_reduc )
// 	return c_reduc.montgomeryReduction( N, R, &n_inv );
	BigNum b( (ULONGLONG) base );
	ULONGLONG n_inv = N.modInverse( b ).toULL();

	BigNum A( 0ULL );
	for( int i = 0; i < n; i++ )
	{
		ULONGLONG ui = (GET_NIBBLE( A.num, 0 ) + GET_NIBBLE( this->num, i ) * GET_NIBBLE( B.num, 0 )) * n_inv;
		BigNum xiy( (ULONGLONG) GET_NIBBLE( this->num, i ) );
		BigNum uim( ui );
		xiy = xiy.karatsubaMultiply( B );
		uim = uim.karatsubaMultiply( N );

		A.classicalAddition( xiy, NULL );
		A.classicalAddition( uim, NULL );
		A.right_shift( base, NULL );
	}

	if( A >= N )
		A.classicalSubtract( N, NULL );
	return A;
}

// Montgomery Reduction: Given T, m -> TR^-1 * mod m (HAC 14.85)
// GCD(m,b) = 1, R = b ^ n, m' = -m^-1 mod b, T < m * R
BigNum BigNum::montgomeryReduction( BigNum &N, ULONGLONG n, BigNum *N_inv /*= NULL */ )
{
// 	BigNum reduc, tempQuotient, tempRemainder, temp;
// 
// 	// IF n_inv passed NULL, then compute it
// 	if( N_inv == NULL )
// 	{
// 		BigNum* inv = new BigNum;
// 		*inv = N.modInverse( R );
// 		N_inv = inv;
// 	}
// 
// 	// temp = this % R
// 	this->classicalDivision( R, tempQuotient, tempRemainder );
// 
// 	// temp *= n_inv
// 	temp = tempRemainder.karatsubaMultiply( *N_inv );
// 
// 	// temp % R
// 	temp.classicalDivision( R, tempQuotient, tempRemainder );
// 	temp = tempRemainder.karatsubaMultiply( N );
// 
// 	// temp += this
// 	temp.classicalAddition( *this, NULL );
// 
// 	// temp / R
// 	temp.classicalDivision( R, tempQuotient, tempRemainder );
// 	return tempQuotient;

	BigNum b( (ULONGLONG) base );
	ULONGLONG n_inv = N.modInverse( b ).toULL();

	BigNum A = *this;
	for( int i = 0; i < n - 1; i++ )
	{
		BigNum ui( (ULONGLONG) (GET_NIBBLE( A.num, i ) * n_inv) % base);
		BigNum temp = N.karatsubaMultiply( ui );
		temp.left_shift( i, NULL );
		A.classicalAddition( temp, NULL );
	}
	A.right_shift( n, NULL );

	if( A >= N )
		A.classicalSubtract( N, NULL );
	return A;
}

// Sliding-Window exponentiation: X = A ^ E mod N (HAC 14.85)
// X = this ^ E mod N
BigNum BigNum::montSlidingWindowExp( BigNum &e, BigNum& N )
{
	const ULONGLONG k = 6;
	BigNum window[128]; // Our sliding window: 2 << k

	// Pre-computation:
	window[0] = *this;
	window[1] = this->karatsubaMultiply( *this );

	ULONGLONG max = (2 << (k - 1));// -1;
	for( ULONGLONG i = 1; i < max; i++ )
		window[2 * i] = window[2 * i - 2].karatsubaMultiply( window[1] );

	BigNum unfinished;
	return unfinished;
}

BigNum BigNum::slidingWindowExp( BigNum& e )
{
	const ULONGLONG k = 6;
	BigNum window[128]; // Our sliding window: 2 << k

	// Pre-computation:
	window[0] = *this;
	window[1] = this->karatsubaMultiply( *this );
	ULONGLONG max = (2 << (k - 1));// -1;
	for( ULONGLONG i = 1; i < max; i++ )
		window[2 * i] = window[2 * i - 2].karatsubaMultiply( window[1] );

	BigNum A( 1ULL );
	for( ULONGLONG i = e.numDigits - 1; i >= 0; i-- )
	{
		if( GET_NIBBLE( e.num, i ) == 0 )
			A = A.karatsubaMultiply( A );
		else
			;
	}

	BigNum unfinished;
	return unfinished;
}

// a * x + b * y = v, where v = gcd(x,y)
// See Algorithm 14.61 of HAC
// BigNum BigNum::modInverse( BigNum& other )
// {
// 	BigNum g( "1" );
// 	BigNum x = *this;
// 	BigNum y = other;
// 
// 	while( (GET_NIBBLE( x.num, 0 ) % 2 == 0) && GET_NIBBLE( y.num, 0 ) % 2 == 0 )
// 	{
// 		x.right_shift( 1, NULL );
// 		y.right_shift( 1, NULL );
// 		g.left_shift( 1, NULL );
// 	}
// 
// 	BigNum u = x;
// 	BigNum v = y;
// 	BigNum A( "1" );
// 	BigNum B( "0" );
// 	BigNum C( "0" );
// 	BigNum D( "1" );
// 
// STEP_4:
// 	while( GET_NIBBLE( u.num, 0 ) % 2 == 0 )
// 	{
// 		u.right_shift( 1, NULL );
// 		if( GET_NIBBLE( A.num, 0 ) % 2 == 0 && GET_NIBBLE( B.num, 0 ) % 2 == 0 )
// 		{
// 			A.right_shift( 1, NULL );
// 			B.right_shift( 1, NULL );
// 		}
// 		else
// 		{
// 			A.classicalAddition( y, NULL );
// 			A.right_shift( 1, NULL );
// 			B.classicalSubtract( x, NULL );
// 			B.right_shift( 1, NULL );
// 
// 		}
// 	}
// 
// 	while( GET_NIBBLE( v.num, 0 ) % 2 == 0 )
// 	{
// 		v.right_shift( 1, NULL );
// 		if( GET_NIBBLE( C.num, 0 ) % 2 == 0 && GET_NIBBLE( D.num, 0 ) % 2 == 0 )
// 		{
// 			C.right_shift( 1, NULL );
// 			D.right_shift( 1, NULL );
// 		}
// 		else
// 		{
// 			C.classicalAddition( y, NULL );
// 			C.right_shift( 1, NULL );
// 			D.classicalSubtract( x, NULL );
// 			D.right_shift( 1, NULL );
// 
// 		}
// 	}
// 
// 	if( u >= v )
// 	{
// 		u.classicalSubtract( v, NULL );
// 		A.classicalSubtract( C, NULL );
// 		B.classicalSubtract( D, NULL );
// 	}
// 	else
// 	{
// 		v.classicalSubtract( u, NULL );
// 		C.classicalSubtract( A, NULL );
// 		D.classicalSubtract( B, NULL );
// 	}
// 
// 	if( u.numDigits == 0 )
// 	{
// 		BigNum a = C;
// 		BigNum b = D;
// 		BigNum retV = v.karatsubaMultiply( g );
// 	}
// 	else
// 		goto STEP_4;
// }