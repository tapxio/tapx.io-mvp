#include "tapxdgoods.hpp"

using namespace eosio;
using std::string;

void tapxdgoods::create(name issuer, name token_name, bool fungible, bool
      burnable, bool transferable, uint64_t max_supply) 
{
	 require_auth( _self );

	 auto nft_dasset = dasset{ max_supply, 0 };

	 tokenstats_index tokenstats_table(_self,token_name.value);
	 auto existing = tokenstats_table.find(token_name.value);
	 check( existing == tokenstats_table.end(), "nft allread eixts"); 

	 tokenstats_table.emplace( _self, [&]( auto& row ) {
	 	row.fungible = fungible;
	 	row.burnable = burnable;
	 	row.transferable = transferable;
	 	row.issuer = issuer;
	 	row.token_name = token_name;
	 	row.max_supply = nft_dasset;
	 });
}

/**
*  uint64_t quantity = 1
**/
void tapxdgoods::issue(name to, name token_name, string metadata_type, 
     string metadata_uri, string memo) {
	 tokenstats_index tokenstats_table(_self,token_name.value);
	 auto existing = tokenstats_table.find(token_name.value);
	 check( existing != tokenstats_table.end(), "token with symbol does not exist, create token before issue"); 

	 const auto& ts = *existing;
	 require_auth(ts.issuer);

	 check( 1 <= ts.max_supply.amount - ts.current_supply,  "quantity exceeds available supply");

	 tokenstats_table.modify( ts, same_payer,[&]( auto& row ) {
		row.current_supply += 1;
	 });

	 owner_index tokeninfo_table(_self,_self.value);

	 tokeninfo_table.emplace( _self, [&]( auto& row ) {
	 	row.serial_number = tokeninfo_table.available_primary_key();
	 	row.owner = to;
	 	row.token_name = token_name;
	 	row.metadata_type = metadata_type;
	 	row.metadata_uri = metadata_uri;
	 });
}


void tapxdgoods::burnnft(name owner, vector<uint64_t> tokeninfo_ids) {
	require_auth(owner);

	for (vector<uint64_t>::const_iterator iter = tokeninfo_ids.cbegin(); iter != tokeninfo_ids.cend(); iter++)
	{
		owner_index owner_account(_self,_self.value);
		auto owner_nft = owner_account.find(*iter);
		check( owner_nft != owner_account.end(), "nft didn't eixts"); 
		check( owner_nft-> owner == owner, "owner doses not own token with specified ID");

		owner_account.erase(owner_nft); 
	}
}

void tapxdgoods::transfernft(name from, name to, vector<uint64_t> tokeninfo_ids, string memo) {
	check( from != to , "cannot transfer to self" );
	require_auth( from );
	check( is_account( to ) , "to account does not exist");

	require_recipient( from );
	require_recipient( to );

	check( memo.size() <= 256, "memo has more than 256 bytes" );

	auto payer = has_auth( to ) ? to : from;
	for (vector<uint64_t>::const_iterator iter = tokeninfo_ids.cbegin(); iter != tokeninfo_ids.cend(); iter++)
	{
		owner_index from_account(_self,_self.value);
		auto send_nft = from_account.find(*iter);
		// check( false, *iter);
		check( send_nft != from_account.end(), "nft didn't eixts"); 
		check( send_nft-> owner == from, "sender doses not own token with specified ID");

		const auto& ts = *send_nft;
		from_account.modify( ts, same_payer,[&]( auto& row ) {
			row.owner = to;
		});
	}
}

EOSIO_DISPATCH(tapxdgoods, (create)(issue)(burnnft)(transfernft))