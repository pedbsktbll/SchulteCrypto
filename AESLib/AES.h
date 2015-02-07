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
	bool encrypt(BYTE* buffer, DWORD& buffSize, DWORD& commitSize, bool cbcMode = false, BYTE* cbcIV = NULL);
	bool decrypt(BYTE* buffer, DWORD& buffSize, bool cbcMode = false);
	bool setKey(BYTE* userKey, unsigned char userKeySize);
	bool getKey(BYTE* userKey, unsigned char& userKeySize);

	static void initializeTables();
	static void randBytes( BYTE* buffer, DWORD size );

protected:
	class CipherKey
	{
	public:
		CipherKey();
		CipherKey( BYTE* userKey, unsigned char userKeySize );
		virtual ~CipherKey();

		void generateRoundKeys();
		void getRoundKey( BYTE roundKey[16], unsigned char num );
		const BYTE* getKey();
		unsigned char getKeySize();
		unsigned char getNumRounds();

	protected:
		void rotWord( BYTE* w );
		void subWord( BYTE* w );
		void xorRcon( BYTE* w, unsigned char index );

		BYTE* key;
		unsigned char keySize; // Nk = 4, 6, or 8 //// 16, 24, or 32 bytes
//		unsigned char keyCols; // Nb = 4 //// 32 bytes
		unsigned char rounds;  // Nr = 10, 12, or 14
		BYTE** roundKeys;
	};

	class State
	{
	public:
		State( CipherKey* key );
		virtual ~State();
		void nextBytes( BYTE* );
		BYTE* getBytes();
		void cipher();
		void decipher();

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

		BYTE* state;
		unsigned char numRounds;
		CipherKey* key;

	private:
		static const unsigned char stateSize = 16;
	};

	CipherKey* key;

private:
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