var lin = require("esprima.js");

var b = lin.parse("T‿ = []", { loc: true, range:true });
//var b = lin.parse("x = []", { loc: true, range:true });
//console.log(JSON.stringify(JSON.parse(JSON.stringify(b))));
console.log(JSON.stringify(JSON.parse(JSON.stringify(b)), undefined, 4));
