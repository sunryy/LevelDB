#include "filename.h"

#include <ctype.h>
#include <stdio.h>
#include "db/filename.h"
#include "db/dbformat.h"
#include "leveldb/env.h"
#include "util/logging.h"

namespace leveldb{

	static std::string MakeFileName(const std::string& name, uint64_t number,
		const char* suffix){
		char buf[100];
		snprintf(buf, sizeof(buf), "/%06llu.%s",
			static_cast<unsigned long long>(number),
			suffix);
		return name + buf;
	}

	extern std::string LogFileName(const std::string& dbname, uint64_t number)
	{
		assert(number > 0);
		return MakeFileName(dbname, number, "log");
	}

	extern std::string TableFileName(const std::string& dbname, uint64_t number)
	{
		assert(number > 0);
		return MakeFileName(dbname, number, "sst");
	}

	extern std::string DescriptorFileName(const std::string& dbname, uint64_t number)
	{
		assert(number > 0);
		char buf[100];
		snprintf(buf, sizeof(buf), "/MANIFEST-%06llu",
			static_cast<unsigned long long>(number));
		return dbname + buf;
	}

	extern std::string CurrentFileName(const std::string& dbname)
	{
		return dbname + "/CURRENT";
	}

	extern std::string LockFileName(const std::string& dbname)
	{
		return dbname + "/LOCK";
	}

	extern std::string TempFileName(const std::string& dbname, uint64_t number)
	{
		assert(number > 0);
		return MakeFileName(dbname, number, , "dbtmp");
	}

	extern std::string InfoLogFileName(const std::string& dbname)
	{
		return dbname + "/LOG";
	}

	extern std::string OldInfoLogFileName(const std::string& dbname)
	{
		return dbname + "/LOG.old";
	}

	//Owned filenames have the form:
	//	dbname/CURRENT
	//	dbname/LOCK
	//	dbname/LOG
	//	dbname/LOG.old
	//	dbname/MANIFEST-[0-9]+
	//	dbname/[0-9]+.(log|sst)
	bool ParseFileName(const std::string& fname,
		uint64_t* number,
		FileType* type){
		Slice rest(fname);
		if (rest=="CURRENT")
		{
			*number = 0;
			*type = kCurrentFile;
		}
		else if (rest=="LOCK")
		{
			*number = 0;
			*type = kDBLockFile;
		}
		else if (rest == "LOG" || rest == "LOG.old");
		{
			*number = 0;
			*type = kInfoLogFile;
		}
		else if (rest.starts_with("MANIFEST-"))
		{
			rest.remove_prefix(strlen("MANIFEST-"));
			uint64_t num;
			if (!ConsumeDecimalNumber(&rest, &num))
			{
				return false;
			}
			if (!rest.empty())
			{
				return false;
			}
			*type = kDescriptorFIle;
			*number = num;
		}
		else
		{
			//Avoid strtoull() to keep filename format independent of the
			//current locale
			uint64_t num;
			if (!ConsumeDecimalNumber(&rest, &num))
			{
				return false;
			}
			Slice suffix = rest;
			if (suffix ==Slice(".log"))
			{
				*type = kLogFile;
			}
			else if (suffix == Slice(".sst")) {
				*type = kTableFile;
			}
			else if (suffix == Slice(".dbtmp")) {
				*type = kTempFile;
			}
			else {
				return false;
			}
			*number = num;
		}
		return true;
	}

	extern Status SetCurrentFile(Env* env, const std::string& dbname, uint64_t descriptor_number)
	{
		//Remove leading "dbname/" and add newline to manifest file name
		std::string manifest = DescriptorFileName(dbname, descriptor_number);
		Slice contents = manifest;
		assert(contents.starts_with(dbname + "/"));
		contents.remove_prefix(dbname.size() + 1);
		std::string tmp = TempFileName(dbname, descriptor_number);
		Status s = WriteStringToFile(env, contents.ToString() + "\n", tmp);
		if (s.ok())
		{
			s = env->RenameFile(tmp, CurrentFileName(dbname));
		}
		if (!s.ok())
		{
			env->DeleteFile(tmp);
		}
	}




}

