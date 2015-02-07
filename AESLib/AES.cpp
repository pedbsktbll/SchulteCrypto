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

bool AES::encrypt( BYTE* buffer, DWORD& buffSize, DWORD& commitSize, bool cbcMode /*= false*/, BYTE* cbcIV /*= NULL*/ )
{
	if( key == NULL )
		return false;

	// Ensure the buffer commit size is big enough for us to encrypt
	unsigned char padBytes = 16 - (buffSize % 16);
	if( buffSize + padBytes + (cbcMode ? 16 : 0) > commitSize )
	{
		commitSize += padBytes + (cbcMode ? 16 : 0);
 		return false;
	}

	// Pad buffer
	for( unsigned char i = 0; i < padBytes; i++ )
		buffer[buffSize + i] = padBytes;

	// Generate RoundKeys
	key->generateRoundKeys();
	State s( key );

	// CBC Mode vars:
	BYTE initVector[16];
	BYTE* lastState = initVector;
	BYTE cbcBuffers[2][16];
	if( cbcMode )
	{
		// Initialization Vector for CBC mode
		cbcIV == NULL ? randBytes( initVector, 16 ) : memcpy( initVector, cbcIV, 16 );

		// Setup rotating buffers for CBC mode since we have to prepend the IV to the buffer to conform with the standard!
		memcpy( cbcBuffers[0], buffer, 16 );
		memcpy( cbcBuffers[1], initVector, 16 );
	}

	// Get Cipher for each block
	for( DWORD totalCiphers = (buffSize + padBytes) / 16, completedCiphers = 0; completedCiphers < totalCiphers; completedCiphers++ )
	{
		if( cbcMode )
		{
			unsigned char indx = (completedCiphers + 1) % 2;
			// Our last iteration generated ciphertext in cbcBuffers[indx], so let's write it out...
			memcpy( &buffer[completedCiphers * 16], cbcBuffers[indx], 16 );
			// ... and get the next buffer ready....
			memcpy( cbcBuffers[indx], &buffer[(completedCiphers + 1) * 16], 16 );

			// Set the next bytes to the other buffer that we got from our last iteration
			s.nextBytes( cbcBuffers[ completedCiphers % 2 ] );
			// ... and XOR it with the last state
			s._xor( lastState );
		}
		else
			s.nextBytes( &buffer[completedCiphers * 16] );		

		s.cipher();

		if( cbcMode )
			lastState = s.getBytes();
	}

	// Let's go ahead and write out the final ciphertext for CBC
	if( cbcMode )
		memcpy( &buffer[buffSize + padBytes], s.getBytes(), 16 );

	buffSize += padBytes + (cbcMode ? 16 : 0);
	return true;
}

bool AES::decrypt(BYTE* buffer, DWORD& buffSize, bool cbcMode /*= false*/)
{
	if( key == NULL || buffer == NULL || buffSize % 16 != 0 || buffSize < 32 )
		return false;

	// Generate RoundKeys
	key->generateRoundKeys();
	State s( key );

	// Initialization Vector
	BYTE initVector[16];
	BYTE* lastState = initVector;
	if( cbcMode )
		memcpy( initVector, buffer, 16 );

	// Get Cipher for each block
	for( DWORD totalCiphers = buffSize / 16, completedCiphers = cbcMode ? 1 : 0; completedCiphers < totalCiphers; completedCiphers++ )
	{
		s.nextBytes( &buffer[completedCiphers * 16] );
		s.decipher();

		if( cbcMode )
		{
			s._xor( lastState );
			lastState = s.getBytes();
			memcpy( &buffer[(completedCiphers - 1) * 16], lastState, 16 );
		}
	}

	// Let's zero out the padding AND additional cbc buffer if applicable
	DWORD zeroSize = 16 * (cbcMode ? 2 : 1);
	SecureZeroMemory( &buffer[buffSize - zeroSize], zeroSize );
	buffSize -= zeroSize;
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
