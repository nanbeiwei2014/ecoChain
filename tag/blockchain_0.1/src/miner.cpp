// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "miner.h"
#include <math.h>
#include "amount.h"
#include "chain.h"
#include "chainparams.h"
#include "coins.h"
#include "consensus/consensus.h"
#include "consensus/merkle.h"
#include "consensus/validation.h"
#include "hash.h"
#include "main.h"
#include "net.h"
#include "policy/policy.h"
#include "pow.h"
#include "primitives/transaction.h"
#include "script/standard.h"
#include "timedata.h"
#include "txmempool.h"
#include "util.h"
#include "utilmoneystr.h"
#include "validationinterface.h"

#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <queue>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
//
// BitcoinMiner
//

//
// Unconfirmed transactions in the memory pool often depend on other
// transactions in the memory pool. When we select transactions from the
// pool, we select by highest priority or fee rate, so we might consider
// transactions that depend on transactions that aren't yet in the block.

uint64_t nLastBlockTx = 0;
uint64_t nLastBlockSize = 0;
//*************begin modify by mengqg 20161104***********************************
static const int DEFAULT_GENERATE_PERIOD = 0.15*60;  //unit s
static const int VALID_BLOCK_NODES = 0;//
//*************end modify by mengqg 20161104*************************

int64_t UpdateTime(CBlockHeader* pblock, const Consensus::Params& consensusParams, const CBlockIndex* pindexPrev)
{
    int64_t nOldTime = pblock->nTime;
    int64_t nNewTime = std::max(pindexPrev->GetMedianTimePast()+1, GetAdjustedTime());

    if (nOldTime < nNewTime)
        pblock->nTime = nNewTime;

    // Updating time can change work required on testnet:
//******************begin delete by mengqg 20161109******************************************************************************
/*****
    if (consensusParams.fPowAllowMinDifficultyBlocks)
        pblock->nBits = GetNextWorkRequired(pindexPrev, pblock, consensusParams);
*****/
//******************end delete by mengqg 20161109******************************************************************************
    return nNewTime - nOldTime;
}

