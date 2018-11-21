# tapx.io-mvp
The TAPx Project is a protocol that will build a blockchain infrastructure to unite 1 million online communities. Our goal is to build a standard, public online community blockchain protocol.

TAPx blockchain protocol will connect online communities and their users. It helps users to create a cross platform identity and also allow users to have their reputations recognized across individual online communities. It also helps online communities to construct their own branded forum token and incentive mechanisms to encourage community activities to increase engagement.

TAPx MVP is built on EOS.  It’s modularized and can be modified to be deployed on other public blockchain very quickly.

We’re publishing components of TAPx as they evolve and mature. We’re still building the MVP, so significant sections of code won’t immediately be published here. We plan to add code to this public repo as new components are tested and deployed.

### TAPx MVP Project Structure
  1. **TAPx-core** is the base contract of branded online community tokens. Based on EOS standard token module, we developed Exchange and Ledger to make individual community economy possible. TAPx can be used to trade-in for branded token.
  2. **Stake** converts the TAPx into the community’s branded token supply according to an exchange rate.
  3. **Ledger** allows users without wallets to transition from regular community account to blockchain account easily. And helps to avoid potential account creation cost.
  4. **Branded Token** carries community owner’s branding. Community owners can claim branded tokens by staking TAPx tokens.

### We created Goldpoint as an sample branded token.

Goldpoint is sample branded token that can be used  Tapatalk Groups communities.  It can be used to purchase Tapatalk services or gifting to other Tapatalk Groups users (tipping) or community Admins/Mods. The supply is determined by staking/unstaking TAPx on the exchange. All Tapatalk users can use this token, regardless of whether or not they have blockchain wallet.

The Goldpoint sample is running on EOS testnet:    
  - TPAx smartcontract: https://jungle.bloks.io/account/tapatalktpx1   
  - Goldpoint smartcontrat: https://jungle.bloks.io/account/tapatalkgdp1    

### Resources
  **Website:**     https://www.tapx.io    
  **Whitepaper:**  https://www.tapx.io/WhitePaper_v2.0.pdf   
  **Branded token API:**  https://github.com/tapxio/tapx.io-mvp/wiki/Branded-Token-API-index
