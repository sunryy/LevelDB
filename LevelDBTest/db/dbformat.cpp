#include "dbformat.h"

#include <stdio.h>
#include "port/port.h"
#include "util/coding.h"

namespace leveldb{

	static uint64_t PackSequenceAndType(uint64_t seq, ValueType t){
		assert(seq <= kMaxSequenceNumber);
		assert(t <= kValueTypeForSeek);
		return (seq << 8) | t;
	}

	extern void AppendInternalKey(std::string* result, const ParsedInternalKey& key)
	{
		result->append(key.user_key.data(), key.user_key.size());
		PutFixed64(result, PackSequenceAndType(key.sequence, key.type));
	}


	std::string ParsedInternalKey::DebugString() const
	{
		char buf[50];
		snprintf(buf, sizeof(buf), "' @%llu : %d",
				(unsigned long long)sequence,
				int(type));
		std::string result = "'";
		result += user_key.ToString();
		result += buf;
		return result;
	}


	std::string InternalKey::DebugString() const
	{
		std::string result;
		ParsedInternalKey parsed;
		if (ParseInternalKey(rep_, &parsed))
		{
			result = parsed.DebugString();
		}
		else
		{
			result = "(bad)";
			result.append(EscapeString(rep_));
		}
		return result;
	}



	const char* InternalKeyComparator::Name() const
	{
		return "leveldb.InternalKeyComparator";
	}

	int InternalKeyComparator::Compare(const Slice& akey, const Slice& bkey) const
	{
		int r = user_comparator_->Compare(ExtractUserKey(akey), ExtractUserKey(bkey));
		if (r == 0)
		{
			const uint64_t anum = DecodeFixed64(akey.data() + akey.size() - 8);
			const uint64_t bnum = DecodeFixed64(bkey.data() + bkey.size() - 8);
			if (anum > bnum)
			{
				r = -1;
			}
			else if (anum < bnum)
			{
				r = +1;
			}
		}
		return r;
	}

	void InternalKeyComparator::FindShortestSeparator(std::string* start, const Slice& limit) const
	{
		//Attempt to shorten the uesr portion of the key
		Slice user_start = ExtractUserKey(*start);
		Slice user_limit = ExtractUserKey(limit);
		std::string tmp(user_start.data(), user_start.size());
		user_comparator_->FindShortestSeparator(&tmp, user_limit);
		if (user_comparator_->Compare(*start, tmp)<0)
		{
			//User key has become larger. Tack on the earliest possible
			//number to the shortened user key.
			PutFixed64(&tmp, PackSequenceAndType(kMaxSequenceNumber, kValueTypeForSeek));
			assert(this->Compare(*start, tmp) < 0);
			assert(this->Compare(tmp, limit) < 0);
			start->swap(tmp);
		}
	}

	void InternalKeyComparator::FindShortSuccessor(std::string* key) const {
		Slice user_key = ExtractUserKey(*key);
		std::string tmp(user_key.data(), user_key.size());
		user_comparator_->FindShortSuccessor(&tmp);
		if (user_comparator_->Compare(user_key, tmp) < 0) {
			// User key has become larger.  Tack on the earliest possible
			// number to the shortened user key.
			PutFixed64(&tmp, PackSequenceAndType(kMaxSequenceNumber, kValueTypeForSeek));
			assert(this->Compare(*key, tmp) < 0);
			key->swap(tmp);
		}
	}

	LookupKey::LookupKey(const Slice& user_key, SequenceNumber s) {
		size_t usize = user_key.size();
		size_t needed = usize + 13;  // A conservative estimate
		char* dst;
		if (needed <= sizeof(space_)) {
			dst = space_;
		}
		else {
			dst = new char[needed];
		}
		start_ = dst;
		dst = EncodeVarint32(dst, usize + 8);
		kstart_ = dst;
		memcpy(dst, user_key.data(), usize);
		dst += usize;
		EncodeFixed64(dst, PackSequenceAndType(s, kValueTypeForSeek));
		dst += 8;
		end_ = dst;
	}




}

