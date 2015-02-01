/*
* State.cpp
* A "State" object consistent with the AES terminology.
* Represents a 16-byte, 4-by-4 matrix of the internal
* state of a cipher undergoing AES.
* @author Josh
*
*
*  Created on: Jan 17, 2012
*      Author: josh
*/

#include "AES.h"
#include <string.h>

AES::State::State( CipherKey* key )
{
	this->key = key;
	numRounds = key->getNumRounds();
}

AES::State::~State()
{
	this->key = NULL;
	numRounds = 0;
}

void AES::State::nextBytes( BYTE* next )
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
	for( int i = 1; i < numRounds; i++ )
	{
		subBytes();
		shiftRows();
		mixCols();
		addRoundKey( i );
	}

	subBytes();
	shiftRows();
	addRoundKey( numRounds );
}

/**
* XORs state with Round Key of the specific round
* Accomplishes confusion
* @param key Key Object containing array of round keys
* @param num The round key number to XOR with
*/
void AES::State::addRoundKey( unsigned char num )
{
	BYTE roundKey[16];
	key->getRoundKey( roundKey, num );
	for( int i = 0; i < stateSize; i++ )
		state[i] ^= roundKey[i];
}

/**
* Substitutes all bytes with corresponding S-box bytes
* Accomplishes confusion
*/
void AES::State::subBytes()
{
	for( int i = 0; i < stateSize; i++ )
		state[i] = (BYTE) sbox[state[i]];
}

/**
* Shifts bytes in each row by 0,1,2, and 3 respectively
* Accomplished diffusion
*/
void AES::State::shiftRows()
{
	BYTE newData[16];
	for( unsigned char i = 0; i < stateSize; i++ )
		newData[i] = state[smat[i]];
	memcpy( state, newData, stateSize );
}

/**
* Most complicated step, multiplies state by matrix in GF.
* Accomplishes diffusion
*/
void AES::State::mixCols()
{
	BYTE newData[stateSize];
	for( int i = 0; i < 4; i++ )
	{
		int a0 = i * 4;
		int a1 = a0 + 1;
		int a2 = a1 + 1;
		int a3 = a2 + 1;

		newData[a0] = (BYTE) (mult2[state[a0]] ^ mult3[state[a1]] ^ state[a2] ^ state[a3]);

		BYTE t1 = state[a0];
		BYTE t2 = mult2[state[a1]];
		BYTE t3 = mult3[state[a2]];
		BYTE t4 = state[a3];
		BYTE t5 = t1 ^ t2 ^ t3 ^ t4;

		newData[a1] = (BYTE) (state[a0] ^ mult2[state[a1]] ^ mult3[state[a2]] ^ state[a3]);
		newData[a2] = (BYTE) (state[a0] ^ state[a1] ^ mult2[state[a2]] ^ mult3[state[a3]]);
		newData[a3] = (BYTE) (mult3[state[a0]] ^ state[a1] ^ state[a2] ^ mult2[state[a3]]);
	}
	memcpy( state, newData, sizeof( BYTE ) * stateSize );
}

/**
* Inverse step of subBytes
*/
void AES::State::invSubBytes()
{
	for( int i = 0; i < stateSize; i++ )
		state[i] = (BYTE) invSbox[state[i]];
}

/**
* Inverse step of shiftRows
*/
void AES::State::invShiftRows()
{
	BYTE newData[stateSize];
	for( int i = 0; i < 16; i++ )
		newData[smat[i]] = state[i];
	memcpy( state, newData, sizeof( BYTE ) * stateSize );
}

/**
* Inverse step of mixCols
*/
void AES::State::invMixCols()
{
	BYTE newData[stateSize];
	for( int i = 0; i < 4; i++ )
	{
		int a0 = i * 4;
		int a1 = a0 + 1;
		int a2 = a1 + 1;
		int a3 = a2 + 1;

		newData[a0] = (BYTE) (mult14[state[a0]] ^ mult11[state[a1]] ^ mult13[state[a2]] ^
			mult9[state[a3]]);
		newData[a1] = (BYTE) (mult9[state[a0]] ^ mult14[state[a1]] ^ mult11[state[a2]] ^
			mult13[state[a3]]);
		newData[a2] = (BYTE) (mult13[state[a0]] ^ mult9[state[a1]] ^ mult14[state[a2]] ^
			mult11[state[a3]]);
		newData[a3] = (BYTE) (mult11[state[a0]] ^ mult13[state[a1]] ^ mult9[state[a2]] ^
			mult14[state[a3]]);
	}
	memcpy( state, newData, sizeof( BYTE ) * stateSize );
}

/**
* XORs this state with another state
* @param s Other state to XOR with
*/
void AES::State::_xor( State& s )
{
	for( int i = 0; i < stateSize; i++ )
		state[i] ^= s.state[i];
}

const BYTE* AES::State::getState()
{
	return state;
}
