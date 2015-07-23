#include "ssl_observer/certificate_map.h"
#include "ssl_observer/main.h"
#include "ssl_observer/certificate.h"

namespace ssl_observer{
	CertificateMap::CertificateMap(){
		OpenSSL_add_all_algorithms();
		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		extensions_.clear();
		files_.resize(0);
	}
	CertificateMap::CertificateMap(const char* file){
		extensions_.clear();
		files_.resize(0);
		if(file!=nullptr){
			files_.assign(file);
		}
	}
	CertificateMap::~CertificateMap(){
		extensions_.clear();
		files_.resize(0);
	}
	CertificateMap::ExtMap CertificateMap::GetCertificateExtensions(const char* file){
		if (file == nullptr&&!files_.size()){
			return extensions_;
		}
		files_ = files_.size() ? files_ : file;
		extensions_.clear();
		X509* cert = Load((char*)files_.c_str(), 0, nullptr);
		if (cert == nullptr){
			return extensions_;
		}
		X509_CINF* cert_inf = cert->cert_info;
		STACK_OF(X509_EXTENSION)* ext_list = cert_inf->extensions;
		if (!cert_inf || !ext_list || sk_X509_EXTENSION_num(ext_list) <= 0){
			return extensions_;
		}
		BIO *bio = BIO_new(BIO_s_mem());
		for (int i = 0; i < sk_X509_EXTENSION_num(ext_list); i++){
			char buffer[4096] = { 0 };
			X509_EXTENSION* ex = X509_get_ext(cert, i);
			ASN1_OBJECT* obj = X509_EXTENSION_get_object(ex);
			i2a_ASN1_OBJECT(bio, obj);
			i2t_ASN1_OBJECT(buffer, 4095, obj);
			BIO_reset(bio);
			const std::string object_name = buffer;
			M_ASN1_OCTET_STRING_print(bio, ex->value);
			buffer[BIO_read(bio, buffer, 4095)] = '\0';
			const std::string text = buffer;
			extensions_.insert(std::make_pair(object_name, text));
		}
		BIO_free(bio);
		X509_free(cert);
		return extensions_;
	}
}