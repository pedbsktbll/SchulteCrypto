#include "RSAKey.h"

const char RSAKey::base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char RSAKey::base64charsDecode[] = {
	66, 66, 66, 66, 66, 66, 66, 66, 66, 64, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
	66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 62, 66, 66, 66, 63, 52, 53,
	54, 55, 56, 57, 58, 59, 60, 61, 66, 66, 00000000000, 65, 66, 66, 66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 66, 66, 66, 66, 66, 66, 26, 27, 28,
	29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 66, 66,
	66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
	66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
	66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
	66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
	66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
	66, 66, 66, 66, 66, 66
};

bool RSAKey::base64Encode( const BYTE* buff, DWORD buffSize, char*& encodedBuff, DWORD& encodedBuffSize )
{
	if( buff == NULL || buffSize == 0 || encodedBuff == NULL )
		return false;

	DWORD completeIntervals = buffSize / 3;
	DWORD extraBytes = buffSize % 3;
	encodedBuffSize = (completeIntervals + (extraBytes > 0 ? 1 : 0)) * 4;
	encodedBuff = new char[encodedBuffSize];

	// 3 chars into 4 base64
	b64Type type;
	DWORD i = 0, j = 0;
	for( DWORD k = 0; k < completeIntervals; k++ )
	{
		// HI! My name be Windows, and I'm a little-endian SHITHEAD!
		type.c3 = buff[i++];
		type.c2 = buff[i++];
		type.c1 = buff[i++];
 
		encodedBuff[j++] = base64chars[type.t4];
		encodedBuff[j++] = base64chars[type.t3];
		encodedBuff[j++] = base64chars[type.t2];
		encodedBuff[j++] = base64chars[type.t1];
	}

	// Time to pad I guess...
	switch( extraBytes )
	{
		case 0: break;
		case 1:
		{
			type.c3 = buff[i++];
			type.c2 = 0;
			type.c1 = 0;

			encodedBuff[j++] = base64chars[type.t4];
			encodedBuff[j++] = base64chars[type.t3];
			encodedBuff[j++] = '=';
			encodedBuff[j++] = '=';
		} break;
		case 2:
		{
			type.c3 = buff[i++];
			type.c2 = buff[i++];
			type.c1 = 0;

			encodedBuff[j++] = base64chars[type.t4];
			encodedBuff[j++] = base64chars[type.t3];
			encodedBuff[j++] = base64chars[type.t2];
			encodedBuff[j++] = '=';
		} break;
	}
	return true;
}

bool RSAKey::base64Decode( const char* buff, DWORD buffSize, BYTE*& decodedBuff, DWORD& decodedBuffSize )
{
	if( buff == NULL || buffSize < 4 || decodedBuff == NULL || buffSize % 4 != 0 )
		return false;

	decodedBuffSize = (buffSize / 4) * 3;
	decodedBuff = new BYTE[decodedBuffSize];

	// 4 base64 into 3 chars
	b64Type type;
	DWORD i = 0, j = 0;
	for( DWORD k = 0; k < buffSize / 4; k++ )
	{
		// HI! My name be Windows, and I'm a little-endian SHITHEAD!
		type.t4 = base64charsDecode[buff[i++]];
		type.t3 = base64charsDecode[buff[i++]];
		type.t2 = base64charsDecode[buff[i++]];
		type.t1 = base64charsDecode[buff[i++]];

		decodedBuff[j++] = type.c3;
		decodedBuff[j++] = type.c2;
		decodedBuff[j++] = type.c1;
	}

	if( buff[buffSize - 1] == '=' )
		decodedBuffSize--;
	if( buff[buffSize - 2] == '=' )
		decodedBuffSize--;
	return true;
}

bool RSAKey::eliminateHeader( const char*& buff, DWORD& buffSize )
{
	// Whoever is calling this function should have already validated the buffer and buffSize
	DWORD startOffset = 0;
	DWORD endOffset = buffSize - 1;

	// Let's eliminate this pesky PKCS #1 HEADER thing
	for( ; startOffset < buffSize && isspace( buff[startOffset] ); startOffset++ );
	if( startOffset == buffSize )
		return false;

	for( ; startOffset < buffSize && buff[startOffset] == '-'; startOffset++ ) ;
	if( startOffset == buffSize )
		return false;

	for( ; startOffset < buffSize && buff[startOffset] != '-'; startOffset++ ) ;
	if( startOffset == buffSize )
		return false;

	for( ; startOffset < buffSize && buff[startOffset] == '-'; startOffset++ ) ;
	if( startOffset == buffSize )
		return false;

	for( ; startOffset < buffSize && isspace(buff[startOffset]); startOffset++ ) ;
	if( startOffset == buffSize )
		return false;
	// Yay! no more HEADER!

	// Let's eliminate this pesky PKCS #1 FOOTER thing
	if( buff[endOffset] == '\0' )
		endOffset--;

	for( ; endOffset > startOffset && isspace( buff[endOffset] ); endOffset-- );
	if( endOffset == startOffset )
		return false;

	for( ; endOffset > startOffset && buff[endOffset] == '-'; endOffset-- ) ;
	if( endOffset == startOffset )
		return false;

	for( ; endOffset > startOffset && buff[endOffset] != '-'; endOffset-- ) ;
	if( endOffset == startOffset )
		return false;

	for( ; endOffset > startOffset && buff[endOffset] == '-'; endOffset-- ) ;
	if( endOffset == startOffset )
		return false;

	for( ; endOffset > startOffset && isspace( buff[endOffset]); endOffset-- ) ;
	if( endOffset == startOffset )
		return false;
	// Yay! no more FOOTER!

	buff += startOffset;
	buffSize = endOffset - startOffset + 1;
	return true;
}
