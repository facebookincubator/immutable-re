Immutable Collections For Reason
================================

# Views
  * Seq: Lazy iterators.
  * Collection: Set like collections of unique values.
  * Indexed: Values indexed by continuous ranges of integers greater than 0.
  * Keyed: Values indexed by hashed and compared keys.

# Concrete Collection Types
  * BiMap: Bi-directional maps that maintain a unique key to value relationship.
  * CopyOnWriteArray: An opaque wrapper around arrays that provides copy on write operations. Good for small lists of values.
  * Deque: Double-ended queue, with amortized O(1) appends/prepends and head/tail access. Indexed look ups are O(log32 n).
  * HashMap: Keyed data, utilizing key hashing and key comparison or equality for collision handling.
  * HashMultiset: Collection of values that maintains a count of the number of times a value has been added to the collection.
  * HashSet: Persistent immutable set of values, utilizing value hashing and key comparison or equality for collision handling.
  * HashSetMultimap: A map of keys to a unique set of values.
  * IntMap: Immutable mapping of sparse ints to values.
  * List: Standard functions for working with OCaml list values.
  * Option: Standard functions for working OCaml option values.
  * SortedMap: Keyed data that is sorted using a comparator function on the keys.
  * SortedSet: Collection of values, sorted using a comparator function.
  * Stack: A FIFO stack, similar to a list, that maintains a count of items in the stack.
  * StackMultimap: A map of keys to the a stack of added values.
  * Table: Sparse tabular data structure for mapping row and columns to values.

# Utilities
  * Comparator: Functions for comparing two values of a given type.
  * Equality: Functions for comparing two values for equality.
  * Hash: Functions for generating unique hash functions.
  * Ordering: The set of valid return values from a comparator funtion.

# Transient Mutability

BiMap, Deque, HashMap, HashMultiset, HashSet, and IntMap support temporary mutability for improved performance. For more details see: http://clojure.org/reference/transients
