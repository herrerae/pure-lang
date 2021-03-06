
/* Copyright (c) 2008-2013 by Albert Graef <Dr.Graef@t-online.de>.

   This file is part of the Pure runtime.

   The Pure runtime is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or (at your
   option) any later version.

   Pure is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef EXPR_HH
#define EXPR_HH

#include "config.h"

#include <stdint.h>
#include <assert.h>
#ifdef USE_MPIR
#include <mpir.h>
#else
#include <gmp.h>
#endif

#include <bitset>
#include <list>
#include <map>
#include <set>
#include <string>

using namespace std;

/* Pure expression data structure and related stuff. */

/* Symbol precedences and fixity. Standard precedence levels of simple
   expressions are 0..PREC_MAX-1 (weakest to strongest, PREC_MAX is defined in
   runtime.h), inside each level infix (non-associative) operators have the
   lowest (0), postfix operators the highest (4) precedence.

   These are reconciled into a single figure, "normalized precedence"
   nprec(prec, fix), which ranges from nprec(0, 0) (weakest infix operator on
   level 0) to nprec(PREC_MAX-1, 4) (strongest postfix operator on the highest
   level).

   This scheme is extended to other syntactic constructions in expressions as
   follows. Primary expressions have normalized precedence NPREC_MAX =
   nprec(PREC_MAX, 0), applications an nprec value above all operators, but
   below the primaries, lambda nprec(-3, 0), case/when/with nprec(-2, 0), and
   if-then-else nprec(-1, 0) (these all bind weaker than any other operator).

   As a special case of primary expressions, a symbol may also have the
   special 'nonfix' and 'outfix' fixity values, indicating a constant symbol
   and user-defined brackets, respectively (with nprec = NPREC_MAX, just like
   any other primary expression). */

typedef int32_t prec_t;
// Don't change the order of these constants, some code depends on it!
enum fix_t { infix, infixl, infixr, prefix, postfix, outfix, nonfix };

/* Maximum precedence values. Note that these definitions are duplicated in
   runtime.h, so make sure that they match up. */

#define PREC_MAX	16777216
#define NPREC_MAX	167772160

/* Some special precedence values for internal use. */

#define NPREC_APP	167772155
#define NPREC_LAMBDA	(-30)
#define NPREC_CASE	(-20)
#define NPREC_COND	(-10)

/* Compute the normalized precedence for a given precedence and fixity, and
   the precedence level of the given normalized precedence. */

prec_t nprec(prec_t prec, fix_t fix = infix);
prec_t prec(prec_t nprec);

inline prec_t nprec(prec_t prec, fix_t fix)
{
  if (fix == outfix || fix == nonfix) fix = infix;
  return 10*prec+fix;
}

inline prec_t prec(prec_t nprec)
{
  return nprec%10;
}

/* Subexpression paths are used in pattern matching. These are actually
   implemented as bitsets where 0 = left, 1 = right subtree of an
   application. Paths to matrix subpatterns are indicated by a second bitset
   which is used as a mask. Maximum size of a path is given by the constant
   MAXDEPTH below. */

#define MAXDEPTH 128

class path {
  size_t size;
  bitset<MAXDEPTH> v;
  bitset<MAXDEPTH> m;
public:
  path() : size(0) { }
  path(size_t sz) : size(sz) { assert(size<=MAXDEPTH); }
  path(const path& p) : size(p.size), v(p.v), m(p.m) { }
  path(const path& p, bool n) : size(p.size+1), v(p.v), m(p.m)
  { assert(size<=MAXDEPTH); v[size-1] = n; m[size-1] = false; }
  path& operator+= (bool n)
  { assert(size<MAXDEPTH); v[size] = n; m[size++] = false; return *this; }
  bool operator== (const path& rhs) const
  { return size == rhs.size && v == rhs.v; }
  bool operator!= (const path& rhs) const
  { return size != rhs.size || v != rhs.v; }
  // subpath check
  bool operator<= (const path& rhs) const
  {
    if (size <= rhs.size) {
      for (size_t i = 0; i < size; i++)
	if (v[i] != rhs.v[i]) return false;
      return true;
    } else
      return false;
  }
  bool operator[] (size_t i) const
  { assert(i<MAXDEPTH); return v[i]; }
  bool msk(size_t i) const
  { assert(i<MAXDEPTH); return m[i]; }
  void set(size_t i, bool n)
  { assert(i<MAXDEPTH); v[i] = n; }
  void setmsk(size_t i, bool n)
  { assert(i<MAXDEPTH); m[i] = n; }
  bool last()  const { assert(size>0); return v[size-1]; }
  size_t len() const { return size; }
};