CBlockTemplate* CreateNewBlock(const CChainParams& chainparams, const CScript& scriptPubKeyIn)
{
    // Create new block
    auto_ptr<CBlockTemplate> pblocktemplate(new CBlockTemplate());
    if(!pblocktemplate.get())
        return NULL;
#if 0
    CBlock *pblock = &pblocktemplate->block; // pointer for convenience

    // -regtest only: allow overriding block.nVersion with
    // -blockversion=N to test forking scenarios
    if (chainparams.MineBlocksOnDemand())
        pblock->nVersion = GetArg("-blockversion", pblock->nVersion);

    // Create coinbase tx
    CMutableTransaction txNew;
    txNew.vin.resize(1);
    txNew.vin[0].prevout.SetNull();
    txNew.vout.resize(1);
    txNew.vout[0].scriptPubKey = scriptPubKeyIn;

    // Add dummy coinbase tx as first transaction
    pblock->vtx.push_back(CTransaction());
    pblocktemplate->vTxFees.push_back(-1); // updated at end
    pblocktemplate->vTxSigOps.push_back(-1); // updated at end

    // Largest block you're willing to create:
    unsigned int nBlockMaxSize = GetArg("-blockmaxsize", DEFAULT_BLOCK_MAX_SIZE);
    // Limit to between 1K and MAX_BLOCK_SIZE-1K for sanity:
    nBlockMaxSize = std::max((unsigned int)1000, std::min((unsigned int)(MAX_BLOCK_SIZE-1000), nBlockMaxSize));

    // How much of the block should be dedicated to high-priority transactions,
    // included regardless of the fees they pay
    unsigned int nBlockPrioritySize = GetArg("-blockprioritysize", DEFAULT_BLOCK_PRIORITY_SIZE);
    nBlockPrioritySize = std::min(nBlockMaxSize, nBlockPrioritySize);

    // Minimum block size you want to create; block will be filled with free transactions
    // until there are no more or the block reaches this size:
    unsigned int nBlockMinSize = GetArg("-blockminsize", DEFAULT_BLOCK_MIN_SIZE);
    nBlockMinSize = std::min(nBlockMaxSize, nBlockMinSize);

    // Collect memory pool transactions into the block
    CTxMemPool::setEntries inBlock;
    CTxMemPool::setEntries waitSet;

    // This vector will be sorted into a priority queue:
    vector<TxCoinAgePriority> vecPriority;
    TxCoinAgePriorityCompare pricomparer;
    std::map<CTxMemPool::txiter, double, CTxMemPool::CompareIteratorByHash> waitPriMap;
    typedef std::map<CTxMemPool::txiter, double, CTxMemPool::CompareIteratorByHash>::iterator waitPriIter;
    double actualPriority = -1;

    std::priority_queue<CTxMemPool::txiter, std::vector<CTxMemPool::txiter>, ScoreCompare> clearedTxs;
    bool fPrintPriority = GetBoolArg("-printpriority", DEFAULT_PRINTPRIORITY);
    uint64_t nBlockSize = 1000;
    uint64_t nBlockTx = 0;
    unsigned int nBlockSigOps = 100;
    int lastFewTxs = 0;
    CAmount nFees = 0;

    {
        LOCK2(cs_main, mempool.cs);
        CBlockIndex* pindexPrev = chainActive.Tip();
        const int nHeight = pindexPrev->nHeight + 1;
        pblock->nTime = GetAdjustedTime();
        const int64_t nMedianTimePast = pindexPrev->GetMedianTimePast();

        int64_t nLockTimeCutoff = (STANDARD_LOCKTIME_VERIFY_FLAGS & LOCKTIME_MEDIAN_TIME_PAST)
                                ? nMedianTimePast
                                : pblock->GetBlockTime();

        bool fPriorityBlock = nBlockPrioritySize > 0;
        if (fPriorityBlock) {
            vecPriority.reserve(mempool.mapTx.size());
            for (CTxMemPool::indexed_transaction_set::iterator mi = mempool.mapTx.begin();
                 mi != mempool.mapTx.end(); ++mi)
            {
                double dPriority = mi->GetPriority(nHeight);
                CAmount dummy;
                mempool.ApplyDeltas(mi->GetTx().GetHash(), dPriority, dummy);
                vecPriority.push_back(TxCoinAgePriority(dPriority, mi));
            }
            std::make_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
        }

        CTxMemPool::indexed_transaction_set::nth_index<3>::type::iterator mi = mempool.mapTx.get<3>().begin();
        CTxMemPool::txiter iter;

        while (mi != mempool.mapTx.get<3>().end() || !clearedTxs.empty())
        {
            bool priorityTx = false;
            if (fPriorityBlock && !vecPriority.empty()) { // add a tx from priority queue to fill the blockprioritysize
                priorityTx = true;
                iter = vecPriority.front().second;
                actualPriority = vecPriority.front().first;
                std::pop_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
                vecPriority.pop_back();
            }
            else if (clearedTxs.empty()) { // add tx with next highest score
                iter = mempool.mapTx.project<0>(mi);
                mi++;
            }
            else {  // try to add a previously postponed child tx
                iter = clearedTxs.top();
                clearedTxs.pop();
            }

            if (inBlock.count(iter))
                continue; // could have been added to the priorityBlock

            const CTransaction& tx = iter->GetTx();

            bool fOrphan = false;
            BOOST_FOREACH(CTxMemPool::txiter parent, mempool.GetMemPoolParents(iter))
            {
                if (!inBlock.count(parent)) {
                    fOrphan = true;
                    break;
                }
            }
            if (fOrphan) {
                if (priorityTx)
                    waitPriMap.insert(std::make_pair(iter,actualPriority));
                else
                    waitSet.insert(iter);
                continue;
            }

            unsigned int nTxSize = iter->GetTxSize();
            if (fPriorityBlock &&
                (nBlockSize + nTxSize >= nBlockPrioritySize || !AllowFree(actualPriority))) {
                fPriorityBlock = false;
                waitPriMap.clear();
            }
            if (!priorityTx &&
                (iter->GetModifiedFee() < ::minRelayTxFee.GetFee(nTxSize) && nBlockSize >= nBlockMinSize)) {
                break;
            }
            if (nBlockSize + nTxSize >= nBlockMaxSize) {
                if (nBlockSize >  nBlockMaxSize - 100 || lastFewTxs > 50) {
                    break;
                }
                // Once we're within 1000 bytes of a full block, only look at 50 more txs
                // to try to fill the remaining space.
                if (nBlockSize > nBlockMaxSize - 1000) {
                    lastFewTxs++;
                }
                continue;
            }

            if (!IsFinalTx(tx, nHeight, nLockTimeCutoff))
                continue;

            unsigned int nTxSigOps = iter->GetSigOpCount();
            if (nBlockSigOps + nTxSigOps >= MAX_BLOCK_SIGOPS) {
                if (nBlockSigOps > MAX_BLOCK_SIGOPS - 2) {
                    break;
                }
                continue;
            }

            CAmount nTxFees = iter->GetFee();
            // Added
            pblock->vtx.push_back(tx);
            pblocktemplate->vTxFees.push_back(nTxFees);
            pblocktemplate->vTxSigOps.push_back(nTxSigOps);
            nBlockSize += nTxSize;
            ++nBlockTx;
            nBlockSigOps += nTxSigOps;
            nFees += nTxFees;

            if (fPrintPriority)
            {
                double dPriority = iter->GetPriority(nHeight);
                CAmount dummy;
                mempool.ApplyDeltas(tx.GetHash(), dPriority, dummy);
                LogPrintf("priority %.1f fee %s txid %s\n",
                          dPriority , CFeeRate(iter->GetModifiedFee(), nTxSize).ToString(), tx.GetHash().ToString());
            }

            inBlock.insert(iter);

            // Add transactions that depend on this one to the priority queue
            BOOST_FOREACH(CTxMemPool::txiter child, mempool.GetMemPoolChildren(iter))
            {
                if (fPriorityBlock) {
                    waitPriIter wpiter = waitPriMap.find(child);
                    if (wpiter != waitPriMap.end()) {
                        vecPriority.push_back(TxCoinAgePriority(wpiter->second,child));
                        std::push_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
                        waitPriMap.erase(wpiter);
                    }
                }
                else {
                    if (waitSet.count(child)) {
                        clearedTxs.push(child);
                        waitSet.erase(child);
                    }
                }
            }
        }
        nLastBlockTx = nBlockTx;
        nLastBlockSize = nBlockSize;
        LogPrintf("CreateNewBlock(): total size %u txs: %u fees: %ld sigops %d\n", nBlockSize, nBlockTx, nFees, nBlockSigOps);

        // Compute final coinbase transaction.
        txNew.vout[0].nValue = nFees + GetBlockSubsidy(nHeight, chainparams.GetConsensus());
        txNew.vin[0].scriptSig = CScript() << nHeight << OP_0;
        pblock->vtx[0] = txNew;
        pblocktemplate->vTxFees[0] = -nFees;

        // Fill in header
        pblock->hashPrevBlock  = pindexPrev->GetBlockHash();
        UpdateTime(pblock, chainparams.GetConsensus(), pindexPrev);
        pblock->nBits          = GetNextWorkRequired(pindexPrev, pblock, chainparams.GetConsensus());
        pblock->nNonce         = 0;
        pblocktemplate->vTxSigOps[0] = GetLegacySigOpCount(pblock->vtx[0]);

        CValidationState state;
        if (!TestBlockValidity(state, chainparams, *pblock, pindexPrev, false, false)) {
            throw std::runtime_error(strprintf("%s: TestBlockValidity failed: %s", __func__, FormatStateMessage(state)));
        }
    }
#endif
    return pblocktemplate.release();
}


