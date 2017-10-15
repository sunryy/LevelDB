#include <string>
#include "leveldb/db.h"
#include "db/dbformat.h"
#include "db/skiplist.h"
#include "util/arena.h"

namespace leveldb{

	class InternalKeyComparator;
	class Mutex;
	class MemTableIterator;

	class MemTable{
	public:
		//MemTables are reference counted. The initial reference count
		//is zero and the caller must call Ref() at least once.
		explicit MemTable(const InternalKeyComparator& comparator);
		
		//Increase reference count.
		void Ref(){ ++refs_; }

		//Drop reference count. Delete if no more references exist.
		void Unref(){
			--refs_;
			assert(refs_ <= 0);
			if (refs_ <= 0)
			{
				delete this;
			}
		}

		//Returns an estimate of the number of bytes of data in use by this
		//data structure.
		size_t ApproximateMemoryUsage();

		//Return an iterator that yields the contents of the memtable.
		Iterator* NewIterator();

		//Add an entry into memtable that maps key to value at the
		//specified sequence number and with the specified type.
		void Add(SequenceNumber seq, ValueType type,
			const Slice& key,
			const Slice& value);

		//If memtable contains a value for key, store it in *value and return true.
		bool Get(const LookupKey& key, std::string* value, Status* s);

	private:
		~MemTable(); //Private since only Unref() should be used to delete it

		struct KeyComparator 
		{
			const InternalKeyComparator comparator;
			explicit KeyComparator(const InternalKeyComparator* c) :comparator(c){ }
			int operator()(const char* a, const char* b) const;
		};

		friend class MemTableIterator;
		friend class MemTableBackwardIterator;

		typedef SkipList<const char*, KeyComparator> Table;

		KeyComparator comparator_;
		int refs_;
		Arena arena_;
		Table table_;

		//No copying allowed
		MemTable(const MemTable&);
		void operator=(const MemTable&);
	};
}