// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"

#include "uint256.h"
#include "arith_uint256.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

// For equihash_parameters_acceptable.
#include "crypto/equihash.h"
#include "net.h"
#include "validation.h"
#define equihash_parameters_acceptable(N, K) \
    ((CBlockHeader::HEADER_SIZE + equihash_solution_size(N, K))*MAX_HEADERS_RESULTS < \
     MAX_PROTOCOL_MESSAGE_LENGTH-1000)

#include "base58.h"
#include <assert.h>
#include <boost/assign/list_of.hpp>
#include <limits>

#include "chainparamsseeds.h"
#include "pow.h"
static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = ArithToUint256(arith_uint256(nNonce));
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.nHeight  = 0;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);

    return genesis;
}

/**
 * Build the genesis block. Note tha)t the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4asendcoinsentry.cpp5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    const CScript genesisOutputScript = CScript() << ParseHex("0x000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

void CChainParams::UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));


class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 2100000000;
        consensus.BIP34Height = 227931;
        consensus.BIP34Hash = uint256S("0x000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8");
        consensus.BIP65Height = 388381; // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.BIP66Height = 363725; // 00000000000000000379eaa19dce8c9b722d46ae6a57c2f1a988119488b50931
        consensus.BTGHeight = 491407; // Around 10/25/2017 12:00 UTC
        consensus.BTGPremineWindow = 8000;
        consensus.BTGZawyLWMAHeight = std::numeric_limits<int>::max(); // Not activated on mainnet
        consensus.BTGPremineEnforceWhitelist = true;
        consensus.powLimit = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimitStart = uint256S("0000000fffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimitLegacy = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        consensus.nDigishieldAveragingWindow = 30;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nDigishieldAveragingWindow);
        consensus.nDigishieldMaxAdjustDown = 32;
        consensus.nDigishieldMaxAdjustUp = 16;

        consensus.nZawyLwmaAveragingWindow = 45;
        consensus.nZawyLwmaAjustedWeight = 13632;
        consensus.BTGMaxFutureBlockTime =  10 * 60; // 10 mins
        
        consensus.nPowTargetTimespanLegacy = 7 * 6 * 60 * 60; // 1 minute
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespanLegacy / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1462060800; // May 1st, 2016
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1493596800; // May 1st, 2017

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1479168000; // November 15th, 2016.
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1510704000; // November 15th, 2017.

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000723d3581fe1bd55373540a");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x0000000000000000003b9ce759c2a087d52abc4266f8f4ebd6d768b89defa50a"); //477890

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xe1;
        pchMessageStart[1] = 0x47;
        pchMessageStart[2] = 0x6d;
        pchMessageStart[3] = 0x44;
        nDefaultPort = 8338; // different port than Bitcoin
        nPruneAfterHeight = 100000;
        const size_t N = 200, K = 9;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;


        genesis = CreateGenesisBlock(1525997277, 2, 0x1d00ffff, 1, 12500 * COIN);

        while (!CheckProofOfWork(genesis.GetHash(consensus), genesis.nBits, false, consensus)) {
            genesis.nNonce = ArithToUint256(UintToArith256(genesis.nNonce) + 1);
        }

//        genesis = CreateGenesisBlock(1525997277, 2083236893, 0x1d00ffff, 1, 12500 * COIN);
        printf("genesis.GetHash = %s\n", genesis.GetBlockHeader().nNonce.ToString().c_str());

        consensus.hashGenesisBlock = genesis.GetHash(consensus);
        printf("genesis.GetHash = %s\n", genesis.GetHash(consensus).ToString().c_str());
        printf("genesis.hashMerkleRoot = %s\n", genesis.GetBlockHeader().hashMerkleRoot.ToString().c_str());
        assert(consensus.hashGenesisBlock == uint256S("855b134ecf981b350ae44a55d711970a0ffd400572babd62b265919925c65076"));
        assert(genesis.hashMerkleRoot == uint256S("09f57ee53501cecb09edc4f51397215c68657edb730da3fbab5bcd7e85112a7e"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top

        vSeeds.emplace_back("eu-dnsseed.bitcoingold-official.org", true);
        vSeeds.emplace_back("dnsseed.bitcoingold.org", true);
        vSeeds.emplace_back("dnsseed.btcgpu.org", true);

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38);  // prefix: G
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,23);  // prefix: A
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,128);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData) {
            {
                { 0, uint256S("0x0000000069e244f73d78e8fd29ba2fd2ed618bd6fa2ee92559f542fdb26e7c1d")},
            }
        };

        chainTxData = ChainTxData{
            // Data as of block 000000000000000000d97e53664d17967bd4ee50b23abb92e54a34eb222d15ae (height 478913).
            1525997277, // * UNIX timestamp of last known number of transactions
            0,  // * total number of transactions between genesis and that timestamp
                        //   (the tx=... number in the SetBestChain debug.log lines)
            1.0         // * estimated number of transactions per second after that timestamp
        };

        vPreminePubkeys = {
            { "02bb4feeb207779054aacb49b61d325f46e0b98afb301a66dc32ca3e7484f419ef" },
        };
    }
};

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 2100000000;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = -1;
        consensus.BIP66Height = -1;
        consensus.BTGHeight = 1;
        consensus.BTGZawyLWMAHeight = -1; // Activated on testnet
        consensus.BTGPremineWindow = 50;
        consensus.BTGPremineEnforceWhitelist = false;
        consensus.powLimit = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimitStart = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimitLegacy = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        consensus.nDigishieldAveragingWindow = 30;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nDigishieldAveragingWindow);
        consensus.nDigishieldMaxAdjustDown = 32;
        consensus.nDigishieldMaxAdjustUp = 16;

        consensus.nZawyLwmaAveragingWindow = 45;
        consensus.nZawyLwmaAjustedWeight = 13632;
        consensus.BTGMaxFutureBlockTime = 7 * 10 * 60; // 70 mins
        
        consensus.nPowTargetTimespanLegacy = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespanLegacy / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 1514764800; // January 1st, 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 1546300800; // January 1st, 2019

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 1514764800; // January 1st 2018
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 1546300800; // January 1st 2019

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xe2;
        pchMessageStart[1] = 0x48;
        pchMessageStart[2] = 0x6e;
        pchMessageStart[3] = 0x45;
        nDefaultPort = 18338;
        nPruneAfterHeight = 1000;
        const size_t N = 200, K = 9;  // Same as mainnet.
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;

        genesis = CreateGenesisBlock(1525839537, 2, 0x1d00ffff, 1, 12500 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash(consensus);
        assert(consensus.hashGenesisBlock == uint256S("c93e841bb756a762cda11705b8b3f2ccd9339f11dc1805810fc9cbc86e4e0340"));
        assert(genesis.hashMerkleRoot == uint256S("0x4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));
        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top

        vSeeds.emplace_back("eu-test-dnsseed.bitcoingold-official.org", true);
        vSeeds.emplace_back("test-dnsseed.bitcoingold.org", true);
        vSeeds.emplace_back("test-dnsseed.btcgpu.org", true);

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;


        checkpointData = (CCheckpointData) {
            {
                {0, uint256S("0x00000000e0781ebe24b91eedc293adfea2f557b53ec379e78959de3853e6f9f6")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.BTGHeight = 2000;
        consensus.BTGZawyLWMAHeight = -1; // Activated on regtest
        consensus.BTGPremineWindow = 10;
        consensus.BTGPremineEnforceWhitelist = false;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimitStart = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimitLegacy = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

        consensus.nDigishieldAveragingWindow = 30;
        consensus.nDigishieldMaxAdjustDown = 32;
        consensus.nDigishieldMaxAdjustUp = 16;

        consensus.nZawyLwmaAveragingWindow = 45;
        consensus.nZawyLwmaAjustedWeight = 13632;
        consensus.BTGMaxFutureBlockTime = 7 * 10 * 60; // 70 mins

        consensus.nPowTargetTimespanLegacy = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 10 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");
        
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;

        nDefaultPort = 18444;
        nPruneAfterHeight = 1000;
        const size_t N = 48, K = 5;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;

//        genesis = CreateGenesisBlock(1525839537, 2, 0x207fffff, 1, 12500 * COIN);
//        consensus.hashGenesisBlock = genesis.GetHash(consensus);
//        assert(consensus.hashGenesisBlock == uint256S("0x0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206"));
//        assert(genesis.hashMerkleRoot == uint256S("583bb4261f33dc1c04f31125c8d43ef259c0a7a86f57560b0df0501369e72c41"));

        genesis = CreateGenesisBlock(1231006505, 2083236893, 0x1d00ffff, 1, 12500 * COIN);

        consensus.hashGenesisBlock = genesis.GetHash(consensus);
        printf("genesis.GetHash = %s\n", genesis.GetHash(consensus).ToString().c_str());
        printf("genesis.hashMerkleRoot = %s\n", genesis.GetBlockHeader().hashMerkleRoot.ToString().c_str());
        assert(consensus.hashGenesisBlock == uint256S("855b134ecf981b350ae44a55d711970a0ffd400572babd62b265919925c65076"));
        assert(genesis.hashMerkleRoot == uint256S("09f57ee53501cecb09edc4f51397215c68657edb730da3fbab5bcd7e85112a7e"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData) {
            {
                {0, uint256S("0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206")},
            }
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
    }
    
};

class BitcoinAddressChainParam : public CMainParams
{
public:
    BitcoinAddressChainParam()
    {
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,0);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,5);
    }
};

static std::unique_ptr<CChainParams> globalChainParams;
static BitcoinAddressChainParam chainParamsForAddressConversion;

const CChainParams &Params()
{
    assert(globalChainParams);
    return *globalChainParams;
}

const CChainParams &BitcoinAddressFormatParams()
{
    return chainParamsForAddressConversion;
}

std::unique_ptr<CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams());
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    globalChainParams->UpdateVersionBitsParameters(d, nStartTime, nTimeout);
}


static CScript CltvMultiSigScript(const std::vector<std::string>& pubkeys, uint32_t lock_time) {
    assert(pubkeys.size() == 6);
    CScript redeem_script;
    if (lock_time > 0) {
        redeem_script << lock_time << OP_CHECKLOCKTIMEVERIFY << OP_DROP;
    }
    redeem_script << 4;
    for (const std::string& pubkey : pubkeys) {
        redeem_script << ToByteVector(ParseHex(pubkey));
    }
    redeem_script << 6 << OP_CHECKMULTISIG;
    return redeem_script;
}

bool CChainParams::IsPremineAddressScript(const CScript& scriptPubKey, uint32_t height) const {
    static const int LOCK_TIME = 3 * 365 * 24 * 3600;  // 3 years
    static const int LOCK_STAGES = 3 * 12;  // Every month for 3 years
    assert((uint32_t)consensus.BTGHeight <= height &&
           height < (uint32_t)(consensus.BTGHeight + consensus.BTGPremineWindow));
    int block = height - consensus.BTGHeight;
    int num_unlocked = consensus.BTGPremineWindow * 40 / 100;  // 40% unlocked.
    int num_locked = consensus.BTGPremineWindow - num_unlocked;  // 60% time-locked.
    int stage_lock_time = LOCK_TIME / LOCK_STAGES / consensus.nPowTargetSpacing;
    int stage_block_height = num_locked / LOCK_STAGES;
    const std::vector<std::string> pubkeys = vPreminePubkeys[block % vPreminePubkeys.size()];  // Round robin.
    CScript redeem_script;
    if (block < num_unlocked) {
        redeem_script = CltvMultiSigScript(pubkeys, 0);
    } else {
        int locked_block = block - num_unlocked;
        int stage = locked_block / stage_block_height;
        int lock_time = consensus.BTGHeight + stage_lock_time * (1 + stage);
        redeem_script = CltvMultiSigScript(pubkeys, lock_time);
    }
    CScript target_scriptPubkey = GetScriptForDestination(CScriptID(redeem_script));
    return scriptPubKey == target_scriptPubkey;
}
