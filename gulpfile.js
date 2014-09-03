var gulp = require('gulp');
var argv = require('yargs').argv;
var concat = require('gulp-concat');
//var exec = require('gulp-exec');
//var uglify = require('gulp-uglify');
var clean = require('gulp-clean');
var mixotroph = require('gulp-mixotroph');
var rename = require('gulp-rename');
//var lazypipe = require('lazypipe');
var exec = require('child_process').exec;
var replace = require('gulp-replace');
var run = require('gulp-run');
var runSequence = require('run-sequence');

function toShell(err, stdout, stderr) {
    console.log(stdout);
    console.log(stderr);
    if (err != null)
        { console.log(err); }
}

function makeExecCallback(func) {
    var r = function(a,b,c) {
        toShell(a,b,c);
        func();
    }
    return r;
}

gulp.task('cleanTmp', function() {
    return gulp.src(['tmp/*.cpp', 'tmp/*.js'], {read: false})
        .pipe(clean());
});
gulp.task('cleanASM', ['cleanTmp'], function() {
    return gulp.src('build/asm/*', {read: false})
        .pipe(clean());
});
gulp.task('cleanFFI', ['cleanTmp'], function() {
    return gulp.src('build/ffi/*', {read: false})
        .pipe(clean());
});

gulp.task('cleanAll', ['cleanASM', 'cleanFFI']);

gulp.task('noThrow52', function() {
    return;
// gulp.src('src/linprima.cpp')
  //  .pipe(gulp.dest('tmp'));
});

SRC_ORDER=['include/debug.hpp','include/stringutils.h', 'include/charutils.h', 'include/enums.h', 'include/podt.h', 'include/jsonutils.h', 'include/JsonDecompressor.h', 'include/Node.h', 'include/constants.h', 'include/NodesComments.h', 'include/LinprimaTask.h','include/WrappingNode.h', 'include/Tokenizer.h', 'include/parsepod.h', 'include/ParseFuncs.h', 'src/podt.cpp', 'src/Node.cpp', 'src/NodesComments.cpp', 'src/LinprimaTask.cpp', 'src/Tokenizer.cpp', 'src/ParseFuncs.cpp', 'src/JsonDecompressor.cpp', 'src/linprima.cpp']
//'src/podt.h','src/jsonutils.h','src/Node.h','src/constants.h', 'src/NodesComments.h', 'src/LinprimaTask.h', 'src/Tokenizer.h', 'src/LinprimaTask.cpp', 'src/Tokenizer.cpp', 'src/NodesComments.cpp', 'src/linprima.cpp']

gulp.task('joinSrc', function() {
    return gulp.src(SRC_ORDER)
    .pipe(concat('linprima.cpp'))
    .pipe(gulp.dest('tmp'));
});

gulp.task('joinSrcProfiny', function() {
    return gulp.src(SRC_ORDER)
    .pipe(concat('linprima.cpp'))
    .pipe(replace('DEBUGIN("', '\n PROFINY_SCOPE \n DEBUGIN("'))
    .pipe(gulp.dest('tmp'));
});

gulp.task('throw52', function(callback) { //gperftools
    exec("python src/throw52.py -v 0 tmp/linprima.cpp tmp/linprima2.cpp && mv tmp/linprima2.cpp tmp/linprima.cpp", 
        makeExecCallback(callback));
});

gulp.task('makeCompressors', function(callback) { //gperftools
    exec("./tools/compression/./init.sh", 
        makeExecCallback(callback));
});

gulp.task('addLibs', function() {
    return gulp.src(['lib/rapidjson*',
                     'tools/compression/cmpR_out/jsoncompress.cpp',
                     'tmp/linprima.cpp'])
    .pipe(concat('src.cpp'))
    .pipe(gulp.dest('tmp'));
});

function completeWrapper(mixMode, mixSnippetPath, dest) {
   return gulp.src('src/linprima-wrap.js')
    .pipe(mixotroph({mode:mixMode, snippetPath:mixSnippetPath}))
    .pipe(rename('linprima.js'))
    .pipe(gulp.dest(dest));
}

//node neads this for readFileSync to work on gulp dests, which are flushed to disk asynchronously.

gulp.task('sync', function(callback) {
    exec("sync",
        makeExecCallback(callback));
});

//profiny
gulp.task('ffitcall', function(callback) {
     exec("clang++ -Wall -O3 -D THROWABLE -D HASMAIN -D USE_PROFINY -D" + 
          ((argv.graph !== undefined)? 
           "PROFINY_CALL_GRAPH_PROFILER":
           "PROFINY_FLAT_PROFILER") + 
          " -std=c++11 -stdlib=libc++ " +
          " -lboost_timer -lboost_chrono -lboost_system" + 
          " -o \"profiny_test\" \"tmp/src.cpp\"", 
        makeExecCallback(callback));
});

