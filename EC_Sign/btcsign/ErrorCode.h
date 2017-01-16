//
// Created by sdk on 17-1-12.
//

#ifndef BTCSIGN_ERRORCODE_H
#define BTCSIGN_ERRORCODE_H

#ifdef __cplucplus
extern "C" {
#endif

enum ErrCode
{
    ERR_CODE_SUCC             = 0,
    ERR_CODE_STR_LEN             = 1,
    ERR_CODE_PRI_KEY_INVALID     = 2,
    ERR_CODE_SET_KEY_ERROR       = 3,
    ERR_CODE_DATA_SIGN_FAILURE   = 4,
    ERR_CODE_DECODE_PUBKEY_FAIL  = 5,
    ERR_CODE_DECODE_SIGN_FAIL    = 6,
    ERR_CODE_VERIFY_SIGN_FAIL    = 7,
};

extern char ErrMsg[][32];

/*
enum ErrMsg
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
*/
#ifdef __cplucplus
}
#endif

#endif //BTCSIGN_ERRORCODE_H
