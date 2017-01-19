// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KEY_H
#define BITCOIN_KEY_H

#include "pubkey.h"
#include "secure.h"
#include "uint256.h"

#include <stdexcept>
#include <vector>


/** 
 * secp256k1:
 * const unsigned int PRIVATE_KEY_SIZE = 279;
 * const unsigned int PUBLIC_KEY_SIZE  = 65;
 * const unsigned int SIGNATURE_SIZE   = 72;
 *
 * see www.keylength.com
 * script supports up to 75 for single byte push
 */

/**
 * secure_allocator is defined in allocators.h
 * CPrivKey is a serialized private key, with all parameters included (279 bytes)
 */
typedef std::vector<unsigned char, secure_allocator<unsigned char> > CPrivKey;

/** An encapsulated private key. */
class CKey
{
private:
    //! Whether this private key is valid. We check for correctness when modifying the key
    //! data, so fValid should always correspond to the actual state.
    bool fValid;

    //! Whether the public key corresponding to this private key is (to be) compressed.
    bool fCompressed;

    //! The actual byte data
    unsigned char vch[32];

    //! Check whether the 32-byte array pointed to be vch is valid keydata.
    bool static Check(const unsigned char* vch);

public:
    //! Construct an invalid private key.
    CKey() : fValid(false), fCompressed(false)
    {
        LockObject(vch);
    }

    //! Copy constructor. This is necessary because of memlocking.
    CKey(const CKey& secret) : fValid(secret.fValid), fCompressed(secret.fCompressed)
    {
        LockObject(vch);
        memcpy(vch, secret.vch, sizeof(vch));
    }

    //! Destructor (again necessary because of memlocking).
    ~CKey()
    {
        UnlockObject(vch);
    }

    friend bool operator==(const CKey& a, const CKey& b)
    {
        return a.fCompressed == b.fCompressed && a.size() == b.size() &&
               memcmp(&a.vch[0], &b.vch[0], a.size()) == 0;
    }

    //! Initialize using begin and end iterators to byte data.
    template <typename T>
    void Set(const T pbegin, const T pend, bool fCompressedIn)
    {
        if (pend - pbegin != 32) {
            fValid = false;
            return;
        }
        if (Check(&pbegin[0])) {
            memcpy(vch, (unsigned char*)&pbegin[0], 32);
            fValid = true;
            fCompressed = fCompressedIn;
        } else {
            fValid = false;
        }
    }

    //! Simple read-only vector-like interface.
    unsigned int size() const { return (fValid ? 32 : 0); }
    const unsigned char* begin() const { return vch; }
    const unsigned char* end() const { return vch + size(); }

    //! Check whether this private key is valid.
    bool IsValid() const { return fValid; }

    //! Check whether the public key corresponding to this private key is (to be) compressed.
    bool IsCompressed() const { return fCompressed; }

    //! Generate a new private key using a cryptographic PRNG.
    void MakeNewKey(bool fCompressed);

    /**
     * Compute the public key from a private key.
     * This is expensive.
     */
    CPubKey GetPubKey() const;

    /**
     * Create a DER-serialized signature.
     * The test_case parameter tweaks the deterministic nonce.
     */
    bool Sign(const uint256& hash, std::vector<unsigned char>& vchSig, uint32_t test_case = 0) const;

    /**
     * Verify thoroughly whether a private key and a public key match.
     * This is done using a different mechanism than just regenerating it.
     */
    bool VerifyPubKey(const CPubKey& vchPubKey) const;

};


/** Initialize the elliptic curve support. May not be called twice without calling ECC_Stop first. */
void ECC_Start(void);

/** Deinitialize the elliptic curve support. No-op if ECC_Start wasn't called first. */
void ECC_Stop(void);

/** Check that required EC support is available at runtime. */
bool ECC_InitSanityCheck(void);

#endif // BITCOIN_KEY_H
