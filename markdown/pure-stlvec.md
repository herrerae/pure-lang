<a name="doc-pure-stlvec"></a>

pure-stlvec
===========

<a name="module-stlvec"></a>

<a name="module-stlvecalgorithms"></a>

Version 0.4, March 06, 2017

| Peter Summerland &lt;<p.summerland@gmail.com>&gt;

Pure's interface to C++ vectors, specialized to hold pointers to arbitrary
Pure expressions, and the C++ Standard Template Library algorithms that act on
them.

Copying
-------

| Copyright (c) 2011 by Peter Summerland &lt;<p.summerland@gmail.com>&gt;.

All rights reserved.

pure-stlvec is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

pure-stlvec is distributed under a BSD-style license, see the COPYING file for
details.

Installation
------------

pure-stlvec-0.4 is included in the "umbrella" addon,
[pure-stllib](pure-stllib.html), which is available at
<https://bitbucket.org/purelang/pure-lang/downloads>. After you have
downloaded and installed [pure-stllib](pure-stllib.html), you will be able to
use pure-stlvec (and [pure-stlmap](pure-stlmap.html), as well).

Overview
--------

The C++ Standard Template Library ("STL") is a library of generic containers
(data structures designed for storing other objects) and a rich set of generic
algorithms that operate on them. pure-stlvec provides an interface to one of
its most useful containers, "vector", adopted to hold pointers to Pure
expressions. The interface provides Pure programmers with a mutable container
"stlvec", that, like the STL's vector, holds a sequence of objects that can be
accessed in constant time according to their position in the sequence.

### Modules

The usual operations for creating, accessing and modifying stlvecs are
provided by the stlvec module. Most of the operations are similar in name and
function to those provided by the Pure Library for other containers. As is the
case for their Pure Library counterparts, these operations are in the global
namespace. There are a few operations that have been placed in the stl
namespace usually because they do not have Pure Library counterparts.

In addition to the stlvec module, pure-stlvec provides a group of modules,
stlvec::modifying, stlvec::nonmodifying, stlvec::sort, stlvec::merge,
stlvec::heap, stlvec::minmax and stlvec::numeric, that are straight wrappers
the STL algorithms (specialized to work with STL vectors of pointers to Pure
expressions). This grouping of the STL algorithms follows that found at
<http://www.cplusplus.com/reference/algorithm/>. This web page contains a
table that summarizes of all of the algorithms in one place.

