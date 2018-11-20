# tapx.io-mvp
TAPx Project is a protocol, which will build a foundation to unit 1 million online communities.

Our goal is to build a standard, public online community blockchain protocol.

TAPx is a blockchain protocol connects online communities and their users. It helps the users to build cross platform identities so their reputations are recognized across individual online community. It also helps online communities to construct their own forum economy and incentive mechanisms to encourage community activities and create more value.

TAPx MVP is built on EOS but it’s modularized and can be deployed onto other public blockchain very quickly.

We’re publishing components of TAPx  as they evolve and mature. We’re still building the MVP, so significant sections of code won’t immediately be published here. We plan to add code to this public repo as new components are tested and deployed.

### TAPx MVP Project Structure
  1. **TAPx-core** is the base contract of branded online community tokens. Based on EOS standard token module, we developed Exchange and Ledger to make individual community economy possible. TAPx can be used to trade-in for branded token.
  2. **Stake** converts the TAPx into the community’s branded token supply according to an exchange rate.
  3. **Ledger** allows users without wallets to transition from regular community account to blockchain account easily. And helps to avoid potential account creation cost.
  4. **Branded Token** carries community owners’ brandings. Community owner can claim branded token by staking TAPx token.

### We created Goldpoint as an sample branded token.

Goldpoint is the token that can be used among all Tapatalk communities. It can be used to purchase Tapatalk services or gifting to others. The supply is determined by staking/unstaking TAPx on the exchange. All Tapatalk users can use this token, regardless of whether they have blockchain wallet or not.

The Goldpoint sample is running on EOS testnet:    
  - TPAx smartcontract: https://jungle.bloks.io/account/tapatalktpx1   
  - Goldpoint smartcontrat: https://jungle.bloks.io/account/tapatalkgdp1    

### Resources
  **Website:**    https://www.tapx.io    
  **whitepaper**  https://www.tapx.io/WhitePaper_v2.0.pdf   
