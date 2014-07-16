var gulp = require('gulp');
//var argv = require('yargs').argv;
var concat = require('gulp-concat');
var exec = require('gulp-exec');
//var uglify = require('gulp-uglify');
var clean = require('gulp-clean');
var mixotroph = require('gulp-mixotroph');
//var lazypipe = require('lazypipe');


gulp.task('cleanTmp', function() {
    return gulp.src("tmp/*.cpp", {read: false})
        .pipe(clean());
});
gulp.task('cleanASM', ['cleanTmp'], function() {
    return gulp.src("build/ASM/*", {read: false})
        .pipe(clean());
});
gulp.task('cleanFFI', ['cleanTmp'], function() {
    return gulp.src("build/FFI/*", {read: false})
        .pipe(clean());
});

gulp.task('cleanAll', ["cleanASM", "cleanFFI"]);

gulp.task('prepareSource', function() {
    gulp.src(["lib/libjson*","tmp/newlinebuffer.c","src/linprima.cpp"])
    .pipe(concat("src.cpp"))
    .pipe(gulp.dest("tmp"));
});

gulp.task('ffiWrapper', function() {
   return gulp.src("src/linprima-wrap.js")
    .pipe(mixotroph({mode:"FFI", snippetdir:"tmp/"}))
    .pipe(gulp.dest("build/ffi/"));
});

gulp.task('fficomp', ["cleanFFI", "fficomp", "ffiWrapper"]);
gulp.task('ffiprod', ["cleanFFI", "fficomprod", "ffiWrapper"]);


gulp.task('ffic', ['prepareSource'], function() {
    return gulp.src("")
        .pipe(exec("clang++ -Wall -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so"));
});

gulp.task('ffip', ['prepareSource'], function() {
    return gulp.src("")
        .pipe(exec("clang++ -O2 -std=c++11 -stdlib=libc++ -shared -fPIC tmp/src.cpp -o build/ffi/linprima.x64.so"));
});

//#compiled code is shimmed into js module,
//#so this must be part of pipeline.
gulp.task('asmc', ['prepareSource'], function() {
    return gulp.src("src/linprima.js")
        .pipe(exec("emcc -std=c++11 -s EXPORTED_FUNCTIONS=\"['parseExtern', 'tokenizeExtern']\" tmp/src.cpp -o tmp/lin.asm.js"))
        .pipe(mixotroph({mode:"ASM",snippetdir:"tmp/"}))
        .pipe(gulp.dest("build/asm/"));
});

gulp.task('asmp', ['cleanASM', 'prepareSource'], function() {
    return gulp.src("src/linprima.js")
        .pipe(exec("emcc -O3 -std=c++11 -s EXPORTED_FUNCTIONS=\"['parseExtern', 'tokenizeExtern']\" tmp/src.cpp -o tmp/lin.asm.js"))
        .pipe(mixotroph({mode:"ASM",snippetdir:"tmp/"}))
        .pipe(gulp.dest("build/asm/"));
});