pure-stlvec provides an "umbrella" module,
[stlvec::algorithms](#module-stlvec::algorithms), that pulls in all of the STL
algorithm interface modules in one go. The STL algorithm wrapper functions
reside in the stl namespace and have the same names as their counterparts in
the STL.

### Simple Examples

Here are some examples that use the basic operations provided by the stlvec
module.

    > using stlvec;

    > let sv1 = stlvec (0..4); members sv1;
    [0,1,2,3,4]

    > insert (sv1,stl::svend) (5..7); members sv1;
    STLVEC #<pointer 0xaf4d2c0>
    [0,1,2,3,4,5,6,7]

    > sv1!3;
    3

    > sv1!![2,4,6];
    [2,4,6]

    > replace sv1 3 33; members sv1;
    STLVEC #<pointer 0xaf4d2c0>
    [0,1,2,33,4,5,6,7]

    > stl::erase (sv1,2,5); members sv1;
    STLVEC #<pointer 0xaf4d2c0>
    [0,1,5,6,7]

    > insert (sv1,2) [2,3,4];  members sv1;
    STLVEC #<pointer 0xaf4d2c0>
    [0,1,2,3,4,5,6,7]

    > let pure_vector = stl::vector (sv1,1,5); pure_vector;
    {1,2,3,4}

    > stlvec pure_vector;
    STLVEC #<pointer 0x9145a38>

    > members ans;
    [1,2,3,4]

     > map (+10) sv1;
    [10,11,12,13,14,15,16,17]

    > map (+10) (sv1,2,5);
    [12,13,14]

    > foldl (+) 0 sv1;
    28

    > [x+10 | x = sv1; x mod 2];
    [11,13,15,17]

    > {x+10 | x = (sv1,2,6); x mod 2};
    {13,15}

Here are some examples that use STL algorithms.

    > using stlvec::algorithms;

    > stl::reverse (sv1,2,6); members sv1;
    ()
    [0,1,5,4,3,2,6,7]

    > stl::stable_sort sv1 (>); members sv1;
    ()
    [7,6,5,4,3,2,1,0]

    > stl::random_shuffle sv1; members sv1 1;
    ()
    [1,3,5,4,0,7,6,2]

    > stl::partition sv1 (<3); members (sv1,0,ans); members sv1;
    3
    [1,2,0]
    [1,2,0,4,5,7,6,3]

    > stl::transform sv1 (sv1,0) (*2); members sv1;
    -1
    [2,4,0,8,10,14,12,6]

    > let sv2 = emptystlvec;

    > stl::transform sv1 (sv2,stl::svback) (div 2); members sv2;
    -1
    [1,2,0,4,5,7,6,3]

Many more examples can be found in the pure-stlvec/ut directory.

### Members and Sequences of Members

Throughout the documentation for pure-stlvec, the member of a stlvec that is
at the nth position in the sequence of expressions stored in the stlvec is
referred to as its nth member or nth element. The nth member of a stlvec, sv,
is sometimes denoted by sv!n. The sequence of members of sv starting at
position i up to but not including j is denoted by sv\[i,j). There is a
"past-the-end" symbol, stl::svend, that denotes the position after that
occupied by the last member contained by a stlvec.

For example, if sv contains the sequence "a", "b", "c" "d" and "e", sv!0 is
"a", sv\[1,3) is the sequence consisting of "b" followed by "c" and
v\[3,stl::svend) denotes the sequence consisting of "d" followed by "e".

### STL Iterators and Value Semantics

In C++ a programmer accesses a STL container's elements by means of
"iterators", which can be thought of as pointers to the container's elements.
A single iterator can be used to access a specific element, and a pair of
iterators can be used to access a "range" of elements. By convention, such a
range includes the member pointed to by the first iterator and all succeeding
members up to but not including the member pointed to by the second iterator.
Each container has a past-the-end iterator that can be used to specifiy ranges
that include the container's last member.

In the case of vectors there is an obvious correspondence between an iterator
that points to an element and the element's position (starting at zero) in the
vector. pure-stlvec uses this correspondence to designate a stlvec's members
in a way that makes it relatively easy to see how pure-stlvec's functions are
acting on the stlvec's underlying STL vector by referencing the STL's
documentation. Thus, if sv is a stlvec, and j is an int, "replace sv j x" uses
the STL to replace the element pointed to by the iterator for position j of
sv's underlying STL vector. If, in addition, k is an int, stl::sort (sv,j,k)
(&lt;) uses the STL to sort the elements in the range designated by the "jth"
and "kth" iterators for sv's underlying STL vector. This range, written as
sv\[j,k), is the subsequence of sv that begins with the element at position j
and ends with the element at position (k-1).

Besides iterators, another cornerstone of the STL is its "value semantics",
i.e., all of the STL containers are mutable and if a container is copied, all
of its elements are copied. pure-stlvec deals with the STL's value semantics
by introducing mutable and nonmutable stlvecs, and by storing smart pointers
to objects (which have cheap copies) rather than the actual objects.

### Iterator Tuples

As mentioned in the previous section, in C++ ranges are specified by a pair of
STL iterators.

In pure-stlvec ranges of elements in a stlvec are specified by "iterator
tuples" rather than, say, actual pointers to STL iterators. Iterator tuples
consist of the name of a stlvec followed by one of more ints that indicate
positions (starting from zero) of the stlvec's elements.

To illustrate how iterator tuples are used, consider the STL stable\_sort
function, which sorts objects in the range \[first, last) in the order imposed
by comp. Its C++ signature looks like this:

> void stable\_sort ( RandomAccessIterator first, RandomAccessIterator last,
> Compare comp )

The corresponding pure-stlvec function, from the stlvec::sort module, looks
like this:

> stable\_sort (msv, first, last) comp

where msv is a mutable stlvec, and first and last are ints. The first thing
that the Pure stable\_sort does is create a pair of C++ iterators that point
to the elements in msv's underlying STL vector that occupy the positions
designated by first and last. Next it wraps the Pure comp function in a C++
function object that, along with the two iterators, is passed to the C++
stable\_sort function.

For convenience, (sv,stl::svbeg, stl::svend) can be written simply as sv.
Thus, if first were stl::svbeg (or 0), and last were stl::svend (or \#msv, the
number of elements in msv), the last Pure call could be written:

> stable\_sort msv comp

It should be noted that often the STL library provides a default version of
its functions, which like stable\_sort, use a comparator or other callback
function provided by the caller. E.g., the C++ stable\_sort has a default
version that assumes the "&lt;" operator can be used on the elements held by
the container in question:

> void stable\_sort ( RandomAccessIterator first, RandomAccessIterator last)

The corresponding functions provided by the pure-stlvec modules rarely, if
ever, supply a default version. A typical example is stlvec::sort's
stable\_sort which must be called with a comparator callback function:

> stable\_sort msv (&lt;);

Note also that the comparator (e.g., (&lt;)), or other function being passed
to a pure-stlvec algorithm wrapper is almost always the last parameter. This
is the opposite of what is required for similar Pure functions, but is
consistent with the STL calling conventions.

### Predefined Iterator Tuple Indexes

The following integer constants are defined in the stl namespace for use in
iterator tuples.

<a name="stl::svbeg"></a>*constant* `stl::svbeg = 0`, <a name="stl::svend"></a>`stl::svend = -1`, <a name="stl::svback"></a>`stl::svback = -2`

:   <!-- -->

These three symbols are declared as nonfix. `svend` corresponds to STL's
past-end iterator for STL vectors. It makes it possible to specify ranges that
include the last element of an stlvec. I.e., the iterator tuple
(sv,stl::svbeg,stl::svend) would specify sv\[0,n), where n is the number of
elements in sv. In order to understand the purpose of `svback`, it is
necessary to understand a bit about STL's "back insert iterators."

### Back Insert Iterators

Many of the STL algorithms insert members into a target range designated by an
iterator that points to the first member of the target range. Consistent with
raw C usage, it is ok to copy over existing elements the target stlvec. E.g.,:

    > using stlvec::modifying;

    > let v1 = stlvec (0..2);

    > let v2 = stlvec ("a".."g");

    > stl::copy v1 (v2,2) $$ members v2;
    ["a","b",0,1,2,"f","g"]

This is great for C++ programmers, but for Pure programmers it is almost
always preferable to append the copied items to the end of a target stlvec,
rather than overwriting all or part or part of it. This can be accomplished
using stl::svback. E.g.,:

    > stl::copy v1 (v2,stl::svback) $$ members v2;
    ["a","b",0,1,2,"f","g",0,1,2]

In short, when a pure-stlvec function detects "stl::svback" in a target
iterator tuple, it constructs a STL "back inserter iterator" and passes it on
to the corresponding wrapped STL function.

### Data Structure

Currently, stlvecs are of the form (STLVEC x) or (CONST\_STLVEC x), where
STLVEC AND CONST\_STLVEC are defined as nonfix symbols in the global namespace
and x is a pointer to the underlying STL vector. The stlvec module defines
corresponding type tags, stlvec and const\_stlvec, so the programmer never
needs to worry about the underlying representaton.

This representation may change in the future, and must not be relied upon by
client modules. In particular, one must never attempt to use the embedded
pointer directly.

As the names suggest, stlvecs are mutable and const\_stlvecs are immutable.
Functions that modify a stlvec will simply fail unless the stlvec is mutable.

    > let v = const_stlvec $ stlvec (0..3); v2;
    CONST_STLVEC #<pointer 0x8c1dbf0>

    > replace v 0 100; // fails
    replace (CONST_STLVEC #<pointer 0x9f07690> 0 100

### Types

pure-stlvec introduces six type tags, all of which are in the global
namespace:

<a name="mutable_stlvec/type"></a>*type* `mutable_stlvec`
:   The type for a mutable stlvec.

<a name="const_stlvec/type"></a>*type* `const_stlvec`
:   The type for an immutable stlvec.

<a name="stlvec/type"></a>*type* `stlvec`
:   The type for a stlvec, mutable or immutable.

<a name="mutable_svit/type"></a>*type* `mutable_svit`
:   The type for an iterator tuple whose underlying stlvec is mutable.

<a name="const_svit/type"></a>*type* `const_svit`
:   The type for an iterator tuple whose underlying stlvec is immutable.

<a name="svit/type"></a>*type* `svit`
:   The type for an iterator tuple. The underlying stlvec can be mutable or
    immutable.

<!-- -->
### Copy-On-Write Semantics

The pure-stlvec module functions do not implement automatic copy-on-write
semantics. Functions that modify stlvec parameters will simply fail if they
are passed a const\_stlvec when they expect a mutable\_stlvec.

For those that prefer immutable data structures, stlvecs can be converted to
const\_stlvecs (usually after they have been created and modified within a
function) by the `const_stlvec` function. This function converts a mutable
stlvec to an immutable stlvec without changing the underlying STL vector.

Typically, a "pure" function that "modifies" a stlvec passed to it as an
argument will first copy the input stlvec to a new locally scoped (mutable)
stlvec using the stlvec function. It will then modify the new stlvec and use
const\_stlvec to make the new stlvec immutable before it is returned. It
should be noted that several of the STL algorithms have "copy" versions which
place their results directly into a new stlvec, which can eliminate the need
to copy the input stlvec. E.g.:

    > let sv1 = stlvec ("a".."e");

    > let sv2 = emptystlvec;

    > stl::reverse_copy sv1 (sv2,stl::svback) $$ members sv2;
    ["e","d","c","b","a"]

Without reverse\_copy, one would have had to copy sv1 into sv2 and then
reverse sv2.

If desired, in Pure it is easy to write functions that have automatic
copy-on-write semantics. E.g.,

    > my_replace csv::const_stlvec i x = my_replace (stlvec csv) i x;
    > my_replace sv::stlvec i x = replace sv i x;

### Documentation

The pure-stllib/doc directory includes a rudimentary cheatsheet,
pure-stllib-cheatsheet.pdf, that shows the signatures of all of the functions
provided by pure-stlvec (and by [pure-stlmap](pure-stlmap.html) as well).

The documentation of the functions provided by the stlvec module are
reasonably complete. In contrast, the descriptions of functions provided by
the STL algorithm modules are purposely simplified (and may not, therefore, be
technically accurate). This reflects that fact that the functions provided by
pure-stlvec have an obvious correspondence to the functions provided by the
STL, and the STL is extremely well documented. Furthermore, using the Pure
interpreter, it is very easy to simply play around with with any of the
pure-stlvec functions if there are doubts, especially with respect to "corner
cases." Often this leads to a deeper understanding compared to reading a
precise technical description.

A good book on the STL is STL Tutorial and Reference Guide, Second Edition, by
David R. Musser, Gillmer J. Derge and Atul Saini. A summary of all of the STL
algorithms can be found at <http://www.cplusplus.com/reference/stl/>.

### Parameter Names

In the descriptions of functions that follow, parameter names used in function
descriptions represent specific types of Pure objects:

sv

:   stlvec (mutable or immutable)

csv

:   const (i.e., immutable) stlvec

msv

:   mutable stlvec

x

:   an arbitrary Pure expression

xs

:   a list of arbitrary Pure expressions

count, sz, n

:   whole numbers to indicate a number of elements, size of a vector, etc

i,j

:   whole numbers used to designate indexes into a stlvec

f,m,l

:   whole numbers (or stl::beg or stl::svend) designating the "first",
    "middle" or "last" iterators in a stlvec iterator tuple

p

:   a whole number (or other iterator constant such as stl::svend or
    stl::svback) used in a two element iterator tuple (e.g., (sv,p))

(sv,p)

:   an iterator tuple that will be mapped to an iterator that points to the
    pth position of sv's underlying STL vector, v, (or to a back iterator on v
    if p is stl::svback)

(sv,f,l)

:   an iterator tuple that will be mapped to the pair of iterators that are
    designated by (sv,f) and (sv,l)

(sv,f,m,l)

:   an iterator tuple that will be mapped to the iterators that are designated
    by (sv,f), (sv,m) and (sv,l)

sv\[f,l)

:   the range of members beginning with that at (sv,f) up to but not including
    that at (con,l)

comp

:   a function that accepts two objects and returns true if the first argument
    is less than the second (in the strict weak ordering defined by comp), and
    false otherwise

unary\_pred

:   a function that accepts one object and returns true or false

bin\_pred

:   a function that accepts two objects and returns true or false

unary\_fun

:   a function that accepts one objects and returns another

bin\_fun

:   a function that accepts two objects and returns another

gen\_fun

:   a function of one parameter that produces a sequence of objects, one for
    each call

For readability, and to correspond with the STL documentation, the words
"first", "middle", and "last", or variants such as "first1" are often used
instead of f,m,l.

Error Handling
--------------

The functions provided this module handle errors by throwing exceptions.

### Exception Symbols

<a name="bad_argument/stlvec"></a>`bad_argument`
:   This exception is thrown when a function is passed an unexpected value. A
    subtle error to watch for is a malformed iterator tuple (e.g., one with
    the wrong number of elements).

<a name="bad_function/stlvec"></a>`bad_function`
:   This exception is thrown when a purported Pure call-back function is not
    even callable.

<a name="failed_cond/stlvec"></a>`failed_cond`
:   This exception is thrown when a Pure call-back predicate returns a value
    that is not an int.

<a name="out_of_bounds/stlvec"></a>`out_of_bounds`
:   This exception is thrown if the specified index is out of bounds.

<a name="range_overflow/stlvec"></a>`range_overflow`
:   This exception is thrown by functions that write over part of a target
    stlvec (e.g., copy) when the target range too small to accommodate the
    result.

<a name="range_overlap/stlvec"></a>`range_overlap`
:   This exception is thrown by algorithm functions that write over part of a
    target stlvec when the target and source ranges overlap in a way that is
    not allowed.

<!-- -->
In addition, any exception thrown by a Pure callback function passed to a
pure-stlvec function will be caught and be rethrown by the pure-stlvec
function.

### Examples

    > using stlvec, stlvec::modifying;

    > let sv1 = stlvec (0..4); members sv1;
    [0,1,2,3,4]

    > let sv2 = stlvec ("a".."e"); members sv2;
    ["a","b","c","d","e"]

    > sv1!10;
    <stdin>, line 25: unhandled exception 'out_of_bounds' ...

    > stl::copy sv1 (sv2,10);
    <stdin>, line 26: unhandled exception 'out_of_bounds' ...

    > stl::copy sv1 (sv2,2,3); // sb (sv2,pos)
    <stdin>, line 22: unhandled exception 'bad_argument' ...

    > stl::copy sv1 (sv2,2);
    <stdin>, line 23: unhandled exception 'range_overflow' ...

    > stl::copy sv2 (sv2,2);
    <stdin>, line 24: unhandled exception 'range_overlap' ...

    > stl::copy (sv1,1,3) (sv2,0); members sv2; // ok
    2
    [1,2,"c","d","e"]

    > stl::sort sv2 (>); // apples and oranges 
    <stdin>, line 31: unhandled exception 'failed_cond'

    > listmap (\x->throw DOA) sv1; // callback function throws exception
    <stdin>, line 34: unhandled exception 'DOA' ...

Operations Included in the stlvec Module
----------------------------------------

The stlvec module provides functions for creating, accessing and modifying
stlvecs. In general, operations that have the same name as a corresponding
function in the Pure standard library are in the global namespace. The
remaining functions, which are usually specific to stlvecs, are in the stl
namespace.

Please note that "stlvec to stlvec" functions are provided by the pure-stl
algorithm modules. Thus, for example, the stlvec module does not provide a
function that maps one stlvec onto a new stlvec. That functionality, and more,
is provided by stl::transform, which can be found in the stlvec::modifying
module.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec;

### Operations in the Global Namespace

When reading the function descriptions that follow, please bear in mind that
whenever a function is passed an iterator tuple of the form (sv,first, last),
first and last can be dropped, leaving (sv), or simply sv. The function will
treat the "unary" iterator tuple (sv) as (sv, stl::svbeg, stl::svend).

<a name="emptystlvec/stlvec"></a>`emptystlvec`
:   return an empty stlvec

<a name="stlvec"></a>`stlvec source /stlvec`
:   create a new stlvec that contains the elements of source; source can be a
    stlvec, an iterator tuple(sv,first,last), a list or a vector (i.e., a
    matrix consisting of a single row or column). The underlying STL vector is
    always a new STL vector. I.e., if source is a stlvec the new stlvec does
    not share source's underlying STL vector.

<a name="mkstlvec/stlvec"></a>`mkstlvec x count`
:   create a new stlvec consisting of count x's.

<a name="const_stlvec/stlvec"></a>`const_stlvec source`
:   create a new const\_stlvec that contains the elements of source; source
    can be a stlvec, an iterator tuple(sv,first,last), a list or a vector
    (i.e., a matrix consisting of a single row or column). If source is a
    stlvec (mutable or const), the new const\_stlvec shares source's
    underlying STL vector.

<a name="#/stlvec"></a>`# sv`
:   return the number of elements in sv.

<!-- -->
Note that \# applied to an iterator tuple like (sv,b,e) will just return the
number of elements in the tuple. Use stl::bounds if you need to know the
number of elements in the range denoted by an iterator tuple.

<a name="!/stlvec"></a>`sv ! i`
:   return the ith member of sv

<!-- -->
Note that !k applied to an iterator tuple like (sv,b,e) will just return the
kth element of the tuple. In addition, in stlvec, integers used to denote
postions (as in !k) or in iterators, *always*, are relative to the beginning
of the underlying vector. So it makes no sense to apply ! to an iterator
tuple.

<a name="first/stlvec"></a>`first sv`, <a name="last/stlvec"></a>`last sv`
:   first and last member of sv

<a name="members/stlvec"></a>`members (sv, first, last)`
:   return a list of values stored in sv\[first,last)

<a name="replace/stlvec"></a>`replace msv i x`
:   replace the ith member of msv by x and return x; throws out\_of\_bounds if
    i is less than 0 or great or equal to the number of elements in msv

<a name="update/stlvec"></a>`update msv i x`
:   the same as replace except that update returns msv instead of x. This
    function is DEPRECATED.

<a name="append/stlvec"></a>`append sv x`
:   append x to the end of sv

<a name="insert/stlvec"></a>`insert (msv,p) xs`, <a name="insert/stlvec"></a>`insert (msv,p) (sv,first,last)`
:   insert members of the list xs or the range sv\[first, last) into msv, all
    preceding the pth member of msv. Members are shifted to make room for the
    inserted members

<a name="rmfirst/stlvec"></a>`rmfirst msv`, <a name="rmlast/stlvec"></a>`rmlast msv`
:   remove the first or last member from msv

<a name="erase/stlvec"></a>`erase (msv,first,last)`, <a name="erase/stlvec"></a>`erase (msv,p)`, <a name="erase/stlvec"></a>`erase msv`
:   remove msv\[first,last) from msv, remove msv!p from msv, or make msv
    empty. Members are shifted to occupy vacated slots

<a name="==/stlvec"></a>`sv1 == sv2`, <a name="~=/stlvec"></a>`sv1 ~= sv2`
:   (x == y) is the same as stl::allpairs (==) x y and x \~= y is simply
    \~(allpairs (==) x y)

<!-- -->
Note that `==` and `~==` are not defined for iterator tuples (the rules would
never be executed because == is defined on tuples in the Prelude).

The stlvec module provides convenience functions that apply map, catmap,
foldl, etc, to directly access Pure expressions stored in a stlvec.

<a name="map/stlvec"></a>`map unary_fun (sv, first, last)`
:   one pass equivalent of map unary\_fun \$ members (sv, first, last)

<a name="listmap/stlvec"></a>`listmap unary_fun (sv, first, last)`
:   same as map, used in list comprehensions

<a name="catmap/stlvec"></a>`catmap unary_fun (sv, first, last)`
:   one pass equivalent of catmap unary\_fun \$ members (sv, first, last)

<a name="do/stlvec"></a>`do unary_fun (sv, first, last)`
:   one pass equivalent of do unary\_fun \$ members (sv, first, last)

<a name="foldl/stlvec"></a>`foldl bin_fun x (sv, first, last)`
:   one pass equivalent of foldl bin\_fun x \$ members (sv, first, last)

<a name="foldl1/stlvec"></a>`foldl1 bin_fun (sv, first, last)`
:   one pass equivalent of foldl1 bin\_fun \$ members (sv, first, last)

<a name="filter/stlvec"></a>`filter unary_pred (sv, first, last)`
:   one pass equivalent of filter unary\_pred \$ members (sv, first, last)

<!-- -->
The following four functions map (or catmap) stlvecs onto row and col
matrixes, primarily for use in matrix comprehensions.

<a name="rowmap/stlvec"></a>`rowmap unary_fun (sv, first, last)`, <a name="rowcatmap/stlvec"></a>`rowcatmap unary_fun (sv, first, last)`, <a name="colmap/stlvec"></a>`colmap unary_fun (sv, first, last)`, <a name="colcatmap/stlvec"></a>`colcatmap unary_fun (sv, first, last)`

:   <!-- -->

### Operations in the stl Namespace

<a name="stl::empty/stlvec"></a>`stl::empty sv`
:   test whether sv is empty

<a name="stl::vector/stlvec"></a>`stl::vector (sv,first,last)`
:   create a Pure vector that contains the members of sv\[first,last)

<a name="stl::allpairs/stlvec"></a>`stl::allpairs bin_pred (sv1, first1, last1) (sv2, first2, last2)`
:   returns true if bin\_pred is true for all corresponding members of
    sv1\[first1, last1) and sv2\[first2, last2)

<a name="stl::bounds/stlvec"></a>`stl::bounds (sv,first,last)`
:   throws out-of-bounds if first or last is out of bounds. returns the tuple
    (sv,first,last) except that if first is stl::begin it will be replaced by
    0 and if last is stl::svend it will be replaced by the number of elements
    in sv.

<a name="stl::reserve/stlvec"></a>`stl::reserve msv count`
:   modify the underlying STL vector to have at least count slots, useful for
    packing data into a fixed size vector and possibly to speed up the
    addition of new members

<a name="stl::capacity/stlvec"></a>`stl::capacity sv`
:   return the number of slots (as opposed to the number of elements) held by
    the underlying STL vector

<!-- -->
### Examples

See ut\_stlvec.pure and ut\_global\_stlvec.pure in the pure-stlvec/ut
directory.

STL Nonmodifying Algorithms
---------------------------

The stlvec::nonmodifying module provides an interface to the STL's
non-modifying sequence operations.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::nonmodifying;

All of the functions are in the stl namespace.

### Operations

<a name="stl::for_each/stlvec"></a>`stl::for_each (sv, first, last) unary_fun`
:   applies unary\_fun to each of the elements in sv\[first,last)

<a name="stl::find/stlvec"></a>`stl::find (sv, first, last) x`
:   returns the position of the first element in sv\[first,last) for which
    (==x) is true (or stl::svend if not found)

<a name="stl::find_if/stlvec"></a>`stl::find_if (sv, first, last) unary_pred`
:   returns the position of the first element in sv\[first,last) for which
    unary\_pred is true (or stl::svend if not found)

<a name="stl::find_first_of/stlvec"></a>`stl::find_first_of (sv1, first1, last1) (sv2, first2, last2) bin_pred`
:   Returns the position of the first element, x, in sv1\[first1,last1) for
    which there exists y in sv2\[first2,last2) and (bin\_pred x y) is true (or
    stl::svend if no such x exists).

<a name="stl::adjacent_find/stlvec"></a>`stl::adjacent_find (sv, first, last) bin_pred`
:   search sv\[first,last) for the first occurrence of two consecutive
    elements (x,y) for which (bin\_pred x y) is true. Returns the position of
    x, if found, or stl::svend if not found)

