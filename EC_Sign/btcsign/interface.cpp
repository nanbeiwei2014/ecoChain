//
// Created by sdk on 16-12-12.
//

#define CKEYANDSIGN_BUILD (1)

#include "interface.h"
#include "key.h"
#include "base58.h"
#include "utilstrencodings.h"
#include "ErrorCode.h"
#include <boost/scoped_ptr.hpp>

#include <vector>
#include <string>

const char* g_version = "1.0";

void GetCurrentVersion( char* version )
{
    strncpy( version, g_version, strlen(g_version) );
    return ;
}


//#include <boost/foreach.hpp>

using namespace std;
static boost::scoped_ptr<ECCVerifyHandle> g_verifyHandle;

/* 初始化加密*/
void InitCrypt( void )
{
    ECC_Start();
    g_verifyHandle.reset( new ECCVerifyHandle() );
}

/* 停止加密*/
void StopCrypt( void )
{
    g_verifyHandle.reset();
    ECC_Stop();
}

static bool
__attribute__((constructor))
	init(void)
{

	InitCrypt();
	return true;
}

static bool
__attribute__((destructor))
	fini(void)
{
	StopCrypt();			
	return true;
}

int GetPriPubKey( char* seeds, char *prikey, char *pubkey, char* errmsg )
{
    bool b_compress = true; //CanSupportFeature( FEATURE_COMPRPUBKEY )
    int len = 0;

    CKey secret;
    if ( NULL == seeds )
    {
        secret.MakeNewKey( b_compress );
    }
    else
    {
        vector< unsigned char> vch;
        len = (int) strlen( seeds );
        if( len != 32 ) /* 传过来的种子参数必须是32位，则否返回错误*/
        {
            strncpy( errmsg,ErrMsg[ERR_CODE_STR_LEN],strlen( ErrMsg[ERR_CODE_STR_LEN]));
            return ERR_CODE_STR_LEN;
        }

        vch.resize(len);
        string tempseeds = seeds;
        vch.assign( tempseeds.begin(),tempseeds.end());
        secret.Set( &vch[0], &vch[vch.size()], b_compress );
    }

    /* 获得公钥 */
    CPubKey pub_key = secret.GetPubKey();
    assert( secret.VerifyPubKey( pub_key ) );

    /* 把公钥转换成base58编码格式 */
    vector< unsigned char> tempVch;
    tempVch.resize(pub_key.size());
    tempVch.assign(pub_key.begin(),pub_key.end());
    string temppubkey = EncodeBase58( tempVch );
    strncpy( pubkey,temppubkey.c_str(),temppubkey.length() );

    /* 获得私钥*/
    string pri_key(CBitcoinSecret(secret).ToString());
    //prikey = pri_key;
    strncpy( prikey, pri_key.c_str(), pri_key.length() );

    strncpy( errmsg,ErrMsg[ERR_CODE_SUCC],strlen( ErrMsg[ERR_CODE_SUCC]));
    return ERR_CODE_SUCC;
}

int GetDataSign( char* prikey, char* data, char* sign, char* errmsg )
{
    string tempsign;
    CBitcoinSecret vchSecret;
    bool bFlag = vchSecret.SetString( prikey );
    if ( !bFlag )
    {
        strncpy( errmsg,ErrMsg[ERR_CODE_PRI_KEY_INVALID],strlen( ErrMsg[ERR_CODE_PRI_KEY_INVALID]));
        return ERR_CODE_PRI_KEY_INVALID;
    }

    CKey key = vchSecret.GetKey();
    if ( !key.IsValid() )
    {
        strncpy( errmsg,ErrMsg[ERR_CODE_SET_KEY_ERROR],strlen( ErrMsg[ERR_CODE_SET_KEY_ERROR]));
        return ERR_CODE_SET_KEY_ERROR;
    }

    std::vector<unsigned char> vchSign;
    string tempdata = data;
    uint256 hashMsg = Hash( tempdata.begin(), tempdata.end() );
    if (!key.Sign( hashMsg,vchSign) )
    {
        strncpy( errmsg,ErrMsg[ERR_CODE_DATA_SIGN_FAILURE],strlen( ErrMsg[ERR_CODE_DATA_SIGN_FAILURE]));
        return ERR_CODE_DATA_SIGN_FAILURE;
    }

    /* 把签名转换成base58编码格式 */
    tempsign = EncodeBase58(vchSign);
    strncpy( sign, tempsign.c_str(),tempsign.length() );

    strncpy( errmsg,ErrMsg[ERR_CODE_SUCC],strlen( ErrMsg[ERR_CODE_SUCC]));
    return ERR_CODE_SUCC;
}

int VerifySign( const char *pubkey, char *data, char *sign, char* errmsg )
{
    vector<unsigned char> vch_pub_key;
    /* 把得到的数据以base58格式解码 */
    bool bdecodeRet = DecodeBase58( pubkey, vch_pub_key );
    if ( !bdecodeRet )
    {
        strncpy( errmsg,ErrMsg[ERR_CODE_DECODE_PUBKEY_FAIL],strlen( ErrMsg[ERR_CODE_DECODE_PUBKEY_FAIL]));
        return ERR_CODE_DECODE_PUBKEY_FAIL;
    }

    /* 把得到的签名以base58格式解码 */
    std::vector<unsigned char> vchSign;
    bool bSign = DecodeBase58( sign, vchSign );
    if ( false == bSign )
    {
        strncpy( errmsg,ErrMsg[ERR_CODE_DECODE_SIGN_FAIL],strlen( ErrMsg[ERR_CODE_DECODE_SIGN_FAIL]));
        return ERR_CODE_DECODE_SIGN_FAIL;
    }

    /* 根据数据生成hash */
    string tempdata = data;
    uint256 hashMsg = Hash( tempdata.begin(), tempdata.end() );

    CPubKey cpub_key( vch_pub_key );
    bool bVer = cpub_key.Verify( hashMsg,vchSign );
    if ( false == bVer )
    {
        strncpy( errmsg,ErrMsg[ERR_CODE_VERIFY_SIGN_FAIL],strlen( ErrMsg[ERR_CODE_VERIFY_SIGN_FAIL]));
        return ERR_CODE_VERIFY_SIGN_FAIL;
    }

    strncpy( errmsg,ErrMsg[ERR_CODE_SUCC],strlen( ErrMsg[ERR_CODE_SUCC]));
    return ERR_CODE_SUCC;
}

void EncodeBase64( const unsigned char *str, unsigned int len, unsigned char* outStr, UINT *outLen )
{
    string retStr = EncodeBase64( str,len );

    *outLen = (UINT)retStr.length();
    strncpy( (char*)outStr, retStr.c_str(), *outLen );
    return ;

}

void DecodeBase64( const unsigned char *str, unsigned int len, unsigned char *outStr, UINT *outLen )
{
    string inStr((char*)str );

    string tmpStr = DecodeBase64( inStr );
    *outLen= (UINT)tmpStr.length();
    strncpy( (char*)outStr, tmpStr.c_str(), *outLen);
}

char ErrMsg[][32] =
{
        "success",
        "the string's length error",
        "private key invalid",
        "set key error",
        "data signature failure",
        "decode public key failure",
        "decode sign failure",
        "verify sign failure",
};

