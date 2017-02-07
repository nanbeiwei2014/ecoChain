// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "chain.h"
#include "coins.h"
#include "consensus/validation.h"
#include "core_io.h"
#include "init.h"
#include "keystore.h"
#include "main.h"
#include "merkleblock.h"
#include "net.h"
#include "policy/policy.h"
#include "primitives/transaction.h"
#include "rpcserver.h"
#include "script/script.h"
#include "script/interpreter.h"
#include "script/script_error.h"
#include "script/sign.h"
#include "script/standard.h"
#include "txmempool.h"
#include "uint256.h"
#include "utiltime.h"
#include "utilstrencodings.h"
#include "util.h"
#include "clientversion.h"

#include <stdint.h>

#include <boost/assign/list_of.hpp>

#include <univalue.h>

using namespace std;

const int IntMaxRecordNum = 100;
const int IntMaxBlockNum = 100;
const int GetMaxColumn = 100;
const int TimeZone = 28800;

bool StrToLower( string &str )
{
	for( unsigned int i = 0; i < str.length(); i++ )
	{
        if ( str[i] >='A' && str[i] <= 'Z' )
        {
            str[i] = str[i] + 32;
        }
	}
	return true;
}

bool IsNum( string str )
{
    const char *p = str.c_str();
    for( unsigned int i = 0; i < str.length(); i++ )
    {
        if ( i == 4 || i == 7 || i == 10 || i == 13 || i==16 )
            continue;
        if ( p[i] < 48 || p[i] > 57 )
            return false;
    }
    return true;
}

int64_t GetSecondsByStr( string timeStr )
{
    if ( timeStr.length() != 19 )
    {
        return -1;
    }
    const char *p = timeStr.c_str();
    if ( p[4] != '-' || p[7] != '-' || p[10] !=' ' || p[13] != ':' || p[16] != ':' )
        return -1;
    if ( !IsNum( timeStr ))
    {
        return -1;
    }

    struct tm timeinfo;
    sscanf( p, "%d-%d-%d %d:%d:%d",&timeinfo.tm_year,&timeinfo.tm_mon,&timeinfo.tm_mday,
            &timeinfo.tm_hour,&timeinfo.tm_min,&timeinfo.tm_sec );
    timeinfo.tm_year -= 1900; /**/
    timeinfo.tm_mon  -= 1;    /**/
    return mktime( &timeinfo );
}

int64_t GetStartTimeByStr( string timeStr )
{
    if ( timeStr.length() != 19 )
    {
        return -1;
    }
    const char *p = timeStr.c_str();
    if ( p[4] != '-' || p[7] != '-' || p[10] !=' ' || p[13] != ':' || p[16] != ':' )
        return -1;

    string baseStr = timeStr.substr( 0,timeStr.length()-9 );
    string startTime = baseStr + " 00:00:00";
    return GetSecondsByStr( startTime );
}

int64_t GetEndTimeByStr( string timeStr )
{
    if ( timeStr.length() != 19 )
    {
        return -1;
    }
    const char *p = timeStr.c_str();
    if ( p[4] != '-' || p[7] != '-' || p[10] !=' ' || p[13] != ':' || p[16] != ':' )
        return -1;

    string baseStr = timeStr.substr( 0,timeStr.length()-9 );
    string EndTime = baseStr + " 23:59:59";
    return GetSecondsByStr( EndTime );
}

/* add by sdk begin */
/*********************************************************************
 * 上层应用调用，根据hash查询该hash相应的数据
 *********************************************************************/
UniValue get_data_from_sys( const UniValue& params, bool bHelp )
{
    if( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "get_data_from_sys  \"data_id\" \n"
             "\nNOTE:By default this function only works is when the data is in the mempool.\n"
             "Returns the data you have been puts in mempool.\n"

             "\nArguments:\n"
             "1. \"data_id\"        (string, required) A json objects about this data's hash\n"
             "\nResult:\n"
             "{\n"
                "\"publickey\":\"publickey\" (string) string of publickey\n"
                "\"data\":\"data\"       (string) string of the data\n"
                "\"sign\":\"signature\"  (string) string of the sign\n"
                "\"blockhash\":\"hash\", (string) the block hash\n"
             "}\n"
             "\nExamples\n"
             + HelpExampleCli("get_data_from_sys", "\"[{\\\"data_id\\\":\\\"myid\\\"}]\"")
             + HelpExampleRpc("get_data_from_sys", "\"[{\\\"data_id\\\":\\\"myid\\\"}]\"")
        );
    }

    LOCK( cs_main );
    uint256 hash = ParseHashV( params[0], "parameter 1");
    Cqkgj_basic_data data;
    uint256 hashBlock;
    if( !get_transaction( hash, data, hashBlock ) )
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY, "No information avaliable about the data you want get it");
    }

    LogPrintf("[%s:%s:%d],data_id:%s\n", __FILE__, __FUNCTION__,__LINE__,hash.GetHex());

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("publicKey",data.m_address));
    result.push_back(Pair("data",data.m_data));
    result.push_back(Pair("sign",data.m_sign));
    result.push_back(Pair("blockHash",hashBlock.GetHex()));

    LogPrintf("[%s:%s:%d],Leave... blockHash:%s\n", __FILE__, __FUNCTION__,__LINE__,hashBlock.GetHex());
    return result;
}

