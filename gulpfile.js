var gulp = require('gulp');
var argv = require('yargs').argv;
var concat = require('gulp-concat');
//var exec = require('gulp-exec');
//var uglify = require('gulp-uglify');
var clean = require('gulp-clean');
var mixotroph = require('gulp-mixotroph');
var rename = require('gulp-rename');
//var lazypipe = require('lazypipe');
var spawn = require('child_process').spawn;
var replace = require('gulp-replace');
var run = require('gulp-run');
var runSequence = require('run-sequence');
var fs = require('fs');

//--- clean js wrappers ---------------------------------------
gulp.task('cleanTmp', function() {
    return gulp.src('tmp/*', {read: false})
        .pipe(clean());
});

//--- tooling ---------------------------------------------------
//gulp.task('makeCompressors', function(callback) { //gperftools
//    exec("./tools/compression/./init.sh", 
//        makeExecCallback(callback));
//});

//--- compilation (call scons) --------------------------------
gulp.task('compile', function(callback) {
    args = []
    var convertVal = function(val) {
        if (val === true){
            return 1;
        }
        return val;
    }
    for (var i in argv) {
        if (i != '$0' && i != '_') {
            args.push(i + '=' + convertVal(argv[i]));
        }
    }
    var child = spawn('scons', args, {detached: false,
                          stdio:[process.stdin, 
                                 process.stdout, 
                                 process.stderr]});
    child.on('exit', callback);
});

//--- js wrapping -------------------------------------------------
gulp.task('cleanASM', function() {
    return gulp.src('build/asm/*', {read: false})
        .pipe(clean());
});
gulp.task('cleanFFI', function() {
    return gulp.src('build/ffi/*', {read: false})
        .pipe(clean());
});

function completeWrapper(mixMode, mixSnippetPath, dest) {
   return gulp.src('src/linprima-wrap.js')
    .pipe(mixotroph({mode:mixMode, snippetPath:mixSnippetPath}))
    .pipe(rename('linprima.js'))
    .pipe(gulp.dest(dest));
}

gulp.task('ffiWrapper', function() {
    return completeWrapper('FFI', 'src/snippets', 'build/ffi');
});

gulp.task('umdFixes', function() {
    var b = gulp.src('tmp/linprim.asm.raw.js')
    .pipe(replace('export','emccport')) //nodejs module importing
    .pipe(rename('linprima.asm.js'))
    .pipe(gulp.dest('tmp'));
});

gulp.task('asmWrapper', function() {
    completeWrapper('ASM', 'tmp/', 'build/asm/');
});

//node neads this for readFileSync to work on gulp dests, which are flushed to disk asynchronously.
gulp.task('sync', function(callback) {
    var child = spawn('sync', [], {});
    child.on('exit', callback);
});

//--- compilation + js wrapping ------------------------------------------
gulp.task('asmjs', function() {
    argv.asmjs = 1;
    runSequence('compile', 'umdFixes', 'sync', 'asmWrapper');
});
gulp.task('clang', function() {
    argv.clang = 1;
    runSequence('compile', 'ffiWrapper');
});
gulp.task('gcc', function() {
    argv.gcc = 1;
    runSequence('compile', 'ffiWrapper');
});

//--- speed profiling shortcuts ----------------------------------------
gulp.task('runproftest', function(callback) {
    var child = spawn('./tmp/./test_profiny', [], {})
    child.on('exit', function() { 
        fs.rename('profiny.out', 'tmp/profiny.out', callback);
    });
});

gulp.task('processProfinyOut', function(callback) {
    var execPath = './tools/profiny/./'
    if (argv.graph) {
        execPath += 'graphprocess.py'
    } else {
        execPath += 'flatprocess.sh'
    }
    var child = spawn(execPath, [], {})
    child.on('exit', callback);
});

gulp.task('prof', function() {
    argv.clang = 1;
    argv.profiny = 1;
    runSequence('compile', 'runproftest', 'processProfinyOut');
});

//--- memory profiling shortcuts ---------------------------------------
var memstep=""
var memOutfolder=""
var tmpPath = 'tmp/'

var massif = function(callback, heap) {
    try {
        fs.mkdirSync(tmpPath+'mem/')
    } catch(e) {}
    try {
        fs.mkdirSync(tmpPath+'mem/'+memOutfolder)
    } catch(e) {} 
    var l_out = tmpPath+'mem/'+memOutfolder+memstep + 
        ((heap)?'heap':'both')
    var args = ['--tool=massif', 
                '--max-snapshots=150',
                '--stacks='+((heap)?'no':'yes'),
                '--massif-out-file='+l_out,
                tmpPath+'test_gdb']
    var valgrind = spawn('valgrind', args, {'stdio':[null, 
                                                     process.stdio, 
                                                     process.stderr]});
    valgrind.on('exit', function() {
        var f_out = fs.openSync(l_out+'_msprint', 'a');
        var ms_print = spawn('ms_print', 
                             [l_out], 
                             {detached: true, stdio:[null, f_out, null]});
        ms_print.unref();
        if (heap) { massif(callback, false); }
        else { callback(); }

    });
}
gulp.task('massif', function(callback) { massif(callback, true); });

gulp.task('incrMemStep', function(callback) {
    if (memstep == "reg") {
        argv.lowmem = 1;
        memstep = "lowmem"
    } else if (memstep == "lowmem") {
        argv.limj = 1;
        memstep = "limj"
    }
    callback();
});

gulp.task('mem', function() {
    argv.clang = 1;
    argv.gdb = 1;
    //get it so folders are ordered consecutively.
    memOutfolder = String((new Date()).getTime()).substr(5,5) + "/"

    //argv.lowmem = 0;
    //argv.limj = 0;
    //memstep = "reg"

    argv.lowmem = 1;
    argv.limj = 1;
    memstep = "limj"

    runSequence(//'compile', 'massif', 'incrMemStep',
               //'compile', 'massif', 'incrMemStep',
               'compile', 'massif');
});

//--- build prod ------------------------------------------
gulp.task('prod', function() {
    argv = {'asmjs':1}
    runSequence('compile', 'umdFixes', 'sync', 'asmWrapper');
    argv = {'clang':1}
    runSequence('compile', 'ffiWrapper');
});

//-- clean everything ------------------------------------
gulp.task('cleanAll', ['cleanASM', 'cleanFFI', 'cleanTmp']);
