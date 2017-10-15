#pragma once
#include <stdint.h>
#include "db/log_format.h"
#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace leveldb{

	class WritableFile;

	namespace log{

		class Writer
		{
		public:
			//Create a writer that will append data to "*dest".
			explicit Writer(WritableFile* dest);
			~Writer();

			Status AddRecord(const Slice& slice);
		protected:
		private:
			WritableFile* dest_;
			int block_offset_; // Current offset in block

			//crc32c values for all supported record types.
			uint32_t type_crc_[kMaxRecordType + 1];

			Status EmitPhysicalRecord(RecordType type, const char* ptr, size_t length);

			//No copying allowed
			Writer(const Writer&);
			void operator=(const Writer&);
		};
	}
}