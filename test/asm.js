var lin = require("linprimasm");
var b = lin.parse("var x = 4;",{});
//var b = lin.parse("var o = {one: function() {} two:2};new   ᠎             　﻿a", { 'range':true, 'loc': true, 'tolerant': true });
/*var b = lin.parse("var o = {one: function() {} two:2};", {"tolerant": true});
b = lin.parse("var o = {one: function() {} two:2};", {"tolerant": true, "loc": true});
b = lin.parse("var o = {one: function() {} two:2};", {"comment":false,"range":true,"loc":true,"tokens":true,"raw":true,"tolerant":true});
b = lin.parse("var o = {one: function() {} two:2};", {"comment":false,"range":true,"loc":true,"tokens":true,"raw":true,"tolerant":true});*/

console.log(JSON.stringify(b, undefined, 4));
