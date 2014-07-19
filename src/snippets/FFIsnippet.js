    var ffi = require("ffi");
    var _linprimaMod = ffi.Library("linprima", {
        'tokenizeExtern': ['string', ['string', 'string']],
        'parseExtern': ['string', ['string', 'string']],
    });

