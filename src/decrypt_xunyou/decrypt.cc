#include "decrypt_xunyou/decrypt.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

namespace decrypt_xunyou{
	Decrypt::Decrypt(const char* filename) :file_name_(filename){
		OpenSSL_add_all_algorithms();
		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		Load();
	}
	Decrypt::~Decrypt(){
		file_name_.resize(0);
		file_data_.resize(0);
	}
	bool Decrypt::Load(){
		std::vector<std::uint8_t> data;
		std::fstream fs(file_name(), std::ios::in | std::ios::binary);
		if (!fs.is_open()){
			return false;
		}
		fs.seekg(0, std::ios::end);
		std::streampos ps = fs.tellg();
		fs.seekg(0, std::ios::beg);
		data.resize(ps);
		fs.read((char *)&data[0], ps);
		fs.close();
		file_data_ = data;
		return true;
		//return decode(data);
	}
	const std::string Decrypt::file_name() const{
		return file_name_;
	}
	const std::vector<std::uint8_t> Decrypt::file_data() const{
		return file_data_;
	}
	bool Decrypt::decode(const std::vector<std::uint8_t>& encode){
		std::vector<std::uint8_t> based_64;
		std::vector<std::uint8_t> result;
		based_64.resize(encode.size());
		int len = EVP_DecodeBlock(based_64.data(), encode.data(), encode.size());
		if(len!=-1&&len!=0){
			file_data_.resize((len != 0 && len != -1) ? len + 1 : 0);
			memmove(file_data_.data(), based_64.data(), file_data_.size());
			return true;
		}
		else{
			file_data_.resize(0);
			return false;
		}
	}
}