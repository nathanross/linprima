var gulp = require('gulp');
//var argv = require('yargs').argv;
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

gulp.task('prepareSource', function() {
    return gulp.src(['lib/libjson*','tmp/newlinebuffer.c','src/linprima.cpp'])
    .pipe(concat('src.cpp'))
    .pipe(gulp.dest('tmp'));
});

gulp.task('prepareSourceEx', function() {
    return gulp.src('src/linprima.cpp')
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
//    var cb = function(a,b,c) { toShell(a,b,c); callback(); };    
    exec("sync",
        makeExecCallback(callback));
});

function makeExecCallback(func) {
    var r = function(a,b,c) {
        toShell(a,b,c);
        func();
    }
    return r;
}


gulp.task('ffigcall', ['prepareSource'], function(callback) { //gperftools
    exec("clang++ -fno-builtin -g -O0 -Wall -D THROWABLE -std=c++11 -stdlib=libc++  -ltcmalloc tmp/src.cpp -o test.out", 

        makeExecCallback(callback));
});
gulp.task('ffidcall', ['prepareSource'], function(callback) { //gdb, valgrind
    exec("clang++ -g -O0 -Wall -D THROWABLE -std=c++11 -stdlib=libc++ tmp/src.cpp -o test.out", 

        makeExecCallback(callback));
});
gulp.task('fficcall', ['prepareSource'], function(callback) {
    exec("clang++ -Wall -D THROWABLE -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", 
        makeExecCallback(callback));
});

gulp.task('ffipcall', ['prepareSource'], function(callback) {
    exec("clang++ -O2 -D THROWABLE -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", 
        makeExecCallback(callback));
});



gulp.task('ffiWrapper', function() {
    return completeWrapper('FFI', 'src/snippets', 'build/ffi');
});
gulp.task('ffig', function() { //gdb and valgrind debugging
    runSequence('cleanFFI', 'ffigls call', 'ffiWrapper');
});
gulp.task('ffid', function() { //gdb and valgrind debugging
    runSequence('cleanFFI', 'ffidcall', 'ffiWrapper');
});
gulp.task('ffic', function() {
    runSequence('cleanFFI', 'fficcall', 'ffiWrapper');
});
gulp.task('ffip', function() {
    runSequence('cleanFFI', 'ffipcall', 'ffiWrapper');
});

    //asm requires code to be built before completing wrapper
    //as code is substituted into wrapper.
gulp.task('asmccall', ['prepareSource'], function(callback) {
    var cb = function(a,b,c) { toShell(a,b,c); callback(); };
    exec("emcc -std=c++11 -O2 -s DISABLE_EXCEPTION_CATCHING=2 -s RELOOPER_BUFFER_SIZE=419430400 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS', '_someTest']\" tmp/src.cpp -o tmp/linprima.asm.0.js", 
        makeExecCallback(callback));
});

gulp.task('asmpcall', ['prepareSource'], function(callback) {
//    var cb = function(a,b,c) { toShell(a,b,c); callback(); };    
    exec("emcc -O3 -std=c++11 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS']\" tmp/src.cpp -o tmp/linprima.asm.0.js",
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
    .pipe(replace(/  +header =/g, ' var header =')) //exceptions in strict mode
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

//gulp.task('asmdebug', function(callback) {
//    runSequence('cleanASM', 'asmdebugcall', 'sync', 'asmWrapper', 'passToTest', callback);
//});
gulp.task('asmq', function(callback) {
    runSequence('umdNodeAsmFix', 'sync', 'asmWrapper', 'passToTest', callback);
});
gulp.task('asmc', function(callback) {
    runSequence('cleanASM', 'asmccall', 'umdFixes', 'sync', 'asmWrapper',  'passToTest', callback);
});

gulp.task('asmp', function(callback) {
    runSequence('cleanASM', 'asmpcall', 'umdFixes', 'sync', 'asmWrapper', 'passToTest', callback);
});
