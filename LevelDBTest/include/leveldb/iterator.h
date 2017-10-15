#pragma once
#include "leveldb/slice.h"
#include "leveldb/status.h"

namespace leveldb{

	class Iterator
	{
	public:
		Iterator();
		virtual ~Iterator();

		//An iterator is either positioned at a key/value pair, or not valid.
		virtual bool Valid() const = 0;
		
		//Position at the first key in the source.
		virtual void SeekToFirst() = 0;

		//Position at the last key in the source.
		virtual void SeekToLast() = 0;

		//Position at the first key in the source that at or past target 
		//The iterator is Valid() after this call iff the source contains an entry
		//that comes at or past target.
		virtual void Seek(const Slice& target) = 0;

		//Moves to the next entry in the soure.
		virtual void Next() = 0;

		//Moves to the previous entry in the source.
		virtual void Prev() = 0;

		//Return the key for the current entry.
		virtual Slice key() const = 0;

		//Return the value for the current entry.
		virtual Slice value() const = 0;

		//If an error has occurred, return it.
		virtual Status status() const = 0;

		//Clients are allowed to register function/arg1/arg2 triples that
		//will be invoked when this iterator is destroyed.
		typedef void(*CleanupFunction)(void* arg1, void* arg2);
		void RegisterCleanup(CleanupFunction function, void* arg1, void* arg2);

	protected:
	private:
		struct Cleanup{
			CleanupFunction function;
			void* arg1;
			void* arg2;
			Cleanup* next;
		};
		Cleanup cleanup_;

		//No copying allowed
		Iterator(const Iterator&);
		void operator=(const Iterator&);
	};
	//Return an empty iterator (yields nothing)
	extern Iterator* NewEmptyIterator();
	//Return an empty iterator with the specified status.
	extern Iterator* NeErrorIterator(const Status& status);
}