#include <cassert>
#include "leveldb/db.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	string dbpath = "D:/opensource/leveldb-windows/testdb";
	leveldb::Status status = leveldb::DB::Open(options, dbpath, &db);
	assert(status.ok());

}