// Driver.cpp : Defines the entry point for the console application.
//

#include "DriverAES.h"
#include "../RSALib/RSAPublicKey.h"
#include "../RSALib/RSAPrivateKey.h"

#include <stdio.h>

#pragma comment(lib, "RSALib.lib")

#define MSG_SUC "Success!"
#define MSG_FAL "___FAILED___!"

int DriverRSA()
{
	RSAPublicKey r;
	r.modulus.initialize( "CA1" ); // 3233
	r.publicExponent.initialize( "11" ); // 17

	BigNum plainText( "41" ); //65
	BigNum cipherText = r.cipher( plainText );

	RSAPrivateKey p;
	p.modulus.initialize( "CA1" ); //3233
	p.privateExponent.initialize( "AC1" ); // 2753
	BigNum plaintTextDecrypted = p.decipher( cipherText );
	int a = 0;
	return  0;
}