/* Smart expression pointers (see below for the full definition). These are
   used recursively as components in matrix representations and rule lists in
   the expression data structure. */

class expr;

/* Expression lists and lists of those. These are used to represent
   collections of expressions and generic matrix data in a structured way
   which facilitates code generation. In the case of exprll, each list member
   represents a matrix "row" which is in turn described by a list of
   "columns". Moreover, the exprset data structure is used to represent sets
   of expressions in some contexts. */

typedef list<expr> exprl;
typedef list<exprl> exprll;
typedef set<expr> exprset;

/* Auxiliary information on local variables in a rule. */

// type guards
struct vguard {
  int32_t tag, ttag;
  path p;
  vguard(int32_t _tag, int32_t _ttag, path _p)
    : tag(_tag), ttag(_ttag), p(_p) { }
};
typedef list<vguard> vguardl;

// variable equations (nonlinearities)
struct veqn {
  int32_t tag;
  path p, q;
  veqn(int32_t _tag, path _p, path _q)
    : tag(_tag), p(_p), q(_q) { }
};
typedef list<veqn> veqnl;

struct vinfo {
  vguardl guards;
  veqnl eqns;
  vinfo() { }
  vinfo(const vguardl& g, const veqnl& e) :guards(g), eqns(e) { }
};

/* Rule lists are used to encode collections of equations and other rule sets
   in 'case' expressions and the like. See the definition of the rule struct
   at the end of this header. */

struct rule;
typedef list<rule> rulel;

/* Environments provide a map of function and variable symbols to the
   corresponding rule sets (used in 'with' expressions and the global
   environment), subterm paths (used to perform variable binding for 'case'
   and 'when' clauses on the fly) and variable values (used in the global
   environment). See the definition of the env_info struct at the end of this
   header. */

struct env_info;
typedef map<int32_t,env_info> env;

/* The expression data structure. NOTE: EXPR is for internal use, only to be
   used via the smart pointer type expr below! */

struct matcher; // declared in matcher.hh

struct EXPR {

  // special type tags:
  enum {
    VAR		= 0,	// locally bound variable
    FVAR	= -1,   // locally bound function
    APP		= -2,	// function application
    // built-in (C) types:
    INT		= -3,	// 32 bit signed integer
    BIGINT	= -4,	// bigint (mpz_t)
    DBL		= -5,	// double precision floating point number
    STR		= -6,	// utf-8 string (char*)
    PTR		= -7,	// generic pointer (void*)
    // conditionals and binding expressions:
    COND	= -8,	// conditional expression (if-then-else)
    COND1	= -9,	// one-way conditional (guarded expression)
    LAMBDA	= -10,	// lambda expression
    CASE	= -11,	// case expression
    WHEN	= -12,	// when expression
    WITH	= -13,	// with expression
    // wrapper for embedded runtime expressions
    WRAP	= -14,	// pointer to variable binding (GlobalVar*)
    // GSL-compatible matrix types:
    MATRIX	= -32,  // generic GSL matrix, symbolic matrices
    DMATRIX	= -31,	// double matrix
    CMATRIX	= -30,	// complex matrix
    IMATRIX	= -29,	// integer matrix
    /* Other values in the range -17..-32 are reserved for later use in the
       runtime expression data structure. Note that all matrix-related tags,
       taken as an unsigned binary quantity, are of the form 0xffffffe0+t,
       where the least significant nibble t=0x0..0xf denotes corresponding
       subtypes in runtime matrix data. For compile time expressions only the
       EXPR::MATRIX tag (t=0) is used. */
  };

