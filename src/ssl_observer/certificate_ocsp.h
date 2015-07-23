#ifndef SSL_OBSERVER_CERTIFICATE_OCSP_H_
#define SSL_OBSERVER_CERTIFICATE_OCSP_H_

#include "ssl_observer/main.h"
#include <openssl/asn1_mac.h>
#include "ssl_observer/certificate.h"

namespace ssl_observer{
	int i2d_OCSP_CERTIDa(OCSP_CERTID *a, unsigned char **pp){
		M_ASN1_I2D_vars(a);
		M_ASN1_I2D_len(a->hashAlgorithm, i2d_X509_ALGOR);
		M_ASN1_I2D_len(a->issuerNameHash, i2d_ASN1_OCTET_STRING);
		M_ASN1_I2D_len(a->issuerKeyHash, i2d_ASN1_OCTET_STRING);
		M_ASN1_I2D_len(a->serialNumber, i2d_ASN1_INTEGER);
		M_ASN1_I2D_seq_total();
		M_ASN1_I2D_put(a->hashAlgorithm, i2d_X509_ALGOR);
		M_ASN1_I2D_put(a->issuerNameHash, i2d_ASN1_OCTET_STRING);
		M_ASN1_I2D_put(a->issuerKeyHash, i2d_ASN1_OCTET_STRING);
		M_ASN1_I2D_put(a->serialNumber, i2d_ASN1_INTEGER);
		M_ASN1_I2D_finish();
	}

	std::vector<std::uint8_t> OCSPVerify(const char* file){
		std::vector<std::uint8_t> based_64;
		std::vector<std::uint8_t> result;
		X509* cert = Load((char*)file, 0, nullptr);
		OCSP_CERTID* cert_id = OCSP_cert_id_new(const_cast<EVP_MD*>(EVP_sha1()),
			X509_get_subject_name(cert),
			X509_get0_pubkey_bitstr(cert),
			X509_get_serialNumber(cert));
		unsigned char* cert_ocsp = nullptr;
		int len = i2d_OCSP_CERTID(cert_id, &cert_ocsp);
		result.resize(len * 2);
		based_64.resize(4096);
		memmove(result.data(), cert_ocsp, len);
		unsigned char sssss[] = {
			 0x30 ,0x4e ,0x30 ,0x4c ,0xa0 ,0x03 ,0x02 ,0x01
			,0x00 ,0x30 ,0x45 ,0x30 ,0x43 ,0x30 ,0x41 ,0x30
			,0x09 ,0x06 ,0x05 ,0x2b ,0x0e ,0x03 ,0x02 ,0x1a
			,0x05 ,0x00 ,0x04 ,0x14 ,0x00 ,0xeb ,0x0c ,0xcc
			,0xf4 ,0x71 ,0x6c ,0xba ,0x46 ,0x23 ,0xa3 ,0xd5
			,0x2f ,0x98 ,0xd4 ,0x3d ,0xf6 ,0x30 ,0xa9 ,0x1d
			,0x04 ,0x14 ,0x88 ,0x27 ,0x17 ,0x09 ,0xa9 ,0xb6
			,0x18 ,0x60 ,0x8b ,0xec ,0xeb ,0xba ,0xf6 ,0x47
			,0x59 ,0xc5 ,0x52 ,0x54 ,0xa3 ,0xb7 ,0x02 ,0x08
			,0x14 ,0x41 ,0x48 ,0xe6 ,0x56 ,0xeb ,0xaa ,0xb8 };
		int based64_len = EVP_EncodeBlock(based_64.data(), sssss, sizeof(sssss));
		std::string ssss = reinterpret_cast<char*>(based_64.data());
		return based_64;
	}
}

#endif