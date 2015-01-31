#pragma once
#include <Windows.h>

class AES
{
public:
	AES();
	~AES();
	bool encrypt(BYTE* buffer, DWORD buffSize, DWORD& commitSize, bool cbcMode = true);
	bool decrypt(BYTE* buffer, DWORD buffSize, bool cbcMode = true);
	bool setKey(BYTE* userKey, DWORD userKeySize);
	bool getKey(BYTE* userKey, DWORD& userKeySize);

protected:
	BYTE* key;
	DWORD keySize;

	class State
	{
	public:
		State( unsigned char rounds );
		~State();
		State& operator=(const State&);
		void nextBytes( BYTE* );
		BYTE* getBytes();
		void cipher();

		void _xor( State& );
		void _xor( BYTE* );

	protected:
		void addRoundKey( int );
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
	};

private:
	void randBytes( BYTE*, int );

};

