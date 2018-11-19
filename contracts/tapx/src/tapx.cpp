/**
 *  tapx.cpp
 *  copyright TAPx.io
 */

#include "tapx.hpp"

namespace eosio {

void tapx::create( account_name issuer, asset maximum_supply )
{
    require_auth( _self );

    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable( _self, sym.name() );
    auto existing = statstable.find( sym.name() );
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}


void tapx::issue(account_name to, asset quantity, string memo )
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

void tapx::retire( asset quantity, string memo )
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

void tapx::transfer( account_name from,
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

void tapx::sub_balance( account_name owner, asset value ) {
   accounts from_acnts( _self, owner );

   const auto& from = from_acnts.get( value.symbol.name(), "no balance object found" );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

   from_acnts.modify( from, owner, [&]( auto& a ) {
         a.balance -= value;
      });
}

void tapx::add_balance( account_name owner, asset value, account_name ram_payer )
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

void tapx::open( account_name owner, symbol_type symbol, account_name ram_payer )
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

void tapx::close( account_name owner, symbol_type symbol ) {
   accounts acnts( _self, owner );
   auto it = acnts.find( symbol.name() );
   eosio_assert( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
   eosio_assert( it->balance.amount == 0, "Cannot close because the balance is not zero." );
   acnts.erase( it );
}

void tapx::createlgid(account_name ledger_id) {
  require_auth( _self );
  
  asset TAPxAsset = asset(0, S(4, TAP));

  //Search for tap balance table for ledger account existing 
  tapbalances ttbls( _self, TAPxAsset.symbol.name() );

  auto newledgerid = ttbls.find( ledger_id );
  if( newledgerid == ttbls.end() ) {
    //If account does not existing, create new ledger account
    ttbls.emplace( _self, [&]( auto& a ){
      a.balance = TAPxAsset;
      a.ledger_id = ledger_id;
    });
  } else {
    //Assertion. Trying to create redundant ledger account
    eosio_assert( false, "ledger ID already exists" );
  }
}

void tapx::depledger(account_name tapx_from, account_name ledger_to , asset quantity) {
  require_auth( tapx_from );
  
  asset TAPxAsset = asset(0, S(4, TAP));
  eosio_assert( quantity.symbol == TAPxAsset.symbol, "symbol precision mismatch" );

  //Search for tap balance table for ledger account existing 
  tapbalances ttbls( _self, TAPxAsset.symbol.name() );

  auto depositledgerid = ttbls.find( ledger_to );
  if( depositledgerid == ttbls.end() ) {
    //Assertion. Trying to deposit to a non-existing ledger account
    eosio_assert( false, "ledger ID doesn't exist" );
  } else {
    //Transfer tapx to self contract as deposit
    transfer(tapx_from, _self, quantity, "deposit tapx");

    //Update the deposit on tap balance table
    ttbls.modify( depositledgerid, 0, [&]( auto& a ) {
      a.balance += quantity;
    });
  }
}

void tapx::wdrledger(account_name ledger_from, account_name tapx_to, asset quantity) {
  require_auth( _self );

  asset TAPxAsset = asset(0, S(4, TAP));
  eosio_assert( quantity.symbol == TAPxAsset.symbol, "symbol precision mismatch" );

  //Search for tap balance table for ledger ID account existing 
  tapbalances ttbls( _self, TAPxAsset.symbol.name() );

  auto withdrawledgerid = ttbls.find( ledger_from );
  if( withdrawledgerid == ttbls.end() ) {
    //Assertion. Trying to withdraw from a non-existing ledger ID account
    eosio_assert( false, "ledger ID doesn't exist" );
  } else {
    //Update the withdraw on tap balance table
    ttbls.modify( withdrawledgerid, 0, [&]( auto& a ) {
      eosio_assert( a.balance.amount >= quantity.amount, "overdrawn balance" );
      a.balance -= quantity;
    });

    //Transfer tapx from self contract to tap_to EOS account
    transfer(_self, tapx_to, quantity, "withdraw tapx");
  }
}

void tapx::trfledger( account_name ledger_from, account_name ledger_to, asset quantity) {
  require_auth( _self );

  asset TAPxAsset = asset(0, S(4, TAP));
  eosio_assert( quantity.symbol == TAPxAsset.symbol, "symbol precision mismatch" );

  //Search for tap balance table for ledger ID account existing 
  tapbalances ttbls( _self, TAPxAsset.symbol.name() );

  auto subledgerid = ttbls.find( ledger_from );
  if( subledgerid == ttbls.end() ) {
    //Assertion. Trying to sub from a non-existing ledger ID account
    eosio_assert( false, "ledger from ID doesn't exist" );
  } else {
    //Search for tap balance table for ledger to account existing 
    auto addledgerid = ttbls.find( ledger_to );
    if( addledgerid == ttbls.end() ) {
      //Assertion. Trying to deposit to a non-existing ledger to account
      eosio_assert( false, "ledger to account doesn't exist" );
    } else {
      //Sub from subledgerid on tap balance table
      ttbls.modify( subledgerid, 0, [&]( auto& a ) {
        eosio_assert( a.balance.amount >= quantity.amount, "overdrawn balance" );
        a.balance -= quantity;
      });

      //Then add the balance
      ttbls.modify( addledgerid, 0, [&]( auto& a ) {
        a.balance += quantity;
      });
    }
  }
}

//create brand token by TAPx
void tapx::stake(account_name account, asset quantity, symbol_type symbolo) {
    require_auth( account );
    //brand token user account
    eosio_assert( is_account( account ), "brand token user account does not exist" );
    eosio_assert( symbolo.is_valid(), "invalid symbol name" );
    //transfer TAPx to this contract
    trf_tapx(account,_self, quantity, "stake" );
    //increate the support of brand token
    asset newquantitybt = asset{quantity.amount * 10,symbolo};
    add_supply(account,newquantitybt,"stake" );
}

//exchange to TAPx with brand token
void tapx::unstake(account_name account, asset quantity, symbol_type symbolo) {
    require_auth( account );
    //brand token user account
    eosio_assert( is_account( account ), "brand token user account does not exist" );
    eosio_assert( symbolo.is_valid(), "invalid symbol name" );
    //unstake the support of brand token
    sub_supply(account,quantity,"unstake" );
    //transfer TAPx to user's address
    asset newquantitybt = asset{quantity.amount / 10,symbolo};
    trf_tapx(_self,account, newquantitybt, "unstake" );
}


} /// namespace eosio

EOSIO_ABI( eosio::tapx, (create)(issue)(transfer)(open)(close)(retire)(depledger)(wdrledger)(trfledger)(stake)(unstake)(createlgid))
