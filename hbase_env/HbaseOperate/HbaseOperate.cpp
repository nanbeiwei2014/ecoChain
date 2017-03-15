#include "HbaseOperate.h" 
#include "log4cxx/log4cxx.h" 
#include "log4cxx/propertyconfigurator.h" 

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("HbaseOperate.cpp")); 
using namespace std; 
  
CHbaseOperate::CHbaseOperate() : 
socket((TSocket*)NULL), transport((TBufferedTransport*)NULL), protocol((TBinaryProtocol*)NULL), client(NULL), hbaseServiceHost("127.0.0.1"), hbaseServicePort(9090), isConnected(false) 
{ 
	log4cxx::PropertyConfigurator::configure("log4cxx.properties");
} 
  
CHbaseOperate::~CHbaseOperate() 
{ 
  if (isConnected) { 
    disconnect(); 
  } 
  if (NULL != client) { 
    delete client; 
    client = NULL; 
  } 
} 
  
/** 
 * Connect Hbase. 
 * 
 */
bool CHbaseOperate::connect() 
{ 
  if (isConnected) { 
    LOG4CXX_INFO(logger, "Already connected, don't need to connect it again"); 
    return true; 
  } 
  
  try { 
    socket.reset(new TSocket(hbaseServiceHost, hbaseServicePort)); 
    transport.reset(new TBufferedTransport(socket)); 
    protocol.reset(new TBinaryProtocol(transport)); 
  
    client = new HbaseClient(protocol); 
  
    transport->open(); 
  } catch (const TException &tx) { 
    LOG4CXX_ERROR(logger, "Connect Hbase error : " << tx.what()); 
    return false; 
  } 
  
  isConnected = true; 
  return isConnected; 
} 
  
/** 
 * Connect Hbase. 
 * 
 */
bool CHbaseOperate::connect(std::string host, int port) 
{ 
  hbaseServiceHost = host; 
  hbaseServicePort = port; 
  
  return connect(); 
} 
  
/** 
 * Disconnect from Hbase. 
 * 
 */
bool CHbaseOperate::disconnect() 
{ 
  if (!isConnected) { 
    LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't disconnect from it"); 
    return false; 
  } 
  
  if (NULL != transport) { 
    try { 
      transport->close(); 
    } catch (const TException &tx) { 
      LOG4CXX_ERROR(logger, "Disconnect Hbase error : " << tx.what()); 
      return false; 
    } 
  } else { 
    return false; 
  } 
  
  isConnected = false; 
  return true; 
} 
  
/** 
 * Put a row to Hbase. 
 * 
 * @param tableName   [IN] The table name. 
 * @param rowKey      [IN] The row key. 
 * @param column      [IN] The "column family : qualifier". 
 * @param rowValue    [IN] The row value. 
 * @return True for successfully put the row, false otherwise. 
 */
bool CHbaseOperate::putRow(const string &tableName, const string &rowKey, const string &column, const string &rowValue) 
{ 
  if (!isConnected) { 
    LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
    return false; 
  } 
  const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
  try { 
    std::vector<Mutation> mutations; 
    mutations.push_back(Mutation()); 
    mutations.back().column = column; 
    mutations.back().value = rowValue; 
    client->mutateRow(tableName, rowKey, mutations, dummyAttributes); 
  
  } catch (const TException &tx) { 
    LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 
    return false; 
  } 
  
  return true; 
} 

  
/** 
 * Get a Hbase row. 
 * 
 * @param result      [OUT] The object which contains the returned data. 
 * @param tableName   [IN] The Hbase table name, e.g. "MyTable". 
 * @param rowKey      [IN] The Hbase row key, e.g. "kdr23790". 
 * @param columnName  [IN] The "column family : qualifier". 
 * @return True for successfully get the row, false otherwise. 
 */