/* get data list */
UniValue GetDataList( const UniValue &params, bool bHelp )
{
    LogPrintf("[%s:%s:%d],Enter function!\n", __FILE__, __FUNCTION__,__LINE__ );

    if ( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetDataList  \"intRecordNum\" \n"
             "\nGet the new record data.\n"
             "If  the parameter's value less than 1 or begger than MAX_RECORD_NUM, you will be get nothing!\n"
             "\nArguments:\n"
             "1. \"IntRecordNum\"      (int, Mandatory) How many records you wants to get.\n"
             "\nResult:\n"
             "[\n"
                "{\n"
                   "\"hash\":\"data's hash\"   (string) hex string of the data's hash\n"
                   "\"description\":\"record's description\" (string) hex string of the public value\n"
                   "\"confirm_cnt\":\"confirm count\" (string) hex string of the public value\n"
                "},\n"
             "]\n"
             "\nExamples\n"
             + HelpExampleCli("GetDataList", "\"IntRecordNum\"")
             + HelpExampleRpc("GetDataList", "\"IntRecordNum\"")
             );
    }

    LOCK( cs_main );

    int recordNum = params[0].get_int();
    if ( recordNum < 1 || recordNum > IntMaxRecordNum )
    {
        throw JSONRPCError( RPC_INVALID_PARAMETER, "The Num is out of range");
    }

    LogPrintf( "[%s:%s:%d],IntRecordNum:%d\n", __FILE__, __FUNCTION__,__LINE__,recordNum );

    UniValue res( UniValue::VARR );
    CBlockIndex *pblock = chainActive.Tip();
    int uiChainHeight = pblock->nHeight;
    LogPrintf( "[%s:%s:%d],chainHeight:%d\n", __FILE__, __FUNCTION__, __LINE__, uiChainHeight );

    int i = 0;
    int temp = 0;
    while ( i < recordNum )
    {
        int uiConfirmCnt = uiChainHeight - pblock->nHeight;
        CBlock block;
        if ( !ReadBlockFromDisk( block, pblock, Params().GetConsensus() ) )
        {
            throw JSONRPCError( RPC_INTERNAL_ERROR, "Can't read block from disk");
        }

        int size = block.qvtx.size();
        LogPrintf( "[%s:%s:%d],block data size:%d\n", __FILE__, __FUNCTION__, __LINE__, size );

        if ( i + size > recordNum )
            temp = recordNum - i;
        else
            temp = size;

        for ( int j = 0; j < temp; j++ )
        {
            UniValue obj( UniValue::VOBJ );
            obj.push_back( Pair( "hash",block.qvtx[j].get_hash().GetHex() ) );
            obj.push_back( Pair( "data",block.qvtx[j].m_data));
            obj.push_back( Pair( "confirm", uiConfirmCnt ));
            res.push_back( obj );
        } // end of for() loop

        i += size;

        pblock = pblock->pprev;
        if ( NULL == pblock )
            break;
    } // end of while() loop

    LogPrintf("[%s:%s:%d],Leave function!\n", __FILE__, __FUNCTION__,__LINE__ );
    return res;
}

