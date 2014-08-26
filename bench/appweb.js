
requirejs.config({
    baseUrl: './scripts/', 
    paths: { 
        lib : './bower_components/',
        build : './build/'
    }
});

requirejs(['simplebench', 'jquery.js', 'build/linprimasm.js'], function() { 

var simplebench = require('simplebench');
var tmp = require('jquery.js');
var linprima = require('build/linprimasm.js');



var runWeb = function(path, filelist) {
    this.jquery = jQuery;
    var printResultsCallback = function(sectionName, content) {
    }
    var getFileCallback = function (fname) { 
        /* return jQuery.ajax({
            type:"GET",
            url:"cases/active/" + fname,
            async: false
        }).responseText; */
    return '/*global CompilerContext, shouldCompileTo */\ndescribe(\'blocks\', function() {\n  it("array", function() {\n    var string   = "{{#goodbyes}}{{text}}! {{/goodbyes}}cruel {{world}}!";\n    var hash     = {goodbyes: [{text: "goodbye"}, {text: "Goodbye"}, {text: "GOODBYE"}], world: "world"};\n    shouldCompileTo(string, hash, "goodbye! Goodbye! GOODBYE! cruel world!",\n                    "Arrays iterate over the contents when not empty");\n\n    shouldCompileTo(string, {goodbyes: [], world: "world"}, "cruel world!",\n                    "Arrays ignore the contents when empty");\n\n  });\n\n  it("array with @index", function() {\n    var string = "{{#goodbyes}}{{@index}}. {{text}}! {{/goodbyes}}cruel {{world}}!";\n    var hash   = {goodbyes: [{text: "goodbye"}, {text: "Goodbye"}, {text: "GOODBYE"}], world: "world"};\n\n    var template = CompilerContext.compile(string);\n    var result = template(hash);\n\n    equal(result, "0. goodbye! 1. Goodbye! 2. GOODBYE! cruel world!", "The @index variable is used");\n  });\n\n  it("empty block", function() {\n    var string   = "{{#goodbyes}}{{/goodbyes}}cruel {{world}}!";\n    var hash     = {goodbyes: [{text: "goodbye"}, {text: "Goodbye"}, {text: "GOODBYE"}], world: "world"};\n    shouldCompileTo(string, hash, "cruel world!",\n                    "Arrays iterate over the contents when not empty");\n\n    shouldCompileTo(string, {goodbyes: [], world: "world"}, "cruel world!",\n                    "Arrays ignore the contents when empty");\n  });\n\n  it("block with complex lookup", function() {\n    var string = "{{#goodbyes}}{{text}} cruel {{../name}}! {{/goodbyes}}";\n    var hash     = {name: "Alan", goodbyes: [{text: "goodbye"}, {text: "Goodbye"}, {text: "GOODBYE"}]};\n\n    shouldCompileTo(string, hash, "goodbye cruel Alan! Goodbye cruel Alan! GOODBYE cruel Alan! ",\n                    "Templates can access variables in contexts up the stack with relative path syntax");\n  });\n\n  it("block with complex lookup using nested context", function() {\n    var string = "{{#goodbyes}}{{text}} cruel {{foo/../name}}! {{/goodbyes}}";\n\n    shouldThrow(function() {\n      CompilerContext.compile(string);\n    }, Error);\n  });\n\n  it("block with deep nested complex lookup", function() {\n    var string = "{{#outer}}Goodbye {{#inner}}cruel {{../../omg}}{{/inner}}{{/outer}}";\n    var hash = {omg: "OMG!", outer: [{ inner: [{ text: "goodbye" }] }] };\n\n    shouldCompileTo(string, hash, "Goodbye cruel OMG!");\n  });\n\n  describe(\'inverted sections\', function() {\n    it("inverted sections with unset value", function() {\n      var string = "{{#goodbyes}}{{this}}{{/goodbyes}}{{^goodbyes}}Right On!{{/goodbyes}}";\n      var hash = {};\n      shouldCompileTo(string, hash, "Right On!", "Inverted section rendered when value isn\'t set.");\n    });\n\n    it("inverted section with false value", function() {\n      var string = "{{#goodbyes}}{{this}}{{/goodbyes}}{{^goodbyes}}Right On!{{/goodbyes}}";\n      var hash = {goodbyes: false};\n      shouldCompileTo(string, hash, "Right On!", "Inverted section rendered when value is false.");\n    });\n\n    it("inverted section with empty set", function() {\n      var string = "{{#goodbyes}}{{this}}{{/goodbyes}}{{^goodbyes}}Right On!{{/goodbyes}}";\n      var hash = {goodbyes: []};\n      shouldCompileTo(string, hash, "Right On!", "Inverted section rendered when value is empty set.");\n    });\n\n    it("block inverted sections", function() {\n      shouldCompileTo("{{#people}}{{name}}{{^}}{{none}}{{/people}}", {none: "No people"},\n        "No people");\n    });\n\n    it("block inverted sections with empty arrays", function() {\n      shouldCompileTo("{{#people}}{{name}}{{^}}{{none}}{{/people}}", {none: "No people", people: []},\n        "No people");\n    });\n  });\n});\n';
    };
    var jsonDiffCallback = function(ctrl, exp) {
        return undefined;
    }

    return this.runCmp(filelist, 
                       getFileCallback,
                       printResultsCallback,
                       jsonDiffCallback);
};

simplebench.setLinprima(linprima);
simplebench.runCmpBench(runWeb);


} );
