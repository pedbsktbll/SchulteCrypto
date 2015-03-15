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

bool BigNum::operator==(ULONGLONG& other)
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
	for( int carry = 0, result = 0, i = 0, byteOffset = 0; i < (int) retVal.numDigits;
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
	retVal.validateNumDigits();
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
	retVal.validateNumDigits();
	return retVal;
}

BigNum& BigNum::operator-=(BigNum& other)
{
	BigNum retVal = *this - other;
	*this = retVal;
	return *this;
}

BigNum BigNum::operator*(BigNum& other)
{
//	return classicalMultiply( other );
	return karatsubaMultiply( other );
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
BigNum BigNum::operator^(BigNum& other)
{
	BigNum zero( "0" );
	BigNum one( "1" );
	BigNum two( "2" );
	if( other.numDigits == 0 || other == zero )
		return one;
	if( other == one )
		return *this;

	BigNum halved = *this ^ (other / two);
// 	BigNum testHalved = this->classicalPow( other / two );
// 	if( !(halved == testHalved) )
// 	{
// 		DebugBreak();
// 	}

//	DWORD d = halved.numDigits + 1;
// 	char* arr = new char[d];
// 	halved.toArray( arr, d );

	return other % two == zero ? halved * halved : halved * halved * *this;
}

BigNum BigNum::classicalPow( BigNum& other )
{
	BigNum zero( "0" );
	BigNum one( "1" );
	BigNum two( "2" );
	if( other.numDigits == 0 || other == zero )
		return one;
	if( other == one )
		return *this;

	BigNum halved = this->classicalPow(other / two);

	//	DWORD d = halved.numDigits + 1;
	// 	char* arr = new char[d];
	// 	halved.toArray( arr, d );

	BigNum m = halved.classicalMultiply( halved );
	return other % two == zero ? m : m.classicalMultiply(*this);
}

BigNum BigNum::pow_modulo( BigNum& power, BigNum& modulo )
{
	BigNum zero( "0" );
	BigNum one( "1" );
	BigNum two( "2" );
	if( power.numDigits == 0 || power == zero )
		return one;
	if( power == one )
		return *this;

	BigNum halved = this->pow_modulo( power / two, modulo );
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

	// Multiplies everything out
	BigNum retVal( bot->numDigits + top->numDigits );
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
	return retVal;
}

BigNum BigNum::karatsubaMultiply( BigNum& other )
{
	this->validateNumDigits();
	other.validateNumDigits();
	if( this->numDigits == 0 || other.numDigits == 0 )
		return (ULONGLONG) 0;

	if( numDigits <= 7 && other.numDigits <= 7 )
	{
		return this->toULL() * other.toULL();
// 		BigNum retVal = this->toULL() * other.toULL();
// 		BigNum test = this->classicalMultiply( other );
// 		if( !(retVal == test) )
// 		{
// 			DebugBreak();
// 		}
// 		return retVal;
	}

	if( this->numDigits <= 2 || other.numDigits <= 2 )
		return this->classicalMultiply( other );

	DWORD totalDigits = this->numDigits > other.numDigits ? this->numDigits : other.numDigits;
	DWORD m = (totalDigits + 1) / 2;

	// m must be less than both string lenths
	if( this->numDigits < m || other.numDigits < m )
		return this->classicalMultiply( other );

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
	BigNum retVal = z2 + z1 + z0;
	return retVal;
}


void BigNum::classicalDivide( BigNum& other, BigNum& quotient, BigNum& remainder )
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
//			for( int j = 0; j <= (dividendWorkingSet.numDigits + 1) / 2; j++ )
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
					dividendWorkingSet.increaseCapacity( other.allocatedBytes + 1 );
					break;
				}
			}
		}
	}
	remainder = dividendWorkingSet;
	remainder.validateNumDigits();
	quotient.validateNumDigits();
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
