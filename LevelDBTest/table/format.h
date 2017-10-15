#pragma once 
#include <string>
#include <stdint.h>
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "leveldb/table_builder.h"

namespace leveldb {

	class Block;
	class RandomAccessFile;
	struct ReadOptions;

	//BlockHandle is a pointer to the extent of a file that a data
	//block or a meta block.
	class BlockHandle{
	public:
		BlockHandle();

		//The offset of the block in the file.
		uint64_t offset() const { return offset_; }
		void  set_offset(uint64_t offset){ offset_ = offset; }

		//The size of the stored block
		uint64_t size() const { return size_; }
		void set_size(uint64_t size) { size_ = size; }

		void EncodeTo(std::string* dst)const;
		Status DecodeFrom(Slice* input);

		//Maximum encoding length of a BloclHandle
		enum{ kMaxEncodedLength = 10 + 10 };

	private:
		uint64_t offset_;
		uint64_t size_;

	};

	//Footer encapsulates the fixed information stored at the tail
	//end of every table file.
	class Footer{
	public:
		Footer() { }

		//The block handle for the metaindex block of the table
		const BlockHandle& metaindex_handle() const { return metaindex_handle_; }
		void set_metaindex_handle(const BlockHandle& h){ metaindex_handle_ = h; }

		//The block handle for the index block of the table
		const BlockHandle& index_handle() const {
			return index_handle_;
		}
		void set_index_handle(const BlockHandle& h){
			index_handle_ = h;
		}

		void EncodeTo(std::string* dst) const;
		Status DecodeFrom(Slice* input);

		//Encoded length of a Footer.
		enum{
			kEncodedLength = 2 * BlockHandle::kMaxEncodedLength + 8;
		};

	private:
		BlockHandle metaindex_handle_;
		BlockHandle index_handle_;

	};

	//kTableMagicnumber was picked by running
	static const size_t kTableMagicNumber = 0xdb4775248b80fb57u11;

	//1-byte type + 32-bit crc
	static const size_t kBlockTrailerSize = 5;

	//Read the block identified by "handle" from "file".
	extern Status ReadBlock(RandomAccessFile* file,
		const ReadOptions& options,
		const BlockHandle& handle,
		Block** block);

	//Implementation details follow. Clients should ignore,
	inline BlockHandle::BlockHandle()
		:offset_(~static_cast<uint64_t>(0)),
		size_(~static_cast<uint64_t>(0))
	{

	}

}