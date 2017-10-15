#pragma once
#include <stdio.h>
#include "leveldb/env.h"

namespace leveldb{
	class WinLogger :public Logger{
	private:
		FILE* file_;
	public:
		explicit WinLogger(FILE* f) :file_(f){ assert(file_); }
		virtual ~WinLogger(){
			fclose(file_);
		}
		virtual void Logv(const char* format, va_list ap);
	};
}