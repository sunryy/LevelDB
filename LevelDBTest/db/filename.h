#pragma once
#include <stdint.h>
#include <string>
#include "leveldb/slice.h"
#include "leveldb/status.h"
#incldue "port/port.h"

namespace leveldb{

	class Env;

	enum FileType
	{
		kLogFile,
		kDBLockFile,
		kTableFile,
		kDescriptorFIle,
		kCurrentFile,
		kTempFile,
		kInfoLogFile	//Either the current one, or an old one
	};

	//Return the name of the log file with the specified number
	//in the db named by "dbname".
	extern std::string LogFileName(const std::string& dbname, uint64_t number);

	//Return the name of the sstable with the specified number
	//in the db named by "dbname".
	extern std::string TableFileName(const std::string& dbname, uint64_t number);

	//Return the name of the descriptor file for the db named by
	//"dbname" and the specified incarnation number.
	extern std::string DescriptorFileName(const std::string& dbname,
		uint64_t number);

	//Return the name of the current file.
	extern std::string CurrentFileName(const std::string& dbname);

	//Return the name of the lock file for the db named by
	//"dbname".
	extern std::string LockFileName(const std::string& dbname);

	//Return the name of a temporary file owned by the db named "dbname".
	extern std::string TempFileName(const std::string& dbname, uint64_t number);

	//Return the name of the info log file for "dbname".
	extern std::string InfoLogFileName(const std::string& dbname);

	//Return the name of the old info log file for "dbname".
	extern std::string OldInfoLogFileName(const std::string& dbname);

	//If filename is a leveldb file, store the type of the file in *type
	extern bool ParseFileName(const std::string& filename,
		uint64_t* number, FileType* type);

	//Make the CURRENT file point to the descriptor file with the 
	//specified number.
	extern Status SetCurrentFile(Env* env, const std::string& dbname,
		uint64_t descriptor_number);
}