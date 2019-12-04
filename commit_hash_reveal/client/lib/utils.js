// https://stackoverflow.com/questions/18338890/are-there-any-sha-256-javascript-implementations-that-are-generally-considered-t
async function sha256(message) {
    // encode as UTF-8
    const msgBuffer = new TextEncoder('utf-8').encode(message);                    

    // hash the message
    const hashBuffer = await crypto.subtle.digest('SHA-256', msgBuffer);

    // convert ArrayBuffer to Array
    const hashArray = Array.from(new Uint8Array(hashBuffer));

    // convert bytes to hex string                  
    const hashHex = hashArray.map(b => ('00' + b.toString(16)).slice(-2)).join('');
    return hashHex;
}

function displaySuccess(message) {
    $("#user-alert").addClass('alert-success').removeClass('alert-warning')
    $("#user-alert").show();
    $("#user-alert").html(message);    
}

function displayError(message) {
    console.error(message);
    $("#user-alert").addClass('alert-warning').removeClass('alert-success')
    $("#user-alert").show();
    $("#user-alert").html(message);    
}