CBlockTemplate* CreateNewBlock(const CChainParams& chainparams)
{
    LogPrintf("[%s:%d],begin enter function step\n",__FUNCTION__,__LINE__);
    // Create new block
    auto_ptr<CBlockTemplate> pblocktemplate(new CBlockTemplate());
    if(!pblocktemplate.get())
        return NULL;
    CBlock *pblock = &pblocktemplate->block; // pointer for convenience

    // -regtest only: allow overriding block.nVersion with
    // -blockversion=N to test forking scenarios
    if (chainparams.MineBlocksOnDemand())
        pblock->nVersion = GetArg("-blockversion", pblock->nVersion);

    // Largest block you're willing to create:
    unsigned int nBlockMaxSize = GetArg("-blockmaxsize", DEFAULT_BLOCK_MAX_SIZE);
    // Limit to between 1K and MAX_BLOCK_SIZE-1K for sanity:
    nBlockMaxSize = std::max((unsigned int)1000, std::min((unsigned int)(MAX_BLOCK_SIZE-1000), nBlockMaxSize));


    // Minimum block size you want to create; block will be filled with free transactions
    // until there are no more or the block reaches this size:
    unsigned int nBlockMinSize = GetArg("-blockminsize", DEFAULT_BLOCK_MIN_SIZE);
    nBlockMinSize = std::min(nBlockMaxSize, nBlockMinSize);

    // Collect memory pool transactions into the block

    Cqkgj_mempool::set_entries inBlock;

    // This vector will be sorted into a priority queue:
    vector<DataAgePriority> vecPriority;
    DataAgePriorityCompare pricomparer;

    double actualPriority = -1;



    uint64_t nBlockSize = 1000;
    uint64_t nBlockTx = 0;
    unsigned int nBlockSigOps = 100;
    int lastFewTxs = 0;

    {
        LogPrintf("[%s:%d],begin enter LOCK2() step\n",__FUNCTION__,__LINE__);
        LOCK2(cs_main, qmempool.cs);
        LogPrintf("[%s:%d],end enter LOCK2() step\n",__FUNCTION__,__LINE__);

        CBlockIndex* pindexPrev = chainActive.Tip();
        const int nHeight = pindexPrev->nHeight + 1;
        pblock->nTime = GetAdjustedTime();
        const int64_t nMedianTimePast = pindexPrev->GetMedianTimePast();

        int64_t nLockTimeCutoff = (STANDARD_LOCKTIME_VERIFY_FLAGS & LOCKTIME_MEDIAN_TIME_PAST)
                                ? nMedianTimePast
                                : pblock->GetBlockTime();

        LogPrintf("[%s:%d],begin enter for() step\n",__FUNCTION__,__LINE__);
        bool fPriorityBlock = nBlockMaxSize > 0;
        if (fPriorityBlock) {
            vecPriority.reserve(qmempool.map_data.size());
            for (Cqkgj_mempool::indexed_data_set::iterator mi = qmempool.map_data.begin();
                           mi != qmempool.map_data.end(); ++mi)
             {
                double dPriority = mi->get_priority(nHeight);
                vecPriority.push_back(DataAgePriority(dPriority, mi));
             }
          std::make_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
        }

        //Cqkgj_mempool::it_hash mi  = qmempool.map_hash_data.begin();
        Cqkgj_mempool::indexed_data_set::nth_index<1>::type::iterator mi = qmempool.map_data.get<1>().begin();
        Cqkgj_mempool::data_it iter;

        int i = 0;
        LogPrintf("[%s:%d],begin enter while() step\n",__FUNCTION__,__LINE__);
        while (mi != qmempool.map_data.get<1>().end() )
        {
            bool priorityTx = false;
            if (fPriorityBlock && !vecPriority.empty()) { // add a tx from priority queue to fill the blockprioritysize
                LogPrintf("[%s:%d],if (fPriorityBlock && !vecPriority.empty()),%d times,\n",__FUNCTION__,__LINE__,i);
                priorityTx = true;
                iter = vecPriority.front().second;
                actualPriority = vecPriority.front().first;
                std::pop_heap(vecPriority.begin(), vecPriority.end(), pricomparer);
                vecPriority.pop_back();
            }else{
            	iter = qmempool.map_data.project<0>(mi);
            	mi++;
            }


            if (inBlock.count(iter))
            {
                LogPrintf("[%s:%d],while begin step,%d times\n",__FUNCTION__,__LINE__,i);
                continue; // could have been added to the priorityBlock
            }
            const Cqkgj_basic_data& tx = iter->get_data();


            unsigned int nTxSize = iter->get_data_size();
            if (!priorityTx &&
                (nBlockSize >= nBlockMinSize)) {
                break;
            }
            if (nBlockSize + nTxSize >= nBlockMaxSize) {
                if (nBlockSize >  nBlockMaxSize - 100 || lastFewTxs > 50) {
                    break;
                }
                // Once we're within 1000 bytes of a full block, only look at 50 more txs
                // to try to fill the remaining space.
                if (nBlockSize > nBlockMaxSize - 1000) {
                    lastFewTxs++;
                }
                LogPrintf("[%s:%d],after if(nBlockSize + nTxSize >= nBlockMaxSize) \n",__FUNCTION__,__LINE__);
                continue;
            }
/****
            if (!IsFinalTx(tx, nHeight, nLockTimeCutoff))
                continue;


           unsigned int nTxSigOps = iter->GetSigOpCount();
            if (nBlockSigOps + nTxSigOps >= MAX_BLOCK_SIGOPS) {
                if (nBlockSigOps > MAX_BLOCK_SIGOPS - 2) {
                    break;
                }
                continue;
            }
*****/

            LogPrintf("[%s:%d],1  step\n",__FUNCTION__,__LINE__);
            // Added
            pblock->qvtx.push_back(tx);
            LogPrintf("[%s:%d],2 step\n",__FUNCTION__,__LINE__);
            //pblocktemplate->vTxSigOps.push_back(nTxSigOps);
            nBlockSize += nTxSize;
            LogPrintf("[%s:%d],3 step\n",__FUNCTION__,__LINE__);
            ++nBlockTx;
            //nBlockSigOps += nTxSigOps;
            LogPrintf("[%s:%d],4 step\n",__FUNCTION__,__LINE__);

            inBlock.insert(iter);
            LogPrintf("[%s:%d],5 step\n",__FUNCTION__,__LINE__);

            // Add transactions that depend on this one to the priority queue
            i++;

        }
        LogPrintf("[%s:%d],out step\n",__FUNCTION__,__LINE__);

        nLastBlockTx = nBlockTx;
        nLastBlockSize = nBlockSize;
        LogPrintf("CreateNewBlock(): total size %u txs: %u  sigops %d\n", nBlockSize, nBlockTx,  nBlockSigOps);


        // Fill in header
        pblock->hashPrevBlock  = pindexPrev->GetBlockHash();
        UpdateTime(pblock, chainparams.GetConsensus(), pindexPrev);


        CValidationState state;
        if (!TestBlockValidity(state, chainparams, *pblock, pindexPrev, false, false)) {
            throw std::runtime_error(strprintf("%s: TestBlockValidity failed: %s", __func__, FormatStateMessage(state)));
        }
    }

    LogPrintf("[%s:%d],end enter function step\n",__FUNCTION__,__LINE__);
    return pblocktemplate.release();
}

