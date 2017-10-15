#pragma once
#include <set>
#include <utility>
#include <vector>
#include "db/dbformat.h"

namespace leveldb{

	class VersionSet;

	struct FileMetaData{
		int refs;
		int allowed_seeks;	//Seeks allowed util compaction
		uint64_t number;
		uint64_t file_size;	//File size in bytes
		InternalKey smallest;	//Smallest internal key served by table
		InternalKey largest;	//Largest internal key served by table

		FileMetaData() :refs(0), allowed_seeks(1 << 30), file_size(0){ }
	};

	class VersionEdit{
	public:
		VersionEdit() { clear(); }
		~VersionEdit() { }

		void clear();

		void SetComparatorName(const Slice& name){
			has_comparator_ = true;
			comparator_ = name.ToString();
		}

		void SetLogNumber(uint64_t num){
			has_log_number_ = true;
			log_number_ = num;
		}

		void SetPrevLogNumber(uint64_t num){
			has_prev_log_number_ = true;
			prev_log_number_ = num;
		}

		void setNextFile(uint64_t num){
			has_next_file_number_ = true;
			next_file_number_ = num;
		}

		void SetLastSequence(SequenceNumber seq){
			has_last_sequence_ = true;
			last_sequence_ = seq;
		}

		void SetComparatorPointer(int level, const InternalKey& key){
			compact_pointers_.push_back(std::make_pair(level, key));
		}

		//Add the specified file at the specified number.
		void AddFile(int level, uint64_t file,
			uint64_t file_size,
			const InternalKey& smallest,
			const InternalKey& largest){
			FileMetaData f;
			f.number = file;
			f.file_size = file_size;
			f.smallest = smallest;
			f.largest = largest;
			new_files_.push_back(std::make_pair(level, f));
		}


	private:
		friend class VersionSet;

		typedef std::set<std::pair<int, uint64_t>>DeletedFileSet;
		std::string comparator_;
		uint64_t log_number_;
		uint64_t prev_log_number_;
		uint64_t next_file_number_;
		SequenceNumber last_sequence_;
		bool has_comparator_;
		bool has_log_number_;
		bool has_prev_log_number_;
		bool has_next_file_number_;
		bool has_last_sequence_;

		std::vector<std::pair<int, InternalKey>> compact_pointers_;
		DeletedFileSet deleted_files_;
		std::vector<std::pair<int, FileMetaData>> new_files_;

	};
}