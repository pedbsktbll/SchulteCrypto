/*
* AES.h
*
*  Created on: Jan 15, 2012
*      Author: josh
*/

#pragma once
#include <Windows.h>

class AES
{
public:
	AES();
	virtual ~AES();
	bool encrypt(BYTE* buffer, DWORD buffSize, DWORD& commitSize, bool cbcMode = true);
	bool decrypt(BYTE* buffer, DWORD buffSize, bool cbcMode = true);
	bool setKey(BYTE* userKey, unsigned char userKeySize);
	bool getKey(BYTE* userKey, unsigned char& userKeySize);

	static void initializeTables();

protected:
	class CipherKey
	{
	public:
		CipherKey();
		CipherKey( BYTE* userKey, unsigned char userKeySize );
		virtual ~CipherKey();

		void generateRoundKeys();
		void getRoundKey( BYTE roundKey[16], unsigned char num );
//		long getSeed();
		const BYTE* getKey();
		unsigned char getKeySize();
		unsigned char getNumRounds();

	protected:
		BYTE* key;
		unsigned char keySize, keyCols, rounds;
		BYTE** roundKeys;

	private:
		void genRandKey( BYTE*, int );
		void subBytes( BYTE*, int );
	};

	class State
	{
	public:
		State( CipherKey* key );
		virtual ~State();
//		State& operator=(const State&);
		void nextBytes( BYTE* );
		BYTE* getBytes();
		void cipher();

		void _xor( State& );
		void _xor( BYTE* );

	protected:
		void addRoundKey( unsigned char num );
		void subBytes();
		void shiftRows();
		void mixCols();
		void invSubBytes();
		void invShiftRows();
		void invMixCols();
		const BYTE* getState();

//		BYTE state[16];
		BYTE* state;
		unsigned char numRounds;
		CipherKey* key;

	private:
		static const unsigned char stateSize = 16;
	};

	CipherKey* key;

private:
	void randBytes( BYTE*, int );

	const static unsigned char Rcon[256];
	const static unsigned char sbox[256];
	const static unsigned char invSbox[256];
	const static unsigned char smat[16];
	const static unsigned char mult2[256];
	const static unsigned char mult3[256];
	const static unsigned char mult9[256];
	const static unsigned char mult11[256];
	const static unsigned char mult13[256];
	const static unsigned char mult14[256];
};