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
	this->keySize = rounds = 0;

	if( (userKeySize != 32 && userKeySize != 24 && userKeySize != 16) || userKey == NULL )
		return;

	key = new BYTE[this->keySize = userKeySize];
	memcpy( key, userKey, userKeySize );

	rounds = keySize / 4 + 6;
}
AES::CipherKey::CipherKey()
{
	key = new BYTE[this->keySize = 32];
	roundKeys = NULL;
	AES::randBytes( key, keySize );

	rounds = keySize / 4 + 6;
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
	keySize = rounds = 0;
}

/**
 * 5.2 Key Expansion
 * Generate Round Keys from key
 */
void AES::CipherKey::generateRoundKeys()
{
	unsigned char Nk = keySize / 4;
	// Initializing roundKeys
	roundKeys = new BYTE*[(rounds + 1) * 4];
	for( int i = 0; i < (rounds + 1) * 4; i++ )
		roundKeys[i] = new BYTE[4];

	// First Nk words of the expanded key are filled with the CipherKey
	for( int i = 0; i < Nk; i++ )
		for( int j = 0; j < 4; j++ )
			roundKeys[i][j] = key[4 * i + j];

	BYTE temp[4];
	for( int i = Nk; i < (rounds + 1) * 4; i++ )
	{
		for( int j = 0; j < 4; j++ )
			temp[j] = roundKeys[i-1][j];

		if( i % Nk == 0 )
		{
			rotWord( temp );
			subWord( temp );
			xorRcon( temp, i / Nk );
		}
		else if( Nk > 6 && i % Nk == 4 )
			subWord( temp );

		for( int j = 0; j < 4; j++ )
			roundKeys[i][j] = roundKeys[i - Nk][j] ^ temp[j];
	}
}

void AES::CipherKey::rotWord( BYTE* w )
{
	BYTE temp = w[0];
	w[0] = w[1];
	w[1] = w[2];
	w[2] = w[3];
	w[3] = temp;
}

/**
* Substitutes all bytes with corresponding S-box bytes
* @param bytes Byte array to substitute
* @return Corresponding substituted byte array
*/
void AES::CipherKey::subWord( BYTE* w )
{
	for( int i = 0; i < 4; i++ )
		w[i] = sbox[w[i]];
}

void AES::CipherKey::xorRcon( BYTE* w, unsigned char index )
{
	w[0] ^= Rcon[index];
}

/**
* Return 16-bit round key for the specified round
* @param num Specified round
* @return 16-bit array representing the num round key
*/
void AES::CipherKey::getRoundKey( BYTE roundKey[16], unsigned char num )
{
	for( int i = num * 4, k = 0; i < (num + 1) * 4; i++ )
		for( int j = 0; j < 4; j++ )
			roundKey[k++] = roundKeys[i][j];
}

/**
* Returns total number of rounds
* @return Number of rounds
*/
unsigned char AES::CipherKey::getNumRounds()
{
	return rounds;
}

const BYTE* AES::CipherKey::getKey()
{
	return key;
}

unsigned char AES::CipherKey::getKeySize()
{
	return keySize;
}
