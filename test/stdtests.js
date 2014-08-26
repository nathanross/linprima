this;

null;

    42;

(1 + 2 ) * 3;

(1) + (2  ) + 3;

4 + 5 << (6);

x = [];

x = [ ];

x = [ 42 ];

x = [ 42, ];

x = [ ,, 42 ];

x = [ 1, 2, 3, ];

x = [ 1, 2,, 3, ];

日本語 = [];

x = {};

x = { };

x = { answer: 42 };

x = { if: 42 };

x = { true: 42 };

x = { false: 42 };

x = { null: 42 };

x = { "answer": 42 };

x = { x: 1, x: 2 };

x = { get width() { return m_width } };

x = { get undef() {} };

x = { get if() {} };

x = { get true() {} };

x = { get false() {} };

x = { get null() {} };

x = { get "undef"() {} };

x = { get 10() {} };

x = { set width(w) { m_width = w } };

x = { set if(w) { m_if = w } };

x = { set true(w) { m_true = w } };

x = { set false(w) { m_false = w } };

x = { set null(w) { m_null = w } };

x = { set "null"(w) { m_null = w } };

x = { set 10(w) { m_null = w } };

x = { get: 42 };

x = { set: 43 };

x = { __proto__: 2 };

x = {"__proto__": 2 };

x = { get width() { return m_width }, set width(width) { m_width = width; } };

/* block comment */ 42;

42 /* block comment 1 */ /* block comment 2 */;

var p1;/* block comment 1 */ /* block comment 2 */;

/*42*/;

(a + /* assignmenr */b ) * c;

/* assignmenr */;
 a = b;

42 /*The*/ /*Answer*/;

42 /*the*/ /*answer*/;

42 /* the * answer */;

42 /* The * answer */;

/* multiline;
comment;
should;
be;
ignored */ 42;

/*a;

b*/ 42;

/*a;
b*/ 42;

/*a;
b*/ 42;

/*a;
c*/ 42;

// one\n;

// line comment;
42;

42 // line comment;

// Hello, world!;
42;

// Hello, world!;

// Hallo, world!;

//;
42;

//;

// ;

/**/42;

42/**/;

// Hello, world!;

//   Another hello;
42;

if (x) { doThat() // Some comment;
 };

if (x) { // Some comment;
doThat(); };

if (x) { /* Some comment */ doThat() };

if (x) { doThat() /* Some comment */ };

switch (answer) { case 42: /* perfect */ bingo() };

switch (answer) { case 42: bingo() /* perfect */ };

/* header */ (function(){ var version = 1; }).call(this);

(function(){ var version = 1; /* sync */ }).call(this);

function f() { /* infinite */ while (true) { } /* bar */ var each; };

<!-- foo;

var x = 1<!--foo;

--> comment;

<!-- comment;

 --> comment;

 /* block comment */  --> comment;

/* block comment */--> comment;

/* not comment*/; i-->0;

while (i-->0) {};


0;

3;

5;

42;

.14;

3.14159;

6.02214179e+23;

1.492417830e-10;

0x0;

0x0;

0e+100 ;

0e+100;

0xabc;

0xdef;

0X1A;

0x10;

0x100;

0X04;

02;

012;

0012;

"Hello";

"\n\r\t\v\b\f\\\'\"\0";

"\u0061";

"\x61";

"\u00";

"\xt";

"Hello\nworld";

"Hello\02World";

"Hello\012World";

"Hello\122World";

"Hello\0122World";

"Hello\312World";

"Hello\412World";

"Hello\812World";

"Hello\712World";

"Hello\0World";


"Hello\1World";

"\u{714E}\u{8336}";

"\u{20BB7}\u{91CE}\u{5BB6}";

var x = /[a-z]/i;

var x = /[x-z]/i;

var x = /[a-c]/i;

var x = /[P QR]/i;

var x = /[\]/]/;

var x = /foo\/bar/;

var x = /=([^=\s])+/g;

var x = /42/g.test;

new Button;

new Button();

new new foo;

new new foo();

new foo().bar();

new foo[bar];

new foo.bar();

( new foo).bar();

foo(bar, baz);

(    foo  )();

universe.milkyway;

universe.milkyway.solarsystem;

universe.milkyway.solarsystem.Earth;

universe[galaxyName, otherUselessName];

universe[galaxyName];

universe[42].galaxies;

universe(42).galaxies;

universe(42).galaxies(14, 3, 77).milkyway;

earth.asia.Indonesia.prepareForElection(2014);

universe.if;

universe.true;

universe.false;

universe.null;

x++;

x--;

eval++;

eval--;

arguments++;

arguments--;

++x;

--x;

++eval;

--eval;

++arguments;

