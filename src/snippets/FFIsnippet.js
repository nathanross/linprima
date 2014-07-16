    var ffi = require("ffi");
    var _linprimaMod = ffi.Library("esp", {
        'tokenizeExtern': ['string', ['string', 'string']],
        'parseExtern': ['string', ['string', 'string']],
    });