  // special flag values used during compilation:
  enum {
    OVF		= 1,	// overflowed int constant -> bigint
    PAREN	= 1<<1,	// parenthesized expression
    QUAL	= 1<<2,	// qualified id
    ASQUAL	= 1<<3,	// qualified id in "as" pattern
    // flags to control the pretty-printing of head symbols of rules
    LOCAL	= 1<<4,	// local symbol, suppress all namespace qualifiers
    GLOBAL	= 1<<5,	// global symbol, suppress default namespace qualifier
  };

  uint32_t refc;  // reference counter
  int32_t tag;	  // type tag or symbol

  // data:
  union {
    int32_t i;	  // INT
    mpz_t   z;    // BIGINT
    double  d;	  // DBL
    char   *s;	  // STR
    void   *p;	  // PTR
    struct {	  // VAR, FVAR
      int32_t vtag; // real symbol
      path   *p;    // subterm path (VAR)
      uint8_t idx;  // de Bruin index
    } v;
    EXPR   *x[3]; // APP, COND, COND1
    exprll *xs;   // MATRIX
    struct {      // LAMBDA
      exprl *xs;  // arguments
      rule  *r;   // rule (r->rhs is body)
    } l;
    struct {	  // CASE, WHEN, WITH
      EXPR  *x;   // expression
      union {
	rulel *r; // rule list (CASE, WHEN)
	env   *e; // function environment (WITH)
      };
    } c;
  } data;

  // matching automaton (LAMBDA, CASE; vector for WHEN):
  matcher *m;

  // compilation flags:
  uint16_t flags;

  // extra built-in type tag used in code generation:
  int32_t ttag;

  // "as" patterns:
  int32_t astag;
  path *aspath;

  EXPR *incref() { refc++; return this; }
  uint32_t decref() { if (refc > 0) --refc; return refc; }
  void del() { if (decref() == 0) delete this; }
  static EXPR *newref(EXPR *x) { return x?x->incref():0; }

  EXPR(int32_t _tag) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { data.p = 0; }
  EXPR(int32_t _tag, int32_t _vtag, uint8_t _idx,
       int32_t _ttag = 0, const path& _p = path()) :
    refc(0), tag(_tag), m(0), flags(0), ttag(_ttag), astag(0), aspath(0)
  { assert(_tag == VAR || _tag == FVAR);
    data.v.vtag = _vtag; data.v.idx = _idx;
    data.v.p = (_tag == VAR)?new path(_p):0; }
  EXPR(int32_t _tag, int32_t _i) :
    refc(0), tag(_tag), m(0), flags(0), ttag(_tag), astag(0), aspath(0)
  { assert(_tag == INT); data.i = _i; }
  EXPR(int32_t _tag, mpz_t _z, bool c = false) :
    refc(0), tag(_tag), m(0), flags(c?OVF:0), ttag(_tag), astag(0), aspath(0)
  { assert(_tag == BIGINT); mpz_init_set(data.z, _z); mpz_clear(_z); }
  EXPR(int32_t _tag, double _d) :
    refc(0), tag(_tag), m(0), flags(0), ttag(_tag), astag(0), aspath(0)
  { assert(_tag == DBL); data.d = _d; }
  explicit EXPR(int32_t _tag, char *_s) :
    refc(0), tag(_tag), m(0), flags(0), ttag(_tag), astag(0), aspath(0)
  { assert(_tag == STR); data.s = _s; }
  explicit EXPR(int32_t _tag, void *_p) :
    refc(0), tag(_tag), m(0), flags(0), ttag(_tag>0?0:_tag), astag(0), aspath(0)
  { assert(_tag > 0 || _tag == PTR || _tag == WRAP); data.p = _p; }
  EXPR(int32_t _tag, EXPR *_arg1, EXPR *_arg2, EXPR *_arg3) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { assert(_tag == COND);
    data.x[0] = newref(_arg1); data.x[1] = newref(_arg2);
    data.x[2] = newref(_arg3); }
  EXPR(int32_t _tag, EXPR *_arg1, EXPR *_arg2) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { assert(_tag == COND1);
    data.x[0] = newref(_arg1); data.x[1] = newref(_arg2); }
  EXPR(int32_t _tag, exprl *_xs, rule *_r) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { assert(_tag == LAMBDA); data.l.xs = _xs; data.l.r = _r; }
  EXPR(int32_t _tag, EXPR *_arg, rulel *_rules) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { assert(_tag == CASE || _tag == WHEN);
    data.c.x = newref(_arg); data.c.r = _rules; }
  EXPR(int32_t _tag, EXPR *_arg, env *_e) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { assert(_tag == WITH);
    data.c.x = newref(_arg); data.c.e = _e; }
  EXPR(int32_t _tag, exprll *_args) :
    refc(0), tag(_tag), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { assert(_tag == MATRIX); data.xs = _args; }
  EXPR(EXPR *_fun, EXPR *_arg) :
    refc(0), tag(APP), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { data.x[0] = newref(_fun); data.x[1] = newref(_arg); }
  EXPR(EXPR *_fun, EXPR *_arg1, EXPR *_arg2) :
    refc(0), tag(APP), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { data.x[0] = new EXPR(_fun, _arg1);
    data.x[0]->incref(); data.x[1] = newref(_arg2); }
  EXPR(EXPR *_fun, EXPR *_arg1, EXPR *_arg2, EXPR *_arg3) :
    refc(0), tag(APP), m(0), flags(0), ttag(0), astag(0), aspath(0)
  { data.x[0] = new EXPR(_fun, _arg1, _arg2);
    data.x[0]->incref(); data.x[1] = newref(_arg3); }