<a name="stl::count/stlvec"></a>`stl::count (sv, first, last) x`
:   returns the number of elements in the range sv\[first,last) for which
    (x==) is true

<a name="stl::count_if/stlvec"></a>`stl::count_if (sv, first, last) unary_pred`
:   returns the number of elements in the range sv\[first,last) for which
    unary\_pred is true

<a name="stl::mismatch/stlvec"></a>`stl::mismatch (sv1, first1, last1) (sv2, first2) bin_pred`
:   applies bin\_pred pairwise to the elements of sv1\[first1,last1) and
    (sv2,first2,first2 + n), with n equal to last1-first1 until it finds i and
    j such that bin\_pred (sv1!i) (sv2!j) is false and returns (i,j). If
    bin\_pred is true for all of the pairs of elements, i will be stl::svend
    and j will be first2 + n (or stl::svend)

<a name="stl::equal/stlvec"></a>`stl::equal (sv1, first1, last1) (sv2, first2) bin_pred`
:   applies bin\_pred pairwise to the elements of sv1\[first1,last1) and
    (sv2,first2,first2 + n), with n equal to last1-first1, and returns true if
    bin\_pred is true for each pair

<a name="stl::search/stlvec"></a>`stl::search (sv1, first1, last1) (sv2, first2) bin_pred`
:   using bin\_pred to determine equality of the elements, searches
    sv1\[first1,last1) for the first occurrence of the sequence defined by
    sv2\[first2,last2), and returns the position in sv1 of its first element
    (or stl::svend if not found)

