#pragma once
#include <Windows.h>
#include "BigNum.h"

class RSAKey
{
public:
	RSAKey() {}
	virtual ~RSAKey() = 0 {}

	virtual bool setKey( const char* buff, DWORD buffSize ) = 0;

	bool base64Encode( const BYTE* buff, DWORD buffSize, char*& encodedBuff, DWORD& encodedBuffSize );
	bool base64Decode( const char* buff, DWORD buffSize, BYTE*& decodedBuff, DWORD& decodedBuffSize );

protected:
#pragma pack(push, 1)
	union b64Type
	{
		DWORD triplet : 24;
		struct
		{
			char c1, c2, c3;
		};
		struct
		{
			DWORD t1 : 6;
			DWORD t2 : 6;
			DWORD t3 : 6;
			DWORD t4 : 6;
		};
	};

	struct DER_TLV
	{
		union
		{
			BYTE tag;
			struct
			{
				BYTE tagNum : 5;
				BYTE form : 1;
				BYTE asn1Class : 2;
			};
		} tag;
		DWORD length;
		BYTE* value;
	};

	union BigEndianDWORD
	{
		DWORD num;
		char c1, c2, c3, c4;
		char arr[4];
	};

	struct pkcs15_padding_encryptionBlock // Minimum size is 1 (pad) + 1 (blockType) + 8 (padString) + 1 (pad) = 11 bytes
	{
		BYTE pad = 0;
		BYTE blockType; // set to 02 for public-key operations and set to 00 or 01 for private key operations
		BYTE paddingString[0]; // MUST be at least 8 bytes long
//		BYTE pad2 = 0;
//		BYTE data[0];
	};
#pragma pack(pop)

	bool eliminateHeader( const char*& buff, DWORD& buffSize );
	bool readDER_TLV( BYTE*& dataPtr, DER_TLV& der );
//	void setDER_Values( DER_TLV& der, BigNum& param );

//private:
	static const char base64chars[];
	static const char base64charsDecode[];

	static const BYTE SEQUENCE = 0x30;
	static const BYTE INTEGER =  0x02;

};