/* get new block records */
UniValue GetBlockList( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );
    if ( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetBlockList  \"IntBlockNum\" \n"
             "\nGet the new block record.\n"
             "If  the parameter's value less than 1 or begger than MAX_RECORD_NUM, you will be get nothing but error!\n"
             "\nArguments:\n"
             "1. \"IntBlockNum\"      (int, Mandatory) How many Block records you wants to get.\n"
             "\nResult:\n"
             "[\n"
                "{\n"
                    "\"BlockHeight\":\"block's height\"   (int) the block's height!\n"
                    "\"GenerateTime\":\"block generate time\" (string) hex string of the public value\n"
                    "\"records\":\"how many records\" (string) hex string of the public value\n"
                    "\"node\":\"which node generate this block\" (string) hex string of the public value\n"
                    "\"hash\":\"the block's hash value\" (string) hex string of the public value\n"
                    "\"size\":\"the block's size\" (string) hex string of the public value\n"
                "},\n"
             "]\n"
             "\nExamples\n"
             + HelpExampleCli("GetBlocckList", "\"IntBlockNum\"")
             + HelpExampleRpc("GetBlocckList", "\"IntBlockNum\"")
             );
    }
    LOCK( cs_main );
    //RPCTypeCheck(params, boost::assign::list_of(UniValue::VNUM), true);
    int recordNum = params[0].get_int();
    if ( recordNum < 1 || recordNum > IntMaxBlockNum )
    {
        throw JSONRPCError( RPC_INVALID_PARAMETER, "The Num is out of range");
    }

    LogPrintf( "[%s:%s:%d],recordNum:%d\n", __FILE__, __FUNCTION__, __LINE__, recordNum );

    CBlockIndex *pblockindex = chainActive.Tip();
    if ( NULL == pblockindex )
    {
        throw JSONRPCError( RPC_INVALID_PARAMETER, "There are no block in chain now!" );
    }

    int blockCount = pblockindex->nHeight;
    LogPrintf( "[%s:%s:%d],blockCount:%d\n", __FILE__, __FUNCTION__, __LINE__, blockCount );

    int tempVar = ( blockCount < recordNum ? blockCount : recordNum );
    LogPrintf( "[%s:%s:%d],can get data at most:%d rows\n", __FILE__, __FUNCTION__, __LINE__, tempVar );

    UniValue res(UniValue::VARR);

    for ( int i = 0; i < tempVar; i++ )
    {
        UniValue obj(UniValue::VOBJ);
        CBlockIndex *pBlkIdx = chainActive[ tempVar - i ];
        if ( NULL == pBlkIdx )
        {
            LogPrintf( "[%s:%s:%d]chain index is null!", __FILE__, __FUNCTION__,__LINE__);
            break;
        }

        obj.push_back( Pair( "blockHeight",  pBlkIdx->nHeight) );
        obj.push_back( Pair( "generateTime", pBlkIdx->GetBlockTime()) );
        obj.push_back( Pair( "records",      (int)pBlkIdx->nTx) );
        obj.push_back( Pair( "blockHash",    pBlkIdx->GetBlockHash().GetHex()) );
        obj.push_back( Pair( "generateNode", pBlkIdx->sPubKey ) );

        CBlockHeader blockHeader = pBlkIdx->GetBlockHeader();
        CBlock block( blockHeader );
        obj.push_back( Pair( "blockSize",    (int)::GetSerializeSize( block,SER_DISK, CLIENT_VERSION )));

        res.push_back(obj);
    }

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );
    return res;
}

/* get data last new */
UniValue GetDataLastNew( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    if ( bHelp || params.size() != 0 )
    {
        throw runtime_error(
             "GetDataLastNew \n"
             "\nGet the Latest data record.\n"
             "This functions has no parameter!\n"
             "\nArguments:\n"
             "\nResult:\n"
             "{\n"
                "\"blockNo\":\"The latest block number\"   (string) the block's number!\n"
                "\"generateTime\":\"generate time\" (string) hex string of the public value\n"
             "}\n"
             "\nExamples\n"
             + HelpExampleCli("GetDataLastNew", "")
             + HelpExampleRpc("GetDataLastNew", "")
             );
    }

	LOCK( cs_main );

    UniValue res(UniValue::VOBJ);
    CBlockIndex *pBlkIdx = chainActive.Tip();
    if ( NULL != pBlkIdx )
    {
        res.push_back( Pair("blockNo", pBlkIdx->nHeight) );
        res.push_back( Pair("generateTime", (int)(pBlkIdx->nHeight == 0 ? 0 : pBlkIdx->nTime - pBlkIdx->pprev->nTime) ) );
    }

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    return res;
}