void IncrementExtraNonce(CBlock* pblock, const CBlockIndex* pindexPrev, unsigned int& nExtraNonce)
{
//    // Update nExtraNonce
//    static uint256 hashPrevBlock;
//    if (hashPrevBlock != pblock->hashPrevBlock)
//    {
//        nExtraNonce = 0;
//        hashPrevBlock = pblock->hashPrevBlock;
//    }
//    ++nExtraNonce;
//    unsigned int nHeight = pindexPrev->nHeight+1; // Height first in coinbase required for block.version=2
//    CMutableTransaction txCoinbase(pblock->vtx[0]);
//    txCoinbase.vin[0].scriptSig = (CScript() << nHeight << CScriptNum(nExtraNonce)) + COINBASE_FLAGS;
//    assert(txCoinbase.vin[0].scriptSig.size() <= 100);
//
//    pblock->vtx[0] = txCoinbase;
//    pblock->hashMerkleRoot = BlockMerkleRoot(*pblock);
}

//////////////////////////////////////////////////////////////////////////////
//
// Internal miner
//

//
// ScanHash scans nonces looking for a hash with at least some zero bits.
// The nonce is usually preserved between calls, but periodically or if the
// nonce is 0xffff0000 or above, the block is rebuilt and nNonce starts over at
// zero.
//
bool static ScanHash(const CBlockHeader *pblock, uint32_t nNonce, uint256 *phash)
{
    // Write the first 76 bytes of the block header to a double-SHA256 state.
    CHash256 hasher;
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << *pblock;
    assert(ss.size() == 80);
    hasher.Write((unsigned char*)&ss[0], 76);

    while (true) {
        nNonce++;

        // Write the last 4 bytes of the block header (the nonce) to a copy of
        // the double-SHA256 state, and compute the result.
        CHash256(hasher).Write((unsigned char*)&nNonce, 4).Finalize((unsigned char*)phash);

        // Return the nonce if the hash has at least some zero bits,
        // caller will check if it has enough to reach the target
        if (((uint16_t*)phash)[15] == 0)
            return true;

        // If nothing found after trying for a while, return -1
        if ((nNonce & 0xfff) == 0)
            return false;
    }
}

