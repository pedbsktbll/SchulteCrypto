#pragma once
#include <Windows.h>

class RSAKey
{
public:
	RSAKey() {}
	virtual ~RSAKey() = 0 {}

	bool base64Encode( BYTE* buff, DWORD buffSize, char*& encodedBuff, DWORD& encodedBuffSize );
	bool base64Decode( char* buff, DWORD buffSize, BYTE*& decodedBuff, DWORD& decodedBuffSize );

//private:
	static const char base64chars[];
	static const char base64charsDecode[];

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
#pragma pack(pop)

};