UniValue GetBlockByDate( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    if ( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetBlockByDate \"InputDate\" \n"
             "\nAccording to the date to look for blocks info the Latest data record.\n"
             "\nArguments:\n"
             "\"InputDate\"  (int) The date when you look for"
             "\nResult:\n"
             "[\n"
                "{\n"
                    "\"height\":\"The latest block number\"   (string) the block's number!\n"
                    "\"time\":\"generate time\" (string) hex string of the public value\n"
                    "\"records\":\"how many records\" (string) hex string of the public value\n"
                    "\"hash\":\"the block's hash value\" (string) hex string of the public value\n"
                "}, \n"
             "]\n"
             "\nExamples\n"
             + HelpExampleCli("GetBlockByDate", "\"InputDate\"")
             + HelpExampleRpc("GetBlockByDate", "\"InputDate\"")
             );
    }

    LOCK( cs_main );

    string timeStr = params[0].get_str();
    LogPrintf( "[%s:%s:%d],inPutDate:%s\n", __FILE__, __FUNCTION__, __LINE__, timeStr );

    int64_t ulStartTime = GetStartTimeByStr( timeStr );
    if ( ulStartTime < 0 )
    {
        LogPrintf( "[%s:%s:%d],ulStartTime:%ld\n", __FILE__, __FUNCTION__, __LINE__, ulStartTime );
        throw JSONRPCError( RPC_INVALID_PARAMETER, "time format error!" );
    }

    int64_t ulEndTime = GetEndTimeByStr( timeStr );
    if ( ulEndTime < 0 )
    {
        LogPrintf( "[%s:%s:%d],ulEndTime:%ld\n", __FILE__, __FUNCTION__, __LINE__, ulEndTime );
        throw JSONRPCError( RPC_INVALID_PARAMETER, "time format error!" );
    }

    LogPrintf( "[%s:%s:%d],startTime:%ld,endTime:%ld\n", __FILE__, __FUNCTION__, __LINE__, ulStartTime, ulEndTime );

    int blockHeight = chainActive.Height();
    LogPrintf( "[%s:%s:%d],blockHeight:%d\n", __FILE__, __FUNCTION__, __LINE__, blockHeight );

    UniValue res( UniValue::VARR );
    int i = 0;
    CBlockIndex *pBlkIdx = chainActive[ blockHeight -i ];
    LogPrintf( "[%s:%s:%d],blockHash:%s\n", __FILE__, __FUNCTION__, __LINE__,pBlkIdx->GetBlockHash().GetHex() );
    LogPrintf( "[%s:%s:%d],getBlockTime:%s\n", __FILE__, __FUNCTION__, __LINE__,pBlkIdx->GetBlockTime() );

    LogPrintf( "[%s:%s:%d],begin enter while loop\n", __FILE__, __FUNCTION__, __LINE__ );
    int curItem  = 0;
    while ( NULL != pBlkIdx && i <= blockHeight )
    {
        UniValue obj(UniValue::VOBJ);

        pBlkIdx = chainActive[ blockHeight-i ];
        if ( ulEndTime < pBlkIdx->GetBlockTime() )
        {
            i++;
            continue;
        }

        if ( ulStartTime > pBlkIdx->GetBlockTime() || curItem >= GetMaxColumn )
        {
            LogPrintf( "[%s:%s:%d],blockHash:%s\n", __FILE__, __FUNCTION__, __LINE__,pBlkIdx->GetBlockHash().GetHex() );
            LogPrintf( "[%s:%s:%d],input:%ld,blockTime:%ld,curtimes:%d,max:%d\n", __FILE__, __FUNCTION__, __LINE__, ulStartTime, pBlkIdx->GetBlockTime(), curItem, GetMaxColumn );
            break;
        }

        obj.push_back( Pair("blockHeight", pBlkIdx->nHeight) );
        obj.push_back( Pair("generateTime", (int)(pBlkIdx->nHeight == 0 ? 0 : pBlkIdx->nTime - pBlkIdx->pprev->nTime )) );
        obj.push_back( Pair("records", (int32_t)pBlkIdx->nTx ) );
        obj.push_back( Pair("generateNode", pBlkIdx->sPubKey ) );

        CBlockHeader blockHeader = pBlkIdx->GetBlockHeader();
        CBlock block( blockHeader );
        obj.push_back( Pair("blockSize", (int)::GetSerializeSize( block,SER_DISK, CLIENT_VERSION )));

        obj.push_back( Pair("blockHash",    pBlkIdx->GetBlockHash().GetHex() ) );
        res.push_back( obj );

        i++;
        curItem ++;
        pBlkIdx = chainActive[ blockHeight-i ];
    }

    LogPrintf( "[%s:%s:%d],Leave the process!get %d datas\n", __FILE__, __FUNCTION__, __LINE__, curItem );

    return res;
}

UniValue GetNodeStatus( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );
    if (params.size() != 1 || bHelp )
    {
        throw runtime_error(
            "GetNodeStatus \"NodeCode\"\n"
            "\nGet the private key and public key.\n"
            "If  the parameters is null,the system can generate a private/public key pair.\n"
            "But if the parameters is set ,you must make sure the length of the string equale 32.\n"

            "\nArguments:\n"
            "1. \"string\"        (string, option) the private seed.\n"
            "\nResult:\n"
            "{\n"
                "\"nodeVersion\":1    (int) The node's version\n"
                "\"protocolVersion\":1 (int) The protocol's version\n"
                "\"linkNode\":2 (int) link node's number\n"
                "\"netState\":true (boolean) network's state\n"
                "\"height\":3 (int) node's height\n"
                "\"generateTime\":3  (int) generate block's time\n"
            "}\n"
            "\nExamples\n"
            + HelpExampleCli("GetNodeStatus", "\"node code\"")
            + HelpExampleRpc("GetNodeStatus", "\"node code\"")
            );
    }

    LOCK( cs_main );
    int nodeCode = params[0].get_int();
    if ( nodeCode > (int)g_vAllNodes.size() || nodeCode < 0 )
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY, "Nodes can not found" );
    }

    LogPrintf( "[%s:%s:%d],nodeCode:%d\n", __FILE__, __FUNCTION__, __LINE__, nodeCode );

    UniValue res( UniValue::VOBJ );
    bool bFind = false;
    BOOST_FOREACH( CNode *node, g_vAllNodes )
    {
        if ( nodeCode == node->GetId() )
        {
            bFind = true;
            res.push_back( Pair( "nodeVersion", node->nVersion ));
            res.push_back( Pair( "protocolVersion", node->nVersion ));
            res.push_back( Pair( "linkNodes", g_vAllNodes.size()-1 ));
            res.push_back( Pair( "netState", node->m_bNetState ));
            break;
        }
    }
    if ( true == bFind )
    {
        int height = chainActive.Height();
        CBlockIndex *pBlkIdx = chainActive.Tip();

        res.push_back( Pair( "blockHeight", height ));
        res.push_back( Pair("generateTime", (int)(height == 0 ? 0 : pBlkIdx->nTime - pBlkIdx->pprev->nTime )) );
    }
    else
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY, "node not found" );
    }

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    return res;
}

