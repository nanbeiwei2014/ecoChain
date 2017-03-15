#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <iostream>
#include "HbaseOperate.h"

/*
int main(int argc, char** argv)
{
	
	CHbaseOperate g_ho;
	g_ho.connect("127.0.0.1", 9090);
	g_ho.putRow("demo_table","key_for_lhy","entry:","test hello");
	hbaseRet result;
	const std::string table = "demo_table";
	const std::string rowKey = "key_for_lhy";
	const std::string columnName = "entry:";
	g_ho.getRow(result,table,rowKey,columnName);
	result.print();
	
	g_ho.disconnect();
	return 0;
}
*/

int main(int argc, char** argv)
{
	std::string table = "hbase_table";
	const std::string cols[3] = {"col_1:","col_2:","col_3:"};
	CHbaseOperate g_ho;
	g_ho.connect("192.168.10.51", 9090);
	
	std::vector<std::string> colVec;
	colVec.push_back(cols[0]);
	colVec.push_back(cols[1]);
	colVec.push_back(cols[2]);
	string err_msg_;
	g_ho.drop_table(table, err_msg_);
	g_ho.create_table(table, colVec);
	
	std::vector<std::pair<std::string, int> > colDesc;
	if(g_ho.get_table_descriptors(table,colDesc))
	{
		for(std::vector<std::pair<std::string, int> >::iterator it = colDesc.begin(); it != colDesc.end(); ++it)
		{
			std::cout<<"column: "<<it->first<<" "<<"maxVer: "<<it->second<<std::endl;
		}
	}
	
	std::vector<batchResult> verRes;
	for(int row = 0; row < 100; row++)
	{	
		batchResult inValue;
		inValue.rowkey = ltos(row);
		for(int icols = 0; icols < 3; icols++)
		{
			cell col;
			col.column = cols[icols];
			col.value  = "hello" + ltos(icols);
			inValue.columns.push_back(col);
		}
		verRes.push_back(inValue);
	}
	g_ho.put_batch_rows(table, verRes);
	
	std::vector<batchResult> res;
	//g_ho.get_by_start_rows(table,0,res);
	g_ho.get_scan_rows(table,res);
	for (std::vector<batchResult>::const_iterator it = res.begin(); it != res.end(); ++it)
	{
		std::cout<<"rowkey: "<<it->rowkey<<"  ";
		for (std::vector<cell>::const_iterator iter = (it->columns).begin(); iter != (it->columns).end(); ++iter)
		{
			std::cout<<iter->column<<" ---> "<<iter->value<<"  ";
		}
		std::cout<<std::endl;
	}
	
	std::string err_msg = "";
	//g_ho.drop_table(table, err_msg);
	std::cout<<"drop table: " <<table<<" "<<err_msg<<endl; 
	g_ho.disconnect();
	return 0;
}

