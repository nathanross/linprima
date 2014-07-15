var lin = require("linprimaFFI.js");
var b = lin.tokenize("var f = function(){ echo('hello world'); }");
console.log(JSON.stringify(b));
