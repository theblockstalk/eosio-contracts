const assert = require('assert');
const eoslime = require('eoslime').init('local');
const eos = eoslime.Provider.eos;
const crypto = require("./crypto");

const CONTRACT_WASM_PATH = '../build/helloworld11.wasm';
const CONTRACT_ABI_PATH = '../build/helloworld11.abi';

describe('helloworld11', function () {
    // Increase mocha(testing framework) time, otherwise tests fails
    this.timeout(15000);

    let contractAccount;
    let myAccount;
    let messagesTable;
    
    before(async () => {
        let accounts = await eoslime.Account.createRandoms(1);
        myAccount = accounts[0];
    });

    beforeEach(async () => {
        contractAccount = await eoslime.Contract.deploy(CONTRACT_WASM_PATH, CONTRACT_ABI_PATH);
        messagesTable = contractAccount.messages;
    });

    it('Should store the hash', async () => {
        let messages = await messagesTable.limit(10).find();
        
        assert.equal(messages.length, 0, "Should not have any rows yet");
        const messageString = "hello world";
        const messageHash = crypto.sha256(messageString);
        
        await contractAccount.hi(myAccount.name, messageHash, {from: myAccount});

        messages = await messagesTable.equal(myAccount.name).find();
        const message = messages[0];
        assert.equal(message.user, myAccount.name, "account name not correct");
        assert.equal(message.hash, messageHash, "hash was not stored in the table");
    });

    it('Should verify the hash', async () => {
        const messageString = "hello world";
        const messageHash = crypto.sha256(messageString);

        await contractAccount.hi(myAccount.name, messageHash, {from: myAccount});
        messages = await messagesTable.equal(myAccount.name).find();
        assert.equal(messages.length, 1, "hash was not added to the table");

        await contractAccount.hiverify(myAccount.name, messageString);
        messages = await messagesTable.equal(myAccount.name).find();
        assert.equal(messages.length, 0, "hash was not removed from the table");
    });
});