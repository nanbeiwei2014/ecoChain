/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Hbase_TYPES_H
#define Hbase_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace apache { namespace hadoop { namespace hbase { namespace thrift {

typedef std::string Text;

typedef std::string Bytes;

typedef int32_t ScannerID;

class TCell;

class ColumnDescriptor;

class TRegionInfo;

class Mutation;

class BatchMutation;

class TIncrement;

class TColumn;

class TRowResult;

class TScan;

class TAppend;

class IOError;

class IllegalArgument;

class AlreadyExists;

typedef struct _TCell__isset {
  _TCell__isset() : value(false), timestamp(false) {}
  bool value :1;
  bool timestamp :1;
} _TCell__isset;

class TCell : public virtual ::apache::thrift::TBase {
 public:

  TCell(const TCell&);
  TCell& operator=(const TCell&);
  TCell() : value(), timestamp(0) {
  }

  virtual ~TCell() throw();
  Bytes value;
  int64_t timestamp;

  _TCell__isset __isset;

  void __set_value(const Bytes& val);

  void __set_timestamp(const int64_t val);

  bool operator == (const TCell & rhs) const
  {
    if (!(value == rhs.value))
      return false;
    if (!(timestamp == rhs.timestamp))
      return false;
    return true;
  }
  bool operator != (const TCell &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TCell & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TCell &a, TCell &b);

inline std::ostream& operator<<(std::ostream& out, const TCell& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _ColumnDescriptor__isset {
  _ColumnDescriptor__isset() : name(false), maxVersions(true), compression(true), inMemory(true), bloomFilterType(true), bloomFilterVectorSize(true), bloomFilterNbHashes(true), blockCacheEnabled(true), timeToLive(true) {}
  bool name :1;
  bool maxVersions :1;
  bool compression :1;
  bool inMemory :1;
  bool bloomFilterType :1;
  bool bloomFilterVectorSize :1;
  bool bloomFilterNbHashes :1;
  bool blockCacheEnabled :1;
  bool timeToLive :1;
} _ColumnDescriptor__isset;

class ColumnDescriptor : public virtual ::apache::thrift::TBase {
 public:

  ColumnDescriptor(const ColumnDescriptor&);
  ColumnDescriptor& operator=(const ColumnDescriptor&);
  ColumnDescriptor() : name(), maxVersions(3), compression("NONE"), inMemory(false), bloomFilterType("NONE"), bloomFilterVectorSize(0), bloomFilterNbHashes(0), blockCacheEnabled(false), timeToLive(2147483647) {
  }

  virtual ~ColumnDescriptor() throw();
  Text name;
  int32_t maxVersions;
  std::string compression;
  bool inMemory;
  std::string bloomFilterType;
  int32_t bloomFilterVectorSize;
  int32_t bloomFilterNbHashes;
  bool blockCacheEnabled;
  int32_t timeToLive;

  _ColumnDescriptor__isset __isset;

  void __set_name(const Text& val);

  void __set_maxVersions(const int32_t val);

  void __set_compression(const std::string& val);

  void __set_inMemory(const bool val);

  void __set_bloomFilterType(const std::string& val);

  void __set_bloomFilterVectorSize(const int32_t val);

  void __set_bloomFilterNbHashes(const int32_t val);

  void __set_blockCacheEnabled(const bool val);

  void __set_timeToLive(const int32_t val);

  bool operator == (const ColumnDescriptor & rhs) const
  {
    if (!(name == rhs.name))
      return false;
    if (!(maxVersions == rhs.maxVersions))
      return false;
    if (!(compression == rhs.compression))
      return false;
    if (!(inMemory == rhs.inMemory))
      return false;
    if (!(bloomFilterType == rhs.bloomFilterType))
      return false;
    if (!(bloomFilterVectorSize == rhs.bloomFilterVectorSize))
      return false;
    if (!(bloomFilterNbHashes == rhs.bloomFilterNbHashes))
      return false;
    if (!(blockCacheEnabled == rhs.blockCacheEnabled))
      return false;
    if (!(timeToLive == rhs.timeToLive))
      return false;
    return true;
  }
  bool operator != (const ColumnDescriptor &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ColumnDescriptor & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(ColumnDescriptor &a, ColumnDescriptor &b);

inline std::ostream& operator<<(std::ostream& out, const ColumnDescriptor& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _TRegionInfo__isset {
  _TRegionInfo__isset() : startKey(false), endKey(false), id(false), name(false), version(false), serverName(false), port(false) {}
  bool startKey :1;
  bool endKey :1;
  bool id :1;
  bool name :1;
  bool version :1;
  bool serverName :1;
  bool port :1;
} _TRegionInfo__isset;

class TRegionInfo : public virtual ::apache::thrift::TBase {
 public:

  TRegionInfo(const TRegionInfo&);
  TRegionInfo& operator=(const TRegionInfo&);
  TRegionInfo() : startKey(), endKey(), id(0), name(), version(0), serverName(), port(0) {
  }

  virtual ~TRegionInfo() throw();
  Text startKey;
  Text endKey;
  int64_t id;
  Text name;
  int8_t version;
  Text serverName;
  int32_t port;

  _TRegionInfo__isset __isset;

  void __set_startKey(const Text& val);

  void __set_endKey(const Text& val);

  void __set_id(const int64_t val);

  void __set_name(const Text& val);

  void __set_version(const int8_t val);

  void __set_serverName(const Text& val);

  void __set_port(const int32_t val);

  bool operator == (const TRegionInfo & rhs) const
  {
    if (!(startKey == rhs.startKey))
      return false;
    if (!(endKey == rhs.endKey))
      return false;
    if (!(id == rhs.id))
      return false;
    if (!(name == rhs.name))
      return false;
    if (!(version == rhs.version))
      return false;
    if (!(serverName == rhs.serverName))
      return false;
    if (!(port == rhs.port))
      return false;
    return true;
  }
  bool operator != (const TRegionInfo &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TRegionInfo & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TRegionInfo &a, TRegionInfo &b);

inline std::ostream& operator<<(std::ostream& out, const TRegionInfo& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _Mutation__isset {
  _Mutation__isset() : isDelete(true), column(false), value(false), writeToWAL(true) {}
  bool isDelete :1;
  bool column :1;
  bool value :1;
  bool writeToWAL :1;
} _Mutation__isset;

class Mutation : public virtual ::apache::thrift::TBase {
 public:

  Mutation(const Mutation&);
  Mutation& operator=(const Mutation&);
  Mutation() : isDelete(false), column(), value(), writeToWAL(true) {
  }

  virtual ~Mutation() throw();
  bool isDelete;
  Text column;
  Text value;
  bool writeToWAL;

  _Mutation__isset __isset;

  void __set_isDelete(const bool val);

  void __set_column(const Text& val);

  void __set_value(const Text& val);

  void __set_writeToWAL(const bool val);

  bool operator == (const Mutation & rhs) const
  {
    if (!(isDelete == rhs.isDelete))
      return false;
    if (!(column == rhs.column))
      return false;
    if (!(value == rhs.value))
      return false;
    if (!(writeToWAL == rhs.writeToWAL))
      return false;
    return true;
  }
  bool operator != (const Mutation &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Mutation & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(Mutation &a, Mutation &b);

inline std::ostream& operator<<(std::ostream& out, const Mutation& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _BatchMutation__isset {
  _BatchMutation__isset() : row(false), mutations(false) {}
  bool row :1;
  bool mutations :1;
} _BatchMutation__isset;

class BatchMutation : public virtual ::apache::thrift::TBase {
 public:

  BatchMutation(const BatchMutation&);
  BatchMutation& operator=(const BatchMutation&);
  BatchMutation() : row() {
  }

  virtual ~BatchMutation() throw();
  Text row;
  std::vector<Mutation>  mutations;

  _BatchMutation__isset __isset;

  void __set_row(const Text& val);

  void __set_mutations(const std::vector<Mutation> & val);

  bool operator == (const BatchMutation & rhs) const
  {
    if (!(row == rhs.row))
      return false;
    if (!(mutations == rhs.mutations))
      return false;
    return true;
  }
  bool operator != (const BatchMutation &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const BatchMutation & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(BatchMutation &a, BatchMutation &b);

inline std::ostream& operator<<(std::ostream& out, const BatchMutation& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _TIncrement__isset {
  _TIncrement__isset() : table(false), row(false), column(false), ammount(false) {}
  bool table :1;
  bool row :1;
  bool column :1;
  bool ammount :1;
} _TIncrement__isset;

class TIncrement : public virtual ::apache::thrift::TBase {
 public:

  TIncrement(const TIncrement&);
  TIncrement& operator=(const TIncrement&);
  TIncrement() : table(), row(), column(), ammount(0) {
  }

  virtual ~TIncrement() throw();
  Text table;
  Text row;
  Text column;
  int64_t ammount;

  _TIncrement__isset __isset;

  void __set_table(const Text& val);

  void __set_row(const Text& val);

  void __set_column(const Text& val);

  void __set_ammount(const int64_t val);

  bool operator == (const TIncrement & rhs) const
  {
    if (!(table == rhs.table))
      return false;
    if (!(row == rhs.row))
      return false;
    if (!(column == rhs.column))
      return false;
    if (!(ammount == rhs.ammount))
      return false;
    return true;
  }
  bool operator != (const TIncrement &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TIncrement & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TIncrement &a, TIncrement &b);

inline std::ostream& operator<<(std::ostream& out, const TIncrement& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _TColumn__isset {
  _TColumn__isset() : columnName(false), cell(false) {}
  bool columnName :1;
  bool cell :1;
} _TColumn__isset;

class TColumn : public virtual ::apache::thrift::TBase {
 public:

  TColumn(const TColumn&);
  TColumn& operator=(const TColumn&);
  TColumn() : columnName() {
  }

  virtual ~TColumn() throw();
  Text columnName;
  TCell cell;

  _TColumn__isset __isset;

  void __set_columnName(const Text& val);

  void __set_cell(const TCell& val);

  bool operator == (const TColumn & rhs) const
  {
    if (!(columnName == rhs.columnName))
      return false;
    if (!(cell == rhs.cell))
      return false;
    return true;
  }
  bool operator != (const TColumn &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TColumn & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TColumn &a, TColumn &b);

inline std::ostream& operator<<(std::ostream& out, const TColumn& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _TRowResult__isset {
  _TRowResult__isset() : row(false), columns(false), sortedColumns(false) {}
  bool row :1;
  bool columns :1;
  bool sortedColumns :1;
} _TRowResult__isset;

class TRowResult : public virtual ::apache::thrift::TBase {
 public:

  TRowResult(const TRowResult&);
  TRowResult& operator=(const TRowResult&);
  TRowResult() : row() {
  }

  virtual ~TRowResult() throw();
  Text row;
  std::map<Text, TCell>  columns;
  std::vector<TColumn>  sortedColumns;

  _TRowResult__isset __isset;

  void __set_row(const Text& val);

  void __set_columns(const std::map<Text, TCell> & val);

  void __set_sortedColumns(const std::vector<TColumn> & val);

  bool operator == (const TRowResult & rhs) const
  {
    if (!(row == rhs.row))
      return false;
    if (__isset.columns != rhs.__isset.columns)
      return false;
    else if (__isset.columns && !(columns == rhs.columns))
      return false;
    if (__isset.sortedColumns != rhs.__isset.sortedColumns)
      return false;
    else if (__isset.sortedColumns && !(sortedColumns == rhs.sortedColumns))
      return false;
    return true;
  }
  bool operator != (const TRowResult &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TRowResult & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TRowResult &a, TRowResult &b);

inline std::ostream& operator<<(std::ostream& out, const TRowResult& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _TScan__isset {
  _TScan__isset() : startRow(false), stopRow(false), timestamp(false), columns(false), caching(false), filterString(false), batchSize(false), sortColumns(false), reversed(false) {}
  bool startRow :1;
  bool stopRow :1;
  bool timestamp :1;
  bool columns :1;
  bool caching :1;
  bool filterString :1;
  bool batchSize :1;
  bool sortColumns :1;
  bool reversed :1;
} _TScan__isset;

class TScan : public virtual ::apache::thrift::TBase {
 public:

  TScan(const TScan&);
  TScan& operator=(const TScan&);
  TScan() : startRow(), stopRow(), timestamp(0), caching(0), filterString(), batchSize(0), sortColumns(0), reversed(0) {
  }

  virtual ~TScan() throw();
  Text startRow;
  Text stopRow;
  int64_t timestamp;
  std::vector<Text>  columns;
  int32_t caching;
  Text filterString;
  int32_t batchSize;
  bool sortColumns;
  bool reversed;

  _TScan__isset __isset;

  void __set_startRow(const Text& val);

  void __set_stopRow(const Text& val);

  void __set_timestamp(const int64_t val);

  void __set_columns(const std::vector<Text> & val);

  void __set_caching(const int32_t val);

  void __set_filterString(const Text& val);

  void __set_batchSize(const int32_t val);

  void __set_sortColumns(const bool val);

  void __set_reversed(const bool val);

  bool operator == (const TScan & rhs) const
  {
    if (__isset.startRow != rhs.__isset.startRow)
      return false;
    else if (__isset.startRow && !(startRow == rhs.startRow))
      return false;
    if (__isset.stopRow != rhs.__isset.stopRow)
      return false;
    else if (__isset.stopRow && !(stopRow == rhs.stopRow))
      return false;
    if (__isset.timestamp != rhs.__isset.timestamp)
      return false;
    else if (__isset.timestamp && !(timestamp == rhs.timestamp))
      return false;
    if (__isset.columns != rhs.__isset.columns)
      return false;
    else if (__isset.columns && !(columns == rhs.columns))
      return false;
    if (__isset.caching != rhs.__isset.caching)
      return false;
    else if (__isset.caching && !(caching == rhs.caching))
      return false;
    if (__isset.filterString != rhs.__isset.filterString)
      return false;
    else if (__isset.filterString && !(filterString == rhs.filterString))
      return false;
    if (__isset.batchSize != rhs.__isset.batchSize)
      return false;
    else if (__isset.batchSize && !(batchSize == rhs.batchSize))
      return false;
    if (__isset.sortColumns != rhs.__isset.sortColumns)
      return false;
    else if (__isset.sortColumns && !(sortColumns == rhs.sortColumns))
      return false;
    if (__isset.reversed != rhs.__isset.reversed)
      return false;
    else if (__isset.reversed && !(reversed == rhs.reversed))
      return false;
    return true;
  }
  bool operator != (const TScan &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TScan & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TScan &a, TScan &b);

inline std::ostream& operator<<(std::ostream& out, const TScan& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _TAppend__isset {
  _TAppend__isset() : table(false), row(false), columns(false), values(false) {}
  bool table :1;
  bool row :1;
  bool columns :1;
  bool values :1;
} _TAppend__isset;

class TAppend : public virtual ::apache::thrift::TBase {
 public:

  TAppend(const TAppend&);
  TAppend& operator=(const TAppend&);
  TAppend() : table(), row() {
  }

  virtual ~TAppend() throw();
  Text table;
  Text row;
  std::vector<Text>  columns;
  std::vector<Text>  values;

  _TAppend__isset __isset;

  void __set_table(const Text& val);

  void __set_row(const Text& val);

  void __set_columns(const std::vector<Text> & val);

  void __set_values(const std::vector<Text> & val);

  bool operator == (const TAppend & rhs) const
  {
    if (!(table == rhs.table))
      return false;
    if (!(row == rhs.row))
      return false;
    if (!(columns == rhs.columns))
      return false;
    if (!(values == rhs.values))
      return false;
    return true;
  }
  bool operator != (const TAppend &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TAppend & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(TAppend &a, TAppend &b);

inline std::ostream& operator<<(std::ostream& out, const TAppend& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _IOError__isset {
  _IOError__isset() : message(false) {}
  bool message :1;
} _IOError__isset;

class IOError : public ::apache::thrift::TException {
 public:

  IOError(const IOError&);
  IOError& operator=(const IOError&);
  IOError() : message() {
  }

  virtual ~IOError() throw();
  std::string message;

  _IOError__isset __isset;

  void __set_message(const std::string& val);

  bool operator == (const IOError & rhs) const
  {
    if (!(message == rhs.message))
      return false;
    return true;
  }
  bool operator != (const IOError &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const IOError & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
  mutable std::string thriftTExceptionMessageHolder_;
  const char* what() const throw();
};

void swap(IOError &a, IOError &b);

inline std::ostream& operator<<(std::ostream& out, const IOError& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _IllegalArgument__isset {
  _IllegalArgument__isset() : message(false) {}
  bool message :1;
} _IllegalArgument__isset;

class IllegalArgument : public ::apache::thrift::TException {
 public:

  IllegalArgument(const IllegalArgument&);
  IllegalArgument& operator=(const IllegalArgument&);
  IllegalArgument() : message() {
  }

  virtual ~IllegalArgument() throw();
  std::string message;

  _IllegalArgument__isset __isset;

  void __set_message(const std::string& val);

  bool operator == (const IllegalArgument & rhs) const
  {
    if (!(message == rhs.message))
      return false;
    return true;
  }
  bool operator != (const IllegalArgument &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const IllegalArgument & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
  mutable std::string thriftTExceptionMessageHolder_;
  const char* what() const throw();
};

void swap(IllegalArgument &a, IllegalArgument &b);

inline std::ostream& operator<<(std::ostream& out, const IllegalArgument& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _AlreadyExists__isset {
  _AlreadyExists__isset() : message(false) {}
  bool message :1;
} _AlreadyExists__isset;

class AlreadyExists : public ::apache::thrift::TException {
 public:

  AlreadyExists(const AlreadyExists&);
  AlreadyExists& operator=(const AlreadyExists&);
  AlreadyExists() : message() {
  }

  virtual ~AlreadyExists() throw();
  std::string message;

  _AlreadyExists__isset __isset;

  void __set_message(const std::string& val);

  bool operator == (const AlreadyExists & rhs) const
  {
    if (!(message == rhs.message))
      return false;
    return true;
  }
  bool operator != (const AlreadyExists &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const AlreadyExists & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
  mutable std::string thriftTExceptionMessageHolder_;
  const char* what() const throw();
};

void swap(AlreadyExists &a, AlreadyExists &b);

inline std::ostream& operator<<(std::ostream& out, const AlreadyExists& obj)
{
  obj.printTo(out);
  return out;
}

}}}} // namespace

#endif
