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
#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif

#include <stdint.h>

#include <boost/assign/list_of.hpp>

#include <univalue.h>

using namespace std;

string FILE_NAME="test.log";

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
             "\"address\"         (string) string of address\n"
             "\"data\"            (string) string of the data\n"
             "\"sign\"            (string) string of the sign\n"

             "\nExamples\n"
             + HelpExampleCli("get_data_from_sys", "\"[{\\\"data_id\\\":\\\"myid\\\"}]\"")
             + HelpExampleRpc("get_data_from_sys", "\"[{\\\"data_id\\\":\\\"myid\\\"}]\"")
        );
    }
    LOCK( cs_main );
    uint256 hash = ParseHashV( params[0], "parameter 1");
    Cqkgj_basic_data data;
    if( !get_transaction(hash,data))
    {
        throw JSONRPCError( RPC_INVALID_ADDRESS_OR_KEY,"No information avaliable about the data you want get it");
    }

    LogPrintf("[%s:%d],data_id:%s\n",__FUNCTION__,__LINE__,hash.GetHex());

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("address",data.m_address));
    result.push_back(Pair("data",data.m_data));
    result.push_back(Pair("sign",data.m_sign));

    return result;
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
             "\"publicKey\":\"public value\" (string) hex string of the public value\n"
             "\nExamples\n"
             + HelpExampleCli("get_new_key", "")
             + HelpExampleCli("get_new_key", "\"string of seed\"")
             + HelpExampleRpc("get_new_key", "")
             + HelpExampleRpc("get_new_key", "\"string of seed\"")
        );
    }
    LOCK( cs_main );
    RPCTypeCheck(params, boost::assign::list_of(UniValue::VSTR)(UniValue::VARR)(UniValue::VARR)(UniValue::VSTR), true);

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
        LogPrintf("[%s:%d],paramas:%s,length:%d\n",__FUNCTION__,__LINE__,str,len);
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

	/* 获得私钥*/
    string pri_key(CBitcoinSecret(secret).ToString());

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("PriKey",pri_key));
    //result.push_back(Pair("PubKey",pub_key.GetID().ToString()));
    result.push_back(Pair("PubKey",pub_key.ToString()));

    return result;
}

UniValue send_data_for_sign( const UniValue& params, bool bHelp )
{
    if( bHelp || params.size() < 1 )
    {
        throw runtime_error(
            "send_data_for_sign \"hexstring\"\n"
            "\nsign data parameters for data that comes from application.\n"
            "\nArguments:\n"
            "{\"pubkey\":\"addr value\",\"data\":\"data value\",\"prikey\":\"prikey value\"}\n"
            "\nResult:\n"
            "\"signatures\" (string) the data's sign \n"
            + HelpExampleCli("send_data_for_sign", "{\"pubkey\":\"public value\",\"data\":\"data value\",\"prikey\":\"prikey value\"}")
            + HelpExampleRpc("send_data_for_sign", "{\"pubkey\":\"public value\",\"data\":\"data value\",\"prikey\":\"prikey value\"}")
         );
    }
    LOCK( cs_main );
    RPCTypeCheck( params, boost::assign::list_of(UniValue::VOBJ)(UniValue::VSTR),true);
    if ( params[0].isNull() )
    {
        throw JSONRPCError( RPC_INVALID_PARAMETER, "Invalid parameter,arguments 1 must be non-null");
    }

    string pri_key;
    string pub_key;
    string get_data;
    UniValue data = params[0].get_obj();
    std::vector<std::string>vData = data.getKeys();
    BOOST_FOREACH( const string &name, vData )
    {
        if ( "prikey" == name )
        {
            pri_key = data[name].get_str();
        }
        if ( "pubkey" == name )
        {
            pub_key = data[name].get_str();
        }
        if ( "data" == name )
        {
            get_data = data[name].get_str();
        }
    }

    LogPrintf("[%s:%d],addr:%s,prikey:%s,data:%s\n",__FUNCTION__,__LINE__,pub_key,pri_key,get_data);

    std::vector<unsigned char> vch_sign;
    CKeyID keyid;
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

    create_sign( key, vch_sign, keyid, basic_data );
    string ret = EncodeBase58(vch_sign);
    //ret.insert(ret.begin(),vch_sign.begin(),vch_sign.end());
    LogPrintf("[%s:%d],sign:%s,prikey:%s,data:%s\n",__FUNCTION__,__LINE__,ret,pri_key,get_data);
    return ret;
}

UniValue send_data_to_sys(const UniValue& params, bool bHelp)
{
    if ( bHelp  || params.size() != 1)
    {
        throw runtime_error(
            "send_data_to_sys \"{\"address\":\"address info\",\"data\":\"data info\",\"sign\":\"sign value\"}\"\n"
            "\nSend a application data to mempool .\n"
            "Returns hex-encoded data's hash.\n"

            "\nArguments:\n"
            "1. \"address\"        (string, required) A json string of json objects\n"
            "2. \"data\"           (string, required) a json object with outputs\n"
            "3. \"sign\"           (numeric, optional, default=0) Raw locktime. Non-0 value also locktime-activates inputs\n"
            "\nResult:\n"
            "\"data_id\"       (string) data's hash\n"

            "\nExamples\n"
            + HelpExampleCli("send_data_to_sys", "\"{\\\"address\\\":\\\"address\\\",\\\"data\\\":\\\"data\\\",\\\"sign\\\":\\\"sign value\\\"}\"")
            + HelpExampleRpc("send_data_to_sys", "\"{\\\"address\\\":\\\"address\\\",\\\"data\\\":\\\"data\\\",\\\"sign\\\":\\\"sign value\\\"}\"")
        );
    }

    LOCK( cs_main );
    RPCTypeCheck( params, boost::assign::list_of(UniValue::VOBJ)(UniValue::VSTR), true );
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
    BOOST_FOREACH( const string &name, vData )
    {
        if( "data" == name )
        {
            str_data = get_data[name].get_str();
        }
        else if ( "address" == name )
        {
            str_addr = get_data[name].get_str();
        }
        else
        {
            str_sign = get_data[name].get_str();
        }
    }

    LogPrintf("[%s:%d],addr:%s,data:%s,sign:%s",__FUNCTION__,__LINE__,str_addr,str_data,str_sign);

    Cqkgj_basic_data data(str_addr,str_data,str_sign);

    vector<unsigned char> vch_pub_key;
    bool bdecodeRet = DecodeBase58( str_addr,vch_pub_key );
    if ( !bdecodeRet )
    {
        LogPrintf("[%s:%d],public key:[%s] decode error!\n",__FUNCTION__,__LINE__,str_addr);
        throw JSONRPCError( RPC_INVALID_PARAMETER, "decode public key error!");
    }
    //vch_pub_key.resize(str_addr.length());
    //vch_pub_key.assign(str_addr.begin(),str_addr.end());

    //CScript script;
    //if (!check_sign(data, vch_pub_key, script))
    //{
    //    return UniValue(UniValue::VNUM, "{\"result\":\"data verify failure!\"}");
    //}

    //写入内存池
    //Cqkgj_process_data	newProData(data, 0, 123.00, 1,0);
    CValidationState state;
    bool bRet = AddToMempool( qmempool, state, data );
    if ( false == bRet )
    {
        throw JSONRPCError(RPC_TRANSACTION_REJECTED,strprintf("%i:%s",state.GetRejectCode(),state.GetRejectReason()));
    }

    //触发广播数据到其他节点的广播消息
    RelayQkgjMsg(data);

    UniValue result;
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
