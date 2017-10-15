#pragma once
#include <string>
#include <stdint.h>
#include "db/dbformat.h"
#include "leveldb/cache.h"
#include "leveldb/table.h"

namespace leveldb{

	class Env;

	class TableCache{
	public:
		TableCache(const std::string& dbname, const Options* options, int entries);
		~TableCache();

		//Return an iterator for the specified file number(the corresponding
		//file length must be exactly "file_size" bytes).
		Iterator* NewIterator(const ReadOptions& options,
			uint64_t file_number,
			uint64_t file_size,
			Table** tableptr = NULL);

		//Evict any entry for the specified fiel number
		void Evict(uint64_t file_number);

	private:
		Env* const env_;
		const std::string dbname_;
		const Options* options_;
		Cache* cache_;

	};
}