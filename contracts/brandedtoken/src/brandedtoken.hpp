/**
 *  brandedtoken.hpp
 *  copyright TAPx.io
 */

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;

   class brandedtoken : public contract {
      public:
         brandedtoken( account_name self ):contract(self){}
         /**
         * Standard token contract - create
         *
         * @param account_name  EOS account of creator
         * @param symbol_type   created token symbol, format : "0.0000 XXX"
         **/
         [[eosio::action]]
         void create( account_name issuer,
                      symbol_type symbolo);

         /**
         * Standard token contract - issue
         *
         * @param account_name  issuer EOS account
         * @param asset    token asset info
         * @param memo     memo
         **/
         [[eosio::action]]
         void issue( account_name to, asset quantity, string memo );

         /**
         * Standard token contract - retire
         *
         * @param asset    token asset info
         * @param memo     memo
         **/
         [[eosio::action]]
         void retire( asset quantity, string memo );

         /**
         * Standard token contract - transfer
         *
         * @param from    transfer from EOS account
         * @param to      transfer to EOS account
         * @param asset   token asset info
         * @param memo    memo
         **/
         [[eosio::action]]
         void transfer( account_name from,
                        account_name to,
                        asset        quantity,
                        string       memo );

         /**
         * Standard token contract - open  
         *
         * @param owner   owner EOS account
         * @param symbol  asset symbol
         * @param payer   payer for transaction space cost on EOS
         **/
         [[eosio::action]]
         void open( account_name owner, symbol_type symbol, account_name payer );

         /**
         * Standard token contract - close 
         *
         * @param owner   owner EOS account
         * @param symbol  asset symbol
         **/
         [[eosio::action]]
         void close( account_name owner, symbol_type symbol );

         /**
         * Brand token contract - add supply
         *
         * @param quantity add supply token asset info
         * @param memo     memo
         */
         [[eosio::action]]
         void addsupply(asset quantity, string memo);

         /**
         * Brand token contract - sub supply 
         *
         * @param quantity sub supply token asset info
         * @param memo     memo
         */
         [[eosio::action]]
         void subsupply(asset quantity, string memo);

         /**
         * Deposit brand token from EOS account to ledger account
         *
         * @param btoken_from  EOS account that send brand token
         * @param lgid_to    ledger account that receive brand token
         * @param quantity  deposit asset quantity
         **/
         [[eosio::action]]
         void depbtoken(account_name btoken_from, account_name lgid_to, asset quantity);

         /**
         * Withdraw brand token from ledger account to EOS account
         *
         * @param lgid_from  ledger account that send brand token
         * @param btoken_to  EOS account that receive brand token
         * @param quantity  withdraw asset quantity
         **/
         [[eosio::action]]
         void wdrbtoken(account_name lgid_from, account_name btoken_to, asset quantity);

         /**
         * Transfer brand token between ledger accounts
         *
         * @param lgid_from  ledger account that send brand token
         * @param lgid_to  ledger account that receive brand token
         * @param quantity  transfer asset quantity
         **/
         [[eosio::action]]
         void trfbtoken(account_name lgid_from, account_name lgid_to, asset quantity);

         /**
         * Create new account on ledger
         *
         * @param ledger  new ledger ID
         * @param symbol_type   brand token symbol, format : "0.0000 XXX"
         **/
         [[eosio::action]]
         void createlgid(account_name lgid, symbol_type symbolo);

         inline asset get_supply( symbol_name sym )const;

         inline asset get_balance( account_name owner, symbol_name sym )const;

      private:
         struct [[eosio::table]] account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.name(); }
         };

         struct [[eosio::table]] currency_stats {
            asset          supply;
            asset          max_supply;
            account_name   issuer;

            uint64_t primary_key()const { return supply.symbol.name(); }
         };

         typedef eosio::multi_index<N(accounts), account> accounts;
         typedef eosio::multi_index<N(stat), currency_stats> stats;

         void sub_balance( account_name owner, asset value );
         void add_balance( account_name owner, asset value, account_name ram_payer );

         //ledger brand token balance table
         struct [[eosio::table]] btokenbal {
            account_name    lgid;       // will create a secondary index on this
            asset           balance;

            auto            primary_key()const { return lgid; }
            EOSLIB_SERIALIZE( btokenbal, (lgid)(balance))
         };
         typedef eosio::multi_index<N(btokenbals), btokenbal> btokenbals;

      public:
         struct transfer_args {
            account_name  from;
            account_name  to;
            asset         quantity;
            string        memo;
         };
   };

   asset brandedtoken::get_supply( symbol_name sym )const
   {
      stats statstable( _self, sym );
      const auto& st = statstable.get( sym );
      return st.supply;
   }

   asset brandedtoken::get_balance( account_name owner, symbol_name sym )const
   {
      accounts accountstable( _self, owner );
      const auto& ac = accountstable.get( sym );
      return ac.balance;
   }

} /// namespace eosio
