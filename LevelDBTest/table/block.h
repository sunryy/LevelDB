#pragma once
#include <stddef.h>
#include <stdint.h>
#include "leveldb/iterator.h"

namespace leveldb{

	class Comparator;

	class Block{
	public:
		//Initialize the block with the specified contents.
		Block(const char* data, size_t size);
		~Block();

		size_t size() const { return size_; }
		iterator* NewIterator(const Comparator* comparator);

	private:
		uint32_t NumRestarts() const;
		const char* data_;
		size_t size_;
		uint32_t restart_offset_;	//Offset in data_ of restart array
		//No copying allowed
		Block(const Block&);
		void operator=(const Block&);

		class Iter;
	};
}