ScatterJS.plugins( new ScatterEOS() )
let ScatJS = ScatterJS, ScatEOS = new ScatterEOS();
const JsonRpc = eosjs_jsonrpc.JsonRpc;
const Api = eosjs_api.Api;

let account; // account connected with scatter
let eos; // initialized eosjs.Api
const CONTRACT_ACCOUNT = "hello5world1";

const network = ScatterJS.Network.fromJson({
    blockchain:'eos',
    chainId:'e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473',
    host:'api.jungle.alohaeos.com',
    port:443,
    protocol:'https'
});

const rpc = new JsonRpc(network.fullhost());

async function login() {
    const connected = await ScatterJS.connect('Commit reveal app', {network})
    if(!connected) return displayError("Log in with Scatter to use this app");

    eos = ScatterJS.eos(network, Api, {rpc});

    const id = await ScatterJS.login()
    if(!id) return displayError('No account could be found');
    account = ScatterJS.account('eos');
}

const transact = async function(action, data, successMsg) {
    try {
        const result = await eos.transact({
            actions: [{
                account: CONTRACT_ACCOUNT,
                name: action,
                authorization: [{
                    actor: account.name,
                    permission: account.authority,
                }],
                data: data,
            }]
        }, {
            blocksBehind: 3,
            expireSeconds: 30,
        })
        const txId = result.transaction_id;
        const url = "https://eosauthority.com/transaction/" + txId + "?network=jungle";
        const alertHtml = "See transaction <a href='"+url+"'>here</a>.";
        displaySuccess("<b>" + successMsg + "</b> "+ alertHtml);

    } catch (e) {
        console.error(e);
        let message = 'Caught exception: ' + JSON.stringify(e.json, null, 2);
        displayError(message)
    }
}

let walletContract = {}

walletContract.commit = async function (message_hash) {
    await transact("commit", {
        from: account.name,
        message_hash: message_hash
    }, "The hash has been added/updated in the table!")
}

walletContract.reveal = async function (accountName, message) {
    await transact("reveal", {
        from: accountName,
        message: message
    }, "The message matched the hash and removed the row from the table!")
}

walletContract.getMessages = async function() {
    const args = {
        code: CONTRACT_ACCOUNT,
        table: "messages",
        scope: CONTRACT_ACCOUNT,
        lower_bound: account.name
    }
    return await rpc.get_table_rows(args);
}