bool CHbaseOperate::getRow(hbaseRet &result, const std::string &tableName, const std::string &rowKey, const std::string &columnName) 
{ 
	if (!isConnected) { 
	LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't read data from it"); 
	return false; 
	} 

	std::vector<std::string> columnNames; 
	columnNames.push_back(columnName); 

	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
	std::vector<TRowResult> rowResult; 
	try { 
	client->getRowWithColumns(rowResult, tableName, rowKey, columnNames,dummyAttributes); 
	} catch (const TException &tx) { 
	LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 
	return false; 
	} 

	if (0 == rowResult.size()) { 
	LOG4CXX_WARN(logger, "Got no record with the key : [" << rowKey << "]"); 
	return false; 
	} 

	for (size_t i = 0; i < rowResult.size(); i++) 
	{
		for(std::map<std::string,TCell>::const_iterator it = rowResult[i].columns.begin(); it != rowResult[i].columns.end(); ++it) 
		{
			result.rowValue = it->second.value; 
			result.ts = it->second.timestamp; 
			break;
		}
		break;
	}
	return true; 
}

bool CHbaseOperate::create_table(std::string table_name, std::vector<std::string>  &ColVec)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	}	
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}
	
	std::vector<ColumnDescriptor> columns;
	for(std::vector<std::string>::iterator it = ColVec.begin(); it != ColVec.end(); ++it)
	{
		columns.push_back(ColumnDescriptor());
		columns.back().name = *it;
		columns.back().maxVersions = 10;
	}
	try 
	{
       client->createTable(table_name, columns);
    }
	catch (const AlreadyExists &ae) 
	{
		LOG4CXX_WARN(logger, "WARN:" << ae.message); 
		return false;
    }
	return true;
}

bool CHbaseOperate::get_table_descriptors(std::string table_name, std::vector<std::pair<std::string, int> >  &ColVec)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	}	
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}

	std::map<std::string,ColumnDescriptor> columnMap;
    client->getColumnDescriptors(columnMap, table_name);
    //std::cout << "column families in " << t << ": " << std::endl;
    for (std::map<std::string,ColumnDescriptor>::const_iterator it = columnMap.begin(); it != columnMap.end(); ++it) 
	{
		ColVec.push_back(std::make_pair(it->second.name, it->second.maxVersions));
		//std::cout << "  column: " << it->second.name << ", maxVer: " << it->second.maxVersions << std::endl;
    }	
}

bool CHbaseOperate::drop_table(std::string table_name, std::string &err_msg)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	}
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}
	std::vector<std::string> tables; 
	client->getTableNames(tables);
	for (std::vector<std::string>::const_iterator it = tables.begin(); it != tables.end(); ++it) 
	{
		if (table_name == *it) 
		{
			if (client->isTableEnabled(*it)) 
			{
				client->disableTable(*it);
			}
			client->deleteTable(*it);
			return true;
		}
	}
	err_msg = "not find table " + table_name;
	return false;
}

static void
printRow(const std::vector<TRowResult> &rowResult)
{
  for (size_t i = 0; i < rowResult.size(); i++) {
    std::cout << "row: " << rowResult[i].row << ", cols: ";
    for ( std::map<std::string,TCell>::const_iterator it = rowResult[i].columns.begin(); 
         it != rowResult[i].columns.end(); ++it) {
      std::cout << it->first << " => " << it->second.value << "; ";
    }
    std::cout << std::endl;
  }
}

