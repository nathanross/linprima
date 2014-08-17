
requirejs.config({
    baseUrl: './scripts/', 
    paths: { 
        lib : './bower_components/',
        build : './build/',
        jquery : 'bower_components/jquery/dist/jquery'
    }
});
var simplebench = requirejs(['simplebench']);

var runWeb = function(path, filelist) {
    if (!('jquery' in this)) {
        this.jquery = require('jquery');
    }
    var getFileCallback = function (fname) { 
        
    };
    return this.runCmp(filelist, getFileCallback);
};