<a name="stl::search_n/stlvec"></a>`stl::search_n (sv, first, last) count x bin_pred`
:   using bin\_pred to determine equality of the elements, searches
    sv\[first,last) for a sequence of count elements that equal x. If such a
    sequence is found, it returns the position of the first of its elements,
    otherwise it returns stl::svend

<a name="stl::find_end/stlvec"></a>`stl::find_end (sv1, first1, last1) (sv2, first2, last2) bin_pred`
:   using bin\_pred to determine equality of the elements, searches
    sv1\[first1,last1) for the last occurrence of sv2\[first2,last2). Returns
    the position of the first element in sv1 of the occurrence (or stl::svend
    if not found).

<!-- -->
### Examples

See ut\_nonmodifying.pure in the pure-stlvec/ut directory.

STL Modifying Algorithms
------------------------

The stlvec::modifying module provides an interface to the STL's modifying
algorithms.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::modifying;

All of the functions are in the stl namespace.

### Operations

<a name="stl::copy/stlvec"></a>`stl::copy (sv, first1, last1) (msv, first2)`
:   copies the elements in sv\[first1,last1) into the range whose first
    element is (msv,first2)

<a name="stl::copy_backward/stlvec"></a>`stl::copy_backward (sv,first1,last1) (msv,last2)`
:   copies the elements in sv\[first1,last1), moving backward from (last1),
    into the range msv\[first2,last2) where first2 is last2 minus the number
    of elements in sv\[first1,last1)