  ~EXPR();
};

/* Smart expression pointers. These take care of reference counting
   automagically. */

class expr {
  EXPR* p;
  // debug helper
  void debug(const string &msg);
  // smart pointer machinery
  EXPR* operator-> () const { assert(p); return p; }
  EXPR& operator*  () const { assert(p); return *p; }
  // hash table
  static map<EXPR*,uint32_t> h;
  static uint32_t key;

public:
  expr() : p(0) { }
  expr(EXPR* _p) : p(_p) { if (p) p->incref(); }
  ~expr() { if (p) { if (p->refc == 1) h.erase(p); p->del(); } }
  expr(const expr& x) : p(x.p) { if (p) p->incref(); }
  expr& operator= (const expr& x)
  {
    EXPR* const old = p;
    p = x.p;
    if (p) p->incref();
    if (old) old->del();
    return *this;
  }
  bool operator== (const expr& x) const
  { return p == x.p; }
  bool operator!= (const expr& x) const
  { return p != x.p; }
  bool operator< (const expr& x) const
  { return p < x.p; }
  bool operator> (const expr& x) const
  { return p > x.p; }
  bool operator<= (const expr& x) const
  { return p <= x.p; }
  bool operator>= (const expr& x) const
  { return p >= x.p; }

  // constructors
  expr(int32_t tag) :
    p(new EXPR(tag)) { p->incref(); }
  expr(int32_t tag, int32_t vtag, uint8_t idx,
       int32_t ttag = 0, const path& _p = path()) :
    p(new EXPR(tag, vtag, idx, ttag, _p)) { p->incref(); }
  expr(int32_t tag, int32_t i) :
    p(new EXPR(tag, i)) { p->incref(); }
  expr(int32_t tag, mpz_t z, bool c = false) :
    p(new EXPR(tag, z, c)) { p->incref(); }
  expr(int32_t tag, double d) :
    p(new EXPR(tag, d)) { p->incref(); }
  explicit expr(int32_t tag, char *s) :
    p(new EXPR(tag, s)) { p->incref(); }
  // Note: This constructor is also used for constant symbols with a cached
  // pure_expr* value, cf. interpreter::csubst and printer.cc:printx.
  explicit expr(int32_t tag, void *_p) :
    p(new EXPR(tag, _p)) { p->incref(); }
  expr(int32_t tag, expr arg1, expr arg2) :
    p(new EXPR(tag, &*arg1, &*arg2)) { p->incref(); }
  expr(int32_t tag, expr arg1, expr arg2, expr arg3) :
    p(new EXPR(tag, &*arg1, &*arg2, &*arg3)) { p->incref(); }
  expr(int32_t tag, exprl *args, rule *r) :
    p(new EXPR(tag, args, r)) { p->incref(); }
  expr(int32_t tag, expr arg, rulel *rules) :
    p(new EXPR(tag, &*arg, rules)) { p->incref(); }
  expr(int32_t tag, expr arg, env *e) :
    p(new EXPR(tag, &*arg, e)) { p->incref(); }
  expr(int32_t tag, exprll *args) :
    p(new EXPR(tag, args)) { p->incref(); }
  expr(expr fun, expr arg) :
    p(new EXPR(&*fun, &*arg)) { p->incref(); }
  expr(expr fun, expr arg1, expr arg2) :
    p(new EXPR(&*fun, &*arg1, &*arg2)) { p->incref(); }
  expr(expr fun, expr arg1, expr arg2, expr arg3) :
    p(new EXPR(&*fun, &*arg1, &*arg2, &*arg3)) { p->incref(); }

