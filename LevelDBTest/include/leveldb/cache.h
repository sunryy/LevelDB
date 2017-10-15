#pragma once
#include <stdint.h>
#include "leveldb/slice.h"

namespace leveldb{

	class Cache;

	//Create a new cache with a fixed size capacity. This implementation
	//of Cache uses a least-recently-used eviction policy.
	extern Cache* NewLRUCache(size_t capacity);

	class Cache
	{
	public:
		Cache() {}
		//Destroys all existing entries by calling the "deleter"
		//function that was passed to the constructor.
		virtual ~Cache();

		//Opaque handle to an entry stored in the cache.
		struct Handle{ };

		//Insert a mapping from key->value into the cache and assign it the specified charge
		//against the total cache capacity.
		virtual Handle* Insert(const Slice& key, void* value, size_t charge,
			void(*deleter)(const Slice& key, void* value)) = 0;

		//If the cache has no mapping for "key", returns NULL.
		virtual Handle* Lookup(const Slice& key) = 0;

		//Release a mapping returned by a previous Lookup().
		virtual void Release(Handle* handle) = 0;

		//Returns the value encapsulated in a handle returned by a successful Lookup().
		virtual void* Value(Handle* handle) = 0;

		//If the cache contains entry for key, erase it.
		virtual void Erase(const Slice& key) = 0;

		//Return a new numeric id.May be used by multiple clients who are 
		//sharing the same cache to partition the key space.
		virtual uint64_t NewId() = 0;

	protected:
	private:
		void LRU_Remove(Handle* e);
		void LRU_Append(Handle* e);
		void Unref(Handle* e);

		struct Rep;
		Rep* rep_;

		//No copying allowed
		Cache(const Cache&);
		void operator=(const Cache&);
	};

}