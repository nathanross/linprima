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
    return gulp.src('src/linprima.cpp')
    .pipe(gulp.dest('tmp'));
});

gulp.task('throw52', function(callback) { //gperftools
    exec("python src/throw52.py -v 0 src/linprima.cpp tmp/linprima.cpp", 
        makeExecCallback(callback));
});

gulp.task('prepareSource', function() {
    return gulp.src(['lib/libjson*','tmp/newlinebuffer.c','tmp/linprima.cpp'])
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

gulp.task('ffigcall', function(callback) { //gperftools
    exec("clang++ -fno-builtin -g -O0 -Wall -D THROWABLE -std=c++11 -stdlib=libc++  -ltcmalloc tmp/src.cpp -o test.out", 
        makeExecCallback(callback));
});
gulp.task('ffidcall', function(callback) { //gdb, valgrind
    exec("clang++ -g -O0 -Wall -D THROWABLE -std=c++11 -stdlib=libc++ tmp/src.cpp -o test.out", 
        makeExecCallback(callback));
});
gulp.task('fficcall', function(callback) {
    if (argv.t52 !== undefined) {
        exec("clang++ -Wall -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", 
        makeExecCallback(callback));
    } else {
        exec("clang++ -Wall -D THROWABLE -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", 
        makeExecCallback(callback));
    }
});

gulp.task('ffipcall', function(callback) {
    exec("clang++ -O2 -D THROWABLE -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", 
        makeExecCallback(callback));
});



gulp.task('ffiWrapper', function() {
    return completeWrapper('FFI', 'src/snippets', 'build/ffi');
});
gulp.task('ffi', function() { //gdb and valgrind debugging
    runSequence('cleanFFI', 
               (argv.t52 !== undefined)? 'throw52':'noThrow52',
                'prepareSource', 
               (argv.p !== undefined)? 'ffipcall':
               ((argv.g !== undefined)? 'ffigcall': //call like this: gulp ffi --g
               ((argv.d !== undefined)? 'ffidcall': 'fficcall')),
                'ffiWrapper');
});

    //asm requires code to be built before completing wrapper
    //as code is substituted into wrapper.
gulp.task('asmccall', function(callback) {
    var cb = function(a,b,c) { toShell(a,b,c); callback(); };
    exec("emcc -std=c++11 -O2 -s DISABLE_EXCEPTION_CATCHING=2 -s RELOOPER_BUFFER_SIZE=419430400 -s EXPORTED_FUNCTIONS=\"['_parseASMJS', '_tokenizeASMJS', '_someTest']\" tmp/src.cpp -o tmp/linprima.asm.0.js", 
        makeExecCallback(callback));
});

gulp.task('asmpcall', function(callback) {
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
                'throw52',
                'prepareSource',
                (argv.p !== undefined) ? 'asmpcall' : 'asmccall', 
                'umdFixes', 
                'sync',
                'asmWrapper',  
                'passToTest', 
                callback);
});