bool CHbaseOperate::get_batch_rows(std::string &table_name, unsigned long start, unsigned long end, std::vector<batchResult> &verRes)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	}
	
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}
	
	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
	std::vector<std::string> columnNames; 
	std::map<std::string,ColumnDescriptor> columnMap;
    client->getColumnDescriptors(columnMap, table_name);
	for (std::map<std::string,ColumnDescriptor>::const_iterator it = columnMap.begin(); it != columnMap.end(); ++it) 
	{
		columnNames.push_back(it->second.name);
    }

	const int32_t nRows = 50;
	int scanner = client->scannerOpenWithStop(table_name,  ltos(start), ltos(end), columnNames, dummyAttributes);
    try 
	{
		while (true) 
		{
			std::vector<TRowResult> rowResult;
			//client->scannerGet(rowResult, scanner);
			client->scannerGetList(rowResult, scanner, nRows);
			if (rowResult.size() == 0)
			{
				break;
			}
			for (size_t i = 0; i < rowResult.size(); i++) 
			{
				batchResult result;
				result.rowkey =  rowResult[i].row;
				for (std::map<std::string,TCell>::const_iterator it = rowResult[i].columns.begin(); it != rowResult[i].columns.end(); ++it) 
				{
					cell col;
					col.column = it->first;
					col.value  = it->second.value;
					result.columns.push_back(col);
				}
				verRes.push_back(result);
			}
		}
    } 
	catch (const IOError &ioe) 
	{
		LOG4CXX_WARN(logger, cerr << "FATAL: Scanner raised IOError" ); 
		return false;
    }
    client->scannerClose(scanner);
	return true;
}

bool CHbaseOperate::get_scan_rows(std::string &table_name, std::vector<batchResult> &verRes)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	}
	
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}
	
	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
	std::vector<std::string> columnNames; 
	std::map<std::string,ColumnDescriptor> columnMap;
    client->getColumnDescriptors(columnMap, table_name);
	for (std::map<std::string,ColumnDescriptor>::const_iterator it = columnMap.begin(); it != columnMap.end(); ++it) 
	{
		columnNames.push_back(it->second.name);
    }

	const int32_t nRows = 50;
	TScan tscan;
	/*
	//http://www.th7.cn/db/nosql/201603/178426.shtml
	std::string filterStr;
	filterStr = "KeyOnlyFilter ()";
	tscan.__set_filterString(filterStr);  
	*/
	int scanner = client->scannerOpenWithScan(table_name,  tscan, dummyAttributes);
    try 
	{
		while (true) 
		{
			std::vector<TRowResult> rowResult;
			//client->scannerGet(rowResult, scanner);
			client->scannerGetList(rowResult, scanner, nRows);
			if (rowResult.size() == 0)
			{
				break;
			}
			for (size_t i = 0; i < rowResult.size(); i++) 
			{
				batchResult result;
				result.rowkey =  rowResult[i].row;
				for (std::map<std::string,TCell>::const_iterator it = rowResult[i].columns.begin(); it != rowResult[i].columns.end(); ++it) 
				{
					cell col;
					col.column = it->first;
					col.value  = it->second.value;
					result.columns.push_back(col);
				}
				verRes.push_back(result);
			}
		}
    } 
	catch (const IOError &ioe) 
	{
		LOG4CXX_WARN(logger, cerr << "FATAL: Scanner raised IOError" ); 
		return false;
    }
    client->scannerClose(scanner);
	return true;
}

bool CHbaseOperate::get_by_start_rows(std::string &table_name, unsigned long start, std::vector<batchResult> &verRes)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	}
	
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}
	
	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
	std::vector<std::string> columnNames; 
	std::map<std::string,ColumnDescriptor> columnMap;
    client->getColumnDescriptors(columnMap, table_name);
	for (std::map<std::string,ColumnDescriptor>::const_iterator it = columnMap.begin(); it != columnMap.end(); ++it) 
	{
		columnNames.push_back(it->second.name);
    }
	
	const int32_t nRows = 50;
	int scanner = client->scannerOpen(table_name, ltos(start), columnNames, dummyAttributes);
    try 
	{
		while (true) 
		{
			std::vector<TRowResult> rowResult;
			client->scannerGetList(rowResult, scanner, nRows);
			if (rowResult.size() == 0)
			{
				break;
			}
			for (size_t i = 0; i < rowResult.size(); i++) 
			{
				batchResult result;
				result.rowkey =  rowResult[i].row;
				for (std::map<std::string,TCell>::const_iterator it = rowResult[i].columns.begin(); it != rowResult[i].columns.end(); ++it) 
				{
					cell col;
					col.column = it->first;
					col.value  = it->second.value;
					result.columns.push_back(col);
				}
				verRes.push_back(result);
			}
		}
    } 
	catch (const IOError &ioe) 
	{
		LOG4CXX_WARN(logger, cerr << "FATAL: Scanner raised IOError" ); 
		return false;
    }
    client->scannerClose(scanner);
	return true;	
	
}


