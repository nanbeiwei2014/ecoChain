#ifndef __HBASE_OPERATE_H 
#define __HBASE_OPERATE_H 
  
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>

#include "Hbase.h"
#include "util.h"
  
/** 
 * Class to operate Hbase. 
 * 
 * @author Darran Zhang (codelast.com) 
 * @version 11-08-24 
 * @declaration These codes are only for non-commercial use, and are distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. 
 * You must not remove this declaration at any time. 
 */
  
using namespace apache::thrift; 
using namespace apache::thrift::protocol; 
using namespace apache::thrift::transport; 
using namespace apache::hadoop::hbase::thrift; 
  
typedef struct hbaseRet 
{ 
	std::string rowValue; 
	time_t ts; 

	hbaseRet() 
	{ 
		ts = 0; 
	} 
	void print()
	{
		std::cout<<"value: "<< rowValue << " Time: "<<ts<<std::endl;
	}
}hbaseRet; 
 
typedef struct cell
{
	std::string column;
	std::string value;
}cell;
	
typedef struct BatchResult
{
	std::string rowkey;
	std::vector<cell> columns;
}batchResult;
  
class CHbaseOperate 
{ 
public: 
	CHbaseOperate(); 
	virtual ~CHbaseOperate(); 
	bool  connect(); 

private: 
	boost::shared_ptr<TTransport> socket; 
	boost::shared_ptr<TTransport> transport; 
	boost::shared_ptr<TProtocol>  protocol; 

	HbaseClient *client; 

	std::string  hbaseServiceHost; 
	int          hbaseServicePort; 
	bool         isConnected; 

public: 
	bool  connect(std::string host, int port); 
	bool  disconnect(); 
	bool  putRow(const std::string &tableName, const std::string &rowKey, const std::string &column, const std::string &rowValue); 
	bool  getRow(hbaseRet &result, const std::string &tableName, const std::string &rowKey, const std::string &columnName);
  	bool  create_table(std::string table_name, std::vector<std::string>  &ColVec);  
	bool get_table_descriptors(std::string table_name, std::vector<std::pair<std::string, int> >  &ColVec);
	bool drop_table(std::string table_name, std::string &err_msg);
	bool get_batch_rows(std::string &table_name, unsigned long start, unsigned long end, std::vector<batchResult> &verRes);
	bool get_by_start_rows(std::string &table_name, unsigned long start, std::vector<batchResult> &verRes);
	bool put_batch_rows(std::string &table_name, std::vector<batchResult> &verRes);
	
	bool del_row_withColumns(std::string &table_name, const std::string row_key, const std::vector<std::string> columns);
	bool is_table_exists(const std::string table);
	bool del_all_row(const std::string table_name, const std::string row_key);
	bool del_row_withColumn(const std::string table_name, const std::string row_key, const std::string column);
	
	bool get_scan_rows(std::string &table_name, std::vector<batchResult> &verRes);
}; 
  
#endif