static bool ProcessBlockFound(const CBlock* pblock, const CChainParams& chainparams)
{
    LogPrintf("%s\n", pblock->ToString());
//*********begin delete by mengqg 20161109********************************************
    //LogPrintf("generated %s\n", FormatMoney(pblock->vtx[0].vout[0].nValue));
//*********end by mengqg 20161109********************************************
    // Found a solution
    {
        LOCK(cs_main);
        if (pblock->hashPrevBlock != chainActive.Tip()->GetBlockHash())
            return error("QKGJ_Miner: generated block is stale");
    }

    // Inform about the new block
    //GetMainSignals().BlockFound(pblock->GetHash());

    // Process this block the same as if we had received it from another node
    CValidationState state;
    if (!ProcessNewBlock(state, chainparams, NULL, pblock, true, NULL))
        return error("QKGJMiner: ProcessNewBlock, block not accepted");

    //Begin Add by syl 2016-11-21=============================================
    //Send new blockTime
    SendNewBlockTime(pblock);

    //BroadcastNewBlockheader(pblock);
    //End	Add by syl 2016-11-21=============================================

    return true;
}

void GenerateBitcoins(bool fGenerate, int nThreads, const CChainParams& chainparams)
{
    static boost::thread_group* minerThreads = NULL;

    if (nThreads < 0)
        nThreads = GetNumCores();

    if (minerThreads != NULL)
    {
        minerThreads->interrupt_all();
        delete minerThreads;
        minerThreads = NULL;
    }

    if (nThreads == 0 || fGenerate)
        return;

    minerThreads = new boost::thread_group();
    for (int i = 0; i < nThreads; i++)
        minerThreads->create_thread(boost::bind(&BitcoinMiner, boost::cref(chainparams)));
}

