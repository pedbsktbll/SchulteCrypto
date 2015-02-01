// Driver.cpp : Defines the entry point for the console application.
//

#include "../AESLib/AES.h"

#pragma comment(lib, "AESLib.lib")

int wmain(int argc, wchar_t* argv[])
{
	BYTE input[32] = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
	BYTE cipherKey[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

	AES a;
	a.setKey( cipherKey, sizeof( cipherKey) );

	DWORD workingSize = sizeof( input );
	DWORD encryptedLen = 16;
	a.encrypt( input, encryptedLen, workingSize, false );
	a.decrypt( input, workingSize, false );

	return 0;
}