UniValue GetBlockDetail( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    if( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetBlockDetail  \"blockHash\" \n"
             "\nGet the block's detail info private key and public key.\n"
             "If  the parameters is null,the system can generate a private/public key pair.\n"
             "But if the parameters is set ,you must make sure the length of the string equale 32.\n"

             "\nArguments:\n"
             "1. \"string\"        (string, option) the block's hash.\n"
             "\nResult:\n"
             "{\n"
                "\"height\":\"xxxxx\",    (string) block's height.\n"
                "\"confirm\":1            (numeric) confirm's times.\n"
                "\"size\":   2  (numeric) block's size.\n"
                "\"records\":3  (numeric) block contains the number of data.\n"
                "\"generateTime\":4 (numeric) generate block time. \n"
                "\"version\":4  (numeric)  version.\n"
                "\"generateNode\":3 (numeric)  which node generate block\n"
                "\"hash\":\"xxxxx\" (string) block's hash\n"
                "\"prevHash\":\"xxxxx\" (string) previous block's hash.\n"
                "\"nextHash\":\"xxxxx\" (string) next block's hash.\n"
                "\"merkleRoot\":\"xxxxx\" (string) current block's merkel root.\n"
             "}\n"
             "\nExamples\n"
             + HelpExampleCli("GetBlockDetail", "\"blockHash\"")
             + HelpExampleRpc("GetBlockDetail", "\"blockHash\"")
        );
    } // end of if

    LOCK( cs_main );
    string strHash = params[0].get_str();
    LogPrintf( "[%s:%s:%d],inputHash:%s\n", __FILE__, __FUNCTION__, __LINE__, strHash );

    uint256 hash( uint256S( strHash ));
    //uint256 hash = ParseHashV( params[0], "parameter 1");

    if ( 0 == mapBlockIndex.count( hash ))
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY, "Block not found" );
    }

    int chainHeight = chainActive.Tip()->nHeight;

    UniValue res( UniValue::VOBJ );
    CBlockIndex *pBlkIdx = mapBlockIndex[hash];
    if ( NULL != pBlkIdx )
    {
        res.push_back( Pair( "blockHeight",       pBlkIdx->nHeight ));
        res.push_back( Pair( "confirms",      chainActive.Height() - pBlkIdx->nHeight ));

        CBlockHeader blockHeader = pBlkIdx->GetBlockHeader();
        CBlock block( blockHeader );
        res.push_back( Pair( "blockSize",     (int)::GetSerializeSize( block,SER_DISK, CLIENT_VERSION )));

        res.push_back( Pair( "records",      (int32_t)pBlkIdx->nTx ));
        res.push_back( Pair("generateTime", (int)(pBlkIdx->nHeight == 0 ? 0 : pBlkIdx->nTime - pBlkIdx->pprev->nTime )) );
        res.push_back( Pair( "version", pBlkIdx->nVersion ));
        res.push_back( Pair( "generateNode", pBlkIdx->sPubKey ) );

        res.push_back( Pair( "blockHash",         pBlkIdx->GetBlockHash().GetHex() ));

        if ( 0 == pBlkIdx->nHeight ) //if current block is genesis block
        {
            res.push_back( Pair( "prevHash",     "" ));
        }
        else
        {
            res.push_back( Pair( "prevHash",     pBlkIdx->pprev->GetBlockHash().GetHex() ));
        }

        if ( pBlkIdx->nHeight >= chainHeight ) //if current block is the newest block
        {
            res.push_back( Pair( "nextHash",     ""));
        }
        else
        {
            res.push_back( Pair( "nextHash",     chainActive[pBlkIdx->nHeight+1]->GetBlockHash().GetHex() ));
        }
        res.push_back( Pair( "merkleRoot",   pBlkIdx->hashMerkleRoot.GetHex() ));
    }

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );
    return res;
}

