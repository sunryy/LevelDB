#pragma once
#include <string>
namespace leveldb{

	class Slice;

	//A Comparator object provides a total order aross slices that are
	//used as keys in an sstable or a database. 
	class Comparator
	{
	public:
		virtual ~Comparator();

		//Three-way comparison. Returns value:
		//	< 0 iff "a" < "b"
		//	== 0 iff "a" == "b"
		// > 0 iff "a" > "b"
		virtual int Compare(const Slice& a, const Slice& b) const = 0;

		//The name of the comparator. Used to check for comparator
		//mismatches (i.e., a DB created with one comparator is 
		//accessed using  a different comparator.
		virtual const char* Name() const = 0;

		//Advanced functions:these are used to reduce the space requirements for 
		//internal data structures like index blocks.
		virtual void FindShortestSeparator(
			std::string* start, const Slice& limit) const = 0;

		//changes *key to a short string >= *key.
		virtual void FindShortSuccessor(std::string* key) const = 0;
	protected:
	private:
	};

	//Return a builtin comparator that uses lexicographic byte-wise ordering.
	extern const Comparator* BytewiseComparator();
}