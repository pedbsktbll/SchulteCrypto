/*
* AES.cpp
* AES implementation using CBC/ECB with
* support of 128 or 256-bit keys.
*
*  Created on: Jan 15, 2012
*      Author: josh
*/

#include "AES.h"
#include <time.h>

AES::AES()
{
	key = NULL;
}

AES::~AES()
{
	if( key != NULL )
		delete key;
}

bool AES::setKey( BYTE* userKey, unsigned char userKeySize )
{
	if( key != NULL )
		delete key;
	key = new CipherKey( userKey, userKeySize );

	if( key->getKey() == NULL )
	{
		delete key;
		return false;
	}
	return true;
}

bool AES::getKey( BYTE* userKey, unsigned char& userKeySize )
{
	unsigned char keySize = key->getKeySize();
	if( userKey == NULL || userKeySize > keySize )
		return false;
	memcpy( userKey, key->getKey(), keySize );
	userKeySize = keySize;
	return true;
}

bool AES::encrypt( BYTE* buffer, DWORD buffSize, DWORD& commitSize, bool cbcMode /*= true*/ )
{
	if( key == NULL )
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

	// Generate RoundKeys
	key->generateRoundKeys();

	// Set total number of rounds for the state:
//	unsigned char totalRounds = keySize / 4 + 6;
	State s( key );

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
	return true;
}

bool AES::decrypt( BYTE* buffer, DWORD buffSize, bool cbcMode /*= true*/ )
{
	if( key == NULL || buffer == NULL || buffSize % 16 != 0 )
		return false;

	// Generate RoundKeys
	key->generateRoundKeys();

	// Set total number of rounds for the state:
	//	unsigned char totalRounds = keySize / 4 + 6;
	State s( key );

	// Initialization Vector
	BYTE* initVector = new BYTE[16];
	randBytes( initVector, 16 );
	BYTE* lastState = initVector;

	// Get Cipher for each block
	for( DWORD totalCiphers = buffSize / 16, completedCiphers = 0; completedCiphers < totalCiphers; completedCiphers++ )
	{
		s.nextBytes( &buffer[completedCiphers * 16] );

		if( cbcMode )
			s._xor( lastState );

		s.decipher();

		if( cbcMode )
			lastState = s.getBytes();
	}
	return true;
}

void AES::initializeTables()
{

}

void AES::randBytes( BYTE* buffer, DWORD size )
{
	srand( (unsigned int) time( NULL ) );
	for( DWORD i = 0; i < size; i++ )
		buffer[i] = rand() % 256;
}
