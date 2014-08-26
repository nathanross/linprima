var lin = require("linprimasm");

var fs = require("fs");
var testcode = fs.readFileSync(__dirname + "/codetotest.js");
var testopt = fs.readFileSync(__dirname + "/opttotest");

//var b = lin.parse(testcode, testopt);
var b = lin.parse(testcode, testopt);

console.log(JSON.stringify(b, undefined, 4));
