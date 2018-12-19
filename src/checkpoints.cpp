// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
    (     0, uint256("0x0000dd716a317a0ada4c9fdc6ec2982e2e9116f0e528373a0fcd53c0c378fad1") )
    (  1000, uint256("0x9a878d343d5e63dfe8e455ae0e5e387bfdb9e7f05122553cd07a8365ebe5a62f") )
    ( 10000, uint256("0x4207b7388d022839374e4d0cfb51a94670b78b27b4f03561688d490164b413c8") )
    ( 25000, uint256("0x99a4e43113ae3ff1f7ef31bbf2dc5499fc9d149e003cfa3e10e8b054db0c2bbf") )
    ( 50000, uint256("0xf5811f207f83813e12259d31eadf47758b59b52006b98663c056bd2de1469291") )
    (100000, uint256("0x3d4b26cea49591810d53d70b2c9e37d7abd531d2a147cd560d99a9707c0c78a0") )
    (120000, uint256("0x2f18032bc85a8e9fefaf730cc3a0cb2f98eb105a5345e8c76de7600e1ca34f7e") )
    (140000, uint256("0xbbe08f3c1ad71e79f85ce35a1b5379e86a41f536bfc33dbdc9a1f357c24438b0") )
    (144323, uint256("0x4c555879587c42b507ebf3f77f1f409e1fab57996b352ab0c8772fc2b4cc229e") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