  // static methods to generate special types of expressions
  static expr lambda(exprl *xs, expr y, vinfo vi = vinfo());
  static expr cond(expr x, expr y, expr z);
  static expr cond1(expr x, expr y);
  static expr cases(expr x, rulel *rules);
  static expr when(expr x, rulel *rules);
  static expr with(expr x, env *e);
  static expr nil();
  static expr cons(expr x, expr y);
  static expr list(const exprl& xs);
  static expr voidx();
  static expr pair(expr x, expr y);
  static expr tuple(const exprl& xs);

  // generates a unique id for each expr (valid for its lifetime)
  uint32_t hash();

  // access functions
  uint32_t refc()  const { return p->refc; }
  int32_t  tag()   const { return p->tag; }
  int32_t  ttag()  const { return p->ttag; }
  int32_t  vtag()  const { assert(p->tag == EXPR::VAR || p->tag == EXPR::FVAR);
                           return p->data.v.vtag; }
  int32_t  ftag()  const { return (p->tag == EXPR::FVAR)?p->data.v.vtag:
				  p->tag; }
  uint8_t  vidx()  const { assert(p->tag == EXPR::VAR || p->tag == EXPR::FVAR);
                           return p->data.v.idx; }
  path    &vpath() const { assert(p->tag == EXPR::VAR || p->tag == EXPR::FVAR);
                           return *p->data.v.p; }
  int32_t  ival()  const { assert(p->tag == EXPR::INT);
                           return p->data.i; }
  mpz_t   &zval()  const { assert(p->tag == EXPR::BIGINT);
                           return p->data.z; }
  double   dval()  const { assert(p->tag == EXPR::DBL);
                           return p->data.d; }
  char    *sval()  const { assert(p->tag == EXPR::STR);
                           return p->data.s; }
  void    *pval()  const { assert(p->tag > 0 ||
				  p->tag == EXPR::PTR || p->tag == EXPR::WRAP);
                           return p->data.p; }
  expr     xval1() const { assert(p->tag == EXPR::APP ||
				  p->tag == EXPR::COND ||
				  p->tag == EXPR::COND1);
                           return expr(p->data.x[0]); }
  expr     xval2() const { assert(p->tag == EXPR::APP ||
				  p->tag == EXPR::COND ||
				  p->tag == EXPR::COND1);
                           return expr(p->data.x[1]); }
  expr     xval3() const { assert(p->tag == EXPR::COND);
                           return expr(p->data.x[2]); }
  exprl   *largs() const { assert(p->tag == EXPR::LAMBDA);
                           return p->data.l.xs; }
  rule    &lrule() const { assert(p->tag == EXPR::LAMBDA);
                           return *p->data.l.r; }
  expr     xval()  const { assert(p->tag == EXPR::CASE ||
				  p->tag == EXPR::WHEN ||
				  p->tag == EXPR::WITH);
                           return expr(p->data.c.x); }
  exprll  *xvals() const { assert(p->tag == EXPR::MATRIX);
                           return p->data.xs; }
  rulel   *rules() const { assert(p->tag == EXPR::CASE ||
				  p->tag == EXPR::WHEN);
                           return p->data.c.r; }
  env     *fenv()  const { assert(p->tag == EXPR::WITH);
                           return p->data.c.e; }
  matcher *&pm()   const { assert(p->tag == EXPR::LAMBDA ||
				  p->tag == EXPR::CASE ||
				  p->tag == EXPR::WHEN);
                           return p->m; }
  uint16_t&flags() const { return p->flags; }
  int32_t  astag() const { return p->astag; }
  path   &aspath() const { assert(p->aspath); return *p->aspath; }

