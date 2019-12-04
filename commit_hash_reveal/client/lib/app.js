async function onload() {
    $("#user-alert").hide();
    await login();
    updateTable();
    stringTosha256();
}

async function commitTx() {
    const message_hash = $("#message_hash").val();
    await walletContract.commit(message_hash);
    updateTable();
}

async function revealTx() {
    const message = $("#message").val();
    const accountName = $("#account-name").val();
    await walletContract.reveal(accountName, message);
    updateTable();
}

async function updateTable() {
    const tableRows = await walletContract.getMessages();
    if (tableRows.rows && tableRows.rows.length === 1) {
        const row = tableRows.rows[0];
    
        $("#user-account").html(row.user);
        $("#user-hash").html(row.hash);
        $("#user-updated").html(row.last_updated);    
    } else {
        $("#user-account").html(account.name);
        $("#user-hash").html("");
        $("#user-updated").html("");    
    }
}

async function stringTosha256() {
    const text = $("#string").val();
    const hash = await sha256(text);
    $("#hashOfString").val(hash);
}