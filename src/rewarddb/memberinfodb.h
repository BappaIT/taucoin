#ifndef TAUCOIN_MEMBERINFODB_H
#define TAUCOIN_MEMBERINFODB_H

#include "coins.h"
#include "coinsbyscript.h"
#include "dbwrapper.h"
#include "chain.h"
#include "base58.h"
#include "leveldb/db.h"
#include "clubinfodb.h"
#include <stdio.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

extern CCriticalSection cs_memberinfo;

/** View on the reward balance dataset. */
#define MEMBERINFODBPATH "/memberinfo"
class CMemberInfoDB
{
private:
    //! the database itself
    leveldb::DB* pdb;

    //! database options used
    leveldb::Options options;

    //! cache for multi-transaction balance updating
    std::map<std::string, std::string> cacheRecord;

    //! cache for accelerating
    std::map<std::string, std::string> cacheForRead;

    //! clubinfo database used
    CClubInfoDB* _pclubinfodb;

    //! Current updated height
    int currentHeight;

    bool RewardChangeUpdateByPubkey(CAmount rewardChange, std::string pubKey, int nHeight, bool isUndo);

    bool RewardChangeUpdate(CAmount rewardChange, std::string address, int nHeight, bool isUndo);

    bool EntrustByAddress(std::string inputAddr, std::string voutAddress, int nHeight, bool isUndo);

    bool UpdateMpAndTmpByAddress(std::string address, int nHeight, std::string father, bool isUndo);

    bool GetBestFather(const CTransaction& tx, const CCoinsViewCache &view, std::string& bestFather,
                       std::map<std::string, CAmount> vin_val=std::map<std::string, CAmount>(), bool isUndo=false);

    bool WriteDB(std::string key, int nHeight, std::string packer, std::string father,
                 uint64_t mp, uint64_t tmp, CAmount value);

    bool WriteDB(std::string key, int nHeight, std::string strValue);

    bool ReadDB(std::string key, int nHeight, std::string& packer, std::string& father,
                uint64_t& mp, uint64_t& tmp, CAmount& value, bool dbOnly=false);
    bool ReadDB(std::string key, int nHeight, std::string& strValue);

    bool DeleteDB(std::string key, int nHeight);

    bool UpdateCacheFather(std::string address, int inputHeight, std::string newFather, bool isUndo);

    bool UpdateCachePacker(std::string address, int inputHeight, std::string newPacker, bool isUndo);

    bool UpdateCacheFatherAndPacker(std::string address, int inputHeight, std::string newAddr, bool isUndo);

    bool UpdateCacheMpAddOne(std::string address, int inputHeight, bool isUndo);

    bool UpdateCacheTmpByChange(std::string address, int nHeight, uint64_t count, bool isAdd, bool isUndo);

    bool UpdateCacheRewardChange(std::string address, int inputHeight, CAmount rewardChange, bool isUndo);

public:
    //! Constructor
    CMemberInfoDB(CClubInfoDB* pclubinfodb);

    //! As we use CBalanceViews polymorphically, have a destructor
    ~CMemberInfoDB();

    //! Clear the rwdbalance cache
    void ClearCache();

    //! Clear the rwdbalance accelerating cache
    void ClearReadCache();

    //! Commit the database transaction
    bool Commit(int nHeight);

    //! Set current updated height
    void SetCurrentHeight(int nHeight);

    //! Get current updated height
    int GetCurrentHeight() const;

    //! Init the father and mp of the address from genesis block
    bool InitGenesisDB(std::vector<std::string> addresses);

    //! Init the distribution of the reward and check if everything is ok
    bool InitRewardsDist(CAmount memberTotalRewards, const CScript& scriptPubKey, int nHeight, std::string& clubLeaderAddress,
                         CAmount& distributedRewards, std::map<std::string, CAmount>& memberRewards);

    //! Compute the reward of each member
    bool ComputeMemberReward(const uint64_t& txCnt, const uint64_t& totalTXCnt,
                             const CAmount& totalRewards, CAmount& memberReward) const;

    //! Parse the record
    bool ParseRecord(std::string inputStr, std::string &packer, std::string& father,
                     uint64_t& mp, uint64_t &tmp, CAmount& value) const;

    //! Generate a record
    bool GenerateRecord(std::string packer, std::string father, uint64_t mp, uint64_t tmp,
                        CAmount value, std::string& outputStr) const;

    //! Retrieve the packer for a given address
    std::string GetPacker(std::string address, int nHeight);

    //! Retrieve the father for a given address
    std::string GetFather(std::string address, int nHeight);

    //! Retrieve the mining power for a given address
    uint64_t GetTXCnt(std::string address, int nHeight);

    //! Retrieve the total mining power of the whole club where a given address in
    uint64_t GetTotalTXCnt(std::string address, int nHeight);

    //! Retrieve the reward balance for a given address
    CAmount GetRwdBalance(std::string address, int nHeight);

    //! Retrieve a full record for a given address
    void GetFullRecord(std::string address, int nHeight, std::string &packer, std::string& father,
                       uint64_t& mp, uint64_t &tmp, CAmount& value, bool dbOnly=false);
    std::string GetFullRecord(std::string address, int nHeight);

    //! Retrieve the harvest power for a given address if it's a miner
    uint64_t GetHarvestPowerByAddress(std::string address, int nHeight);

    //! Update the Balance dataset
    bool UpdateRewardsByTX(const CTransaction& tx, CAmount blockReward, int nHeight, bool isUndo);

    //! Update the mining power and the father
    bool UpdateFatherAndMpByTX(const CTransaction& tx, const CCoinsViewCache &view, int nHeight,
                               std::map<std::string, CAmount> vin_val=std::map<std::string, CAmount>(), bool isUndo=false);

    //! Update the club leader's distribution rate
    bool RewardRateUpdate(CAmount blockReward, CAmount distributedRewards, std::string clubLeaderAddress, int nHeight);
};

#endif // TAUCOIN_MEMBERINFODB_H