UniValue GetBlockHashByTx( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    if( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetBlockHashByTx  \"recordHash\" \n"
             "\nGet the block's detail info private key and public key.\n"
             "If  the parameters is null,the system can generate a private/public key pair.\n"
             "But if the parameters is set ,you must make sure the length of the string equale 32.\n"

             "\nArguments:\n"
             "1. \"dataHash\"        (string) the record's hash.\n"
             "\nResult:\n"
             "\"hash\":\"xxxxx\" (string) block's hash\n"
             "\nExamples\n"
             + HelpExampleCli("GetBlockHashByTx", "\"recordHash\"")
             + HelpExampleRpc("GetBlockHashByTx", "\"recordHash\"")
        );
    } // end of if

    LOCK( cs_main );
    string timeStr = params[0].get_str();
    uint256 hash = ParseHashV( params[0], "parameter 1");
    LogPrintf( "[%s:%s:%d],dataHash:%s,timeStr:%s\n", __FILE__, __FUNCTION__, __LINE__, hash.GetHex(), timeStr );

    Cqkgj_basic_data data;
    uint256 hashBlock;
    if( !get_transaction( hash, data, hashBlock ) )
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY,"No information avaliable about the data you want get it");
    }

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    return hashBlock.GetHex();
}

UniValue GetBlockHeight( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    if( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetBlockHeight  \"blockHash\" \n"
             "\nGet the block's detail info private key and public key.\n"
             "If  the parameters is null,the system can generate a private/public key pair.\n"
             "But if the parameters is set ,you must make sure the length of the string equale 32.\n"

             "\nArguments:\n"
             "1. \"blockHash\"        (string) the record's hash.\n"
             "\nResult:\n"
             "{\n"
                "\"blockHeight\":1 (uint) block's height\n"
             "}\n"
             "\nExamples\n"
             + HelpExampleCli("GetBlockHeight", "\"blockHash\"")
             + HelpExampleRpc("GetBlockHeight", "\"blockHash\"")
        );
    } // end of if

    LOCK( cs_main );
    string strHash = params[0].get_str();
    LogPrintf( "[%s:%s:%d],blockHash:%s\n", __FILE__, __FUNCTION__, __LINE__, strHash );

    //uint256 hash( uint256S( strHash ));
    uint256 hash = ParseHashV( params[0], "parameter 1");

    if ( 0 == mapBlockIndex.count( hash ))
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY, "Block not found" );
    }

    UniValue res( UniValue::VOBJ );
    CBlockIndex *pBlkIdx = mapBlockIndex[hash];
    if ( NULL != pBlkIdx )
    {
        res.push_back( Pair("blockHeight", pBlkIdx->nHeight));
    }

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );
    return res;
}

UniValue GetBlockHashByHe( const UniValue &params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter the process!\n", __FILE__, __FUNCTION__, __LINE__ );

    if( bHelp || params.size() != 1 )
    {
        throw runtime_error(
             "GetBlockHashByHe  \"blockHash\" \n"
             "\nGet the block's detail info private key and public key.\n"
             "If  the parameters is null,the system can generate a private/public key pair.\n"
             "But if the parameters is set ,you must make sure the length of the string equale 32.\n"

             "\nArguments:\n"
             "1. \"blockHeight\"        (int) the record's height.\n"
             "\nResult:\n"
             "\"blockHeight\":\"xxxxx\" (string) block's hash\n"
             "\nExamples\n"
             + HelpExampleCli("GetBlockHashByHe", "\"blockHeight\"")
             + HelpExampleRpc("GetBlockHashByHe", "\"blockHeight\"")
        );
    } // end of if

    LOCK( cs_main );
    int uiHeight = params[0].get_int();
    LogPrintf( "[%s:%s:%d],uiHeight:%d\n", __FILE__, __FUNCTION__, __LINE__, uiHeight );

    int blockHeight = chainActive.Height();
    LogPrintf( "[%s:%s:%d],blockHeight:%d\n", __FILE__, __FUNCTION__, __LINE__, blockHeight );


    if( uiHeight < 0 || uiHeight > blockHeight )
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY, "Block not found" );

    LogPrintf( "[%s:%s:%d],Leave the process!\n", __FILE__, __FUNCTION__, __LINE__ );
    return chainActive[uiHeight]->GetBlockHash().GetHex();
}

