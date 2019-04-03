/**
 *  tapxdgoods.hpp
 *  copyright TAPx.io
 */
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>
#include <string>
#include <vector>

using namespace eosio;
using std::string;
using std::vector;
typedef uint128_t uuid;

/**
* 第一版，先暂时不实现category，以及 币的版本配置
* 第一版，只实现半同质化货币
**/

class [[eosio::contract]] tapxdgoods : public contract {
  public:
  	using contract::contract;

    [[eosio::action]]
     void create(name issuer,name token_name, bool fungible, bool
          burnable, bool transferable, uint64_t max_supply);

    [[eosio::action]]
    void issue(name to, name token_name, string metadata_type, 
         string metadata_uri, string memo);
    /**
    * 暂停所有代币的所有转账，仅有合约可以调用该函数。如果 pause 参数值为 true，则暂停转账;否则，不暂停。
    **/
  //   [[eosio::action]]
 	// void pausexfer(bool pause);

 	/**
 	* 该函数可用于销毁第三方代币并且释放内存，只有代币的拥有者 (owner) 可以调用该方法，并且 burnable 参数值必须为 true
 	**/
 	[[eosio::action]]
 	void burnnft(name owner, vector<uint64_t> tokeninfo_ids);


 	[[eosio::action]]
 	void transfernft(name from, name to, vector<uint64_t> tokeninfo_ids, string memo);


 private:
 	struct dasset {
	    uint64_t amount;
	    uint8_t  precision; 

	    EOSLIB_SERIALIZE(dasset, (amount)(precision))
	};	



	/**
	*Token 数据表
	*确保只有一对category, token_name对。存储着token是否同质化的，是否可销毁，是否可以进行转账，当前及最大的供应量是多少。token被创建的时候，信息就被写入。由于供应量不会下降，在销毁代币的时候，供应量要记录唯一编码。
	*类似于标准 token 的 currency_stats
	**/
	struct [[eosio::table]] tokenstats {
	    bool     fungible;
	    bool     burnable;
	    bool     transferable;
	    name     issuer;
	    name     token_name;
	    dasset   max_supply;
	    uint64_t current_supply;
	     
	    uint64_t primary_key() const { return token_name.value; }
	};
	typedef eosio::multi_index<"tokenstats"_n, tokenstats> tokenstats_index;

	/**
	*Token 信息表
	*这是非同质化和半同质化token的全局列表。辅助索引提供了按照所有者 (owner) 进行搜索。
	**/
	struct [[eosio::table]] tokeninfo {
	    // uint64_t id;
	    uint64_t serial_number;
	    name owner;
	    name token_name;
	    string metadata_type;
	    string metadata_uri;
	    
	    uint64_t primary_key() const { return serial_number; }
	    uint64_t get_owner() const { return owner.value; }
	};
	// typedef eosio::multi_index<"tokeninfo"_n, tokeninfo> tokeninfo_index;  
	typedef eosio::multi_index<"tokeninfo"_n,tokeninfo, indexed_by<"byowner"_n, const_mem_fun<tokeninfo, uint64_t, &tokeninfo::get_owner>>> owner_index; 
  
};

