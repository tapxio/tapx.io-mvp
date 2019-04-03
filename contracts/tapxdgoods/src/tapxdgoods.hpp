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


class [[eosio::contract]] tapxdgoods : public contract {
  public:
  	using contract::contract;

    [[eosio::action]]
     void create(name issuer,name token_name, bool fungible, bool
          burnable, bool transferable, uint64_t max_supply);

    [[eosio::action]]
    void issue(name to, name token_name, string metadata_type, 
         string metadata_uri, string memo);

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

	struct [[eosio::table]] tokeninfo {
	    uint64_t serial_number;
	    name owner;
	    name token_name;
	    string metadata_type;
	    string metadata_uri;
	    
	    uint64_t primary_key() const { return serial_number; }
	    uint64_t get_owner() const { return owner.value; }
	};
	typedef eosio::multi_index<"tokeninfo"_n,tokeninfo, indexed_by<"byowner"_n, const_mem_fun<tokeninfo, uint64_t, &tokeninfo::get_owner>>> owner_index; 
  
};

