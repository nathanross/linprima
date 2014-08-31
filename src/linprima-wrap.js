

(function (root, factory) {
    'use strict';

    // Universal Module Definition (UMD) to support AMD, CommonJS/Node.js,
    // Rhino, and plain browser loading.

    if (typeof define === 'function' && define.amd) {
        define(['exports'], function(a) { factory(a, root); });
    } else if (typeof exports !== 'undefined') {
        factory(exports, root);
    } else {
        factory((root.esprima = {}), root);
    }
}(this, function (exports, root) {
    'use strict';

    //=ASM= var console = { 'log': function(a) { } };
    //=ASM= if ('console' in root) { console = root.console; }

    //=FFI= //%FFIsnippet.js%
    /*
    //=FFI= //%nodedebug.js%
    */

    //=ASM= //%linprima.asm.js%

    //=ASM= var _linprimaMod = {
    //=ASM= parseExtern : 
    //=ASM= Module.cwrap('parseASMJS', 'string', ['string', 'number', 'string']),
    //=ASM= tokenizeExtern : 
    //=ASM= Module.cwrap('tokenizeASMJS', 'string', ['string', 'number', 'string']),
    //=ASM= };

    var Syntax = {
        AssignmentExpression: 'AssignmentExpression',
        ArrayExpression: 'ArrayExpression',
        ArrowFunctionExpression: 'ArrowFunctionExpression',
        BlockStatement: 'BlockStatement',
        BinaryExpression: 'BinaryExpression',
        BreakStatement: 'BreakStatement',
        CallExpression: 'CallExpression',
        CatchClause: 'CatchClause',
        ConditionalExpression: 'ConditionalExpression',
        ContinueStatement: 'ContinueStatement',
        DoWhileStatement: 'DoWhileStatement',
        DebuggerStatement: 'DebuggerStatement',
        EmptyStatement: 'EmptyStatement',
        ExpressionStatement: 'ExpressionStatement',
        ForStatement: 'ForStatement',
        ForInStatement: 'ForInStatement',
        FunctionDeclaration: 'FunctionDeclaration',
        FunctionExpression: 'FunctionExpression',
        Identifier: 'Identifier',
        IfStatement: 'IfStatement',
        Literal: 'Literal',
        LabeledStatement: 'LabeledStatement',
        LogicalExpression: 'LogicalExpression',
        MemberExpression: 'MemberExpression',
        NewExpression: 'NewExpression',
        ObjectExpression: 'ObjectExpression',
        Program: 'Program',
        Property: 'Property',
        ReturnStatement: 'ReturnStatement',
        SequenceExpression: 'SequenceExpression',
        SwitchStatement: 'SwitchStatement',
        SwitchCase: 'SwitchCase',
        ThisExpression: 'ThisExpression',
        ThrowStatement: 'ThrowStatement',
        TryStatement: 'TryStatement',
        UnaryExpression: 'UnaryExpression',
        UpdateExpression: 'UpdateExpression',
        VariableDeclaration: 'VariableDeclaration',
        VariableDeclarator: 'VariableDeclarator',
        WhileStatement: 'WhileStatement',
        WithStatement: 'WithStatement'
    };

    var genErrorObject = function(errJson) {
        var e = new Error('Line ' + errJson['lineNumber'] +
                          ': ' + errJson['description']);
        e.description = errJson["description"];
        e.index = errJson["index"];
        e.lineNumber = errJson["lineNumber"];
        e.column = errJson["column"];
        return e;
    };
    var genErrorObjectList = function(errorTolerantArray) {
        //errorTolerant returns in the 'errors' key an array
        //of js Error objects. Here we convert a list of error
        //jsons to js Error objects.
        for (var i=0; i<errorTolerantArray.length; i++) {
            errorTolerantArray[i] = genErrorObject(
                errorTolerantArray[i]);
        }
    };

    var tokenize = function(code, options) {
        if (typeof code !== 'string' && !(code instanceof String)) {
            code = String(code);
        }
        var optStr = "{}";
        if (options !== undefined) { 
            if (typeof options == 'string' 
                || (options instanceof String)
        //=ASM= || 'slice' in options
               ) {
                optStr = options;
            } else if ('readObject' in options) {
                optStr = String(options);
            } else { 
                optStr = JSON.stringify(options); 
            }
        }
        var out = JSON.parse(_linprimaMod.tokenizeExtern(code,
                                                   //=ASM= code.length,
                                                         optStr));
        if ("isError" in out) {
            throw genErrorObject(out);
        }
        if (options !== undefined 
            && 'tolerant' in options 
            && options['tolerant'] 
            && 'errors' in out) {
            genErrorObjectList(out['errors']);
        }
        var nodeRoot = out.tokens;
        if ("comments" in out) { nodeRoot.comments = out.comments; }
        if ("errors" in out) { nodeRoot.errors = out.errors; }   
        return nodeRoot;
    };

    var parse = function(code, options) {

        if (typeof code !== 'string' && !(code instanceof String)) {
            code = String(code);
        }
        var optStr = "{}";
        if (options !== undefined) { 
            if (typeof options == 'string' 
                || (options instanceof String) 
        //=ASM= || 'slice' in options
               ) {
                optStr = options;
            } else if ('readObject' in options) {
                optStr = String(options);
            } else {
                optStr = JSON.stringify(options); 
            }
        }
// for debugging test suite.
// sometimes a test will only fail with a particular set of options
// and it's good to see which.
        var strout = _linprimaMod.parseExtern(code, 
                                            //=ASM=   code.length, 
                                                      optStr);
        var out = JSON.parse(strout);

        //var out = JSON.parse(_linprimaMod.parseExtern(code, 
        //                                    //=ASM=   code.length, 
        //                                              optStr));
        if ("isError" in out) {
//            return out;
            throw genErrorObject(out);
        }
        if ("isAssert" in out) {
            var e = new Error(out.description);
            return e;
        }
        if (options !== undefined 
            && 'tolerant' in options 
            && options['tolerant'] 
            && 'errors' in out) {
            genErrorObjectList(out['errors']);
        }
        var path,i, j,cursor, regex, container;
        for (i=0; i<out["regexp"].length; i++) {
            path=out["regexp"][i]; 
            cursor = out["program"];
            for (j=0; j<path.length; j++) {
                cursor = cursor[path[j]];
            }
            container = cursor["value"];
            try {
                regex = new RegExp(container["regexpBody"], 
                                   container["regexpFlags"]);
                cursor["value"] = regex;
//                delete cursor["flags"];
            } catch (e) {
                throw genErrorObject({
                    'description':'Invalid regular expression',
                    'lineNumber': container["lineNumber"],
                    'index': container["index"],
                    'column': container["column"]
                    });
            }
        }
        delete out["regexp"];

        var programOut = out.program;
        if (out.comments != undefined) 
        { programOut.comments = out["comments"]; }
        if (out.errors != undefined) 
        { programOut.errors = out["errors"]; }
        if (out.tokens != undefined) 
        { programOut.tokens = out["tokens"]; }
 //       if (out.debug != undefined) { programOut.debug = out.errors; }   
        return programOut;
    }


    // Sync with *.json manifests.
    exports.version = '2.0.0-dev';

    exports.tokenize = tokenize;

    exports.parse = parse;

    exports.Syntax = (function () {
        var name, types = {};

        if (typeof Object.create === 'function') {
            types = Object.create(null);
        }

        for (name in Syntax) {
            if (Syntax.hasOwnProperty(name)) {
                types[name] = Syntax[name];
            }
        }

        if (typeof Object.freeze === 'function') {
            Object.freeze(types);
        }

        return types;
    }());
}));

//delete window._linprimaMod;
