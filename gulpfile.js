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

function toShell(err, stdout, stderr) {
    console.log(stdout);
    console.log(stderr);
    if (err != null)
        { console.log(err); }
}

gulp.task('cleanTmp', function() {
    return gulp.src('tmp/*.cpp', {read: false})
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

function completeWrapper(mixMode, mixSnippetPath, dest) {
   return gulp.src('src/linprima-wrap.js')
    .pipe(mixotroph({mode:mixMode, snippetPath:mixSnippetPath}))
    .pipe(rename('linprima.js'))
    .pipe(gulp.dest(dest));
}

gulp.task('ffiWrapper', function() {
    return completeWrapper('FFI', 'src/snippets', 'build/ffi');
});

gulp.task('fficomp', ['cleanFFI', 'fficomp', 'ffiWrapper']);
gulp.task('ffiprod', ['cleanFFI', 'fficomprod', 'ffiWrapper']);


gulp.task('ffic', ['prepareSource'], function() {
    exec("clang++ -Wall -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", toShell);
});

gulp.task('ffip', ['prepareSource'], function() {
    exec("clang++ -O2 -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so", toShell);
});

function asmCompDone(err, stdout, stderr) {
    //asm requires code to be built before completing wrapper
    //as code is substituted into wrapper.
    toShell(err, stdout, stderr);
    completeWrapper('ASM', 'tmp/', 'build/asm/');

    //emscripten uses an export member on its module
    //which, when using emscripten's choice of UMD pattern
    //causes a name collision in NodeJS / commonJS module handling
    //leading the emscripten module itself to be returned 
    //rather than the wrapper.
    gulp.src('tmp/linprima.asm.0.js')
    .pipe(replace('export','emscport'))
    .pipe(rename('linprima.asm.js'))
    .pipe(gulp.dest('tmp'));
    gulp.src('build/asm/linprima.js')
    .pipe(rename('esprima.js'))
    .pipe(gulp.dest('esprima/esprima/'))
}

//#compiled code is shimmed into js module,
//#so this must be part of pipeline.
gulp.task('asmcopy', function() {
    asmCompDone(null, "", "");
});
gulp.task('asmc', ['cleanASM', 'prepareSource'], function() {
    exec("emcc -std=c++11 -s EXPORTED_FUNCTIONS=\"['_parseExtern', '_tokenizeExtern']\" tmp/src.cpp -o tmp/linprima.asm.0.js", asmCompDone);
});

gulp.task('asmp', ['cleanASM', 'prepareSource'], function() {
    exec("emcc -O3 -std=c++11 -s EXPORTED_FUNCTIONS=\"['_parseExtern', '_tokenizeExtern']\" tmp/src.cpp -o tmp/linprima.asm.0.js", asmCompDone);
});
