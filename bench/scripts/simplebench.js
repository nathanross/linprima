define(function (require) {
var Benchmark = require('lib/benchmark/benchmark');
var esprima = require('build/esprima.js');
//var promise = require('lib/promise/promise');


var logfunc = console.log;
var linprima = "";
var idstamp = 0;
var getIDstamp = function() {
    return idstamp++;
}

var Role = function(name, callback) {
    this.callback = callback;
    this.name = name;
    //we use ids instead of passing objects
    //to eliminate closure+gc problems.
    this.id = String(getIDstamp());
}

var Source = function(name, obj) {
    this.name = name;
    this.obj = obj;
    this.roleIds = [];
};

Source.prototype = {
    canRole : function(role) {
        this.roleIds.push(role.id);
    }
};

var CompareProgress = function() {
    this.filenum = 0;
    this.fname = "";
    this.ftext = "";
    this.srcCols = {};
    this.numResultCols = 0;
    this.availRoles = [];
    this.rolenum = -1;
};

var CompareVis = function() {
    this.sources = {};
    this.sourceNames = [];
    this.roleById = {};
    this.sourcesWithRole = {};
    this.ctrlname = "";
    this.cmp = undefined;
    this.testForEq = true;
};

CompareVis.CmpError = function(fname, ctrlname, expname, 
                  ctrlout, expout, diffout) {
    this.isErr = true;
    this.fname = fname;
    this.ctrlname = ctrlname;
    this.expname = expname;
    this.ctrlout = ctrlout;
    this.expout = expout;
    this.diffout = diffout;
};

CompareVis.prototype = {
    addSource : function(source, isCtrl) {
        this.sources[source.name] = source;
        this.sourceNames.push(source.name);
        var roleId;
        for (var i=0; i<source.roleIds.length; i++) {
            roleId = source.roleIds[i];
            if (!(roleId in this.sourcesWithRole)) 
            { this.sourcesWithRole[roleId] = []; }
            this.sourcesWithRole[roleId]
                .push(source.name);
        }
        if (isCtrl)
        { this.ctrlname = source.name; }
    },
    useRole : function(role) {
        this.roleById[role.id] = role;
    },
    addComparison : function(fname, fsize, comp) {
        var i, row = [];
        for (i=0; i<this.cmp.numResultCols; i++) {
            row.push('');
        }
        for (i=0; i<comp.length; i++) {
            row[this.cmp.srcCols[comp[i].name]] = String(
                (1000*comp[i].stats.mean)|0);
        } 
        row.push(String(fsize));
        row.push(fname);
        this.fileOutMsg(row.join('\t'));
    },
    fileOutMsg : function(msg) {
        //for bench output only. should rename.
        //difference from just adding to list,
        //is would like to output as rows are available.
        //only other option than console.log is a special
        //callback for this like printResultsCallback
        console.log(msg);
    },
    writeOutMsg : function(msg, cmpError) {
        if (cmpError !== undefined
           && cmpError.isErr) {
            console.log("difference detected in file " + cmpError.fname);
            console.log("between output of control " + 
                        cmpError.ctrlname +
                       " and exp " +
                       cmpError.expname);
            var outs = ['ctrlout', 'expout', 'diffout'];
            for (var i=0;i<outs.length;i++) {
                this.printResultsCallback(outs[i], cmpError[outs[i]]);
            }
            console.log("diff printed below:");
            //console.log(cmpError.diffout);
        }
        console.log(msg);
    },
    runCmp : function(filelist, 
                      getFileCallback, 
                      printResultsCallback,
                      jsonDiffCallback) {
        this.cmp = new CompareProgress();
        this.getFileCallback = getFileCallback;
        this.printResultsCallback = printResultsCallback;
        this.jsonDiffCallback = jsonDiffCallback;
        this.cmp.availRoles = this.sources[this.ctrlname].roleIds;
        var srcnameRow = [];
        var roleRow = [], role;
        var srcname,i,j, col=0;
        for (i=0; i<this.sourceNames.length; i++) {
            srcname = this.sourceNames[i];
            for (j=0; j<this.cmp.availRoles.length; j++) {
                srcnameRow.push(srcname.substr(0,5));
                role = this.roleById[this.cmp.availRoles[j]];
                roleRow.push(role.name);
                this.cmp.srcCols[srcname +"|"+role.name] = col;
                col++;
            }
        }
        this.cmp.numResultCols = col-0;
        srcnameRow.push("fsize");
        srcnameRow.push("fname");
        roleRow.push("");
        roleRow.push("");
        this.fileOutMsg(srcnameRow.join('\t'));
        this.fileOutMsg(roleRow.join('\t'));
        this.cmp.filelist = filelist;
        this._runCmp();
    },
    _makeCompleteCallback : function(fname, fsize) {
        var t = this;
        var r = function() {
            t.addComparison(fname, fsize, this.slice());
            t._runCmp();
        };
        return r;
    },
    _makeBenchCallback : function(role, obj, ftext) {
        var r = function() {
            role.callback(obj, ftext);
        };
        return r;
    },
    _runCmp : function() {
        var role, results, srcname, src, diff;
        var suite, roleId;
        var srcsWithRole, i, t = this;
        var cmp = t.cmp;

        if (this.ctrlname == "") { 
            throw new Error(
                "need to designate a source as control for testing purposes."); 
        }

        if (cmp.rolenum == -1 || cmp.rolenum == 
            cmp.availRoles.length) { 
            
            cmp.rolenum = 0;
            if (cmp.filenum == cmp.filelist.length) { 
                this.writeOutMsg(0); 
                return; 
            }
            cmp.fname = cmp.filelist[cmp.filenum];
            (cmp.filenum)++;
            cmp.ftext = this.getFileCallback(cmp.fname);
        }
            
        roleId = cmp.availRoles[cmp.rolenum];
        sourcesWithRole = this.sourcesWithRole[roleId];
        role = this.roleById[roleId];
        if (cmp.rolenum == 0 && this.testForEq) {
            results = {};
            try {
                results[this.ctrlname] = 
                    role.callback(
                        this.sources[this.ctrlname].obj, cmp.ftext);
            } catch(e) {
                (cmp.rolenum)++;
                this._runCmp();
                return;
            }
            for (i=0; i<sourcesWithRole.length; i++) {
                srcname = sourcesWithRole[i];
                if (srcname == this.ctrlname) { continue; }
                src = this.sources[srcname];
                results[srcname] = 
                    role.callback(src.obj, cmp.ftext);
                diff = this.jsonDiffCallback(results[this.ctrlname],
                                         results[srcname]);
                if (diff !== undefined) {
                    this.writeOutMsg(-1, new CompareVis.CmpError(
                        cmp.filelist[i],
                        this.ctrlname,
                        srcname,
                        JSON.stringify(results[this.ctrlname],undefined,4),
                        JSON.stringify(results[srcname],undefined,4),
                        diff
                    ));
                    return;
                }
            }            
        }

        var BenchDropin = function() {
            this.results = [];
            this.slice = function() {
                return this.results;
            }
        };
        var begin, end, result, bd = new BenchDropin();
        var j, reps = 5;
        
        for (i=0; i<sourcesWithRole.length; i++) {
            result = {};
            srcname = sourcesWithRole[i];
            src = this.sources[srcname];
            result['name'] = srcname +"|"+ role.name;
            begin = (new Date()).getTime(); 
            for (j=0; j<reps; j++) {
                cmp.ftext += " var abcd = 5; ";
                role.callback(src.obj, cmp.ftext);
            }
            end = (new Date()).getTime();            
            result['stats'] = {'mean': ((end - begin) / reps) };
            bd.results.push(result);
        }
        (cmp.rolenum)++;
        (this._makeCompleteCallback(cmp.fname, cmp.ftext.length)).call(bd);
        /*
        suite =new Benchmark.Suite;
        for (i=0; i<sourcesWithRole.length; i++) {
            srcname = sourcesWithRole[i];
            src = this.sources[srcname];
            suite.add(srcname +"|"+ role.name, 
                      this._makeBenchCallback(role, 
                                           src.obj, cmp.ftext));
        }
        suite.on('complete', this._makeCompleteCallback(
            cmp.fname, cmp.ftext.length));
        (cmp.rolenum)++;
        suite.run({ 'async':false });
        */
    }
};

//getFileCallback should accept filename from filelist and
//return filecontents synchronously.

var runCmpBench = function(runInit) {
    CompareVis.prototype.runInit = runInit;
    var fullopt = {'range': true, 
                'loc': true, 
                'comments': true,
                'tolerant': true}
    var sparseopt = {'tolerant': true}

    var sparse = new Role('sparse', function(source, ftext) 
                          { return source.parse(ftext, sparseopt); });
    var full = new Role('full', function(source, ftext) 
                        { return source.parse(ftext, fullopt); });
    var esprimaSrc = new Source('esprima', esprima);
    esprimaSrc.canRole(full);
    esprimaSrc.canRole(sparse);
    var linprimaSrc = new Source('linprima', linprima);
    linprimaSrc.canRole(full);
    linprimaSrc.canRole(sparse);
    var cmpr = new CompareVis('esprima');
    cmpr.addSource(esprimaSrc, true);
    cmpr.addSource(linprimaSrc, false);
    cmpr.useRole(full);
    cmpr.useRole(sparse);
    cmpr.runInit('cases/active', []);
}

var setLinprima = function(callback) {
    linprima = callback
}

return {
    'CompareVis': CompareVis,
    'runCmpBench': runCmpBench,
    'setLinprima' : setLinprima
}

    return "";
});
