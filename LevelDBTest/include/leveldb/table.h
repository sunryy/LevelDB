#pragma once
#include <stdint.h>
#include "leveldb/iterator.h"

namespace leveldb{

	class Block;
	class BlockHandle;
	struct Options;
	class RandomAccessFile;
	struct ReadOptions;

	//A Table is a stored map from strings to strings.Tables are 
	//immutable and persistent.A Table may be safely accessed from
	//multiple threads without external synchronization.
	class Table{
	public:
		//Attempt to open the table that is stored in bytes [0..file_size)
		//of "file", and read the metadata entries necessary to allow
		//retrieving data from the table.

		static Status Open(const Options& options,
			RandomAccessFile* file,
			uint64_t file_size,
			Table** table);

		~Table();

		//Returns a new iterator voer the table contents.
		Iterator* NewIterator(const ReadOptions&) const;

		//Given a key,return an approximate byte offset in the file where
		//the data for that key begins(or would begin if the key were
		//present in the file).The returned value is in terms of file
		//bytes, and so includes effects like compression of the underlying data.
		uint64_t ApproximateOffsetOf(const Slice& key) const;

	private:
		struct Rep;
		Rep* rep_;

		explicit Table(Rep* rep){ rep_ = rep; }
		static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);

		//No copying allowed
		Table(const Table&);
		void operator=(const Table&);

	};
}