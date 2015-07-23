#ifndef DECRYPT_XUNYOU_DECRYPT_H_
#define DECRYPT_XUNYOU_DECRYPT_H_

#include <cstdint>
#include <vector>
#include <string>

namespace decrypt_xunyou{
	class Decrypt
	{
	public:
		explicit Decrypt(const char* filename);
		~Decrypt();
		bool Load();
		const std::string file_name() const;
		const std::vector<std::uint8_t> file_data() const;
	private:
		bool decode(const std::vector<std::uint8_t>& encode);
		Decrypt(const Decrypt&) = delete;
		Decrypt& operator=(const Decrypt&) = delete;
		std::string file_name_;
		std::vector<std::uint8_t> file_data_;
	};
}


#endif // !DECRYPT_XUNYOU_DECRYPT_H_
