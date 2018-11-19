#include "brandedtoken.hpp"

namespace eosio {
void brandedtoken::create( account_name issuer,symbol_type symbolo )
{
    require_auth(_self);

    eosio_assert( symbolo.is_valid(), "invalid symbol name" );

    stats statstable( _self, symbolo.name() );
    auto existing = statstable.find( symbolo.name() );
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    asset newquantity = asset{1,symbolo};

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = symbolo;
       s.max_supply    = newquantity;
       s.issuer        = issuer;
    });
}


void brandedtoken::issue( account_name to, asset quantity, string memo )
{
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto sym_name = sym.name();
    stats statstable( _self, sym_name );
    auto existing = statstable.find( sym_name );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
    const auto& st = *existing;

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify( st, 0, [&]( auto& s ) {
       s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );

    if( to != st.issuer ) {
       SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
    }
}

void brandedtoken::retire( asset quantity, string memo )
{
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto sym_name = sym.name();
    stats statstable( _self, sym_name );
    auto existing = statstable.find( sym_name );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist" );
    const auto& st = *existing;

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must retire positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );

    statstable.modify( st, 0, [&]( auto& s ) {
       s.supply -= quantity;
    });

    sub_balance( st.issuer, quantity );
}

void brandedtoken::transfer( account_name from,
                      account_name to,
                      asset        quantity,
                      string       memo )
{
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );
    eosio_assert( is_account( to ), "to account does not exist");
    auto sym = quantity.symbol.name();
    stats statstable( _self, sym );
    const auto& st = statstable.get( sym );

    require_recipient( from );
    require_recipient( to );

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

void brandedtoken::sub_balance( account_name owner, asset value ) {
   accounts from_acnts( _self, owner );

   const auto& from = from_acnts.get( value.symbol.name(), "no balance object found" );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

   from_acnts.modify( from, owner, [&]( auto& a ) {
         a.balance -= value;
      });
}

void brandedtoken::add_balance( account_name owner, asset value, account_name ram_payer )
{
   accounts to_acnts( _self, owner );
   auto to = to_acnts.find( value.symbol.name() );
   if( to == to_acnts.end() ) {
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
      });
   } else {
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

void brandedtoken::open( account_name owner, symbol_type symbol, account_name ram_payer )
{
   require_auth( ram_payer );
   accounts acnts( _self, owner );
   auto it = acnts.find( symbol.name() );
   if( it == acnts.end() ) {
      acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = asset{0, symbol};
      });
   }
}

