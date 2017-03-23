Immutable Collections For Reason
================================

# Features

## Views
  * Seq: Lazy iterators.
  * Set: Collections of unique values.
  * Map: Mapped key value pairs.

## Concrete Collection Types
  * CopyOnWriteArray: An opaque wrapper around arrays that provides copy on write operations. Good for small lists of values.
  * Deque: Double-ended queue, with amortized O(1) appends/prepends and head/tail access.
  * HashBiMap: Bi-directional maps that maintain a unique key to value relationship.
  * HashMap: Mapped key value pairs, utilizing key hashing and key comparison or equality for collision handling.
  * HashMultiset: Collection of values that maintains a count of the number of times a value has been added.
  * HashSet: Persistent immutable set of values, utilizing value hashing and key comparison or equality for collision handling.
  * HashSetMultimap: A map of keys to a unique set of values.
  * IntMap: Immutable mapping of sparse ints to values.
  * List: Standard functions for working with OCaml list values.
  * Option: Standard functions for working OCaml option values.
  * SortedMap: Mapped key value pairs that are sorted using a comparator function on the keys.
  * SortedSet: Collection of values, sorted using a comparator function.
  * Stack: A FIFO stack, similar to a list, that maintains a count of items in the stack.
  * StackMultimap: A map of keys to the a stack of added values.
  * Table: Sparse tabular data structure for mapping row and columns to values.
  * Vector: Indexed data-structure with amortized O(1) appends/prepends, and log32 indexed lookups and updates. Vector also
    supports efficient slicing operation (range, skip, take) that do not leak memory.

## Utilities
  * Comparator: Functions for comparing two values of a given type.
  * Equality: Functions for comparing two values for equality.
  * Hash: Functions for generating unique hash functions.
  * Ordering: The set of valid return values from a comparator function.

## Transient Mutability

BiMap, Deque, HashMap, HashMultiset, HashSet, IntMap, and Vector support temporary mutability for improved performance. For more details see: http://clojure.org/reference/transients

# Installing

## Installing `immutable` via OPAM

```bash
# On OSX, install opam via Homebrew:
brew update
brew install opam
# On Linux, see here (you will need opam >= 1.2.2): http://opam.ocaml.org/doc/Install.html

opam init
# Add this to your ~/.bashrc (or ~/.zshrc):
#   eval $(opam config env)

opam update
opam switch 4.03.0
eval $(opam config env)
opam install immutable
```

## Contributing to development

```bash
# On OSX, install opam via Homebrew:
brew update
brew install opam
# On Linux, see here (you will need opam >= 1.2.2): http://opam.ocaml.org/doc/Install.html

opam init
# Add this to your ~/.bashrc (or ~/.zshrc):
#   eval $(opam config env)

opam update
opam switch 4.03.0
eval $(opam config env)
git clone git@github.com:facebookincubator/immutable-re.git
cd immutable-re
opam pin add -y immutable .
```

License
-------
Immutable-re is [BSD-licensed](https://github.com/facebookincubator/immutable-re/blob/master/LICENSE.txt). We also provide an additional [patent grant](https://github.com/facebookincubator/immutable-re/blob/master/PATENTS.txt).
