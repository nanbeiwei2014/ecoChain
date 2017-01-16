//
// Created by sdk on 16-12-12.
//

#ifndef TEMP_INTERFACE_H
#define TEMP_INTERFACE_H

#include <string>
using namespace std;

#ifndef CKeyAndSignAPI
# if defined(_WIN32_)
#  ifdef CKEYANDSIGN_BUILD
#   define CKeyAndSignAPI __declspec(dllexport)
#  else
#   define CKeyAndSignAPI
#  endif
# elif defined(__GNUC__) && defined(CKEYANDSIGN_BUILD)
#  define CKeyAndSignAPI __attribute__ ((visibility ("default")))
# else
#  define CKeyAndSignAPI
# endif
#endif

#define UINT unsigned int

void InitCrypt( void );
void StopCrypt( void );

void GetCurrentVersion( char* version );

/* 获得公钥和私钥
 * 第一个参数密钥种子为可选项，如果有值，则长度为32
 * */
int GetPriPubKey( char* seeds, char *prikey, char *pubkey, char* errmsg );

/*
 * 获得数据的sign
 * 输入私钥和数据，返回签名
 */
int GetDataSign( char* prikey, char* data, char* sign, char* errmsg );

/*
 * 验证签名
 * 输入公钥，数据和签名
 * 返回 0 签名正确, 非零错误，查看errmsg信息
 */
int VerifySign( const char *pubkey, char *data, char *sign, char* errmsg );

/**
 *
 **/
void EncodeBase64( const unsigned char *str, unsigned int len, unsigned char* outStr, UINT *outLen );
void DecodeBase64( const unsigned char *str, unsigned int len, unsigned char *outStr, UINT *outLen );

#endif //TEMP_INTERFACE_H