//valgrind/gdb
gulp.task('ffidcall', function(callback) { //gdb, valgrind
    exec("clang++ -g -O3 -Wall -D HASMAIN -D THROWABLE " +
         ((argv.lowmem)? " -D LOWMEM ": "") + 
         ((argv.limj)? " -D LOWMEM -D LIMITJSON ": "") + 
         ((argv.dbg)? " -D DO_DEBUG ": "") + 
         " -std=c++11 -stdlib=libc++ tmp/src.cpp -o test.out", 
        makeExecCallback(callback));
});

//general testing. mirrors production compilation, 
//except no optimization, opt. print out call graph
//opt. use throw52 for easier/faster debugging than emcc
// of any throw52 interactions causing problems.
gulp.task('fficcall', function(callback) {
     exec("clang++ -Wall -D LOWMEM -D LIMITJSON " + 
          ((argv.t52 !== undefined)? "":" -D THROWABLE ") + 
          ((argv.dbg !== undefined)? " -D DO_DEBUG ":"") + 
          " -std=c++11 -stdlib=libc++ -shared -fPIC -o build/ffi/linprima.x64.so tmp/src.cpp", 
        makeExecCallback(callback));
});

gulp.task('ffipcall', function(callback) {
    exec("clang++ -O3 -D THROWABLE -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", 
        makeExecCallback(callback));
});



gulp.task('ffiWrapper', function() {
    return completeWrapper('FFI', 'src/snippets', 'build/ffi');
});
gulp.task('ffi', function() { //gdb and valgrind debugging
    runSequence('cleanFFI', 
               (argv.profiny !== undefined)? 
                'joinSrcProfiny' : 'joinSrc', 
               (argv.t52 !== undefined)? 'throw52':'noThrow52',
               'addLibs',
               (argv.p !== undefined)? 'ffipcall':
               ((argv.g !== undefined)? 'ffigcall': //call like this: gulp ffi --g
               ((argv.d !== undefined)? 'ffidcall': 
               ((argv.profiny !== undefined)? 'ffitcall' : 'fficcall'))),
                'ffiWrapper');
});

    //asm requires code to be built before completing wrapper
    //as code is substituted into wrapper.
gulp.task('asmccall', function(callback) {
    exec("emcc -std=c++11 -O3 -D LOWMEM -D LIMITJSON   -s ALIASING_FUNCTION_POINTERS=0 " +
         ((argv.dbg !== undefined)? " -D DO_DEBUG " : "") +
         " -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS']\" tmp/src.cpp -o tmp/linprima.asm.0.js", 
        makeExecCallback(callback));
});

gulp.task('asmpcall', function(callback) {
    exec("/opt/emscripten/emcc -Oz -std=c++11 -D LOWMEM -D LIMITJSON -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS']\" tmp/src.cpp -o tmp/linprima.asm.0.js",
        makeExecCallback(callback));
});
gulp.task('asmtcall', function(callback) {
    exec("emcc -profiling -std=c++11 -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS']\" tmp/src.cpp -o tmp/linprima.asm.0.js",
        makeExecCallback(callback));
});

    //emscripten uses an export member on its module
    //which, when using esprima's choice of UMD pattern
    //causes a name collision in NodeJS / commonJS module handling
    //leading the emscripten module itself to be returned 
    //rather than the wrapper.

gulp.task('umdFixes', function() {
    var b = gulp.src('tmp/linprima.asm.0.js')
    .pipe(replace('export','emscport')) //nodejs module importing
    .pipe(rename('linprima.asm.js'))
    .pipe(gulp.dest('tmp'));
});

gulp.task('asmWrapper', function() {
    completeWrapper('ASM', 'tmp/', 'build/asm/');
});

gulp.task('passToTest', function() {
    return gulp.src('build/asm/linprima.js')
    .pipe(rename('esprima.js'))
    .pipe(gulp.dest('esprima/esprima/'))
});

gulp.task('asm', function(callback) {
    runSequence('cleanASM',
                'joinSrc',
                'throw52',                
                'addLibs',
                (argv.p !== undefined) ? 'asmpcall' : 
                (argv.prof !== undefined) ? 'asmtcall' :'asmccall', 
                'umdFixes', 
                'sync',
                'asmWrapper',  
                'passToTest', 
                callback);
});

