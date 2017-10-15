#pragma once

#include <stdint.h>
#include <stdio.h>
#include "leveldb/iterator.h"
#include "leveldb/options.h"

namespace leveldb{

	static const int kMajorVersion = 1;
	static const int kMinorVersion = 2;

	struct Options;
	struct ReadOptions;
	struct WriteOptions;
	class WriteBatch;

	//Abstract handle to particular state of a DB.
	//A Snapshot is an immutable object and can therefore be safely
	//accessed from multiple threads without any external synchronization.

	class Snapshot{
	protected:
		virtual ~Snapshot();
	};

	//A range of keys
	struct Range{
		Slice start;	//Included in the range
		Slice limit;	//Not included in the range

		Range(){}
		Range(const Slice& s, const Slice& l) :start(s), limit(1){}
	};

	//A DB is a persistent ordered map from keys to values.
	//A DB is safe for concurrent access from multiple threads without
	//any external synchronization
	class DB
	{
	public:
		//Open the database with the specified "name".
		//Stores a pointer to a heap-allocated database in *dbptr and returns
		//OK on success.
		//Stores NULL in *dbptr and returns a non-OK status on error.
		//Caller should delete *dbptr when it is no longer needed.
		static Status Open(const Options& options,
			const std::string& name,
			DB** dbptr);

		DB(){ }
		virtual ~DB();

		//Set the database entry for "key" to "value". Returns OK on success,
		//and a non-OK status on error.
		//Note: consider setting options.sync=true.
		virtual Status Put(const WriteOptions& options,
			const Slice& key, const Slice& value) = 0;

		//Remove the database entry (if any) for "key". Returns OK on success,
		//and a non-OK status on error. It is not an error if "key" did not exist in the
		//database. 
		//Note:consider setting options.sync = true.
		virtual Status Delete(const WriteOptions& options, const Slice& key) = 0;
		
		//Apply the specified updates to the database.
		//Returns OK on success, non-OK on failure.
		//Note: consider setting options.sync=true.
		virtual Status Write(const WriteOptions& options, WriteBatch* updates) = 0;

		//If the database contains an entry for "key" store the corresponding value in *value
		//and return OK.
		virtual Status Get(const ReadOptions& options,
			const Slice& key, std::string* value) = 0;

		//Return a heap-allocated iterator over the over the contents of the database.
		virtual Iterator* NewIterator(const ReadOptions& options) = 0;

		//Return a handle to the current DB state.
		virtual const Snapshot* GetSnapshot() = 0;

		//Release a previously acquired snapshot.
		virtual void ReleaseSnapshot(const Snapshot* snapshot) = 0;

		//DB implementations can export properties about their state via this method.
		virtual bool GetProperty(const Slice& property, std::string* value) = 0;

		//For each i in [0,n-1], store in "sizes[i]", the approximate file system space
		//used by keys in "[range[i].start.. range[i].limit)".
		virtual void GetApproximateSizes(const Range* range, int n, uint64_t* sizes) = 0;

		//Compact the underlying storage for the key range [*begin, *end].
		virtual void CompactRange(const Slice* begin, const Slice* end) = 0;

	protected:
	private:
		//No copying allowed
		DB(const DB&);
		void operator=(const DB&);
	};

	//Destroy the contents of the specified database.
	//Be very careful using this method.
	Status DestroyDB(const std::string& name, const Options& options);

	//If a DB cannot be opened, you may attemp to call this method to resurrect as much of the
	//contents of the database as possible.
	Status RepairDB(const std::string& dbname, const Options& options);
}