/* get private key and public key */
UniValue get_new_key( const UniValue& params,bool bHelp )
{
    if( bHelp || params.size() > 1 )
    {
        throw runtime_error(
             "get_new_key  \"[string]\" \n"
             "\nGet the private key and public key.\n"
             "If  the parameters is null,the system can generate a private/public key pair.\n"
             "But if the parameters is set ,you must make sure the length of the string equale 32.\n"

             "\nArguments:\n"
             "1. \"string\"        (string, option) the private seed.\n"
             "\nResult:\n"
             "{\n"
             "\"privateKey\":\"private value\"    (string) hex string of the private value\n"
             "\"publickey\":\"public value\" (string) hex string of the public value\n"
             "\nExamples\n"
             + HelpExampleCli("get_new_key", "")
             + HelpExampleCli("get_new_key", "\"string of seed\"")
             + HelpExampleRpc("get_new_key", "")
             + HelpExampleRpc("get_new_key", "\"string of seed\"")
        );
    }
    LOCK( cs_main );
    RPCTypeCheck(params, boost::assign::list_of(UniValue::VSTR), true);

    bool b_compress = true; //CanSupportFeature(FEATURE_COMPRPUBKEY );
    CKey secret;

	/* 如果没有参数，则使用系统种子,否则，使用传过来的参数作为种子 */
    if ( params.size() < 1 )
    {
        secret.MakeNewKey( b_compress );
    }
    else
    {
        std::string str(params[0].get_str());
        vector<unsigned char> vch;
        int len = str.length();
        LogPrintf("[%s:%s:%d],paramas:%s,length:%d\n", __FILE__, __FUNCTION__,__LINE__,str,len);
        vch.resize(len);
        vch.assign(str.begin(),str.end());
        if( len != 32 ) /* 传过来的种子参数必须是32位，则否返回错误*/
        {
            throw JSONRPCError(RPC_INVALID_PARAMETER,"Invalid parameter,parameter 1's length must equals 32");
        }
        secret.Set( &vch[0], &vch[vch.size()], b_compress );
    }

    /* 获得公钥 */
    CPubKey pub_key = secret.GetPubKey();
    assert( secret.VerifyPubKey( pub_key ) );

	/* 把转换成base58编码格式 */
    std::vector< unsigned char> tempVch;
    tempVch.resize(pub_key.size());
    tempVch.assign(pub_key.begin(),pub_key.end());
    string strKey = EncodeBase58( tempVch );

	/* 获得私钥*/
    string pri_key(CBitcoinSecret(secret).ToString());

    LogPrintf("[%s:%s:%d],privateKey:[%s],publickey:[%s]\n", __FILE__, __FUNCTION__,__LINE__,pri_key,strKey);

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("privateKey",pri_key));
    result.push_back(Pair("publicKey",strKey));

    return result;
}

UniValue send_data_for_sign( const UniValue& params, bool bHelp )
{
    LogPrintf( "[%s:%s:%d],Enter process.... \n", __FILE__, __FUNCTION__, __LINE__ );
    if( bHelp || params.size() != 1 )
    {
        throw runtime_error(
            "send_data_for_sign \"hexstring\"\n"
            "\nsign data parameters for data that comes from application.\n"
            "\nArguments:\n"
            "{\"publickey\":\"addr value\",\"data\":\"data value\",\"privateKey\":\"private key value\"}\n"
            "\nResult:\n"
            "\"signatures\" (string) the data's sign \n"
            + HelpExampleCli("send_data_for_sign", "{\"publickey\":\"public value\",\"data\":\"data value\",\"privateKey\":\"private key value\"}")
            + HelpExampleRpc("send_data_for_sign", "{\"publickey\":\"public value\",\"data\":\"data value\",\"privateKey\":\"private key value\"}")
         );
    }
    LOCK( cs_main );
    LogPrintf( "[%s:%s:%d],Check RpcType! \n", __FILE__, __FUNCTION__, __LINE__ );
    RPCTypeCheck( params, boost::assign::list_of(UniValue::VOBJ), true );
    if ( params[0].isNull() )
    {
        LogPrintf( "[%s:%s:%d],parameter 1 is null! \n", __FILE__, __FUNCTION__, __LINE__ );
        throw JSONRPCError( RPC_INVALID_PARAMETER, "Invalid parameter,arguments 1 must be non-null");
    }

    string pri_key;
    string pub_key;
    string get_data;
    UniValue data = params[0].get_obj();
    std::vector<std::string>vData = data.getKeys();
    BOOST_FOREACH( string &name, vData )
    {
        string tempStr = name;
		StrToLower( name );
        if ( "privatekey" == name )
        {
            pri_key = data[tempStr].get_str();
        }
        else if ( "publickey" == name )
        {
            pub_key = data[tempStr].get_str();
        }
        if ( "data" == name )
        {
            get_data = data[tempStr].get_str();
        }
    }

    LogPrintf("[%s:%s:%d],privatekey:%s,data:%s\n", __FILE__, __FUNCTION__,__LINE__,pri_key,get_data);

    std::vector<unsigned char> vch_sign;
    std::string addr,temp;
    Cqkgj_basic_data basic_data(addr,get_data,temp);

    CBitcoinSecret vchSecret;
    bool bFlag = vchSecret.SetString(pri_key);
    if ( !bFlag )
    {
       throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY,"Invalid private key");
    }
    CKey key = vchSecret.GetKey();
    if ( !key.IsValid() )
    {
       throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY,"Private key outside allowed range");
    }

    bool bSign = create_sign( key, vch_sign, basic_data );
    if ( false == bSign )
    {
        LogPrintf("[%s:%s:%d],data signature failure!privatekey:%s,data:%s\n", __FILE__, __FUNCTION__,__LINE__,pri_key,get_data);
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY,"Private signature data error!");
    }

	/* 把签名转换成base58编码格式 */
    string ret = EncodeBase58(vch_sign);
    LogPrintf("[%s:%s:%d],sign:%s,privatekey:%s,data:%s\n", __FILE__, __FUNCTION__,__LINE__,ret,pri_key,get_data);
    LogPrintf( "[%s:%s:%d],Leave process.... \n", __FILE__, __FUNCTION__, __LINE__ );
    return ret;
}

