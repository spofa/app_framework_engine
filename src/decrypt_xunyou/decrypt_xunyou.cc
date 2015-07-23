// decrypt_xunyou.cpp : Defines the entry point for the console application.
//

#include "decrypt_xunyou/stdafx.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "decrypt_xunyou/decrypt.h"

HCRYPTPROV phProv;


signed int __cdecl sub_10058930()
{
	BOOL v1; // esi@3

	if (phProv)
		return 1;
	v1 = CryptAcquireContextW(&phProv, L"MyContainer", L"Microsoft Base Cryptographic Provider v1.0", 1u, 8u);
	if (v1
		|| (v1 = CryptAcquireContextW(&phProv, 0, L"Microsoft Base Cryptographic Provider v1.0", 1u, 0)) != 0
		|| (v1 = CryptAcquireContextW(&phProv, 0, L"Microsoft Base Cryptographic Provider v1.0", 1u, 0x20u)) != 0
		|| (v1 = CryptAcquireContextW(&phProv, 0, L"Microsoft Base Cryptographic Provider v1.0", 1u, 0x28u)) != 0
		|| (v1 = CryptAcquireContextW(&phProv, L"MyContainer", L"Microsoft Base Cryptographic Provider v1.0", 1u, 0)) != 0)
	{
		return 1;
	}
	else
	{
		if (phProv)
		{
			CryptReleaseContext(phProv, 0);
			phProv = 0;
			return 0;
		}
	}
	return v1;
}
int __fastcall sub_10058B90(BYTE *pbData, const BYTE *a2, size_t a3, DWORD dwDataLen,const char* save_name)
{
	BYTE *v4; // edi@1
	const BYTE *v5; // esi@1
	BYTE *v6; // ebx@17
	int v7; // edi@17
	BOOL v8; // esi@19
	int v9; // esi@24
	int v10; // edi@24
	int v11; // eax@24
	char *v13; // [sp+14h] [bp-94h]@5
	HCRYPTHASH phHash; // [sp+18h] [bp-90h]@3
	DWORD pdwDataLen; // [sp+1Ch] [bp-8Ch]@19
	HCRYPTKEY phKey; // [sp+20h] [bp-88h]@3
	char *v17; // [sp+24h] [bp-84h]@25
	char v18; // [sp+28h] [bp-80h]@5
	char v19; // [sp+34h] [bp-74h]@27
	char v20[4]; // [sp+78h] [bp-30h]@24
	int v21; // [sp+7Ch] [bp-2Ch]@24
	int v22; // [sp+80h] [bp-28h]@24
	int v23; // [sp+84h] [bp-24h]@24
	int v24; // [sp+88h] [bp-20h]@24
	int v25; // [sp+8Ch] [bp-1Ch]@24
	int v26; // [sp+90h] [bp-18h]@24
	int v27; // [sp+94h] [bp-14h]@24
	int v28; // [sp+A4h] [bp-4h]@24

	v4 = pbData;
	v5 = a2;
	if (a3 > 0 && sub_10058930())
	{
		phKey = 0;
		phHash = 0;
		if (v5)
		{
			if (!CryptCreateHash(phProv, 0x8003u, 0, 0, &phHash))
			{
				throw std::exception::exception("CryptCreateHash error");
			}
			if (!CryptHashData(phHash, v5, dwDataLen, 0))
			{
				throw std::exception::exception("CryptHashData error");
			}
			if (!CryptDeriveKey(phProv, 0x6801u, phHash, 0, &phKey))
			{
				throw std::exception::exception("CryptDeriveKey error");
			}
			if (!CryptDestroyHash(phHash))
			{
				throw std::exception::exception("CryptDestroyHash error");
			}
			phHash = 0;
		}
		v6 = v4;
		v7 = 0;
		std::ofstream fs(save_name, std::ofstream::out | std::ofstream::binary);
		if (!fs.is_open()){
			return 1;
		}
		for (;;){
			if ((a3 - v7) <= 0x3E8){
				pdwDataLen = a3 - v7;
				v7 = a3;
				v8 = 1;
			}
			else{
				v7 += 0x3E8u;
				pdwDataLen = 0x3E8u;
				v8 = 0;
			}
			if (!CryptDecrypt(phKey, 0, v8, 0, v6, &pdwDataLen)){
				break;
			}
			if (v8){
				fs.write((char*)v6, pdwDataLen);
				fs.flush();
				break;
			}
			fs.write((char*)v6, pdwDataLen);
			fs.flush();
			v6 += pdwDataLen;
		}
		fs.close();
	}
}
static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[4096] = {
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};
::std::string base64_decode(const ::std::string &ascdata){
	std::string retval;
	int bits_collected = 0;
	unsigned int accumulator = 0;

	for (std::string::const_iterator i = ascdata.begin(); i != (ascdata.begin()+ascdata.length()); ++i) {
		const unsigned char c = *i;
		if (::std::isspace(c) || c == '=') {
			continue;
		}
		if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
			break;
		}
		accumulator = (accumulator << 6) | reverse_table[c];
		bits_collected += 6;
		if (bits_collected >= 8) {
			bits_collected -= 8;
			retval += (char)((accumulator >> bits_collected) & 0xffu);
		}
	}
	return retval;
}
bool savefile(const char *name,std::string v)
{

	return true;
}
int wmain(int argc, wchar_t* argv[]){
	decrypt_xunyou::Decrypt decrypt("C:\\Program Files (x86)\\xunyou\\gameconfig.txt");
	const std::vector<std::uint8_t> d = decrypt.file_data();
	std::string ssss = base64_decode((char*)d.data());
	sub_10058B90((BYTE*)ssss.data(), (const BYTE*)"lykjLdy173", ssss.length(), 10, "C:\\Program Files (x86)\\xunyou\\gameconfig_1.txt");
	return 0;
}

