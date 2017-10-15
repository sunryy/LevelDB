#pragma once

#include <stdint.h>
#include "leveldb/options.h"
#include "leveldb/status.h"

namespace leveldb{

	class BlockBuilder;
	class BlockHandle;
	class WritableFile;

	class TableBuilder
	{
	public:
		//Create a builder that will store the contents of the table it is 
		//building in *file.
		TableBuilder(const Options& options, WritableFile* file);
		~TableBuilder();

		//Change the options used by this builder.
		Status ChangeOptions(const Options);

		//Add key, value to the table being constructed.
		void Add(const Slice& key, const Slice& value);

		//Advanced operation: flush any buffered key/value pairs to file.
		void Flush();

		//Return non-ok iff some error has been detected.
		Status status() const;

		//Finish building the table.
		Status Finish();

		//Indicate that the contents of this builder should be abandoned.
		void Abandon();

		//Number of calls to Add() so far.
		uint64_t NumEntries() const;

		//Size of the file generated so far.
		uint64_t FileSize() const;
	protected:
	private:
		bool ok() const { return status().ok(); }
		void WriteBlock(BlockBuilder* block, BlockHandle* handle);

		struct Rep;
		Rep* rep_;
		//No copying allowed
		TableBuilder(const TableBuilder&);
		void operator=(const TableBuilder&);
	};
}