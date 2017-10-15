#pragma once
#include <set>
#include "db/dbformat.h"
#include "db/log_writer.h"

#include "db/snapshot.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "port/port.h"

namespace leveldb{

	class MemTable;
	class TableCache;
	class Version;


}
