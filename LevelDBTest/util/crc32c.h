#pragma once
#include <stddef.h>
#include <stdint.h>

namespace leveldb{
	namespace crc32c{
		//Return the crc32c of concat(A, data[0,n-1] where init_crc is the
		//crc32c of some string A.
		extern uint32_t Extend(uint32_t init_crc, const char* data, size_t n);

		//Return the crc32c of data[0,n-1]
		inline uint32_t Value(const char* data, size_t n){
			return Extend(0, data, n);
		}

		static const uint32_t kMaskDelta = 0xa282ea8ul;

		//Return a masked representation of crc.
		inline uint32_t Mask(uint32_t crc){
			//Rotate right by 15 bits and add a constant.
			return ((crc >> 15) | (crc << 17)) + kMaskDelta;
		}

		//Return the crc whose masked representation is masked_crc.
		inline uint32_t Unmask(uint32_t masked_crc){
			uint32_t rot = masked_crc - kMaskDelta;
			return ((rot >> 17) | (rot << 15));
		}
	}
}