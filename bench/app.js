//var requireNode = require;
//require = undefined;
var fs = require('fs');
var domain = require('domain');
var sh = require('execSync');

var linprima = require('linprima.js');
//var linprima = require('esprimaorig.js');
var requirejs = require('requirejs');
requirejs.config({
    baseUrl: './scripts/', 
    paths: { 
        lib : '../bower_components',
        build : '../build'
    }
});
var simplebench = requirejs('simplebench');

var runNode = function(path) {
    if (!('fs' in this)) {
        this.fs = require('fs');
    }
    //var d1 = domain.create();
    //d1.on('error', function() {     console.log("bye"); });
    //d1.run(function() {
    try {
        fs.mkdirSync("/tmp/simplebench/");
    } catch(e) {
    }
    //});
    var filelist =this.fs.readdirSync(path);
    var getFileCallback = function (fname) {            
        return fs.readFileSync(path + '/' + fname, "utf8");
    };
    var printResultsCallback = function(sectionName, content) {
    };
    var jsonDiffCallback = function(ctrl, exp) {
        var printResults = function(fs, sectionName, jsoncontent) {
            var content = JSON.stringify(jsoncontent, undefined, 4);
            var loc = '/tmp/simplebench/' + sectionName + '.json';
            var fd = fs.openSync(loc, 'w');
            fs.writeSync(fd, content);
            fs.fsyncSync(fd);
            fs.closeSync(fd);
        }

        var result = sh.run('json-diff /tmp/simplebench/ctrlout.json /tmp/simplebench/expout.json > /tmp/simplebench/diffout.json; exit 1');
        sh.run('sync');
        var out = String(fs.readFileSync('/tmp/simplebench/diffout.json'));
        if (out == ' undefined\n') { return undefined; }
        return out;
    }
    return this.runCmp(filelist, 
                       getFileCallback,
                       printResultsCallback,
                       jsonDiffCallback);
};
simplebench.setLinprima(linprima);

simplebench.runCmpBench(runNode);
//simplebench.runCmpBench(true);