UniValue send_data_to_sys(const UniValue& params, bool bHelp)
{
    if ( bHelp  || params.size() != 1)
    {
        throw runtime_error(
            "send_data_to_sys \"{\"publickey\":\"publickey info\",\"data\":\"data info\",\"sign\":\"sign value\"}\"\n"
            "\nSend a application data to mempool .\n"
            "Returns hex-encoded data's hash.\n"

            "\nArguments:\n"
            "1. \"publickey\"        (string, required) A json string of json objects\n"
            "2. \"data\"           (string, required) a json object with outputs\n"
            "3. \"sign\"           (numeric, optional, default=0) Raw locktime. Non-0 value also locktime-activates inputs\n"
            "\nResult:\n"
            "\"data_id\"       (string) data's hash\n"

            "\nExamples\n"
            + HelpExampleCli("send_data_to_sys", "\"{\\\"publickey\\\":\\\"publickey\\\",\\\"data\\\":\\\"data\\\",\\\"sign\\\":\\\"sign value\\\"}\"")
            + HelpExampleRpc("send_data_to_sys", "\"{\\\"publickey\\\":\\\"publibkey\\\",\\\"data\\\":\\\"data\\\",\\\"sign\\\":\\\"sign value\\\"}\"")
        );
    }

    LOCK( cs_main );
    RPCTypeCheck( params, boost::assign::list_of(UniValue::VOBJ), true );
    if ( params[0].isNull() )
    {
        throw JSONRPCError( RPC_INVALID_PARAMETER, "Invalid parameter,arguments 1 must be non-null");
    }

    //Cqkgj_basic_data data;
    string str_data;
    string str_addr;
    string str_sign;
    //解析JSON
    UniValue get_data = params[0].get_obj();
    std::vector<std::string>vData = get_data.getKeys();
    BOOST_FOREACH( string &name, vData )
    {
        string tempStr = name;
        StrToLower( name );
        if( "data" == name )
        {
            str_data = get_data[tempStr].get_str();
        }
        else if ( "publickey" == name )
        {
            str_addr = get_data[tempStr].get_str();
        }
        else
        {
            str_sign = get_data[tempStr].get_str();
        }
    }

    LogPrintf("[%s:%s:%d],addr:%s,data:%s,sign:%s\n", __FILE__, __FUNCTION__,__LINE__,str_addr,str_data,str_sign);

    Cqkgj_basic_data data(str_addr,str_data,str_sign);

    if (!check_sign(data))
    {
        return UniValue(UniValue::VNUM, "{\"result\":\"data verify failure!\"}");
    }
    else
    {
        LogPrintf("[%s:%s:%d],verify signature success\n", __FILE__, __FUNCTION__,__LINE__ );
        //std::cout<<"success"<<std::endl;
    }

    //写入内存池
    //Cqkgj_process_data	newProData(data, 0, 123.00, 1,0);
    uint256 data_hash = data.get_hash();

    bool bIsHaveChain = data_in_chain( data_hash );
    bool bExists = qmempool.exists(data_hash);
    if ( !bExists && !bIsHaveChain )
    {
        CValidationState state;
        bool bRet = AddToMempool( qmempool, state, data );
        if ( false == bRet )
        {
            LogPrintf("[%s:%s:%d],add to mempool return false\n", __FILE__, __FUNCTION__,__LINE__);
            throw JSONRPCError(RPC_TRANSACTION_REJECTED,strprintf("%i:%s",state.GetRejectCode(),state.GetRejectReason()));
        }
    }
    else if( bIsHaveChain )
    {
        LogPrintf("[%s:%s:%d],data[%s] already in block chain!\n", __FILE__, __FUNCTION__,__LINE__, data_hash.GetHex());
        throw JSONRPCError(RPC_TRANSACTION_ALREADY_IN_CHAIN,"data already in block chain!");
    }

    //触发广播数据到其他节点的广播消息
    RelayQkgjMsg(data);

    //UniValue result;
    //result.push_back(Pair("hash",data.get_hash().GetHash());
    //return result;
    uint256 hash_data = data.get_hash();
    return hash_data.GetHex();
    //return UniValue(UniValue::VNUM, "{\"resutl\":\"success\"}");
}
/* add by sdk end */

//Begin	Add by syl 2016-10-31========================================
void RelayQkgjMsg(const Cqkgj_basic_data& msgData)
{
	CInv inv(MSG_TX, msgData.get_hash());

	LOCK(cs_vNodes);
	BOOST_FOREACH(CNode* pnode, vNodes)
	{
		pnode->PushQkgjMsg(inv);
    }
}
//End	Add by syl 2016-10-31========================================