  void set_tag(int32_t tag) { p->tag = tag; }
  void set_ttag(int32_t tag) { p->ttag = tag; }
  void set_astag(int32_t tag) { p->astag = tag; }
  void set_aspath(const path& _p)
  { if (p->aspath) delete p->aspath; p->aspath = new path(_p); }

  bool is_guarded() const;

  bool is_null()   const { return p==0; }
  bool is_fun()    const { return p->tag > 0; }
  bool is_var()    const { return p->tag == EXPR::VAR; }
  bool is_fvar()   const { return p->tag == EXPR::FVAR; }
  bool is_int()    const { return p->tag == EXPR::INT; }
  bool is_dbl()    const { return p->tag == EXPR::DBL; }
  bool is_ptr()    const { return p->tag == EXPR::PTR; }
  bool is_app()    const { return p->tag == EXPR::APP; }
  bool is_lambda() const { return p->tag == EXPR::LAMBDA; }
  bool is_cond()   const { return p->tag == EXPR::COND; }
  bool is_cond1()  const { return p->tag == EXPR::COND1; }
  bool is_case()   const { return p->tag == EXPR::CASE; }
  bool is_when()   const { return p->tag == EXPR::WHEN; }
  bool is_with()   const { return p->tag == EXPR::WITH; }

  bool is_fun(int32_t& f) const
  { if (p->tag > 0) {
      f = p->tag;
      return true;
    } else
      return false;
  }
  bool is_var(int32_t& v, uint8_t& idx, int32_t& ttag, path& _p) const
  { if (p->tag == EXPR::VAR) {
      v = p->data.v.vtag;
      idx = p->data.v.idx;
      ttag = p->ttag;
      _p = *p->data.v.p;
      return true;
    } else
      return false;
  }
  bool is_fvar(int32_t& v, uint8_t& idx) const
  { if (p->tag == EXPR::FVAR) {
      v = p->data.v.vtag;
      idx = p->data.v.idx;
      return true;
    } else
      return false;
  }
  bool is_int(int32_t &i) const
  { if (p->tag == EXPR::INT) {
      i = p->data.i;
      return true;
    } else
      return false;
  }
  bool is_dbl(double &d) const
  { if (p->tag == EXPR::DBL) {
      d = p->data.d;
      return true;
    } else
      return false;
  }
  bool is_ptr(void *&_p) const
  { if (p->tag == EXPR::PTR || p->tag == EXPR::WRAP) {
      _p = p->data.p;
      return true;
    } else
      return false;
  }
  bool is_app(expr &x, expr &y) const
  { if (p->tag == EXPR::APP) {
      x = expr(p->data.x[0]);
      y = expr(p->data.x[1]);
      return true;
    } else
      return false;
  }
  bool is_lambda(exprl *&xs, rule *&r, matcher *&m) const
  { if (p->tag == EXPR::LAMBDA) {
      xs = p->data.l.xs;
      r = p->data.l.r;
      m = p->m;
      return true;
    } else
      return false;
  }
  bool is_cond(expr &x, expr &y, expr &z) const
  { if (p->tag == EXPR::COND) {
      x = expr(p->data.x[0]);
      y = expr(p->data.x[1]);
      z = expr(p->data.x[2]);
      return true;
    } else
      return false;
  }
  bool is_cond1(expr &x, expr &y) const
  { if (p->tag == EXPR::COND1) {
      x = expr(p->data.x[0]);
      y = expr(p->data.x[1]);
      return true;
    } else
      return false;
  }
  bool is_case(expr &x, rulel *&r, matcher *&m) const
  { if (p->tag == EXPR::CASE) {
      x = expr(p->data.c.x);
      r = p->data.c.r;
      m = p->m;
      return true;
    } else
      return false;
  }
  bool is_when(expr &x, rulel *&r, matcher *&m) const
  { if (p->tag == EXPR::WHEN) {
      x = expr(p->data.c.x);
      r = p->data.c.r;
      m = p->m;
      return true;
    } else
      return false;
  }
  bool is_with(expr &x, env *&e) const
  { if (p->tag == EXPR::WITH) {
      x = expr(p->data.c.x);
      e = p->data.c.e;
      return true;
    } else
      return false;
  }
  bool is_nil()    const;
  bool is_cons()   const;
  bool is_list()   const;
  bool is_voidx()  const;
  bool is_pair()   const;
  bool is_cons(expr &x, expr &y) const;
  bool is_list(exprl &xs) const;
  bool is_list2(exprl &xs, expr& tl) const;
  bool is_pair(expr &x, expr &y) const;
  bool is_tuple(exprl &xs) const;
  // Special check for tuples used in list construction.
  bool is_tuplel(exprl &xs) const;
  // Special checks for tuples and lists without "as" patterns on the spine,
  // needed by the printer.
  bool is_tuplep(exprl &xs) const;
  bool is_list2p(exprl &xs, expr& tl) const;
};

