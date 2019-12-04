## Create a new key pair and save it to your wallet
cleos create key --to-console
cleos wallet create --file default
cleos wallet import

## Create a new Testnet account
https://monitor.jungletestnet.io/#account
https://mon-test.telosfoundation.io/account-creation
https://app.telos.net/testnet/developers
maybe through sqrl?

## Get some system tokens
http://monitor.jungletestnet.io/#faucet
https://mon-test.telosfoundation.io/faucet
https://app.telos.net/testnet/developers
maybe through sqrl?

## Check it out on the blockchain
https://eosauthority.com/account/hello5world1?network=jungle
https://eosauthority.com/account/hello5world1?network=telostest

## Get some network resources
cleos --url https://api.jungle.alohaeos.com system buyram hello5world1 hello5world1 "30.0000 EOS"
cleos --url https://api.jungle.alohaeos.com system delegatebw hello5world1 hello5world1 "30.0000 EOS" "10.0000 EOS"

## Check it out on the blockchain
https://eosauthority.com/account/hello5world1?network=jungle

## Deploy contract to the blockchain
cleos --url https://api.jungle.alohaeos.com set code hello5world1 helloworld11.wasm

## Check it out on the blockchain
https://eosauthority.com/account/hello5world1?network=jungle&mode=contract&sub=actions