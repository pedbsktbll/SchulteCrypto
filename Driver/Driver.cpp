// Driver.cpp : Defines the entry point for the console application.
//
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#else
//#define _CrtMemState
#pragma warning(disable : 4005)
#define _CrtMemCheckpoint(...)
#define _CrtMemDifference(...) false
#define _CrtMemDumpStatistics(...)
#define _CrtDumpMemoryLeaks()
#define _CrtSetDbgFlag(...)
#endif

#include "../RSALib/RSAKey.h"
#include "../RSALib/RSAPrivateKey.h"
#include "../RSALib/RSAPublicKey.h"
#include "../RSALib/BigNum.h"
#include "DriverRSA.h"

#include <stdio.h>

#pragma comment(lib, "RSALib.lib")

#define MSG_SUC "Success!"
#define MSG_FAL "___FAILED___!"

bool testAES( BYTE* input, DWORD inputSize, BYTE* key, DWORD keySize, BYTE* expectedCipherText, bool cbcMode = false, BYTE* initVector = NULL );

int wmain( int argc, wchar_t* argv[] )
{
 //	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//	Sleep( 1000 * 10 );
 	return DriverRSA();
// 	BigNum a( "a8e9" );
// 	BigNum b( "26d" );
// 	BigNum c = a + b;
// 	BigNum d = a * b;
// 	BigNum e = a - b;
// 
// 	BigNum f( "1000000000" );
// 	BigNum g( "1" );
// 	BigNum h = f - g;
// 
// 	BigNum a("AC1");
//  	BigNum b( "2" );
//  	BigNum i = a / b;
// 	BigNum j = a % b;
// 
// 	BigNum temp( "10" );
// 	BigNum k = a ^ temp;
// 
// 	BigNum l( "a" );
// 	BigNum m = l ^ l;
// 
// 	BigNum n( "534cebf2379da8eb9456a88441" );
// 	BigNum o( "ca1" );
// 	BigNum p = n / o;
// 	BigNum q = n % o;

// 	BigNum a1( "100" );
// 	BigNum a2( "100" );
// 	BigNum a3 = a1 * a2;

// 	BigNum z( "999999999999999999999999999999" );
// 	BigNum y( "999999999999999999999999999999" );
// 	BigNum x = y * z;
// 	return 0;

// 	BigNum a( "12345" );
// 	BigNum b( "6789" );
// 	BigNum c = a * b;

//	ULONGLONG test = 257 ^ 257;

//   	BigNum a( "AE6" );
//   	BigNum b( "AC1" );


// 	BigNum one( "1" );
//  	BigNum a( "101" );//"101" );//"AE6" );
//  	BigNum b( "100" );//"A" );//"AC1" );
//  	BigNum c = a.classicalPow( b );
//  	BigNum d = a ^ b;
//  	if( !(c == d) )
//  		printf( "Error" );
// 
// 	BigNum i( "1" );
// 	for( ; i < a; i = i + one )
// 	{
// 		c = a.classicalPow( i );
// 		d = a ^ i;
// 		if( !(c == d) )
// 			printf( "Error" );
// 	}
// 
// 	BigNum x( "5" );
// 	BigNum y = a ^ x;
// 	BigNum z = y * y;
// 	BigNum zz = y.classicalMultiply( y );

//	BigNum broke( "1050A0A0501" );
//	BigNum broke( "1060f140f0601" );
//	BigNum broke( "1060f1" );
//	BigNum broke( "106" );
// 	BigNum classical = broke.classicalMultiply( broke );
// 	BigNum mat = broke * broke;
// 	if( !(classical == mat) )
// 		printf("Error");

// 	DWORD d = c.numDigits + 1;
// 	char* arr = new char[d];
// 	c.toArray( arr, d );
// 	int fuck = 0;

// 	BigNum a( "500" );
// 	BigNum pow( "2" );
// 	BigNum mod( "23" );
// 	BigNum retVal = a.pow_modulo( pow, mod );

// 	BYTE chars[] = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
// 	char* encoded;
// 	DWORD encodedSize;
// 
// 	RSAPublicKey k;
// 	k.base64Encode( chars, sizeof( chars ) - 1, encoded, encodedSize );
// 
// 	BYTE* decoded;
// 	DWORD decodedSize;
// 	k.base64Decode( encoded, encodedSize, decoded, decodedSize );
// 
// 	char privKey[] = "-----BEGIN RSA PRIVATE KEY-----\
// MIICXAIBAAKBgQCqGKukO1De7zhZj6+H0qtjTkVxwTCpvKe4eCZ0FPqri0cb2JZfXJ/DgYSF6vUp\
// wmJG8wVQZKjeGcjDOL5UlsuusFncCzWBQ7RKNUSesmQRMSGkVb1/3j+skZ6UtW+5u09lHNsj6tQ5\
// 1s1SPrCBkedbNf0Tp0GbMJDyR4e9T04ZZwIDAQABAoGAFijko56+qGyN8M0RVyaRAXz++xTqHBLh\
// 3tx4VgMtrQ+WEgCjhoTwo23KMBAuJGSYnRmoBZM3lMfTKevIkAidPExvYCdm5dYq3XToLkkLv5L2\
// pIIVOFMDG+KESnAFV7l2c+cnzRMW0+b6f8mR1CJzZuxVLL6Q02fvLi55/mbSYxECQQDeAw6fiIQX\
// GukBI4eMZZt4nscy2o12KyYner3VpoeE+Np2q+Z3pvAMd/aNzQ/W9WaI+NRfcxUJrmfPwIGm63il\
// AkEAxCL5HQb2bQr4ByorcMWm/hEP2MZzROV73yF41hPsRC9m66KrheO9HPTJuo3/9s5p+sqGxOlF\
// L0NDt4SkosjgGwJAFklyR1uZ/wPJjj611cdBcztlPdqoxssQGnh85BzCj/u3WqBpE2vjvyyvyI5k\
// X6zk7S0ljKtt2jny2+00VsBerQJBAJGC1Mg5Oydo5NwD6BiROrPxGo2bpTbu/fhrT8ebHkTz2epl\
// U9VQQSQzY1oZMVX8i1m5WUTLPz2yLJIBQVdXqhMCQBGoiuSoSjafUhV7i1cEGpb88h5NBYZzWXGZ\
// 37sJ5QsW+sJyoNde3xH8vdXhzU7eT82D6X/scw9RZz+/6rCJ4p0=\
// -----END RSA PRIVATE KEY-----";
// 
// 	RSAPrivateKey p;
// 	p.setKey( privKey, sizeof( privKey ) );
	return 0;
}