bool CHbaseOperate::put_batch_rows(std::string &table_name, std::vector<batchResult> &verRes)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	} 
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}	
	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
	try
	{ 
		for(std::vector<batchResult>::iterator it = verRes.begin(); it != verRes.end(); ++it)
		{
			std::vector<cell>  *pcolumns = &((*it).columns);
			
			std::vector<Mutation> mutations; 
			for(std::vector<cell>::iterator iter = pcolumns->begin(); iter != pcolumns->end(); ++iter)
			{
				mutations.push_back(Mutation()); 
				mutations.back().column = (*iter).column; 
				mutations.back().value =  (*iter).value; 
			}
			client->mutateRow(table_name, (*it).rowkey, mutations, dummyAttributes); 
		}
	} 
	catch (const TException &tx) 
	{ 
		LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 
		return false; 
	} 
	return true; 
}

bool CHbaseOperate::del_row_withColumns(std::string &table_name, const std::string row_key, const std::vector<std::string> columns)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	} 
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}	
	std::vector<Mutation> mutations; 
	mutations.clear(); 
	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658
	for ( std::vector<std::string>::const_iterator it = columns.begin(); it != columns.end(); ++it) 
	{ 
		mutations.push_back(Mutation()); 
		mutations.back().column = *it; 
		mutations.back().isDelete = true; 
	} 
	try 
	{ 
		client->mutateRow(table_name, row_key, mutations,dummyAttributes); 
		return true; 
	}
	catch (const TException &tx) 
	{ 
		LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 	
	} 
	return false; 
}

bool CHbaseOperate::is_table_exists(const std::string table_name)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	} 
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}		
	std::vector<std::string>  tables;
	try
	{
		client->getTableNames(tables);
		for ( std::vector<std::string>::const_iterator it = tables.begin(); it != tables.end(); ++it) 
		{   
			if (table_name == *it)
				return true;   
		} 
	}		
	catch (const TException &tx) 
	{ 
		LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 	
	} 
	return false; 	
}



bool CHbaseOperate::del_all_row(const std::string table_name, const std::string row_key)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	} 
	
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}
	const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658	
	try
	{
		client->deleteAllRow(table_name, row_key,dummyAttributes); 
		return true;
	}
	catch (const TException &tx) 
	{ 
		LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 	
	} 
	return false;
}

bool CHbaseOperate::del_row_withColumn(const std::string table_name, const std::string row_key, const std::string column)
{
	if (!isConnected) 
	{ 
		LOG4CXX_ERROR(logger, "Haven't connected to Hbase yet, can't put row"); 
		return false; 
	} 
	if(table_name.empty())
	{
		LOG4CXX_WARN(logger, "table_name is null."); 
		return false;
	}		
	try
	{
		const std::map<Text, Text>  dummyAttributes; // see HBASE-6806 HBASE-4658	
		std::vector<Mutation> mutations; 
		mutations.clear(); 
		mutations.push_back(Mutation()); 
		mutations.back().column = column; 
		mutations.back().isDelete = true; 
		client->mutateRow(table_name, row_key, mutations,dummyAttributes); 
		return true;
	}
	catch (const TException &tx) 
	{ 
		LOG4CXX_ERROR(logger, "Operate Hbase error : " << tx.what()); 	
	} 
	return false;	
	
}








