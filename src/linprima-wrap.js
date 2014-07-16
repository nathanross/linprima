

(function (root, factory) {
    'use strict';

    // Universal Module Definition (UMD) to support AMD, CommonJS/Node.js,
    // Rhino, and plain browser loading.

    /* istanbul ignore next */
    if (typeof define === 'function' && define.amd) {
        define(['exports'], factory);
    } else if (typeof exports !== 'undefined') {
        factory(exports);
    } else {
        factory((root.esprima = {}));
    }
}(this, function (exports) {
    'use strict';

    //_linprimaMod = _LinprimaASM();

    //=ASM= //%linprima.asm.js%
    //=FFI= //%FFIsnippet.js%

    var tokenize = function(code, options) {
        if (typeof code !== 'string' && !(code instanceof String)) {
            code = string(code);
        }
        var optStr = "{}";
        if (options !== undefined) { optStr = JSON.stringify(options); }
        var out = JSON.parse(_linprimaMod.tokenizeExtern(code, optStr));
        var nodeRoot = out.tokenlist;
        if ("comments" in out) { nodeRoot.comments = out.comments; }
        if ("tokens" in out) { nodeRoot.tokens = out.tokens; }
        if ("errors" in out) { nodeRoot.errors = out.errors; }   
        return nodeRoot;
    }

    var parse = function(code, options) {
        if (typeof code !== 'string' && !(code instanceof String)) {
            code = string(code);
        }
        optStr = "{}";
        if (options !== undefined) { optStr = JSON.stringify(options); }
        var out = JSON.parse(_linprimaMod.parseExtern(code, optStr));
        var path,j,cursor, regex;
        for (var i=0; i<out["regex"].length; i++) {
            path=out["regex"][i]; 
            cursor = out["program"];
            for (j=0; j<path.length; j++) {
                cursor = cursor[path[j]];
            }
            try {
                regex = new Regexp(cursor["value"], cursor["flags"]);
                cursor["value"] = regex;
                delete cursor["flags"];
            } catch (e) {
                throwError(Messages["InvalidRegexp"]); //!
            }
        }
        var programOut = out.program;
        program.comments = out["comments"];
        delete out["regex"];
        return programOut;

    }


    // Sync with *.json manifests.
    exports.version = '2.0.0-dev';

    exports.tokenize = tokenize;

    exports.parse = parse;


}));

//delete window._linprimaMod;
