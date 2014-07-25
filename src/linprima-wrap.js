

(function (root, factory) {
    'use strict';

    // Universal Module Definition (UMD) to support AMD, CommonJS/Node.js,
    // Rhino, and plain browser loading.

    if (typeof define === 'function' && define.amd) {
        define(['exports'], factory);
    } else if (typeof exports !== 'undefined') {
        factory(exports);
    } else {
        factory((root.esprima = {}));
    }
}(this, function (exports) {
    'use strict';

    //=FFI= //%FFIsnippet.js%

    //=ASM= //%linprima.asm.js%

    //=ASM= var _linprimaMod = {
    //=ASM= parseExtern : 
    //=ASM= Module.cwrap('parseASMJS', 'string', ['string', 'number', 'string']),
    //=ASM= tokenizeExtern : 
    //=ASM= Module.cwrap('tokenizeASMJS', 'string', ['string', 'number', 'string']),
    //=ASM= };

    var tokenize = function(code, options) {
        if (typeof code !== 'string' && !(code instanceof String)) {
            code = String(code);
        }
        var optStr = "{}";
        if (options !== undefined) { optStr = JSON.stringify(options); }
        var out = JSON.parse(_linprimaMod.tokenizeExtern(code,
                                                   //=ASM= code.length,
                                                         optStr));
        if ("isError" in out) {
            var e = new Error('Line ' + out['lineNumber'] +
                                ': ' + out['description']);
            e.description = out["description"];
            e.index = out["index"];
            e.lineNumber = out["lineNumber"];
            e.column = out["column"];
            throw e;
        }
        var nodeRoot = out.tokenlist;
        if ("comments" in out) { nodeRoot.comments = out.comments; }
        if ("tokens" in out) { nodeRoot.tokens = out.tokens; }
        if ("errors" in out) { nodeRoot.errors = out.errors; }   
        return nodeRoot;
    }

    var parse = function(code, options) {
        if (typeof code !== 'string' && !(code instanceof String)) {
            code = String(code);
        }
        var optStr = "{}";
        if (options !== undefined) { optStr = JSON.stringify(options); }
        var out = JSON.parse(_linprimaMod.parseExtern(code, 
                                            //=ASM=   code.length, 
                                                      optStr));
        if ("isError" in out) {
            var e = new Error('Line ' + out['lineNumber'] +
                                ': ' + out['description']);
            e.description = out["description"];
            e.index = out["index"];
            e.lineNumber = out["lineNumber"];
            e.column = out["column"];
            throw e;
        }
        var path,j,cursor, regex;
        for (var i=0; i<out["regexp"].length; i++) {
            path=out["regexp"][i]; 
            cursor = out["program"];
            for (j=0; j<path.length; j++) {
                cursor = cursor[path[j]];
            }
            try {
                regex = new RegExp(cursor["value"][0], 
                                   cursor["value"][1]);
                cursor["value"] = regex;
//                delete cursor["flags"];
            } catch (e) {
                return { "a": cursor["value"][0] + "::" + cursor["value"][1]};
                    //Messages["InvalidRegexp"]); //!
            }
        }
        delete out["regexp"];
        var programOut = out.program;
        if (out.comments != undefined) { programOut.comments = out["comments"]; }
        if (out.errors != undefined) { programOut.errors = out["errors"]; }
        if (out.tokens != undefined) { programOut.tokens = out["tokens"]; }
        return programOut;
    }


    // Sync with *.json manifests.
    exports.version = '2.0.0-dev';

    exports.tokenize = tokenize;

    exports.parse = parse;


}));

//delete window._linprimaMod;
