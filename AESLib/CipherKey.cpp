/*
* CipherKey.cpp
* A "CipherKey" object consistent with AES terminology.
* Represents a 128/192/256-bit key and the subsequent
* 10/12/14 roundkeys, respectively.
*
*  Created on: Jan 16, 2012
*      Author: Josh
*/

#include "AES.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AES::CipherKey::CipherKey( BYTE* userKey, unsigned char userKeySize )
{
	key = NULL;
	roundKeys = NULL;
	this->keySize = keyCols = rounds = 0;

	if( (userKeySize != 32 && userKeySize != 24 && userKeySize != 16) || userKey == NULL )
		return;

	key = new BYTE[this->keySize = userKeySize];
	memcpy( key, userKey, userKeySize );

	keyCols = userKeySize / 4;
	rounds = keyCols + 6;
}
AES::CipherKey::CipherKey()
{
	key = new BYTE[this->keySize = 32];
	roundKeys = NULL;
//	genRandKey( key, keySize );
	AES::randBytes( key, keySize );

	keyCols = keySize / 4;
	rounds = keyCols + 4 + 2;
}

AES::CipherKey::~CipherKey()
{
	if( key != NULL )
		delete[] key;
	key = NULL;
	if( roundKeys != NULL )
	{
		for( int i = 0; i < 4; i++ )
			delete[] roundKeys[i];
		delete[] roundKeys;
	}
	roundKeys = NULL;
	keySize = keyCols = rounds = 0;
}

/**
 * 5.2 Key Expansion
 * Generate Round Keys from key
 */
void AES::CipherKey::generateRoundKeys()
{
	//	roundKeys = new BYTE*[4];
	//	for(int i = 0; i < 4; i++)
	//		roundKeys[i] = new BYTE[(rounds + 1) * 4];

	roundKeys = new BYTE*[(rounds + 1) * 4];
	for( int i = 0; i < (rounds + 1) * 4; i++ )
		roundKeys[i] = new BYTE[4];

	for( int i = 0; i < keyCols; i++ )
		for( int j = 0; j < 4; j++ )
			roundKeys[i][j] = key[4 * i + j];

	BYTE col[4];
	for( int i = 0; i < 4; i++ )
		col[i] = key[4 * keyCols + (i - 4)];

	for( int i = keyCols; i < (rounds + 1) * 4; )
	{
		// In-line RotWord():
		BYTE temp = col[0];
		col[0] = col[1];
		col[1] = col[2];
		col[2] = col[3];
		col[3] = temp;

		// SubWord():
		subWord( col, 4 );

		for( int j = 0; j < 4; j++ )
			roundKeys[i][j] = col[j] = (roundKeys[i - keyCols][j] ^ col[j] ^ (j == 0 ? Rcon[i / keyCols] : 0));
		i++;
		for( int j = 0; j < 3; j++, i++ )
			for( int k = 0; k < 4; k++ )
				roundKeys[i][k] = col[k] = (roundKeys[i - keyCols][k] ^ col[k]);

		//ONLY for 256-bit keys:
		if( keySize == 32 && i < 60 )
		{
			subWord( col, 4 );
			for( int j = 0; j < 4; j++, i++ )
				for( int k = 0; k < 4; k++ )
					roundKeys[i][k] = col[k] = (BYTE) (roundKeys[i - keyCols][k] ^ col[k]);
		}
	}
}

/**
* Substitutes all bytes with corresponding S-box bytes
* @param bytes Byte array to substitute
* @return Corresponding substituted byte array
*/
void AES::CipherKey::subWord( BYTE* bytes, int length )
{
	for( int i = 0; i < length; i++ )
		bytes[i] = sbox[bytes[i]];
}

/**
* Return 16-bit round key for the specified round
* @param num Specified round
* @return 16-bit array representing the num round key
*/
void AES::CipherKey::getRoundKey( BYTE roundKey[16], unsigned char num )
{
	//	BYTE* roundKey = new BYTE[16];
	for( int i = num * 4, k = 0; i < (num + 1) * 4; i++ )
		for( int j = 0; j < 4; j++ )
			roundKey[k++] = roundKeys[i][j];
	//	return roundKey;
}

// /**
// * Returns long representative of the key
// * @return 64-bit long representative of the key
// */
// long AES::CipherKey::getSeed()
// {
// 	long retVal = 0;
// 	for( int i = 0; i < 8; i++ )
// 		retVal = (retVal << 8) + (key[i]);
// 	return retVal;
// }

/**
* Returns total number of rounds
* @return Number of rounds
*/
unsigned char AES::CipherKey::getNumRounds()
{
	return rounds;
}

// //Size is in bytes
// void AES::CipherKey::genRandKey( BYTE* data, int size )
// {
// 	srand( (unsigned int) time( NULL ) );
// 	for( int i = 0; i < size; i++ )
// 		data[i] = rand() % 256;
// }

const BYTE* AES::CipherKey::getKey()
{
	return key;
}

unsigned char AES::CipherKey::getKeySize()
{
	return keySize;
}
