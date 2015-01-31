#include "AES.h"


AES::AES()
{
	key = NULL;
	keySize = 0;
}


AES::~AES()
{
	if( key != NULL )
		delete[] key;
}

bool AES::setKey( BYTE* userKey, DWORD userKeySize )
{
	if( (userKeySize != 128 && userKeySize != 192 && userKeySize != 256) || userKey == NULL )
		return false;
	if( key != NULL )
		delete[] key;
	key = new BYTE[userKeySize];
	memcpy( key, userKey, userKeySize );
	keySize = userKeySize;
}

bool AES::getKey( BYTE* userKey, DWORD& userKeySize )
{
	if( userKey == NULL || userKeySize > keySize )
		return false;
	memcpy( userKey, key, keySize );
	userKeySize = keySize;
}

bool AES::encrypt( BYTE* buffer, DWORD buffSize, DWORD& commitSize, bool cbcMode /*= true*/ )
{
	if( keySize == 0 || key == NULL )
		return false;

	// Pad buffer
	unsigned char padBytes = 16 - (buffSize % 16);
	if( buffSize + padBytes > commitSize )
	{
		commitSize += padBytes;
 		return false;
	}
	for( unsigned char i = 0; i < padBytes; i++ )
		buffer[buffSize + i] = padBytes;

	// Set total number of rounds for the state:
	unsigned char totalRounds = keySize * 4 + 6;
	State s( totalRounds );

	// Initialization Vector
	BYTE* initVector = new BYTE[16];
	randBytes( initVector, 16 );
	BYTE* lastState = initVector;

	// Get Cipher for each block
	for( DWORD totalCiphers = (buffSize + padBytes) / 16, completedCiphers = 0; completedCiphers < totalCiphers; completedCiphers++ )
	{
		s.nextBytes( &buffer[completedCiphers * 16] );

		if( cbcMode )
			s._xor( lastState );

		s.cipher();

		if( cbcMode )
			lastState = s.getBytes();
	}
}

bool AES::decrypt( BYTE* buffer, DWORD buffSize, bool cbcMode /*= true*/ )
{
	if( keySize == 0 || key == NULL || buffer == NULL || buffSize % 16 != 0 )
		return false;
}

AES::State::State( unsigned char rounds )
{
	numRounds = rounds;
}

void AES::State::nextBytes( BYTE* next)
{
	state = next;
}

BYTE* AES::State::getBytes()
{
	return state;
}

void AES::State::cipher()
{
	addRoundKey( 0 );
	for( int i = 0; i < numRounds; i++ )
	{
		subBytes();
		shiftRows();
		mixCols();
		addRoundKey(i);
	}

	subBytes();
	shiftRows();
	addRoundKey( numRounds );
}