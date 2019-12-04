const crypto = require("crypto");

module.exports.sha256 = function (text) {
    return crypto.createHash('sha256').update(text).digest('hex');
}