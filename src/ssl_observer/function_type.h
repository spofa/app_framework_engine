#ifndef SSL_OBSERVER_FUNCTION_TYPE_H_
#define SSL_OBSERVER_FUNCTION_TYPE_H_

#include "ssl_observer/main.h"
namespace ssl_observer{
	static std::vector<std::uint8_t> Read(const char* filename){
		std::vector<std::uint8_t> result;
		std::ifstream is(filename, std::ifstream::binary);
		if (is) {
			is.seekg(0, is.end);
			std::streamoff length = is.tellg();
			is.seekg(0, is.beg);
			result.resize(static_cast<uint32_t>(length));
			is.read(reinterpret_cast<char*>(&result[0]), length);
			is.close();
		}
		return result;
	}
	static std::wstring CurDirectory(){
		wchar_t full[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, full, MAX_PATH);
		std::wstring out_full(full);
		const size_t pos = out_full.find_last_of(L"\\");
		std::wstring out_dir(out_full.c_str(), pos + sizeof(char));
		return out_dir;
	}
}

#endif