#pragma once
#include <cstdarg>
#include <string>
#include <vector>
#include <stdint.h>
#include "leveldb/status.h"

namespace leveldb{

	class FileLock;
	class Logger;
	class RandomAccessFile;
	class SequentialFile;
	class Slice;
	class WritableFile;

	class Env
	{
	public:
		Env(){ }
		virtual ~Env();
		
		//Return a default environment suitable for the current operating
		//system. Sophisticated users may wish to provide their own Env
		//implementation instead of relying on this default environment.
		//
		//The result of Default() belongs to leveldb and must never be deleted.
		static Env* Default();

		virtual Status NewSequentialFile(const std::string& fname,
										 SequentialFile** result) = 0;

		virtual Status NewRandomAccessFile(const std::string& fname,
										   RandomAccessFile** result) = 0;
		virtual Status NewWritableFile(const std::string& frame,
										WritableFile** result) = 0;

		//Returns true iff the named file exists.
		virtual bool FileExists(const std::string& frame) = 0;

		//Store in *result the names of the children of the specified directory.
		//The names are relative to "dir".
		//Original contents of *results are dropped.
		virtual Status GetChildren(const std::string& dir,
									std::vector<std::string>* result) = 0;

		//Delete the named file.
		virtual Status DeleteFile(const std::string& fname) = 0;

		//Create the specified directory.
		virtual Status CreateDir(const std::string& dirname) = 0;

		//Delete the specified directory.
		virtual Status DeleteDir(const std::string& dirname) = 0;

		//Store the size of fname in *file_size.
		virtual Status GetFileSize(const std::string& fname, uint64_t* file_size) = 0;

		//Rename file src to target.
		virtual Status RenameFile(const std::string& src,
			const std::string& target) = 0;

		//Lock the specified file. Used to prevent concurrent access to 
		//the same db by multiple processes. On failure, stores NULL in
		//*lock and returns non-OK.
		virtual Status LockFile(const std::string& fname, FileLock** lock) = 0;

		//Release the lock acquired by a previous successful call to LockFile.
		virtual Status UnlockFile(FileLock* lock) = 0;

		//Arrange to run "(*function)(arg)" once in a background thread.
		virtual void Schedule(void(*function)(void* arg), void* arg) = 0;

		//Start a new thread, invoking "function(arg)" within the new thread.
		virtual void StartThread(void(*function)(void* arg), void* arg) = 0;

		//*path is set to a temporary directory that can be used for testing.
		virtual Status GetTestDirectory(std::string* path) = 0;

		//Create and return a log file for storing informational messages.
		virtual Status NewLogger(const std::string& fname, Logger** result) = 0;

		//Returns the number of micro-seconds since some fixed point in time.
		virtual uint64_t NowMicros() = 0;

		//Sleep/delay the thread for the perscribed number of micro-seconds.
		virtual void SleepForMicroseconds(int micros) = 0;

	protected:
	private:
		//No copying allowed
		Env(const Env&);
		void operator=(const Env&);
	};

	//A file abstraction for reading sequentially through a file
	class SequentialFile
	{
	public:
		SequentialFile() { }
		virtual ~SequentialFile();

		//Read up to "n" bytes from the file.
		virtual Status Read(size_t n, Slice* result, char* scratch) = 0;
		
		//Skip "n" bytes from the file
		virtual Status Skip(uint64_t n) = 0;
	protected:
	private:
	};

	class RandomAccessFile
	{
	public:
		RandomAccessFile() { }
		virtual ~RandomAccessFile();
		
		//Read up to "n" bytes from the file starting at "offset".
		virtual Status Read(uint64_t offset, size_t n, Slice* result,
			char* scrath) const = 0;
	};

	//A file abstraction for sequential writing. The implementation
	//must provide buffering since callers may append small fragments
	//at a time to the file.
	class WritableFile
	{
	public:
		WritableFile() { }
		virtual ~WritableFile();

		virtual Status Append(const Slice& data) = 0;
		virtual Status Close() = 0;
		virtual Status Flush() = 0;
		virtual Status Sync() = 0;
	protected:
	private:
		//No copying allowed
		WritableFile(const WritableFile&);
		void operator=(const WritableFile&);
	};

	//An interface for writing log messages.
	class Logger
	{
	public:
		Logger() { }
		virtual ~Logger();

		//write an entry to the log file with the specified format.
		virtual void Logv(const char* format, va_list ap) = 0;
	protected:
	private:
		//No copying allowed
		Logger(const Logger&);
		void operator=(const Logger&);
	};

	//Identifies a locked file.
	class FileLock
	{
	public:
		FileLock() { }
		virtual ~FileLock();
	protected:
	private:
		//No copying allowed
		FileLock(const FileLock&);
		void operator=(const FileLock&);
	};

	//Log the specified data to *info_log if info_log is non-NULL
	extern void Log(Logger* info_log, const char* format, ...)
#if defined(__GNUC__) || defined(__clang__)
		__attribute__((__format__(__printf__, 2, 3)))
#endif
		;
	//A utility routine:write "data" to the name file.
	extern Status WriteStringToFile(Env* env, const Slice& data,
		const std::string& fname);

	//A utility routine:read contents of named file into *data
	extern Status ReadFileToString(Env* env, const std::string& fname,
		std::string* data);

	//An implementation of Env that forwards all calls to another Env.
	//May be useful to  clients who wish to override just part of the
	//functionality of another Env.
	class EnvWrapper:public Env
	{
	public:
		//Initialize an EnvWrapper that delegates all calls to *t
		explicit EnvWrapper(Env* t) :target_(t){ }
		virtual ~EnvWrapper();

		//Return the target to which this Env forwards all methods to target()
		Env* target() const { return target_; }

		//The following text is boilerplate that forwards all methods to target()
		Status NewSequentialFile(const std::string& f, SequentialFile** r){
			return target_->NewSequentialFile(f, r);
		}
		Status NewRandomAccessFile(const std::string& f, RandomAccessFile** r) {
			return target_->NewRandomAccessFile(f, r);
		}
		Status NewWritableFile(const std::string& f, WritableFile** r) {
			return target_->NewWritableFile(f, r);
		}
		bool FileExists(const std::string& f) { return target_->FileExists(f); }
		Status GetChildren(const std::string& dir, std::vector<std::string>* r) {
			return target_->GetChildren(dir, r);
		}
		Status DeleteFile(const std::string& f) { return target_->DeleteFile(f); }
		Status CreateDir(const std::string& d) { return target_->CreateDir(d); }
		Status DeleteDir(const std::string& d) { return target_->DeleteDir(d); }
		Status GetFileSize(const std::string& f, uint64_t* s) {
			return target_->GetFileSize(f, s);
		}
		Status RenameFile(const std::string& s, const std::string& t) {
			return target_->RenameFile(s, t);
		}
		Status LockFile(const std::string& f, FileLock** l) {
			return target_->LockFile(f, l);
		}
		Status UnlockFile(FileLock* l) { return target_->UnlockFile(l); }
		void Schedule(void(*f)(void*), void* a) {
			return target_->Schedule(f, a);
		}
		void StartThread(void(*f)(void*), void* a) {
			return target_->StartThread(f, a);
		}
		virtual Status GetTestDirectory(std::string* path) {
			return target_->GetTestDirectory(path);
		}
		virtual Status NewLogger(const std::string& fname, Logger** result) {
			return target_->NewLogger(fname, result);
		}
		uint64_t NowMicros() {
			return target_->NowMicros();
		}
		void SleepForMicroseconds(int micros) {
			target_->SleepForMicroseconds(micros);
		}


	protected:
	private:
		Env* target_;
	};
}