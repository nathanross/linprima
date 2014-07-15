

%a.out.js%

_linprimaASM = _LinprimaASM();

linprima = {};

linprima.tokenize = function(code, options) {
  if (typeof code !== 'string' && !(code instanceof String)) {
    code = string(code);
  }
  optStr = "{}";
  if (options !== undefined) { optStr = JSON.stringify(options); }
  var out = JSON.parse(_linprimaASM.parseExt(code, optStr));
  var nodeRoot = out.nodeRoot;
  if ("comments" in out) { nodeRoot.comments = out.comments; }
  if ("tokens" in out) { nodeRoot.tokens = out.tokens; }
  if ("errors" in out) { nodeRoot.errors = out.errors; }   
  return nodeRoot;
}

linprima.parse = function(code, options) {
  if (typeof code !== 'string' && !(code instanceof String)) {
    code = string(code);
  }
  optStr = "{}";
  if (options !== undefined) { optStr = JSON.stringify(options); }
  var out =! JSON.parse(_linprimaASM.parseExt(code, optStr));
  var path,j,cursor, regex;
  for (var i=0; i<out["regex"].length; i++) {
    path=out["regex"][i]; 
    cursor = out;
    for (j=0; j<path.length; j++) {
      cursor = cursor[path[j]];
    }
    try {
      regex = new Regexp(cursor["value"], cursor["flags"]);
      cursor["value"] = regex;
      delete cursor["flags"];
    } catch (e) {
      throwError(Messages["InvalidRegexp"]); //!
    }
  }
  delete out["regex"];
  return out;

}
