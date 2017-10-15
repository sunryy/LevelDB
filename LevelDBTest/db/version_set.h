#pragma once
#include <map>
#include <set>
#include <vector>

#include "db/dbformat.h"
#include "db/version_edit.h"
#include "port/port.h"

namespace leveldb{

	namespace log{ class Writer; }

	class Compaction;
	class Iterator;
	class MemTable;
	class TableBuilder;
	class TableCache;
	class Version;
	class VersionSet;
	class WritableFile;

	//Return th smallest index i such that files[i]->largest >= key.
	extern int FindFile(const InternalKeyComparator& icmp,
		const std::vector<FileMetaData*>& files,
		const Slice& key);

	//Returns true iff some file in "files" overlaps the user key range
	//[*smallest,*largest].
	extern bool SomeFileOverlapsRange(
		const InternalKeyComparator& icmp,
		bool disjiont_sorted_files,
		const std::vector<FileMetaData*>& files,
		const Slice* smallest_user_key,
		const Slice* largest_user_key);

	class Version{
	public:
		//Append to * iters a sequence of iterators that will
		//yield the contents of this Version when merged together.
		void AddIterators(const ReadOptions&, std::vector<Iterator*>* iters);

		//Lookup the value for key.
		struct GetStats{
			FileMetaData* seek_file;
			int seek_file_level;
		};
		Status Get(const ReadOptions&, const LookupKey& key, std::string* val,
			GetStats* stats);

		//Adds "stats" into the current state.
		bool UpdateStats(const GetStats& stats);

		//Reference count management (so Versions do not disappear out from
		//under live iterators)
		void Ref();
		void Unref();

		void GetOverLappingInputs(
			int level,
			const InternalKey* begin,	//NULL means before all keys
			const InternalKey* end,		//NULl means after all keys
			std::vector<FileMetaData*>* inputs);

		//Returns true iff some file in the specified level overlaps
		//some part of [*smallest_user_key, *largest_user_key].
		bool OverlapInLevel(int level,
			const Slice* smallest_user_key, const Slice* largest_user_key);

		//Returns the level at which we should place a new memtable compaction
		//result that covers the range [smallest_user_key, largest_user_key].
		int PickLevelForMemTableOutput(const Slice& smallest_user_key,
									   const Slice& largest_user_key);

		int NumFiles(int level) const { return files_[level].size(); }

		//Return a human readable string that describes this version's contents.
		std::string DebugString() const;

	private:
		friend class Compaction;
		friend class VersionSet;
		class LevelFileNumIterator;
		Iterator* NewConcatenatingIterator(const ReadOptions&, int level) const;

		VersionSet* vset_;	//VersionSet to which this Version belongs
		Version* next_;		//Next version in linked list
		Version* prev_;		//Previous version in linked list
		int refs_;			//Number of live refs to this version

		//List of flies per level
		std::vector<FileMetaData*> files_[config::kNumLevels];

		//Next file to compact based on seek stats.
		FileMetaData* file_to_compact_;
		int file_to_compact_level_;

		//Level that should be compacted next and its compaction score.
		double compaction_score_;
		int compaction_level_;

		explicit Version(VersionSet* vset)
			:vset_(vset), next_(this), prev_(this), refs_(0),
			file_to_compact_(NULL),
			file_to_compact_level_(-1),
			compaction_score_(-1),
			compaction_level_(-1)
		{

		}

		~Version();

		//No copying allowed
		Version(const Version&);
		void operator=(const Version&);

	};

	class VersionSet{
	public:
		VersionSet(const std::string& dbname,
			const Options* options,
			TableCache* table_cache,
			const InternalKeyComparator*);
		~VersionSet();

		//Apply *edit to the current version to form a new descriptor that
		//is both saved to persistent state and  installed as the new
		//current version.
		Status LogAndApply(VersionEdit* edit, port::Mutex* mu);

		//Recover the last saved descriptor from persistent storage.
		Status Recover();

		//Return the current version.
		Version* current() const { return current_; }

		//Return the current manifest file number
		uint64_t ManifestFileNumber() const { return manifest_file_number_; }

		//Allocate and return a new file number
		uint64_t NewFileNumber() { return next_file_number_; }

		//Return the number of Table files at the specified level.
		int NumLevelFiles(int level) const;

		//Return the combined file size of all files at the specified level.
		int64_t NumLevelBytes(int level) const;

		//Return the last sequence number.
		uint64_t LastSequence() const { return last_sequence_; }

		//Set the last sequence number to s.
		void SetLastSequence(uint64_t s){
			assert(s >= last_sequence_);
			last_sequence_ = s;
		}

		//Mark the specified file number as used.
		void MarkFileNumberUsed(uint64_t number);

		//Return the current log file number.
		uint64_t LogNumber() const { return log_number_; }

		//Return the log file number for the log file that is currently
		//being compacted, or zero if there is no such log file.
		uint64_t PrevLogNumber() const { return prev_log_number_; }

		//Pick level and inputs for a new compaction.
		Compaction* PickCompaction();

		//Return a compaction object for compacting the range [begin, end] in 
		//the specified level.
		Compaction* CompactRange(int level, const InternalKey* begin,
			const InternalKey* end);

		//Return the maximum overlapping data(in bytes) at next level for any
		//file at a level >= 1.
		int64_t MaxNextLevelOverlappingBytes();

		//Create an iterator that reads over the compaction inputs for "*c".
		Iterator* MakeInputIterator(Compaction* c);

		//Returns true iff some level needs a compaction.
		bool NeedsCompaction() const {
			Version* v = current_;
			return (v->compaction_score_ >= 1) || (v->file_to_compact_ != NULL);
		}

		//Add all files listed in any live version to *live.
		void AddLiveFiles(std::set<uint64_t>* live);

		//Return the approximate offset in the database of the data for 
		//"key" as of version "v".
		uint64_t ApproximateOffsetOf(Version* v, const InternalKey& key);

		//Return a human-readable short (single-line) summary of the number
		//of files per level.
		struct LevelSummaryStorage 
		{
			char buffer[100];
		};

		const char* LevelSummary(LevelSummaryStorage* scratch) const;


	private:
		class Builder;

		friend class Compaction;
		friend class Version;

		void Finalize(Version* v);

		void GetRange(const std::vector<FileMetaData*>& inputs,
			InternalKey* smallest,
			InternalKey* largest);

		void GetRange2(const std::vector<FileMetaData*>& inputs1,
			const std::vector<FileMetaData*>& inputs2,
			InternalKey* smallest,
			InternalKey* largest);

		void SetupOtherInputs(Compaction* c);

		//Save current contents to *log
		Status WriteSnapshot(log::Writer* log);
		void AppendVersion(Version* v);

		Env* const env_;
		const std::string dbname_;
		const Options* const options_;
		TableCache* const table_cache_;
		const InternalKeyComparator icmp_;
		uint64_t next_file_number_;
		uint64_t manifest_file_number_;
		uint64_t last_sequence_;
		uint64_t log_number_;
		uint64_t prev_log_number_;	//0 or backing store for memtable being compacted

		//Opened lazily
		WritableFile* descriptor_file_;
		log::Writer* descriptor_log_;
		Version dummy_versions_;	//Head of circular doubly-linked list of versions.
		Version* current_;	//== dummy_versions_.prev_

		//Per-level key at which the next compaction at that level should start.
		//Either an empty string, or a valid InternalKey.
		std::string compact_pointer_[config::kNumLevels];

		//No copying allowed
		VersionSet(const VersionSet&);
		void operator=(const VersionSet&);
	};

}