void brandedtoken::close( account_name owner, symbol_type symbol )
{
   require_auth( owner );
   accounts acnts( _self, owner );
   auto it = acnts.find( symbol.name() );
   eosio_assert( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
   eosio_assert( it->balance.amount == 0, "Cannot close because the balance is not zero." );
   acnts.erase( it );
}

// increase max supply
void brandedtoken::addsupply(asset quantity, string memo){
    require_auth(N(tapatalktpx1));

    auto sym = quantity.symbol;

    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto sym_name = sym.name();
    stats statstable( _self, sym_name );
    auto existing = statstable.find( sym_name );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist" );
    const auto& st = *existing;

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must add positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    
    statstable.modify( st, 0, [&]( auto& s ) {
        s.max_supply += quantity;
    });

}

// reduce max supply
void brandedtoken::subsupply(asset quantity, string memo){
  require_auth(N(tapatalktpx1));

  auto sym = quantity.symbol;

  eosio_assert( sym.is_valid(), "invalid symbol name" );
  eosio_assert( memo.size() <= 256, "invalid symbol name" );

  auto sym_name = sym.name();
  stats statstable(_self, sym_name );
  auto existing = statstable.find( sym_name );
  eosio_assert( existing != statstable.end(), "token with symbol does not exist" );
  const auto& st = *existing;

  eosio_assert( quantity.is_valid(), "invalid quantity" );
  eosio_assert( quantity.amount > 0, "must add positive quantity" );
  eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
  eosio_assert( (st.max_supply.amount - quantity.amount) >= st.supply.amount, "after reducing, max supply must be greater or equal to supply" );

  statstable.modify( st, 0, [&]( auto& s ) {
      s.max_supply -= quantity;
  });

}

void brandedtoken::depbtoken(account_name btoken_from, account_name lgid_to, asset quantity) {
  require_auth( btoken_from );
  
  //Search for token balance table for ledger account existing 
  btokenbals btokenbls( _self, quantity.symbol.name() );

  auto depositlgid = btokenbls.find( lgid_to );
  if( depositlgid == btokenbls.end() ) {
    //Assertion. Trying to deposit to a non-existing ledger account
    eosio_assert( false, "ledger ID doesn't exist" );
  } else {

    //Transfer token to self contract as deposit
    transfer(btoken_from, _self, quantity, "deposit");

    //Update the deposit on token balance table
    btokenbls.modify( depositlgid, 0, [&]( auto& a ) {
      a.balance += quantity;
    });
  }
}

void brandedtoken::wdrbtoken(account_name lgid_from, account_name btoken_to, asset quantity) {
  require_auth( _self );

  //Search for token balance table for ledger account existing 
  btokenbals btokenbls( _self, quantity.symbol.name() );

  auto withdrawlgid = btokenbls.find( lgid_from );
  if( withdrawlgid == btokenbls.end() ) {
    //Assertion. Trying to withdraw from a non-existing ledger account
    eosio_assert( false, "ledger ID doesn't exist" );
  } else {
    //Update the withdraw on token balance table
    btokenbls.modify( withdrawlgid, 0, [&]( auto& a ) {
      eosio_assert( a.balance.amount >= quantity.amount, "overdrawn balance" );
      a.balance -= quantity;
    });

    //Transfer token from self contract to btoken_to EOS account
    transfer(_self, btoken_to, quantity, "withdraw");
  }

}


void brandedtoken::trfbtoken(account_name lgid_from, account_name lgid_to, asset quantity) {
  require_auth( _self );

  //Search for token balance table for ledger account existing 
  btokenbals btokenbls( _self, quantity.symbol.name() );

  auto sublgid = btokenbls.find( lgid_from );
  if( sublgid == btokenbls.end() ) {
    //Assertion. Trying to sub from a non-existing ledger account
    eosio_assert( false, "ledger ID doesn't exist" );
  } else {
    //Search for token balance table for receive ledger account existing 
    auto addlgid = btokenbls.find( lgid_to );
    if( addlgid == btokenbls.end() ) {
      //Assertion. Trying to add to a non-existing ledger account
      eosio_assert( false, "ledger ID doesn't exist" );
    } else {
      //Sub from ledger account on token balance table
      btokenbls.modify( sublgid, 0, [&]( auto& a ) {
        eosio_assert( a.balance.amount >= quantity.amount, "overdrawn balance" );
        a.balance -= quantity;
      });

      //Then add the balance
      btokenbls.modify( addlgid, 0, [&]( auto& a ) {
        a.balance += quantity;
      });
    }

  }
}

void brandedtoken::createlgid(account_name lgid, symbol_type symbolo) {
  require_auth( _self );
  
  //Search for gdp balance table for ledger account existing 
  btokenbals btokenbls( _self, symbolo.name() );

  auto newlgid = btokenbls.find( lgid );
  if( newlgid == btokenbls.end() ) {
    //If account does not existing, create new ledger account
    asset emptyasset = asset(0, symbolo);
    btokenbls.emplace( _self, [&]( auto& a ){
      a.balance = emptyasset;
      a.lgid = lgid;
    });
  } else {
    //Assertion. Trying to create redundant ledger account
    eosio_assert( false, "ledger ID already exists" );
  }
}

} /// namespace eosio

EOSIO_ABI( eosio::brandedtoken, (create)(issue)(transfer)(open)(close)(retire)(addsupply)(subsupply)(depbtoken)(wdrbtoken)(trfbtoken)(createlgid))