<a name="stl::swap_ranges/stlvec"></a>`stl::swap_ranges (sv,first,last) (msv, p)`
:   exchanges the elements in sv\[first, last) with those in msv\[p, p+n)
    where n is last - first

<a name="stl::transform/stlvec"></a>`stl::transform (sv,first,last) (msv, p) unary_fun`
:   applies unary\_fun to the elements of sv\[first,last) and places the
    resulting sequence in msv\[p, p+n) where n is last - first. If sv is
    mutable, msv and sv can be the same stlvec. Returns (msv,p+n)

<a name="stl::transform_2/stlvec"></a>`stl::transform_2 (sv1,first1,last1) (sv2,first2) (msv, p) bin_fun`
:   applies bin\_fun to corresponding pairs of elements of sv1\[first1,last1)
    sv2\[first2,n) and and places the resulting sequence in msv\[p, p+n) where
    n is last1 -first1. Returns (msv,p+n)

<a name="stl::replace_if/stlvec"></a>`stl::replace_if (msv,first,last) unary_pred x`
:   replace the elements of msv\[first,last) that satistfy unary\_pred with x

<a name="stl::replace_copy/stlvec"></a>`stl::replace_copy (sv,first,last) (msv,p) x y`
:   same as [`replace`](#stl::replace/stlvec) (msv,first,last) x y except that
    the modified sequence is placed in msv\[p,p+last-first)

<a name="stl::replace_copy_if/stlvec"></a>`stl::replace_copy_if (sv,first,last) (msv,p) unary_pred x`
:   same as [`replace_if`](#stl::replace_if/stlvec) except that the modified
    sequence is placed in msv\[p,p+last-first)

<a name="stl::fill/stlvec"></a>`stl::fill (msv,first,last) x`
:   replace all elements in msv\[first,last) with x

<a name="stl::fill_n/stlvec"></a>`stl::fill_n (msv,first) n x`
:   replace the elements of msv\[first,first+n) with x

<a name="stl::generate/stlvec"></a>`stl::generate (msv,first,last) gen_fun`

:   replace the elements in msv\[first,last) with the sequence generated by
    successive calls to gen\_fun (), e.g.,

        > let count = ref 0;

        > g _ = n when n = get count + 1; put count n; end;

        > let sv = mkstlvec 0 10;

        > stl::generate sv g $$ members sv;
        [1,2,3,4,5,6,7,8,9,10]

<a name="stl::generate_n/stlvec"></a>`stl::generate_n (msv,first) n gen_fun`
:   replace all elements in msv\[first,first+n) with the sequence generated by
    successive calls to gen\_fen

<a name="stl::remove/stlvec"></a>`stl::remove (msv,first,last) x`
:   same as [`remove_if`](#stl::remove_if/stlvec) (msv,first,last) (==x).

<a name="stl::remove_if/stlvec"></a>`stl::remove_if (msv,first,last) unary_pred`
:   remove elements in msv\[first,last) that satisfy unary\_pred. If n
    elements do not satisfy unary\_pred, they are moved to
    msv\[first,first+n), preserving their relative order. The content of
    msv\[first+n,svend) is undefined. Returns first+n, or stl::svend if
    first+n is greater than the number of elements in msv

<a name="stl::remove_copy/stlvec"></a>`stl::remove_copy (sv,first,last) (msv,first) x`
:   same as [`remove`](#stl::remove/stlvec) except that the purged sequence is
    copied to (msv,first) and sv\[first,last) is not changed

<a name="stl::remove_copy_if/stlvec"></a>`stl::remove_copy_if (sv,first,last) (msv,first) unary_pred`
:   same as [`remove_if`](#stl::remove_if/stlvec) except that the purged
    sequence is copied to (msv,first) and sv\[first,last) is not changed

<a name="stl::unique/stlvec"></a>`stl::unique (msv,first,last) bin_pred`
:   eliminates consecutive duplicates from sv\[first,last), using bin\_pred to
    test for equality. The purged sequence is moved to sv\[first,first+n)
    preserving their relative order, where n is the size of the purged
    sequence. Returns first+n or stl::svend if first+n is greater than the
    number of elements in msv

<a name="stl::unique_copy/stlvec"></a>`stl::unique_copy (sv,first,last) (msv,first) bin_pred`
:   same as [`unique`](#stl::unique/stlvec) except that the purged sequence is
    copied to (msv,first) and sv\[first,last) is not changed

<a name="stl::reverse/stlvec"></a>`stl::reverse (msv,first,last)`
:   Reverses the order of the elements in sv\[first,last).

<a name="stl::reverse_copy/stlvec"></a>`stl::reverse_copy (sv,first,last) (msv,first)`
:   same as [`reverse`](#stl::reverse/stlvec) except that the reversed
    sequence is copied to (msv,first) and sv\[first,last) is not changed.

<a name="stl::rotate/stlvec"></a>`stl::rotate (msv,first,middle,last)`
:   rotates the elements of msv\[first,middle,last\] so that middle becomes
    the first element of msv\[first,last\].

<a name="stl::rotate_copy/stlvec"></a>`stl::rotate_copy (msv,first,middle,last) (msv,first)`
:   same as rotate except that the rotated sequence is copied to (msv,first)
    and sv\[first,last) is not changed.

<a name="stl::random_shuffle/stlvec"></a>`stl::random_shuffle (msv,first,last) int::seed`
:   randomly reorders the elements in msv\[first,last)

<a name="stl::partition/stlvec"></a>`stl::partition (msv,first,last) unary_pred`
:   places the elements in msv\[first,last) that satisfy unary\_pred before
    those that don't. Returns middle, where msv \[first,middle) contains all
    of the elements that satisfy unary\_pre, and msv \[middle, last) contains
    those that do not

<a name="stl::stable_partition/stlvec"></a>`stl::stable_partition (msv,first,last) unary_pred`
:   same as partition except that the relative positions of the elements in
    each group are preserved

<!-- -->
### Examples

See ut\_modifying.pure in the pure-stlvec/ut directory.

STL Sort Algorithms
-------------------

The stlvec::sort module provides an interface to the STL's sorting and binary
search algorithms.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::sort;

All of the functions are in the stl namespace.

### Operations

All of the functions in this module require the caller to supply an ordering
function, comp. The functions (&lt;) and (&gt;) are commonly passed as comp.

<a name="stl::sort/stlvec"></a>`stl::sort (msv, first, last) comp`
:   sorts msv\[first, last)

<a name="stl::stable_sort/stlvec"></a>`stl::stable_sort (msv, first, last) comp`
:   sorts msv\[first, last), preserving the relative order of equal members

<a name="stl::partial_sort/stlvec"></a>`stl::partial_sort (msv, first, middle, last) comp`
:   fills msv\[first, middle) with the elements of msv\[first,last) that would
    appear there if msv\[first,last) were sorted using comp and fills
    msv\[middle,last) with the remaining elements in unspecified order

<a name="stl::partial_sort_copy/stlvec"></a>`stl::partial_sort_copy (sv, first1, last1) (msv, first2, last2) comp`
:   let n be the number of elements in sv\[first1, last1) and r be the number
    of elements in msv\[first2, last2). If r &lt; n,
    [`partial_sort_copy`](#stl::partial_sort_copy/stlvec) fills msv\[first2,
    last2) with the first r elements of what sv\[first1, last1) would be if it
    had been sorted. If r &gt;= n, it fills msv\[first2, first2+n) with the
    elements of sv\[first1, last1) in sorted order. sv\[first1,last1) is
    unchanged

<a name="stl::nth_element/stlvec"></a>`stl::nth_element (msv, first, middle, last) comp`
:   rearranges the elements of msv\[first, last) as follows. Let n be middle
    -first, and let x be the nth smallest element of msv\[first, last). After
    the function is called, sv!middle will be x. All of the elements of
    msv\[first, middle) will be less than x and all of the elements of
    msv\[middle+1, last) will be greater than x

<!-- -->
The next four functions assume that sv\[first, last) is ordered by comp.

<a name="stl::lower_bound/stlvec"></a>`stl::lower_bound (sv, first, last) x comp`
:   returns an int designating the first position into which x can be inserted
    into sv\[first, last) while maintaining the sorted ordering

<a name="stl::upper_bound/stlvec"></a>`stl::upper_bound (sv, first, last) x comp`
:   returns an int designating the last position into which x can be inserted
    into sv\[first, last) while maintaining the sorted ordering

<a name="stl::equal_range/stlvec"></a>`stl::equal_range (sv, first, last) x comp`
:   returns a pair of ints, (lower, upper) where lower and upper would have
    been returned by separate calls to lower\_bound and upper\_bound.

<a name="stl::binary_search/stlvec"></a>`stl::binary_search (sv, first, last) x comp`
:   returns true if x is an element of sv\[first, last)

<!-- -->
### Examples

See ut\_sort.pure in the pure-stlvec/ut directory.

STL Merge Algorithms
--------------------

The stlvec::merge module provides an interface to the STL's merge algorithms.
These algorithms operate on sorted ranges.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::merge;

All of the functions are in the stl namespace.

### Operations

All of the functions in this module require the caller to supply an ordering
function, comp (as for the Pure library sort function). They only work
properly on input ranges that have been previously sorted using comp. The set
operations generally do not check for range overflow because it is not
generally possible to determine the length of the result of a set operation
until after it is completed. In most cases you will get a nasty segmentation
fault if the result is bigger than the target range. The best way to avoid
this possibility it to use a back iterator to specifify the target range.

See parameter naming conventions at ..

<a name="stl::merge/stlvec"></a>`stl::merge (sv1,first1,last1) (sv2,first2,last2) (msv,p) comp`
:   merges the two sorted ranges into the sorted range msv\[p,p+n) where n is
    the total length of the merged sequence

<a name="stl::inplace_merge/stlvec"></a>`stl::inplace_merge (msv,first, middle, last) comp`
:   merges msv\[first,middle) and msv\[middle,last) into the sorted range
    msv\[first,last)

<a name="stl::includes/stlvec"></a>`stl::includes (sv1,first1,last1) (sv2,first2,last2) comp`
:   returns true if every element of sv2\[first2,last2) is an element of
    sv1\[first1,last1)

<a name="stl::set_union/stlvec"></a>`stl::set_union (sv1,first1,last1) (sv2,first2,last2) (msv,p) comp`
:   places the sorted union of sv1\[first1,last1) and sv2\[first2,last2) into
    msv\[p,p+n) where n is the number of elements in the sorted union, and
    returns the past-the-end position of the sorted union

<a name="stl::set_intersection/stlvec"></a>`stl::set_intersection (sv1,first1,last1) (sv2,first2,last2) (msv,p) comp`
:   places the sorted intersection of sv1\[first1,last1) and
    sv2\[first2,last2) into msv\[p,p+n) where n is the number of elements in
    the sorted intersection, and returns p+n (or stl::svend, if applicable)

<a name="stl::set_difference/stlvec"></a>`stl::set_difference (sv1,first1,last1) (sv2,first2,last2) (msv,p) comp`
:   places the sorted difference of sv1\[first1,last1) and sv2\[first2,last2)
    into msv\[p,p+n) where n is the number of elements in the sorted
    difference, and returns p+n (or stl::svend, if applicable)

<a name="stl::set_symmetric_difference/stlvec"></a>`stl::set_symmetric_difference (sv1,first1,last1) (sv2,first2,last2) (msv,p) comp`
:   places the sorted symmetric\_difference of sv1\[first1,last1) and
    sv2\[first2,last2) into msv\[p,p+n) where n is the number of elements in
    the sorted symmetric\_difference, and returns returns p+n (or stl::svend,
    if applicable)

<!-- -->
### Examples

See ut\_merge.pure in the pure-stlvec/ut directory.

STL Heap Algorithms
-------------------

The stlvec::heap module provides an interface to the STL's heap operations.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::heap;

All of the functions are in the stl namespace.

### Operations

All of the functions in this module require the caller to supply an ordering
function, comp (as for the Pure library sort function). The functions (&lt;)
and (&gt;) are commonly passed as comp.

<a name="stl::make_heap/stlvec"></a>`stl::make_heap (msv,first,last) comp`
:   rearranges the elements of msv\[first,last) so that they are a heap, i.e.,
    after this msv!first will be the largest element in msv\[first,last), and
    push\_heap and pop\_heap will work properly

<a name="stl::push_heap/stlvec"></a>`stl::push_heap (msv,first,last) comp`
:   makes msv\[first,last) a heap (assuming that msv\[first,last-1) was a
    heap)

<a name="stl::pop_heap/stlvec"></a>`stl::pop_heap (msv,first,last) comp`
:   swaps msv!first with msv!(last-1), and makes msv\[first,last-1) a heap
    (assuming that msv\[first,last) was a heap)

<a name="stl::sort_heap/stlvec"></a>`stl::sort_heap (msv,first,last) comp`
:   sorts the elements in msv\[first,last)

<!-- -->
### Examples

See ut\_heap.pure in the pure-stlvec/ut directory.

Min/Max STL Algorithms
----------------------

The stlvec::minmax module provides an interface to a few additional STL
algorithms.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::minmax;

All of the functions are in the stl namespace.

### Operations

All of the functions in this module require the caller to supply an ordering
function, comp (as for the Pure library sort function). The functions (&lt;)
and (&gt;) are commonly passed as comp.

<a name="stl::min_element/stlvec"></a>`stl::min_element (sv,first,last) comp`
:   returns the position of the minimal element of sv\[first,last) under the
    ordering defined by comp

<a name="stl::max_element/stlvec"></a>`stl::max_element (sv,first,last) comp`
:   returns the position of the maximal element of sv\[first,last) under the
    ordering defined by comp

<a name="stl::lexicographical_compare/stlvec"></a>`stl::lexicographical_compare (sv1,first1,last1) (sv2,first2,last2) comp`
:   compares sv1\[first1,last1) and sv2\[first2,last2) element by element
    according to the ordering defined by comp, and returns true if the first
    sequence is less than the second

<!-- -->
Algorithms are provided for stepping through all the permutations the elements
of a stlvec. For these purposes, the first permutation has the elements of
msv\[first,last) sorted in ascending order and the last has the elements
sorted in descending order.

<a name="stl::next_permutation/stlvec"></a>`stl::next_permutation (msv,first,last) comp`
:   rearranges msv\[first,last) to produce the next permutation, in the
    ordering imposed by comp. If the elements of the next permutation is
    ordered (ascending or decending) by comp, return false. Otherwise return
    true.

<a name="stl::prev_permutation/stlvec"></a>`stl::prev_permutation (msv,first,last) comp`
:   next\_permutation in reverse

<!-- -->
### Examples

See ut\_minmax.pure in the pure-stlvec/ut directory.

STL Numeric Algorithms
----------------------

The stlvec::numeric module provides an interface to the STL's numeric
algorithms.

### Imports

To use the operations of this module, add the following import declaration to
your program:

    using stlvec::numeric;

All of the functions are in the stl namespace.

### Operations

<a name="stl::accumulate/stlvec"></a>`stl::accumulate (sv,first,last) x bin_fun`
:   accumulate bin\_fun over x and the members of sv\[first,last), like foldl

<a name="stl::inner_product/stlvec"></a>`stl::inner_product (sv1,first1,last1) (sv2,first2,last2) x bin_fun1 bin_fun2`
:   initialize ret with x. Traverse pairs of elements of sv1\[first1,last1)
    and sv2\[first2,last2), denoted by (e1, e2), replacing ret with (bin\_fun1
    ret \$ bin\_fun2 e1 e2). The number pairs traversed is equal to the size
    of sv1\[first1,last1)

<a name="stl::partial_sum/stlvec"></a>`stl::partial_sum (sv,first,last) (msv, p) bin_fun`
:   accumulate bin\_fun f over the elements of sv1\[first1,last1), placing
    itermediate results in msv\[p,p+n), where n is last - first, and returns q
    where m is q - n and msv\[m,q) is the intermediate sequence

<a name="stl::adjacent_difference/stlvec"></a>`stl::adjacent_difference (sv,first,last) (msv, p) bin_fun`
:   produce a sequence of new elements by applying bin\_fun to adjacent
    elements of sv\[first,last), placing the new elements in msv\[p,p+n),
    where n is last - first, with the intermediate results, and returns q
    where m is q - n and msv\[m,q) is the new sequence

<!-- -->
### Examples

See ut\_numeric.pure in the pure-stlvec/ut directory.

Reference Counting
------------------

The following function, also in the stl namespace, is available if you want to
observe how pure-stlvec maintains reference counts for items in its
containers.

<a name="stl::refc/stlvec"></a>`stl::refc x`
:   returns the x's reference count (maintained by the Pure runtime for
    garbage collection purposes)

<!-- -->
Backward Compatibilty
---------------------

This section documents changes in pure-stlvec that might have introduced
backward compatiblity issues.

### pure-stlvec-0.2

Bug fixes.

### pure-stlvec-0.3

Version 0.3 reflects some changes made to make [pure-stlvec](pure-stlvec.html)
consistent with its sister package, [pure-stlmap](pure-stlmap.html).

The [`update`](#update/stlvec) function was deprecated. Please use
[`replace`](#replace/stlvec) instead.

The [`replace`](#replace/stlvec) function was added to the stlvec module. This
function is the same as [`update`](#update/stlvec) except that
"[`replace`](#replace/stlvec) sv i x" returns x instead of sv.

The [`stl::replace`](#stl::replace/stlvec) function was removed from the
stlvec/modifying module. You can use
"[`stl::replace_if`](#stl::replace_if/stlvec) (sv,first,last) (x==) y" instead
of "[`stl::replace`](#stl::replace/stlvec) (sv,first,last) x y" to replace all
instances of x in the specified range.

The function [`null`](#null/stlvec) was removed and
[`stl::empty`](#stl::empty/stlvec) was added to replace it.

The function [`list`](#list/stlvec) was removed. You can use
[`members`](#members/stlvec) instead.

The function [`stl::random_shuffle`](#stl::random_shuffle/stlvec) was changed
to take a seed as a second parameter.

All of the tracing functions were removed.

### pure-stlvec-0.4

Fixed (&gt;) predicate operating on plain old data when passed to STL
algorithms.