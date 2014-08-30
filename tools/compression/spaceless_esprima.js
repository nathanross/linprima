var lin = require("esprimaorig.js");

var fs = require("fs");
var testcode = fs.readFileSync(__dirname + "/cmpR_tmp/codetotest.js");
var testopt = fs.readFileSync(__dirname + "/opttotest");

var b = lin.parse(testcode, JSON.parse(testopt));


//console.log(JSON.stringify(JSON.parse(JSON.stringify(b))));
console.log(JSON.stringify(JSON.parse(JSON.stringify(b)), undefined, 0));