--arguments;

+x;

-x;

~x;

!x;

void x;

delete x;

typeof x;

x * y;

x / y;

x % y;

x + y;

x - y;

x << y;

x >> y;

x >>> y;

x < y;

x > y;

x <= y;

x >= y;

x in y;

x instanceof y;

x < y < z;

x == y;

x != y;

x === y;

x !== y;

x & y;

x ^ y;

x | y;

x + y + z;

x - y + z;

x + y - z;

x - y - z;

x + y * z;

x + y / z;

x - y % z;

x * y * z;

x * y / z;

x * y % z;

x % y * z;

x << y << z;

x | y | z;

x & y & z;

x ^ y ^ z;

x & y | z;

x | y ^ z;

x | y & z;

x || y;

x && y;

x || y || z;

x && y && z;

x || y && z;

x || y ^ z;

y ? 1 : 2;

x && y ? 1 : 2;

x = (0) ? 1 : 2;

() => "test";

e => "test";

(e) => "test";

(a, b) => "test";

e => { 42; };

e => ({ property: 42 });

e => { label: 42 };

(a, b) => { 42; };

(x=1) => x * x;

eval => 42;

arguments => 42;

(a) => 00;

(eval, a) => 42;

(eval = 10) => 42;

(eval, a = 10) => 42;

(x => x);

x => y => 42;

(x) => ((y, z) => (x, y, z));

foo(() => {});

foo((x, y) => {});

(sun) => earth;

x = 42;

eval = 42;

arguments = 42;

x *= 42;

x /= 42;

x %= 42;

x += 42;

x -= 42;

x <<= 42;

x >>= 42;

x >>>= 42;

x &= 42;

x ^= 42;

x |= 42;

a || b && c | d ^ e & f == g < h >>> i + j * k;

{ foo };

{ doThis(); doThat(); };

{};

var x;

var x, y;

var x = 42;

var eval = 42, arguments = 42;

var x = 14, y = 3, z = 1977;

var implements, interface, package;

var private, protected, public, static;

let x;

{ let x };

{ let x = 42 };

{ let x = 14, y = 3, z = 1977 };

const x = 42;

{ const x = 42 };

{ const x = 14, y = 3, z = 1977 };



x;

x, y;

\u0061;

a\u0061;

\u0061a;

\u0061a ;

if (morning) goodMorning();

if (morning) (function(){});

if (morning) var x = 0;

if (morning) function a(){};

if (morning) goodMorning(); else goodDay();

if (true) that();
 else;

if (true) that(); else;

do keep(); while (true);

do keep(); while (true);

do { x++; y--; } while (x < 10);

{ do { } while (false) false };

do that();while (true);

do that();
while (true);

while (true) doSomething();




for(x = 0; x < 42; x++) process(x);

for(x in list) process(x);

for (var x in list) process(x);

for (var x = 42 in list) process(x);

for (let x in list) process(x);

for (var x = y = z in q);

for (var a = b = c = (d in e) in z);

for (var i = function() { return 10 in [] } in list) process(x);

for (a[b in c] in d);

for (a(b in c)[0] in d);

for (a.in in a);

while (true) { continue; };

while (true) { continue };

done: while (true) { continue done };

done: while (true) { continue done; };

__proto__: while (true) { continue __proto__; };

while (true) { break };

done: while (true) { break done };

done: while (true) { break done; };

__proto__: while (true) { break __proto__; };

(function(){ return });

(function(){ return; });

(function(){ return x; });

(function(){ return x * y });

with (x) foo = bar;

with (x) foo = bar;

with (x) { foo = bar };

switch (x) {};

switch (answer) { case 42: hi(); break; };

switch (answer) { case 42: hi(); break; default: break };

start: while (true) break start;
t
__proto__: test;

throw x;

throw x * y;

throw { message: "Error" };

try { } catch (e) { };

try { } catch (eval) { };

try { } catch (arguments) { };

try { } catch (e) { say(e) };

try { } finally { cleanup(stuff) };

try { doThat(); } catch (e) { say(e) };

try { doThat(); } catch (e) { say(e) } finally { cleanup(stuff) };

debugger;

function hello() { sayHi(); };

function eval() { };

function arguments() { };

function test(t, t) { };

(function test(t, t) { });

function eval() { function inner() { "use strict" } };

function hello(a) { sayHi(); };

function hello(a, b) { sayHi(); };

var hi = function() { sayHi() };

var hi = function eval() { };

var hi = function arguments() { };

var hello = function hi() { sayHi() };

(function(){});

function universe(__proto__) { };

function test() { "use strict" + 42; };

x = function(y = 1) {};

function f(a = 1) {};

x = { f: function(a=1) {} };

