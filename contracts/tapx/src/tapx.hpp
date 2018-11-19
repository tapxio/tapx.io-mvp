/**
 *  tapx.hpp
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

   class tapx : public contract {
      public:
         tapx( account_name self ):contract(self){}

         /**
         * Standard token contract - create
         *
         * @param account_name    EOS account of creator
         * @param maximum_supply  created token asset info, format : "0.0000 XXX"
         **/
         [[eosio::action]]
         void create( account_name issuer,
                      asset        maximum_supply);

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
         * Stake TAPx for brand token
         *
         * @param account   EOS account that stake for brand token
         * @param quantity  stake TAPx quantity 
         * @param symbol    brand token symbol, Example 0.0000 GDP
         **/
        [[eosio::action]]
        void stake( account_name account, asset quantity, symbol_type symbol);


        /**
         * Unstake brand token for TAPx
         *
         * @param account     EOS account that unstake for brand token
         * @param quantity    unstake brand token quantity 
         * @param symbol      TAPx symbol, Exmaple: 0.0000 TAP
         **/
        [[eosio::action]]
        void unstake( account_name account, asset quantity, symbol_type symbol);
         
         /**
         * Deposit TAPx from EOS account into ledger account
         *
         * @param tapx_from  EOS account that send TAPx 
         * @param ledger_to  ledger account that receive TAPx
         * @param quantity   TAPx quantity
         **/
         [[eosio::action]]
         void depledger(account_name tapx_from, account_name ledger_to , asset quantity);

         /**
         * Withdraw TAPx from ledger account to EOS account
         *
         * @param ledger_from  ledger account that send TAPx
         * @param tap_to       EOS account that receive TAPx 
         * @param quantity     TAPx quantity
         **/
         [[eosio::action]]
         void wdrledger(account_name ledger_from, account_name tapx_to, asset quantity);

         /**
         * Transfer TAPx between ledger accounts
         *
         * @param lgid_from  ledger account that send TAPx
         * @param lgid_to  ledger account that receive TAPx
         * @param quantity  transfer asset quantity         
         **/
         [[eosio::action]]
         void trfledger( account_name ledger_from, account_name ledger_to, asset quantity);

         /**
         * Create new account on ledger
         *
         * @param ledger  new ledger ID
         **/
         [[eosio::action]]
         void createlgid(account_name ledger_id);

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

         //Ledger TAP balance table
         struct [[eosio::table]] tapbalance {
            account_name    ledger_id;       // will create a secondary index on this
            asset           balance;

            auto            primary_key()const { return ledger_id; }
            EOSLIB_SERIALIZE( tapbalance, (ledger_id)(balance))
         };
         typedef eosio::multi_index<N(tapbalances), tapbalance> tapbalances;

        /**
         * Inline action TAPx transfer
         *
         * @param from    EOS account that send TAPx
         * @param to      EOS account that receive TAPx
         * @param asset   transfer TAPx asset info
         * @param memo    memo
         **/
        void trf_tapx( account_name from, account_name to, asset quantity, string memo ) {
            action(
                permission_level{from,N(active)},
                get_self(),      
                N(transfer),
                std::make_tuple(from, to, quantity,memo)
            ).send();
        }

        /**
         * Inline action add brand token supply
         *
         * @param brandaccount brand token EOS account      
         * @param quantity     TAPx asset info
         * @param memo         memo
         **/
        void add_supply(account_name brandaccount,asset quantity, string memo ) {
            action(
                permission_level{get_self(),N(active)},
                brandaccount,       
                N(addsupply),
                std::make_tuple(quantity,memo)
            ).send();
        }
    
        /**
         * Inline action sub brand token supply
         *
         * @param brandaccount brand token EOS account      
         * @param quantity     TAPx asset info
         * @param memo         memo
         **/
        void sub_supply(account_name brandaccount,asset quantity, string memo ) {
            action(
                permission_level{get_self(),N(active)},
                brandaccount,      
                N(subsupply),
                std::make_tuple(quantity,memo)
            ).send();
        }

      public:
         struct [[eosio::table]] transfer_args {
            account_name  from;
            account_name  to;
            asset         quantity;
            string        memo;
         };
   };

   asset tapx::get_supply( symbol_name sym )const
   {
      stats statstable( _self, sym );
      const auto& st = statstable.get( sym );
      return st.supply;
   }

   asset tapx::get_balance( account_name owner, symbol_name sym )const
   {
      accounts accountstable( _self, owner );
      const auto& ac = accountstable.get( sym );
      return ac.balance;
   }

} /// namespace eosio
