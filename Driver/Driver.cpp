// Driver.cpp : Defines the entry point for the console application.
//

#include "../RSALib/RSAKey.h"
#include "../RSALib/RSAPrivateKey.h"
#include "../RSALib/RSAPublicKey.h"

#include <stdio.h>

#pragma comment(lib, "RSALib.lib")

#define MSG_SUC "Success!"
#define MSG_FAL "___FAILED___!"

bool testAES( BYTE* input, DWORD inputSize, BYTE* key, DWORD keySize, BYTE* expectedCipherText, bool cbcMode = false, BYTE* initVector = NULL );

int wmain( int argc, wchar_t* argv[] )
{
	BYTE chars[] = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
	char* encoded;
	DWORD encodedSize;

	RSAPublicKey k;
	k.base64Encode( chars, sizeof( chars ) - 1, encoded, encodedSize );

	BYTE* decoded;
	DWORD decodedSize;
	k.base64Decode( encoded, encodedSize, decoded, decodedSize );

	char privKey[] = "-----BEGIN RSA PRIVATE KEY-----\
MIICXAIBAAKBgQCqGKukO1De7zhZj6+H0qtjTkVxwTCpvKe4eCZ0FPqri0cb2JZfXJ/DgYSF6vUp\
wmJG8wVQZKjeGcjDOL5UlsuusFncCzWBQ7RKNUSesmQRMSGkVb1/3j+skZ6UtW+5u09lHNsj6tQ5\
1s1SPrCBkedbNf0Tp0GbMJDyR4e9T04ZZwIDAQABAoGAFijko56+qGyN8M0RVyaRAXz++xTqHBLh\
3tx4VgMtrQ+WEgCjhoTwo23KMBAuJGSYnRmoBZM3lMfTKevIkAidPExvYCdm5dYq3XToLkkLv5L2\
pIIVOFMDG+KESnAFV7l2c+cnzRMW0+b6f8mR1CJzZuxVLL6Q02fvLi55/mbSYxECQQDeAw6fiIQX\
GukBI4eMZZt4nscy2o12KyYner3VpoeE+Np2q+Z3pvAMd/aNzQ/W9WaI+NRfcxUJrmfPwIGm63il\
AkEAxCL5HQb2bQr4ByorcMWm/hEP2MZzROV73yF41hPsRC9m66KrheO9HPTJuo3/9s5p+sqGxOlF\
L0NDt4SkosjgGwJAFklyR1uZ/wPJjj611cdBcztlPdqoxssQGnh85BzCj/u3WqBpE2vjvyyvyI5k\
X6zk7S0ljKtt2jny2+00VsBerQJBAJGC1Mg5Oydo5NwD6BiROrPxGo2bpTbu/fhrT8ebHkTz2epl\
U9VQQSQzY1oZMVX8i1m5WUTLPz2yLJIBQVdXqhMCQBGoiuSoSjafUhV7i1cEGpb88h5NBYZzWXGZ\
37sJ5QsW+sJyoNde3xH8vdXhzU7eT82D6X/scw9RZz+/6rCJ4p0=\
-----END RSA PRIVATE KEY-----";

	RSAPrivateKey p;
	p.setKey( privKey, sizeof( privKey ) );
	return 0;
}