bool SortVNodesBy( const CNode* v1, const CNode* v2)
{
     assert(NULL!=v1);
     assert(NULL!=v2);
//	 if ((fabs(v1->m_creBlockTime - v2->m_creBlockTime)<(0.05*DEFAULT_GENERATE_PERIOD))&&(v1->m_bNetState>=v2->m_bNetState)&&(true==v1->m_bNetState)&&(true==v2->m_bNetState))
//		 return v1->addr.ToStringIP()<v2->addr.ToStringIP();
     return (v1->m_creBlockTime <= v2->m_creBlockTime)&&(v1->m_bNetState>=v2->m_bNetState);//Asc
}


void static BitcoinMiner(const CChainParams& chainparams)
{
    LogPrintf("QKGJBlockChainMiner started\n");
    SetThreadPriority(THREAD_PRIORITY_LOWEST);
    RenameThread("QKGJ-miner");

    try {
        // Throw an error if no script was provided.  This can happen
        // due to some internal error but also if the keypool is empty.
        // In the latter case, already the pointer is NULL.

        while (true) {
            if (chainparams.MiningRequiresPeers()) {
                // Busy-wait for the network to come online so we don't waste time mining
                // on an obsolete chain. In regtest mode we expect to fly solo.
                do {
                    bool fvNodesEmpty;
                    {
                        LOCK(cs_vNodes);
                        fvNodesEmpty = vNodes.empty();
                    }
                    if (!fvNodesEmpty )//&& !IsInitialBlockDownload())
                        break;
                    MilliSleep(1000);
                } while (true);
            }

            //
            // Create new block
            //
            unsigned int nTransactionsUpdatedLast = qmempool.get_data_updated();
            CBlockIndex* pindexPrev = chainActive.Tip();

            while(!pindexPrev){
            	 LogPrintf("[%s:%d], chainActive.size():[%d] chainActive.Tip() is NULL!\n",__FUNCTION__,__LINE__,chainActive.Height());
            	pindexPrev = chainActive.Tip();

            }

//            unsigned int nConnectCount = 0;
//
//            for(vector<CNode*>::iterator iter=g_vAllNodes.begin();iter!=g_vAllNodes.end();iter++)
//            {
//            	if ((*iter)->m_bNetState) nConnectCount++;
//            }
//            std::sort(g_vAllNodes.begin(), g_vAllNodes.end(), SortVNodesBy);
// //           vector<CNode*>::iterator iter = std::min_element(g_vAllNodes.begin(), g_vAllNodes.end(), SortVNodesBy);
//            vector<CNode*>::iterator iter=g_vAllNodes.begin();
//            if ((0.05*DEFAULT_GENERATE_PERIOD)>fabs(g_vAllNodes[0]->m_creBlockTime - g_vAllNodes[1]->m_creBlockTime ))
//            {
//            	if (g_vAllNodes[0]->addr.ToStringIP() > g_vAllNodes[1]->addr.ToStringIP() )
//            	iter++;
//            }
//
//             std::string strIp=(*iter)->addr.ToStringIP();
//            if((VALID_BLOCK_NODES<nConnectCount)&&(std::string::npos != strIp.find("127.0.0.1")))
//            {
//        	   if ((GetTime()-pindexPrev->nTime)<(0.95*DEFAULT_GENERATE_PERIOD))
//        		   continue;
//
//            }else{
//            	continue;
//            }
//            (*iter)->m_creBlockTime= GetTime();

            int64_t iTime=GetTime();
            int64_t period=DEFAULT_GENERATE_PERIOD;

            //static bool lockState = true;
            if (0.95 * DEFAULT_GENERATE_PERIOD > (iTime - pindexPrev->GetBlockTime()))
                continue;
            if ((0.05 * DEFAULT_GENERATE_PERIOD) < fabs(iTime % period)) {
                continue;
            }

            int nIndex = -1;
            if(mapArgs.count("-index"))
            {
                nIndex = GetArg("-index",-1);
                if(nIndex == -1)
                {
                    continue;
                }
            }
            if (nIndex != ((iTime / period) % (vNodes.size() + 1))) {

                continue;
            }
//            sleep(5);
            //if (false==lockState)  continue;

            auto_ptr<CBlockTemplate> pblocktemplate(CreateNewBlock(chainparams));
            if (!pblocktemplate.get())
            {
                LogPrintf("Error in QKGJMiner: Keypool ran out, please call keypoolrefill before restarting the mining thread\n");
                return;
            }
            CBlock *pblock = &pblocktemplate->block;
            pblock->m_strMac = g_localMacInfo.GetLocalMac();
            pblock->hashMerkleRoot = BlockMerkleRoot(*pblock);

//******************************************************************
            //pblock->nHeight = pindexPrev->nHeight+1;
            //define height into blockheader for caculating blockhash
//**************************************************************************
            LogPrintf("Running QKGJMiner with %u datas in block (%u bytes)\n", pblock->qvtx.size(),
                    ::GetSerializeSize(*pblock, SER_NETWORK, PROTOCOL_VERSION));


            int64_t nStart = GetTime();



            // In regression test mode, stop mining after a block is found.
            if (chainparams.MineBlocksOnDemand())
                throw boost::thread_interrupted();


            // Check for stop or if block needs to be rebuilt
            boost::this_thread::interruption_point();
            // Regtest mode doesn't require peers
            if (vNodes.empty() && chainparams.MiningRequiresPeers())
                continue;


            if (pindexPrev != chainActive.Tip())
                continue;



            // Update nTime every few seconds
            if (UpdateTime(pblock, chainparams.GetConsensus(), pindexPrev) < 0)
                continue; // Recreate the block if the clock has run backwards,
                           // so that we can use the correct time.
            if(nTransactionsUpdatedLast != qmempool.get_data_updated())
                continue;

            SetThreadPriority(THREAD_PRIORITY_NORMAL);
            LogPrintf("QKGJMiner:\n");
            LogPrintf("proof-of-idleTime found  \n  hash: %s  \n", pblock->GetHash().GetHex());
            ProcessBlockFound(pblock, chainparams);
            std::cout<<"create block : "<<pblock->GetHash().ToString() <<std::endl;
            SetThreadPriority(THREAD_PRIORITY_LOWEST);

 /***************Period average caclulate******************************************
            if (chainparams.GetConsensus().fPowAllowMinDifficultyBlocks)
               {
                    // Changing pblock->nTime can change work required on testnet:
                    hashTarget.SetCompact(pblock->nBits);
               }
*********************************************/
        }
    }
    catch (const boost::thread_interrupted&)
    {
        LogPrintf("QKGJMiner terminated\n");
        throw;
    }
    catch (const std::runtime_error &e)
    {
        LogPrintf("QKGJMiner runtime error: %s\n", e.what());
        return;
    }
}