/* Rules of the form: lhs -> rhs [if qual]. */

struct rule {
  expr lhs, rhs, qual;
  vinfo vi;
  uint32_t temp;
  rule(expr l, expr r, expr q = expr(), uint32_t t = 0)
    : lhs(l), rhs(r), qual(q), temp(t) { }
  rule(expr l, expr r, const vinfo& _vi, expr q = expr(), uint32_t t = 0)
    : lhs(l), rhs(r), qual(q), vi(_vi), temp(t) { }
};

/* Environment entries. */

struct env_info {
  enum { none, lvar, cvar, fvar, fun } t;
  uint32_t temp;
  union {
    // local variable binding (lvar):
    struct {
      int32_t ttag;
      path *p;
    };
    // constant definition (cvar):
    struct {
      expr *cval;
      // As of Pure 0.38, we cache non-scalar constants in a global variable.
      // This holds the runtime expression pointer and the corresponding
      // shadowed variable.
      void *cval_var, *cval_v;
    };
    // free variable definition (fvar):
    void *val; // pointer to memory location holding a runtime expression
    // function definition (fun):
    struct {
      uint32_t argc;
      rulel *rules;
      matcher *m;
      // interface types only (Pure 0.50+)
      exprl *xs;
      rulel *rxs;
      matcher *mxs;
      // This is used to keep track of the warning levels in effect when the
      // interface description was originally parsed, in order to give more
      // precise control over the warning diagnostics produced for an
      // interface definition.
      exprset *compat;
    };
  };
  env_info() : t(none) { }
  env_info(int32_t _ttag, path _p, uint32_t _temp = 0)
    : t(lvar), temp(_temp), ttag(_ttag), p(new path(_p)) { }
  env_info(expr x, uint32_t _temp = 0)
    : t(cvar), temp(_temp), cval(new expr), cval_var(0), cval_v(0)
  { *cval = x; }
  env_info(void *v, uint32_t _temp = 0)
    : t(fvar), temp(_temp), val(v) { }
  env_info(uint32_t c, rulel r, uint32_t _temp = 0)
    : t(fun), temp(_temp), argc(c), rules(new rulel(r)), m(0),
      xs(0), rxs(0), mxs(0), compat(0) { }
  env_info(const env_info& e);
  env_info& operator= (const env_info& e);
  ~env_info();
};

#endif // ! EXPR_HH
