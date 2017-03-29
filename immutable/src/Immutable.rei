/**
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

let module Equality: {
  /** Equality functions for common types. */

  type t 'a = 'a => 'a => bool;
  /** The Equality function type.
   * [equals this that] returns [true] if [this] and [that] are equal, otherwise [false].
   */

  let bytes: t bytes;
  /** Equality for bytes. */

  let char: t char;
  /** Equality for chars. */

  let int: t int;
  /** Equality for ints. */

  let int32: t int32;
  /** Equality for int32s. */

  let int64: t int64;
  /** Equality for int64s. */

  let nativeInt: t nativeint;
  /** Equality for nativeInts. */

  let reference: t 'a;
  /** The reference equality function, analogous to === */

  let string: t string;
  /** Equality for strings. */
};

let module Ordering: {
  /** Represents the absolute ordering of a type when comparing values. */

  type t;

  let equal: t;
  let greaterThan: t;
  let lessThan: t;
};

let module Comparator: {
  /** Comparison functions for common types. */

  type t 'a = 'a => 'a => Ordering.t;
  /** The Comparator function type.
   *  By definition a [compare this that] returns:
   *    [Ordering.greaterThan] if [this] is greater than [that],
   *    [Ordering.lessThan] if [this] is less than [that],
   *    otherwise [Ordering.equals].
   */

  let bytes: t bytes;
  /** Compares bytes. */

  let char: t char;
  /** Compares chars. */

  let int: t int;
  /** Compares ints. */

  let int32: t int32;
  /** Compares int32s. */

  let int64: t int64;
  /** Compares int64s. */

  let nativeInt: t nativeint;
  /** Compares nativeInts. */

  let string: t string;
  /** Compares strings. */

  let toEquality: (t 'a) => (Equality.t 'a);
  /** Converts a Comparator function to an Equality function. */
};

let module Hash: {
  /** Hash functions for common types. */

  type t 'a = 'a => int;
  /** The Hash function type. */
};

let module Equatable: {
  /** Module types implemented by modules that support testing values for equality. */

  module type S = {
    /* Equatable module type signature for types with a parametric type arity of 0. */

    type t;

    let equals: Equality.t t;
    /** An equality function for instances of type [t]. */
  };
};

let module Comparable: {
  /** Module types implemented by modules that support absolute ordering of values. */

  module type S = {
    /** Comparable module type signature for types with a parametric type arity of 0. */

    type t;

    include Equatable.S with type t := t;

    let compare: Comparator.t t;
    /** A comparator function for instances of type [t]. */
  };
};

let module Hashable: {
  /** Module types implemented by modules that support hashing. */

  module type S = {
    /** Hashable module type signature for types with a parametric type arity of 0. */

    type t;

    let hash: Hash.t t;
    /** An hashing function for instances of type [t]. */
  };
};

let module Reduceable: {
  /** Module types implemented by modules that support reducing over values. */

  module type S = {
    /** Reduceable module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    let reduce: while_::('acc => a => bool)? => ('acc => a => 'acc) => 'acc => t => 'acc;
    /** [reduce while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each value in [reduceable], while [predicate] returns true,
     *  accumulating the result.
     */
  };

  module type S1 = {
    /** Reduceable module type signature for types with a parametric type arity of 1. */

    type t 'a;

    let reduce: while_::('acc => 'a => bool)? => ('acc => 'a => 'acc) => 'acc => (t 'a) => 'acc;
    /** [reduce while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each value in [reduceable], while [predicate] returns true,
     *  accumulating the result.
     */
  };
};

let module ReduceableRight: {
  /** Module types implemented by modules that support reducing over
   *  values in both the left to right, and right to left directions.
   */

  module type S = {
    /** ReduceableRight module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Reduceable.S with type a := a and type t := t;

    let reduceRight: while_::('acc => a => bool)? => ('acc => a => 'acc) => 'acc => t => 'acc;
    /** [reduceRight while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each value in [reduceable] while [predicate] returns true, starting
     *  from the right most value, accumulating the result.
     */
  };

  module type S1 = {
    /** ReduceableRight module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include Reduceable.S1 with type t 'a := t 'a;

    let reduceRight: while_::('acc => 'a => bool)? => ('acc => 'a => 'acc) => 'acc => (t 'a) => 'acc;
    /** [reduceRight while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each value in [reduceable] while [predicate] returns true, starting
     *  from the right most value, accumulating the result.
     */
  };
};

let module Streamable: {
  /** Module types implemented by modules that support lazily evaluated
   *  stream functions. All functions defined in this module are O(1).
   */

  module type S1 = {
    /** Streamable module type signature for types with a parametric type arity of 1. */

    type t 'a;

    let buffer: count::int => skip::int => (t 'a) => (t (list 'a));
    /** [buffer count skip stream] returns a Streamable that collects values from [stream]
     *  into list buffers of size [count], skipping [skip] number of values in between the
     *  creation of new buffers. The returned buffers are guaranteed to be of size [count],
     *  and values are dropped if [stream] completes before filling the last buffer.
     */

    let concat: (list (t 'a)) => (t 'a);
    /** [concat streams] returns a Streamable that lazily concatenates all the
     *  Streamables in [streams]. The resulting Streamable returns all the values
     *  in the first Streamable, followed by all the values in the second Streamable,
     *  and continues until the last Streamable completes.
     */

    let defer: (unit => t 'a) => (t 'a);
    /** [defer f] returns a Streamable that invokes the function [f] whenever enumerated. */

    let distinctUntilChangedWith: (Equality.t 'a) => (t 'a) => (t 'a);
    /** [distinctUntilChangedWith equals stream] returns a Streamable that contains only
     *  distinct contiguous values from [stream] using [equals] to equate values.
     */

    let doOnNext: ('a => unit) => (t 'a) => (t 'a);
    /** [doOnNext f stream] returns a Streamable that applies the side effect
     *  function [f] to each value in the stream as they are enumerated.
     */

    let empty: unit => (t 'a);
    /** Returns an empty Streamable. */

    let filter: ('a => bool) => (t 'a) => (t 'a);
    /** [filter f stream] returns a Streamable only including values from [stream]
     *  for which application of the predicate function [f] returns true.
     */

    let flatMap: ('a => t 'b) => (t 'a) => (t 'b);
    /** [flatMap mapper stream] applies the mapper to each value in
     *  [stream], flattening the resulting Streams into a new Stream.
     */

    let flatten: (t (t 'a)) => (t 'a);
    /** [flatten stream] flattens the nested values in [streams] into
     *  a new [stream].
     */

    let generate: ('a => 'a) => 'a => (t 'a);
    /** [generate f initialValue] generates the infinite Streamable [x, f(x), f(f(x)), ...] */

    let map: ('a => 'b) => (t 'a) => (t 'b);
    /** [map f stream] Returns a Stream whose values are the result of
     *  applying the function [f] to each value in [stream].
     */

    let return: 'a => (t 'a);
    /** [return value] returns a single value Streamable containing [value]. */

    let scan: ('acc => 'a => 'acc) => 'acc => (t 'a) => (t 'acc);
    /** [scan f acc stream] returns a Streamable of accumulated values resulting from the
     *  application of the accumulator function [f] to each value in [stream] with the
     *  specified initial value [acc].
     */

    let skip: int => (t 'a) => (t 'a);
    /** [skip count stream] return a Streamable which skips the first [count]
     *  values in [stream].
     */

    let skipWhile: ('a => bool) => (t 'a) => (t 'a);
    /** [skipWhile f stream] return a Streamable which skips values in [stream]
     *  while application of the predicate function [f] returns true, and then returns
     *  the remaining values.
     */

    let startWith: 'a => (t 'a) => (t 'a);
    /** [startWith value stream] returns a Streamable whose first
     *  value is [value], followed by the values in [stream].
     */

    let take: int => (t 'a) => (t 'a);
    /** [take count stream] returns a Streamable with the first [count]
     *  values in [stream].
     */

    let takeWhile: ('a => bool) => (t 'a) => (t 'a);
    /** [takeWhile f stream] returns a Streamable including all values in [stream]
     *  while application of the predicate function [f] returns true, then completes.
     */
  };
};

let module Reducer: {
  /** Module functions for generating modules which provide common reduction functions for Reduceables.
   *  All functions are O(N), unless otherwise noted.
   */

  module type S = {
    type a;
    type t;

    let count: t => int;
    /** [count reduceable] returns the total number values produced by [reduceable] */

    let every: (a => bool) => t => bool;
    /** [every f reduceable] returns true if the predicate [f] returns true for all values in [reduceable].
     *  If [reduceable] is empty, returns [true].
     */

    let find: (a => bool) => t => (option a);
    /** [find f reduceable] return the Some of the first value in [reduceable] for which the
     *  the predicate f returns [true]. Otherwise None.
     */

    let findOrRaise: (a => bool) => t => a;
    /** [findOrRaise f reduceable] return the the first value in [reduceable] for which the
     *  the predicate f returns [true]. Otherwise raises an exception.
     */

    let first: t => (option a);
    /** [first reduceable] returns first value in [reduceable] or None.
     *
     *  Computational Complexity: O(1)
     */

    let firstOrRaise: t => a;
    /** [firstOrRaise reduceable] returns the first value in [reduceable] or raises an exception.
     *
     *  Computational Complexity: O(1)
     */

    let forEach: while_::(a => bool)? => (a => unit) => t => unit;
    /** [forEach while_::predicate f reduceable] iterates through [reduceable] applying the
     *  side effect function [f] to each value, while [predicate] returns true
     */

    let none: (a => bool) => t => bool;
    /** [none f reduceable] returns true if the predicate [f] returns false for all values in [reduceable].
     *  If [reduceable] is empty, returns [true].
     */

    let some: (a => bool) => t => bool;
    /** [some f reduceable] returns true if the predicate [f] returns true for at
     *  least one value in [reduceable]. If [reduceable] is empty, returns [false].
     */
  };

  module type S1 = {
    type t 'a;

    let count: t 'a => int;
    /** [count reduceable] returns the total number values produced by [reduceable] */

    let every: ('a => bool) => (t 'a) => bool;
    /** [every f reduceable] returns true if the predicate [f] returns true for all values in [reduceable].
     *  If [reduceable] is empty, returns [true].
     */

    let find: ('a => bool) => (t 'a) => (option 'a);
    /** [find f reduceable] return the Some of the first value in [reduceable] for which the
     *  the predicate f returns [true]. Otherwise None.
     */

    let findOrRaise: ('a => bool) => (t 'a) => 'a;
    /** [findOrRaise f reduceable] return the the first value in [reduceable] for which the
     *  the predicate f returns [true]. Otherwise raises an exception.
     */

    let first: t 'a => (option 'a);
    /** [first reduceable] returns first value in [reduceable] or None.
     *
     *  Computational Complexity: O(1)
     */

    let firstOrRaise: t 'a => 'a;
    /** [firstOrRaise reduceable] returns the first value in [reduceable] or raises an exception.
     *
     *  Computational Complexity: O(1)
     */

    let forEach: while_::('a => bool)? => ('a => unit) => (t 'a) => unit;
    /** [forEach while_::predicate f reduceable] iterates through [reduceable] applying the
     *  side effect function [f] to each value, while [predicate] returns true
     */

    let none: ('a => bool) => (t 'a) => bool;
    /** [none f reduceable] returns true if the predicate [f] returns false for all values in [reduceable].
     *  If [reduceable] is empty, returns [true].
     */

    let some: ('a => bool) => (t 'a) => bool;
    /** [some f reduceable] returns true if the predicate [f] returns true for at
     *  least one value in [reduceable]. If [reduceable] is empty, returns [false].
     */
  };

  let module Make: (Reduceable: Reduceable.S) => S with type a = Reduceable.a and type t = Reduceable.t;
  /** Module function to create a Reducer for a specific Reduceable type. */

  let module Make1: (Reduceable: Reduceable.S1) => S1 with type t 'a = Reduceable.t 'a;
  /** Module function to create a Reducer for a specific Reduceable type
   *  with a parametric type arity of 1.
   */
};

let module Iterator: {
  /** Functional iterators over a collection of values. Iterators are stateless and can be reused. */
  type t 'a;

  include Reduceable.S1 with type t 'a := t 'a;
  include Streamable.S1 with type t 'a := t 'a;

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Iterators. */
};

let module Iterable: {
  /** Module types implemented by modules that supporting iterating over values. */

  module type S = {
    /** Iterable module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Reduceable.S with type a := a and type t := t;

    let toIterator: t => (Iterator.t a);
    /** [toIterator iterable] returns an Iterator that can be used to iterate over
     * the values in [iterable].
     */
  };

  module type S1 = {
    /** Iterable module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include Reduceable.S1 with type t 'a := t 'a;

    let toIterator: t 'a => (Iterator.t 'a);
    /** [toIterator iterable] returns an Iterator that can be used to iterate over
     * the values in [iterable].
     */
  };
};

let module Sequence: {
  /** Functional pull based sequences. Sequences are generally lazy, computing values as
   *  they are pulled. Sequences are reusable and are guaranteed to produce the
   *  same values, in the same order every time they are enumerated. In addition, Sequences
   *  support eager seeking and zipping. These are their main advantage over Iterators.
   *  In general, only use Sequences when you require support for one or both of these features.
   *  Otherwise Iterators are generally more efficient.
   */

  type t 'a;
  /** The Sequence type. */

  include Iterable.S1 with type t 'a := t 'a;
  include Streamable.S1 with type t 'a := t 'a;

  let seek: int => (t 'a) => (t 'a);
  /** [seek count seq] scans forward [count] values in [seq]. It is the eagerly
   *  evaluated equivalent of [skip count seq].
   *
   *  Computational complexity: O(count).
   */

  let seekWhile: ('a => bool) => (t 'a) => (t 'a);
  /** [seekWhile f seq] scans forward through [seq] while application of
   *  the predicate function [f] returns true. It is the eagerly evaluated
   *  equivalent of [skipWhile f seq].
   *
   *  Computational complexity: O(N).
   */

  let zip: (list (t 'a)) => (t (list 'a));
  /** [zip seqs] returns a Sequence which lazily zips a list of [Sequence]s
   *  into a single Sequence of lists. Values are produce until any Sequence
   *  in [seqs] completes.
   */

  let zip2With: ('a => 'b => 'c) => (t 'a) => (t 'b) => (t 'c);
  /** [zip2With zipper first second] returns a Sequence which lazily zips two Sequences,
   *  combining their values using [zipper]. Values are produce until either [first]
   *  or [second] completes.
   */

  let zip3With: ('a => 'b => 'c => 'd) => (t 'a) => (t 'b) => (t 'c) => (t 'd);
  /** [zip3With zipper first second third] returns a Sequence which lazily zips three Sequences,
   *  combining their values using [zipper]. Values are produce until either [first], [second]
   *  or [third] complete.
   */

  let zipLongest: (list (t 'a)) => (t (list (option 'a)));
  /** [zipLongest seqs] returns a Sequence which zips a list of Sequences
   *  into a single of Sequence of lists. Values are produce until all Sequences
   *  in [seqs] complete.
   */

  let zipLongest2With:
    (option 'a => option 'b => 'c) =>
    (t 'a) =>
    (t 'b) =>
    (t 'c);
  /** [zipLongest2With zipper first second] returns a Sequence which lazily zips two Sequences,
   *  combining their values using [zipper]. Values are produce until both [first]
   *  and [second] complete.
   */

  let zipLongest3With:
    (option 'a => option 'b => option 'c => 'd) =>
    (t 'a) =>
    (t 'b) =>
    (t 'c) =>
    (t 'd);
  /** [zipLongest3With zipper first second third] returns a Sequence which lazily
   *  zips three Sequences, combining their values using [zipper]. Values are produce
   *  until [first], [second] and [third] all complete.
   */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Sequences. */
};

let module Collection: {
  /** Module types implemented by all immutable value collections.
   *
   *  By contract, all functions have a computational complexity of O(1).
   */

  module type S = {
    /** Collection module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Iterable.S with type a := a and type t := t;
    include Equatable.S with type t := t;

    let count: t => int;
    /** [count collection] returns number of values contained in [collection]. */

    let empty: t;
    /** The empty collection of type [t] */

    let isEmpty: t => bool;
    /** [isEmpty collection] returns true if [collection] is empty, otherwise false. */

    let isNotEmpty: t => bool;
    /** [isNotEmpty collection] returns true if [collection] contains at
     *  least one value, otherwise false.
     */

    let toSequence: t => (Sequence.t a);
    /** [toSequence collection] returns a Sequence that can be used to enumerate the collection. */
  };

  module type S1 = {
    /** Collection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include Iterable.S1 with type t 'a := t 'a;

    let count: (t 'a) => int;
    /** [count collection] returns number of values contained in the collection. */

    let isEmpty: (t 'a) => bool;
    /** [isEmpty collection] returns true if [collection] is empty, otherwise false. */

    let isNotEmpty: (t 'a) => bool;
    /** [isNotEmpty collection] returns true if [collection] contains at
     *  least one value, otherwise false.
     */

    let toSequence: (t 'a) => (Sequence.t 'a);
    /** [toSequence collection] returns a Sequence that can be used to enumerate the collection. */
  };
};

let module PersistentCollection: {
  /** Module types implemented by collections supporting fully persistent mutations.
   *  Mutation operations on these types do not mutate the underlying collection, but instead
   *  create a new collection, with the mutation applied.

   *  By contract, all functions have a computational complexity of O(1).
   */

  module type S = {
    /** PersistentCollection module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Collection.S with type a := a and type t := t;

    let removeAll: t => t;
    /** [removeAll collection] return an empty PersistentCollection. Depending on the implementation,
     *  the new collection may share the same configuration as [collection]. For instance, the HashSet
     *  implementations shares the same hash and comparison functions.
     */
  };

  module type S1 = {
    /** PersistentCollection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include Collection.S1 with type t 'a := t 'a;

    let removeAll: t 'a => t 'a;
    /** [removeAll collection] return an empty PersistentCollection. Depending on the implementation,
     *  the new collection may share the same configuration as [collection]. For instance, HashSet
     *  implementations shares the same hash and comparison functions.
     */
  };
};

let module TransientCollection: {
  /** Module types implemented by transiently mutable Collections. Transient collections
   *  are designed to enable fast and efficient batch operations by temporarily enabling mutation
   *  of an underlying collection type. Unlike PersistentCollection functions, TransientCollection
   *  APIs always return the same value reference passed in as an argument, with mutations applied.
   *
   *  By contract, all functions have a computational complexity of O(1).
   */

  module type S = {
    /** TransientCollection module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    let count: t => int;
    /** [count transient] returns number of values contained in [transient]. */

    let empty: unit => t;

    let isEmpty: t => bool;
    /** [isEmpty transient] returns true if [transient] is empty, otherwise false. */

    let isNotEmpty: t => bool;
    /** [isNotEmpty transient] returns true if [collection] contains at
     *  least one value, otherwise false.
     */

    let removeAll: t => t;
    /** [removeAll transient] removes all values from [transient]. */
  };

  module type S1 = {
    /** TransientCollection module type signature for types with a parametric type arity of 0. */

    type t 'a;

    let count: (t 'a) => int;
    /** [count transient] returns number of values contained in [transient]. */

    let isEmpty: (t 'a) => bool;
    /** [isEmpty transient] returns true if [transient] is empty, otherwise false. */

    let isNotEmpty: (t 'a) => bool;
    /** [isNotEmpty transient] returns true if [collection] contains at
     *  least one value, otherwise false.
     */

    let removeAll: (t 'a) => (t 'a);
    /** [removeAll transient] removes all values from [transient]. */
  };
};

let module SequentialCollection: {
  /** Module types implemented by collections that support sequential access to
   *  the left most contained values. Concrete implementations include [Stack] and [SortedSet].
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S = {
    /** SequentialCollection module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Collection.S with type a := a and type t := t;

    let first: t => (option a);
    /** [first collection] returns first value in [collection] or None. */

    let firstOrRaise: t => a;
    /** [firstOrRaise collection] returns the first value in [collection] or throws. */
  };

  module type S1 = {
    /** SequentialCollection module type signature for types with a parametric type arity of 1. */
    type t 'a;

    include Collection.S1 with type t 'a := t 'a;

    let first: (t 'a) => (option 'a);
    /** [first collection] returns first value in [collection] or None. */

    let firstOrRaise: (t 'a) => 'a;
    /** [firstOrRaise collection] returns the first value in [collection] or throws. */
  };
};

let module PersistentSequentialCollection: {
  /** Module types implemented by collections supporting persistent mutations to left
   *  side of the collection.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */
  module type S1 = {
    /** PersistentSequentialCollection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include PersistentCollection.S1 with type t 'a := t 'a;
    include SequentialCollection.S1 with type t 'a := t 'a;

    let addFirst: 'a => (t 'a) => (t 'a);
    /** [addFirst value collection] returns a PersistentSequentialCollection with [value] prepended. */

    let addFirstAll: (Iterator.t 'a) => (t 'a) => (t 'a);
    /** [addFirstAll iter collection] returns a PersistentSequentialCollection with the values in [iter] prepended. */

    let empty: unit => (t 'a);
    /** [empty ()] return an empty PersistentSequentialCollection. */

    let fromReverse: (Iterator.t 'a) => (t 'a);
    /** [fromReverse iter] returns a PersistentSequentialCollection containing the values in [iter]
     *  in reverse order.
     */

    let return: 'a => (t 'a);
    /** [return value] returns a PersistentSequentialCollection containing a single value, [value]. */

    let removeFirstOrRaise: (t 'a) => (t 'a);
    /** [removeFirstOrRaise collection] returns a PersistentSequentialCollection without
     *  the first value or raises an exception if [collection] is empty.
     */
  };
};

let module TransientSequentialCollection: {
  /** Module types implemented by transient collections supporting transient mutations to left
   *  side of the collection.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S1 = {
    /** TransientSequentialCollection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include TransientCollection.S1 with type t 'a := t 'a;

    let addFirst: 'a => (t 'a) => (t 'a);
    /** [addFirst value transient] prepends [value] to [transient]. */

    let addFirstAll: (Iterator.t 'a) => (t 'a) => (t 'a);
    /** [addFirstAll iter transient] prepends all values in [iter] to [transient]. */

    let empty: unit => (t 'a);
    /** [empty ()] returns a new empty TransientSequentialCollection. */

    let first: (t 'a) => option 'a;
    /** [first transient] returns first value in [transient] or None. */

    let firstOrRaise: (t 'a) => 'a;
    /** [firstOrRaise transient] returns the first value in [transient] or raises an exception. */

    let removeFirstOrRaise: (t 'a) => (t 'a);
    /** [removeFirstOrRaise transient] removes the first value from [transient] or raises
     *  an exception if [transient] is empty.
     */
  };
};

let module NavigableCollection: {
  /** Module types implemented by Collections that are ordered or sorted and support
   *  navigation operations.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S = {
    /** NavigableCollection module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include ReduceableRight.S with type a := a and type t := t;
    include SequentialCollection.S with type a := a and type t := t;

    let last: t => (option a);
    /** [last collection] returns last value in [collection] or None.
     *
     *  By contract, implementations are efficient with no worst than O(log N) performance.
     */

    let lastOrRaise: t => a;
    /** [lastOrRaise collection] returns the last value in [collection] or raises an exception.
     *
     *  By contract, implementations are efficient with no worst than O(log N) performance.
     */

    let toIteratorRight: t => (Iterator.t a);
    /* [toIteratorRight collection] returns an Iterator that can be used to iterate over
     * the values in [collection] from right to left.
     */

    let toSequenceRight: t => (Sequence.t a);
    /* [toSequenceRight collection] returns an Sequence that can be used to enumerate
     * the values in [collection] from right to left.
     */
  };

  module type S1 = {
    /** NavigableCollection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include ReduceableRight.S1 with type t 'a := t 'a;
    include SequentialCollection.S1 with type t 'a := t 'a;

    let last: (t 'a) => (option 'a);
    /** [last collection] returns last value in [collection] or None. */

    let lastOrRaise: (t 'a) => 'a;
    /** [lastOrRaise collection] returns the first value in [collection] or raises an exception. */

    let toIteratorRight: (t 'a) => (Iterator.t 'a);
    /* [toIteratorRight collection] returns an Iterator that can be used to iterate over
     * the values in [collection] from right to left.
     */

    let toSequenceRight: (t 'a) => (Sequence.t 'a);
    /* [toSequenceRight collection] returns an Sequence that can be used to enumerate
     * the values in [collection] from right to left.
     */
  };
};

let module PersistentNavigableCollection: {
  /** Module types implemented by collections supporting persistent mutations to both the left
   *  and right side of the collection.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S1 = {
    /** PersistentNavigableCollection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include NavigableCollection.S1 with type t 'a := t 'a;
    include PersistentSequentialCollection.S1 with type t 'a := t 'a;

    let addLast: 'a => (t 'a) => (t 'a);
    /** [addLast value collection] returns a PersistentNavigableCollection with [value] appended.
     *
     *  Complexity: O(1)
     */

    let addLastAll: (Iterator.t 'a) => (t 'a) => (t 'a);
    /** [addLastAll iter collection] returns a PersistentNavigableCollection with the values in [iter] appended. */

    let from: (Iterator.t 'a) => (t 'a);
    /** [from iter] returns a PersistentNavigableCollection containing the values in [iter].
     *
     * Complexity: O(N) the number of values in [iter].
     */

    let removeLastOrRaise: (t 'a) => (t 'a);
    /** [removeLastOrRaise collection] returns a PersistentSequentialCollection without
     *  the last value or raises an exception if [collection] is empty.
     */
  };
};

let module TransientNavigableCollection: {
  /** Module types implemented by transient collections supporting transient mutations to both
   *  the left and rights sides of the collection.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */
  module type S1 = {
    /** TransientNavigableCollection module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include TransientSequentialCollection.S1 with type t 'a := t 'a;

    let addLast: 'a => (t 'a) => (t 'a);
    /** [addLast value transient] appends [value] to [transient].
     *
     *  Complexity: O(1)
     */

    let last: (t 'a) => option 'a;
    /** [tryLast transient] returns the last value in [transient] or None. */

    let lastOrRaise: (t 'a) => 'a;
    /** [last transient] returns the last value in [transient] or raises an exception. */

    let removeLastOrRaise: (t 'a) => (t 'a);
    /** [removeLastOrRaise transient] removes the last value from [transient] or raises
     *  an exception if [transient] is empty.
     */
  };
};

let module rec Set: {
  /** A read only view of a unique Set of values. The intent of this type is to enable
   *  interop between alternative concrete implementations such as SortedSet and HashSet.
   *  The complexity of functions in this module is dependent upon the underlying concrete implementation.
   */

  module type S = {
    /** Set module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Collection.S with type a := a and type t := t;

    let contains: a => t => bool;
    /** [contains value set] returns true if [set] contains at least one instace of [value],
     *  otherwise false;
     *
     *  By contract, an implementation must be efficient, with no worst than O(log N) performance.
     */

    let toSet: t => Set.t a;
    /** [toSet set] returns a Set view of [set]. */
  };

  module type S1 = {
    /** Set module type signature for types with a parametric type arity of 0. */
    type t 'a;

    include Collection.S1 with type t 'a := t 'a;

    let contains: 'a => (t 'a) => bool;
    /** [contains value set] returns true if [set] contains at least one instace of [value],
     *  otherwise false;
     *
     *  By contract, an implementation must be efficient, with no worst than O(log N) performance.
     */
    
    let equals: Equality.t (t 'a);
    /** An equality function for Set.S1 instances. */

    let toSet: (t 'a) => Set.t 'a;
    /** [toSet set] returns a Set view of [set]. */
  };

  type t 'a;
  /** The Set type. */

  include S1 with type t 'a := t 'a;

  let empty: unit => (t 'a);
  /** The empty Set. */

  let intersect: (t 'a) => (t 'a) => (Iterator.t 'a);
  /** [intersect this that] returns an Iterator of unique values
   *  which occur in both [this] and [that].
   */

  let subtract: (t 'a) => (t 'a) => (Iterator.t 'a);
  /** [subtract this that] returns an Iterator of unique value
   *  which occur in [this] but not in [that].
   */

  let union: (t 'a) => (t 'a) => (Iterator.t 'a);
  /** [union this that] returns an Iterator of unique values which occur in either [this] or [that]. */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Iterators. */
};

let module PersistentSet: {
  /** Module types implemented by Set collections supporting persistent mutations.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S = {
    /** PersistentSet module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Set.S with type a := a and type t := t;
    include PersistentCollection.S with type a := a and type t := t;

    let add: a => t => t;
    /** [add value set] returns a PersistentSet containing value. If [set] already contains [value],
     *  it is returned unmodified.
     */

    let addAll: (Iterator.t a) => t => t;
    /** [addAll iter set] returns a PersistentSet with the values in [iter] and all the values in [set]. */

    let from: (Iterator.t a) => t;
    /** [from iter] returns a PersistentSet with all the values in [iter] */

    let intersect: t => t => t;
    /** [intersect this that] returns a PersistentSet of unique values
     *  which occur in both [this] and [that].
     */

    let remove: a => t => t;
    /** [remove value set] returns a PersistentSet that does not contain [value].
     *  If [set] does not contain [value], it is returned unmodified.
     */

    let subtract: t => t => t;
    /** [subtract this that] returns an PersistentSet of unique value
     *  which occur in [this] but not in [that].
     */

    let union: t => t => t;
    /** [union this that] returns an PersistentSet of unique values which occur in either [this] or [that]. */
  };

  module type S1 = {
    /** PersistentSet module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include Set.S1 with type t 'a := t 'a;
    include PersistentCollection.S1 with type t 'a := t 'a;

    let add: 'a => (t 'a) => (t 'a);
    /** [add value set] returns a PersistentSet containing value. If [set] already contains [value],
     *  it is returned unmodified.
     */

    let addAll: (Iterator.t 'a) => (t 'a) => (t 'a);
    /** [addAll iter set] returns a PersistentSet with the values in [iter] and all the values in [set]. */

    let intersect: (t 'a) => (t 'a) => (t 'a);
    /** [intersect this that] returns a PersistentSet of unique values
     *  which occur in both [this] and [that].
     */

    let remove: 'a => (t 'a) => (t 'a);
    /** [remove value set] returns a PersistentSet that does not contain [value].
     *  If [set] does not contain [value], it is returned unmodified.
     */

    let subtract: (t 'a) => (t 'a) => (t 'a);
    /** [subtract this that] returns an PersistentSet of unique values
     *  which occur in [this] but not in [that].
     */

    let union: (t 'a) => (t 'a) => (t 'a);
    /** [union this that] returns an PersistentSet of unique values which occur in either [this] or [that]. */
  };
};

let module TransientSet: {
  /** Module types implemented by transiently mutable sets.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S = {
    /** TransientSet module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include TransientCollection.S with type a := a and type t := t;

    let add: a => t => t;
    /** [add value transient] adds [value] to [transient]. If [transient] already contains [value],
     *  it is returned unmodified.
     */

    let addAll: (Iterator.t a) => t => t;
    /** [addAll iter transient] adds all values in [iter] to [transient]. */

    let contains: a => t => bool;
    /** [contains value set] returns true if [set] contains at least one instace of [value],
     *  otherwise false;
     */

    let remove: a => t => t;
    /** [remove value transient] removes [value] from [transient].
     *  If [transient] does not contain [value], it is returned unmodified.
     */
  };

  module type S1 = {
    /** TransientSet module type signature for types with a parametric type arity of 0. */

    type t 'a;

    include TransientCollection.S1 with type t 'a := t 'a;

    let add: 'a => (t 'a) => (t 'a);
    /** [add value transient] adds [value] to [transient]. If [transient] already contains [value],
     *  it is returned unmodified.
     */

    let addAll: (Iterator.t 'a) => (t 'a) => (t 'a);
    /** [addAll iter transient] adds all values in [iter] to [transient]. */

    let contains: 'a => (t 'a) => bool;
    /** [contains value set] returns true if [set] contains at least one instace of [value],
     *  otherwise false;
     */

    let remove: 'a => (t 'a) => (t 'a);
    /** [remove value transient] removes [value] from [transient].
     *  If [transient] does not contain [value], it is returned unmodified.
     */
  };
};

let module NavigableSet: {
  /*  Module types implemented by Sets that supports navigation operations. */

  module type S = {
    /** NavigableSet module type signature for types with a parametric type arity of 0. */

    type a;
    type t;

    include Set.S with type a := a and type t := t;
    include NavigableCollection.S with type a := a and type t := t;
  };
};

let module PersistentNavigableSet: {
  /** Module types implemented by NavigableSet collections supporting persistent mutations.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S = {
    /** PersistentNavigableSet module type signature for types with a parametric type arity of 0. */
    type a;
    type t;

    include NavigableSet.S with type a := a and type t := t;
    include PersistentSet.S with type a := a and type t := t;

    let removeFirstOrRaise: t => t;
    /** [removeFirstOrRaise set] returns a PersistentNavigableSet without
     *  the first value or raises an exception if [set] is empty.
     */

    let removeLastOrRaise: t => t;
    /** [removeLastOrRaise set] returns a PersistentNavigableSet without
     *  the last value or raises an exception if [set] is empty.
     */
  };
};

let module KeyedReduceable: {
  /** Module types implemented by modules that support reducing over key/value pairs. */

  module type S1 = {
    /** KeyedReduceable module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    let reduce: while_::('acc => k => 'v => bool)? => ('acc => k => 'v => 'acc) => 'acc => (t 'v) => 'acc;
    /** [reduce while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each key/value pair in [reduceable], while [predicate] returns true,
     *  accumulating the result.
     */
  };

  module type S2 = {
    /** KeyedReduceable module type signature for types with a parametric type arity of 2. */

    type t 'k 'v;

    let reduce: while_::('acc => 'k => 'v => bool)? => ('acc => 'k => 'v => 'acc) => 'acc => (t 'k 'v) => 'acc;
    /** [reduce while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each key/value pair in [reduceable], while [predicate] returns true,
     *  accumulating the result.
     */
  };
};

let module KeyedReduceableRight: {
  /** Module types implemented by modules that support reducing over
   *  key/value pairs in both the left to right, and right to left directions.
   */

  module type S1 = {
    /** KeyedReduceableRight module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include KeyedReduceable.S1 with type k := k and type t 'v := t 'v;

    let reduceRight: while_::('acc => k => 'v => bool)? => ('acc => k => 'v => 'acc) => 'acc => (t 'v) => 'acc;
    /** [reduceRight while_::predicate initialValue f reduceable] applies the accumulator
     *  function [f] to each key/value pair in [reduceable] while [predicate] returns true, starting
     *  from the right most key/value pair, accumulating the result.
     */
  };
};

let module KeyedReducer: {
  /** Module functions for generating modules which provide common reduction functions for Reduceables.
   *  All functions are O(N), unless otherwise noted.
   */
  module type S1 = {
    type k;
    type t 'v;

    let count: (t 'v) => int;
    /** [count keyedReduceable] returns the total number key/value pairs produced by [keyedReduceable] */

    let every: (k => 'v => bool) => (t 'v) => bool;
    /** [every f keyedReduceable] returns true if the predicate [f] returns true for all
     *  key/value pairs in [keyedReduceable]. If [keyedReduceable] is empty, returns [true].
     */

    let find: (k => 'v => bool) => (t 'v) => (option (k, 'v));
    /** [find f keyedReduceable] return the Some of the first key/value pair in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise None.
     */

    let findOrRaise: (k => 'v => bool) => (t 'v) => (k, 'v);
    /** [findOrRaise f keyedReduceable] return the the first key/value pair in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise raises an exception.
     */

    let findKey: (k => 'v => bool) => (t 'v) => (option k);
    /** [findKey f keyedReduceable] return the Some of the first key in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise None.
     */

    let findKeyOrRaise: (k => 'v => bool) => (t 'v) => k;
    /** [findOrRaise f keyedReduceable] return the the first key in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise raises an exception.
     */

    let findValue: (k => 'v => bool) => (t 'v) => (option 'v);
    /** [findValue f keyedReduceable] return the Some of the first value in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise None.
     */

    let findValueOrRaise: (k => 'v => bool) => (t 'v) => 'v;
    /** [findOrRaise f keyedReduceable] return the the first value in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise raises an exception.
     */

    let first: (t 'v) => (option (k, 'v));
    /** [first keyedReduceable] returns first key/value pair in [reduceable] or None.
     *
     *  Computational Complexity: O(1)
     */

    let firstOrRaise: (t 'v) => (k, 'v);
    /** [firstOrRaise keyedReduceable] returns the first key/value pair in [keyedReduceable] or raises an exception.
     *
     *  Computational Complexity: O(1)
     */

    let forEach: while_::(k => 'v => bool)? => (k => 'v => unit) => (t 'v) => unit;
    /** [forEach while_::predicate f keyedReduceable] iterates through [keyedReduceable] applying the
     *  side effect function [f] to each key/value pair, while [predicate] returns true
     */

    let none: (k => 'v => bool) => (t 'v) => bool;
    /** [none f keyedReduceable] returns true if the predicate [f] returns false
     *  for all key/value pairs in [keyedReduceable]. If [keyedReduceable] is empty, returns [true].
     */

    let some: (k => 'v => bool) => (t 'v) => bool;
    /** [some f keyedReduceable] returns true if the predicate [f] returns true for at least
     *  one key/value pair in [keyedReduceable]. If [keyedReduceable] is empty, returns [false].
     */
  };

  module type S2 = {
    type t 'k 'v;

    let count: (t 'k 'v) => int;
    /** [count keyedReduceable] returns the total number key/value pairs produced by [keyedReduceable] */

    let every: ('k => 'v => bool) => (t 'k 'v) => bool;
    /** [every f keyedReduceable] returns true if the predicate [f] returns true for all
     *  key/value pairs in [keyedReduceable]. If [keyedReduceable] is empty, returns [true].
     */

    let find: ('k => 'v => bool) => (t 'k 'v) => (option ('k, 'v));
    /** [find f keyedReduceable] return the Some of the first key/value pair in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise None.
     */

    let findOrRaise: ('k => 'v => bool) => (t 'k 'v) => ('k, 'v);
    /** [findOrRaise f keyedReduceable] return the the first key/value pair in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise raises an exception.
     */

    let findKey: ('k => 'v => bool) => (t 'k 'v) => (option 'k);
    /** [findKey f keyedReduceable] return the Some of the first key in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise None.
     */

    let findKeyOrRaise: ('k => 'v => bool) => (t 'k 'v) => 'k;
    /** [findOrRaise f keyedReduceable] return the the first key in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise raises an exception.
     */

    let findValue: ('k => 'v => bool) => (t 'k 'v) => (option 'v);
    /** [findValue f keyedReduceable] return the Some of the first value in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise None.
     */

    let findValueOrRaise: ('k => 'v => bool) => (t 'k 'v) => 'v;
    /** [findOrRaise f keyedReduceable] return the the first value in [keyedReduceable]
     *  for which the the predicate f returns [true]. Otherwise raises an exception.
     */

    let first: (t 'k 'v) => (option ('k, 'v));
    /** [first keyedReduceable] returns first key/value pair in [reduceable] or None.
     *
     *  Computational Complexity: O(1)
     */

    let firstOrRaise: (t 'k 'v) => ('k, 'v);
    /** [firstOrRaise keyedReduceable] returns the first key/value pair in [keyedReduceable] or raises an exception.
     *
     *  Computational Complexity: O(1)
     */

    let forEach: while_::('k => 'v => bool)? => ('k => 'v => unit) => (t 'k 'v) => unit;
    /** [forEach while_::predicate f keyedReduceable] iterates through [keyedReduceable] applying the
     *  side effect function [f] to each key/value pair, while [predicate] returns true
     */

    let none: ('k => 'v => bool) => (t 'k 'v) => bool;
    /** [none f keyedReduceable] returns true if the predicate [f] returns false
     *  for all key/value pairs in [keyedReduceable]. If [keyedReduceable] is empty, returns [true].
     */

    let some: ('k => 'v => bool) => (t 'k 'v) => bool;
    /** [some f keyedReduceable] returns true if the predicate [f] returns true for at least
     *  one key/value pair in [keyedReduceable]. If [keyedReduceable] is empty, returns [false].
     */
  };

  let module Make1: (KeyedReduceable: KeyedReduceable.S1) => S1 with type k = KeyedReduceable.k and type t 'v = KeyedReduceable.t 'v;
  /** Module function to create a KeyedReducer for a specific KeyedReduceable type with a parametric type arity of 1. */

  let module Make2: (KeyedReduceable: KeyedReduceable.S2) => S2 with type t 'k 'v = KeyedReduceable.t 'k 'v;
  /** Module function to create a KeyedReducer for a specific KeyedReduceable type with a parametric type arity of 2. */
};

let module KeyedIterator: {
  /** Functional iterators over a collection of key/value pairs. KeyedIterators are stateless and can be reused.
   *  All functions defined in this module are O(1).
   */
  type t 'k 'v;

  include KeyedReduceable.S2 with type t 'k 'v := t 'k 'v;

  let concat: (list (t 'k 'v)) => (t 'k 'v);
  /** [concat keyedIters] returns a KeyedIterator that lazily concatenates all the
   *  KeyedIterators in [keyedIters]. The resulting KeyedIterator returns all the key/value pairs
   *  in the first KeyedIterator, followed by all the key/value pairs in the second KeyedIterator,
   *  and continues until the last KeyedIterator completes.
   */

  let defer: (unit => t 'k 'v) => (t 'k 'v);
  /** [defer f] returns a KeyedIterator that invokes the function [f] whenever iterated. */

  let distinctUntilChangedWith: keyEquals::(Equality.t 'k) => valueEquals::(Equality.t 'v) => (t 'k 'v) => (t 'k 'v);
  /** [distinctUntilChangedWith equals keyedIter] returns a KeyedIterator that contains only
   *  distinct contiguous key/value pairs from [keyedIter] using [keyEquals] and [valueEquals] to
   *  equate key/value pairs.
   */

  let doOnNext: ('k => 'v => unit) => (t 'k 'v) => (t 'k 'v);
  /** [doOnNext f keyedIter] returns a KeyedIterator that applies the side effect
   *  function [f] to each key/value pair they are iterated.
   */

  let empty: unit => (t 'k 'v);
  /** Returns an empty KeyedIterator. */

  let filter: ('k => 'v => bool) => (t 'k 'v) => (t 'k 'v);
  /** [filter f keyedIter] returns a KeyedIterator only including key/value pairs from [keyedIter]
   *  for which application of the predicate function [f] returns true.
   */

  let flatMap: ('kA => 'vA => t 'kB 'vB) => (t 'kA 'vA) => (t 'kB 'vB);
  /** [flatMap mapper keyedIter] returns a KeyedIterator which applies [mapper] to each value in
   *  [keyedIter], flattening the results.
   */

  let fromEntries: Iterator.t ('k, 'v) => (t 'k 'v);
  /** [fromEntries iter] returns a KeyedIterator view of  key/value tuples in [iter]. */

  let generate: genKey::('k => 'v => 'k) => genValue::('k => 'v => 'v) => 'k => 'v => (t 'k 'v);
  /** [generate genKey gen value k v] generates an infinite KeyedSequence
   *  where the keys are [k, genKey(k v), genKey(genKey(k v), v1), ...]
   *  and values are [v, genValue(k, v), genValue(k1, genValue(k, v)), ...]
   */

  let keys: (t 'k 'v) => (Iterator.t 'k);
  /** [keys keyedIter] returns an Iterator view of the keys in [keyedIter] */

  let map: keyMapper::('kA => 'vA => 'kB) => valueMapper::('kA => 'vA => 'vB) => (t 'kA 'vA) => (t 'kB 'vB);
  /** [map keyMapper::keyMapper valueMapper::valueMapper keyedIter] returns a KeyedIterator
   *  whose keys are the result applying [keyMapper] to each key, and whose values are the result
   *  of applying [valueMapper] to each value in [keyedIter].
   */

  let mapKeys: ('a => 'v => 'b) => (t 'a 'v) => (t 'b 'v);
  /** [mapKeys mapper keyedIter] returns a KeyedIterator with mapper applied
   *  to each key in [keyedIter].
   */

  let mapValues: ('k => 'a => 'b) => (t 'k 'a) => (t 'k 'b);
  /** [mapValues mapper keyedIter] returns a KeyedIterator with mapper applied
   *  to each value in [keyedIter].
   */

  let return: 'k => 'v => (t 'k 'v);
  /** [return key value] returns a KeyedIterator containing the pair ([key], [value]). */

  let scan: ('acc => 'k => 'v => 'acc) => 'acc => (t 'k 'v) => (Iterator.t 'acc);
  /** [scan f acc keyedIter] returns a KeyedIterator of accumulated values resulting from the
   *  application of the accumulator function [f] to each value in [keyedIter] with the
   *  specified initial value [acc].
   */

  let skip: int => (t 'k 'v) => (t 'k 'v);
  /** [skip count keyedIter] return a KeyedIterator which skips the first [count]
   *  values in [keyedIter].
   */

  let skipWhile: ('k => 'v => bool) => (t 'k 'v) => (t 'k 'v);
  /** [skipWhile f keyedIter] return a KeyedIterator which skips key/value pairs in [keyedIter]
   *  while application of the predicate function [f] returns true, and then returns
   *  the remaining values.
   */

  let startWith: 'k => 'v => (t 'k 'v) => (t 'k 'v);
  /** [startWith key value keyedIter] returns a KeyedIterator whose first
   *  pair is (key, value), followed by the key/value pairs in [keyedIter].
   */

  let take: int => (t 'k 'v) => (t 'k 'v);
  /** [take count keyedIter] returns a KeyedIterator with the first [count]
   *  key/value pairs in [keyedIter].
   */

  let takeWhile: ('k => 'v => bool) => (t 'k 'v) => (t 'k 'v);
  /** [takeWhile f keyedIter] returns a KeyedIterator including all values in [keyedIter]
   *  while application of the predicate function [f] returns true, then completes.
   */

  let toIterator: (t 'k 'v) => (Iterator.t ('k, 'v));
  /** [toIterator keyedIter] returns an Iterator view of the key/value pairs in [keyedIter] as tuples. */

  let values: (t 'k 'v) => Iterator.t 'v;
  /** [values keyedIter] returns an Iterator view of the values in [keyedIter] */

  let module KeyedReducer: KeyedReducer.S2 with type t 'k 'v := t 'k 'v;
   /* KeyedReducer module for KeyedIterators. */
};

let module KeyedIterable: {
  /** Module types implemented by modules that supporting iterating over key/value pairs. */

  module type S1 = {
    /** KeyedIterable module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include KeyedReduceable.S1 with type k := k and type t 'v := t 'v;

    let toIterator: t 'v => Iterator.t (k, 'v);
    /** [toIterator keyedIterable] returns an Iterator that can be used to iterate over
     *  the key/value pairs in [keyedIterable] as tuples.
     */

    let toKeyedIterator: t 'v => KeyedIterator.t k 'v;
    /** [toKeyedIterator keyedIterable] returns a KeyedIterator that can be used to iterate over
     *  the key/value pairs in [keyedIterable].
     */
  };

  module type S2 = {
    /** KeyedIterable module type signature for types with a parametric type arity of 2. */

    type t 'k 'v;

    include KeyedReduceable.S2 with type t 'k 'v := t 'k 'v;

    let toIterator: t 'k 'v => Iterator.t ('k, 'v);
    /** [toIterator keyedIterable] returns an Iterator that can be used to iterate over
     *  the key/value pairs in [keyedIterable] as tuples.
     */

    let toKeyedIterator: t 'k 'v => KeyedIterator.t 'k 'v;
    /** [toKeyedIterator keyedIterable] returns a KeyedIterator that can be used to iterate over
     *  the key/value pairs in [keyedIterable].
     */
  };
};

let module KeyedCollection: {
  /** Module types implemented by all immutable keyed collections. This module
   *  signature does not impose any restrictions on the relationship between
   *  keys and associated values.
   *
   *  By contract, all functions have a computational complexity of O(1),
   *  unless otherwise noted.
   */

  module type S1 = {
     /** KeyedCollection module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include KeyedIterable.S1 with type k := k and type t 'v := t 'v;

    let containsKey: k => t 'v => bool;
    /** [containsKey key keyed] returns true if [keyed] contains an association from [key] to
     *  one or more values, otherwise false.
     *
     *  By contract, [containsKey] is efficient with no worst than O(log N) performance.
     */

    let count: t 'v => int;
    /** [count keyed] returns number of key/value pairs contained in [keyed]. */

    let isEmpty: (t 'v) => bool;
    /** [isEmpty keyed] returns true if [keyed] is empty, otherwise false. */

    let isNotEmpty: (t 'v) => bool;
    /** [isNotEmpty keyed] returns true if [keyed] contains at
     *  least one value, otherwise false.
     */

    let keys: (t 'v) => (Set.t k);
    /** [keys keyed] return a Set view of the keys in [keyed]. */

    let toSequence: (t 'v) => (Sequence.t (k, 'v));
    /* [toSequence keyed] returns an Sequence that can be used to enumerate
     * the key/value pairs in [keyed] as tuples.
     */
  };

  module type S2 = {
    /** KeyedCollection module type signature for types with a parametric type arity of 2. */

    type t 'k 'v;

    include KeyedIterable.S2 with type t 'k 'v := t 'k 'v;

    let containsKey: 'k => t 'k 'v => bool;
    /** [containsKey key keyed] returns true if [keyed] contains an association from [key] to
     *  one or more values, otherwise false.
     *
     *  By contract, [containsKey] is efficient with no worst than O(log N) performance.
     */

    let count: t 'k 'v => int;
    /** [count keyed] returns number of key/value pairs contained in [keyed]. */

    let isEmpty: (t 'k 'v) => bool;
    /** [isEmpty keyed] returns true if [keyed] is empty, otherwise false. */

    let isNotEmpty: (t 'k 'v) => bool;
    /** [isNotEmpty keyed] returns true if [keyed] contains at
     *  least one value, otherwise false.
     */

    let keys: (t 'k 'v) => (Set.t 'k);
    /** [keys keyed] return a Set view of the keys in [keyed]. */

    let toSequence: (t 'k 'v) => (Sequence.t ('k, 'v));
    /* [toSequence keyed] returns an Sequence that can be used to enumerate
     * the key/value pairs in [keyed] as tuples.
     */
  };
};

let module PersistentKeyedCollection: {
  /** Module types implemented by KeyedCollections supporting fully persistent mutations.
   *  Mutation operations on these types do not mutate the underlying collection, but instead
   *  create a new collection with the mutation applied.
   */

  module type S1 = {
    /** PersistentKeyedCollection module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include KeyedCollection.S1 with type k := k and type t 'v := t 'v;

    let remove: k => (t 'v) => (t 'v);
    /** [remove key keyed] removes all values associated with [key] from [keyed]
     *
     *  By contract, [remove] is efficient with no worst than O(log N) performance.
     */

    let removeAll: (t 'v) => (t 'v);
    /** [removeAll keyed] return an empty PersistentKeyedCollection. Depending on the implementation,
     *  the new PersistentKeyedCollection may share the same configuration as [keyed]. For instance,
     *  the HashMap implementation shares the same hash and comparison functions.
     *
     *  Computational complexity: O(1)
     */
  };

  module type S2 = {
    /** PersistentKeyedCollection module type signature for types with a parametric type arity of 2. */

    type t 'k 'v;

    include KeyedCollection.S2 with  type t 'k 'v := t 'k 'v;

    let remove: 'k => (t 'k 'v) => (t 'k 'v);
    /** [remove key keyed] removes all values associated with [key] from [keyed]
     *
     *  By contract, [remove] is efficient with no worst than O(log N) performance.
     */

    let removeAll: (t 'k 'v) => (t 'k 'v);
    /** [removeAll keyed] return an empty PersistentKeyedCollection. Depending on the implementation,
     *  the new PersistentKeyedCollection may share the same configuration as [keyed]. For instance,
     *  the HashMap implementation shares the same hash and comparison functions.
     *
     *  Computational complexity: O(1)
     */
  };
};

let module TransientKeyedCollection: {
  /** Module types implemented by transiently mutable KeyedCollections. Transient collections
  *  are designed to enable fast and efficient batch operations by temporarily enabling mutation
  *  of an underlying collection type. Unlike PersistentKeyedCollection functions, TransientKeyedCollection
  *  APIs always return the same value reference passed in as an argument, with mutations applied.
  *
  *  By contract, all functions have a computational complexity of O(1), unless otherwise noted.
  */

  module type S1 = {
    /** TransientKeyedCollection module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    let containsKey: k => (t 'v) => bool;
    /** [containsKey key transient] returns true if [transient] contains an association from [key] to
     *  one or more values, otherwise false.
     *
     *  By contract, [containsKey] is efficient with no worst than O(log N) performance.
     */

    let count: (t 'v) => int;
    /** [count transient] returns number of key/value pairs contained in [transient]. */

    let isEmpty: (t 'v) => bool;
    /** [isEmpty transient] returns true if [transient] is empty, otherwise false. */

    let isNotEmpty: (t 'v) => bool;
    /** [isNotEmpty transient] returns true if [transient] contains at
     *  least one value, otherwise false.
     */

    let remove: k => (t 'v) => (t 'v);
    /** [remove key transient] removes all values associated with [key] from [transient]
     *
     *  By contract, [remove] is efficient with no worst than O(log N) performance.
     */

    let removeAll: (t 'v) => (t 'v);
    /** [removeAll transient] removes all key/value pairs from [transient]. */
  };

  module type S2 = {
    /** TransientKeyedCollection module type signature for types with a parametric type arity of 1. */

    type t 'k 'v;

    let containsKey: 'k => (t 'k 'v) => bool;
    /** [containsKey key transient] returns true if [transient] contains an association from [key] to
     *  one or more values, otherwise false.
     *
     *  By contract, [containsKey] is efficient with no worst than O(log N) performance.
     */

    let count: (t 'k 'v) => int;
    /** [count transient] returns number of key/value pairs contained in [transient]. */

    let isEmpty: (t 'k 'v) => bool;
    /** [isEmpty transient] returns true if [transient] is empty, otherwise false. */

    let isNotEmpty: (t 'k 'v) => bool;
    /** [isNotEmpty transient] returns true if [transient] contains at
     *  least one value, otherwise false.
     */

    let remove: 'k => (t 'k 'v) => (t 'k 'v);
    /** [remove key transient] removes all values associated with [key] from [transient]
     *
     *  By contract, [remove] is efficient with no worst than O(log N) performance.
     */

    let removeAll: (t 'k 'v) => (t 'k 'v);
    /** [removeAll transient] removes all key/value pairs from [transient]. */
  };
};

let module NavigableKeyedCollection: {
  /** Module types implemented by KeyedCollections that are ordered or sorted and support
   *  navigation operations.
   */

  module type S1 = {
    /** NavigableKeyedCollection module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include KeyedCollection.S1 with type k := k and type t 'v := t 'v;
    include KeyedReduceableRight.S1 with type k := k and type t 'v := t 'v;

    let first: (t 'v) => (option (k, 'v));
    /** [first keyed] returns first value in [keyed] or None.
     *
     *  By contract, no worst than O(log N) performance.
     */

    let firstOrRaise: (t 'v) => (k, 'v);
    /** [firstOrRaise keyed] returns first value in [keyed] or raises an exception.
     *
     *  By contract, no worst than O(log N) performance.
     */

    let last: (t 'v) => (option (k, 'v));
    /** [last keyed] returns last value in [keyed] or None.
     *
     *  By contract, no worst than O(log N) performance.
     */

    let lastOrRaise: (t 'v) => (k, 'v);
    /** [lastOrRaise keyed] returns last value in [keyed] or raises an exception.
     *
     *  By contract, no worst than O(log N) performance.
     */

    let toIteratorRight: t 'v => Iterator.t (k, 'v);
    /* [toIteratorRight keyed] returns an Iterator that can be used to iterate over
     * the key/value pairs in [keyed] as tuples from right to left.
     */

    let toKeyedIteratorRight: t 'v => KeyedIterator.t k 'v;
    /* [toKeyedIteratorRight keyed] returns an KeyedIterator that can be used to iterate over
     * the key/value pairs in [keyed] from right to left.
     */

    let toSequenceRight: (t 'v) => (Sequence.t (k, 'v));
    /* [toSequenceRight keyed] returns an Sequence that can be used to enumerate
     * the key/value pairs in [keyed] as tuples from right to left.
     */
  };
};

let module rec Map: {
  /** A read only view of a mappings keys to values. The intent of this type is to enable
   *  interop between alternative concrete implementations such as SortedMap and HashMap.
   *  The complexity of functions in this module is dependent upon the underlying concrete implementation.
   */

   module type S1 = {
     /** Map module type signature for types with a parametric type arity of 1. */

     type k;
     type t 'v;

     include KeyedCollection.S1 with type k := k and type t 'v := t 'v;

     let get: k => (t 'v) => (option 'v);
     /** [get key map] returns the value associated with [key] in [map] or None */

     let getOrRaise: k => (t 'v) => 'v;
     /** [getOrRaise key map] returns the value associated with [key] in [map] or raises an exception */

     let toMap: (t 'v) => Map.t k 'v;
     /** [toMap map] returns a Map view of [map] */
   };

   module type S2 = {
     /** Map module type signature for types with a parametric type arity of 1. */

     type t 'k 'v;

     include KeyedCollection.S2 with type t 'k 'v := t 'k 'v;

     let get: 'k => (t 'k 'v) => (option 'v);
     /** [get key map] returns the value associated with [key] in [map] or None */

     let getOrRaise: 'k => (t 'k 'v) => 'v;
     /** [getOrRaise key map] returns the value associated with [key] in [map] or raises an exception */

     let toMap: (t 'k 'v) => Map.t 'k 'v;
     /** [toMap map] returns a Map view of [map] */
   };

  type t 'k 'v;
  /** The map type. */

  include S2 with type t 'k 'v := t 'k 'v;

  let empty: unit => (t 'k 'v);
  /** The empty Map. */

  let map: ('k => 'a => 'b) => (t 'k 'a) => (t 'k 'b);
  /** [map f map] returns lazy Map that computes values lazily by applying [f].
   *
   *  Note, the results of applying [f] to a given key/value pair are not
   *  memoized. Therefore [f] must be a pure function.
   */

  let module KeyedReducer: KeyedReducer.S2 with type t 'k 'v := t 'k 'v;
   /* KeyedReducer module for Maps. */
};

let module PersistentMap: {
  /** Module types implemented by Map collections supporting persistent mutations.
   *
   */

  module type S1 = {
    /** PersistentMap module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include PersistentKeyedCollection.S1 with type k := k and type t 'v := t 'v;
    include Map.S1 with type k := k and type t 'v := t 'v;

    let alter: k => (option 'v => option 'v) => (t 'v) => (t 'v);
    /** [alter key f map] return a PersistentMap applying the mutation function to the value
     *  associated with key or None if no association exists. If [f] returns Some, the value
     *  associated with key is either added or updated. If [f] returns None,
     *  the value associated with key is removed if an association previously existed.
     *
     *  By contract, [alter] is efficient with no worst than O(log N) performance.
     */

    let empty: unit => (t 'v);
    /** [empty ()] Return an empty PersistentMap. */

    let from: (KeyedIterator.t k 'v) => (t 'v);
    /** [from keyedIterator] returns a PersistentMap including the key/value pairs in [keyedIterator].
     *
     *  By contract, [from] is efficient with no worst than O(N log N) performance.
     */

    let fromEntries: (Iterator.t (k, 'v)) => (t 'v);
    /** [fromEntries iter] returns a PersistentMap including the key/value pairs in [iter].
     *
     *  By contract, [fromEntries] is efficient with no worst than O(N log N) performance.
     */

    let merge: (k => (option 'vAcc) => (option 'v) => (option 'vAcc)) => (t 'vAcc) => (t 'v) => (t 'vAcc);
    /** [merge f acc next] return a PersistentMap that is the result of reducing [acc] with [next].
     *  The callback [f] is applied to the union of keys from [acc] and [next], with the values
     *  associated with each key, or None. If [f] returns None, the associated key/value pair is
     *  removed from the accumulator. If [f] returns Some, the associated key/value pair is
     *  added or update to the accumulator.
     *
     *  By contract, [merge] is efficient with no worst than O(N log N) performance.
     */

    let put: k => 'v => (t 'v) => (t 'v);
    /** [put key value map] returns a PersistentMap with an association
     *  from [key] to [value] added [map].
     *
     *  By contract, [put] is efficient with no worst than O(log N) performance.
     */

    let putAll: (KeyedIterator.t k 'v) => (t 'v) => (t 'v);
    /** [putAll keyedIter map] returns a PersistentMap, adding associations from all key/value pairs
     *  in [keyedIter] to [map],
     *
     *  By contract, [putAll] is efficient with no worst than O(N log N) performance.
     */

    let putAllEntries: (Iterator.t (k, 'v)) => (t 'v) => (t 'v);
    /** [putAllEntries iter map] returns a PersistentMap, adding associations from all key/value pairs
     *  in [iter] to [map].
     *
     *  By contract, [putAllEntries] is efficient with no worst than O(N log N) performance.
     */
  };

  module type S2 = {
    /** PersistentMap module type signature for types with a parametric type arity of 1. */

    type t 'k 'v;

    include PersistentKeyedCollection.S2 with type t 'k 'v := t 'k 'v;
    include Map.S2 with type t 'k 'v := t 'k 'v;

    let alter: 'k => (option 'v => option 'v) => (t 'k 'v) => (t 'k 'v);
    /** [alter key f map] return a PersistentMap applying the mutation function to the value
     *  associated with key or None if no association exists. If [f] returns Some, the value
     *  associated with key is either added or updated. If [f] returns None,
     *  the value associated with key is removed if an association previously existed.
     *
     *  By contract, [alter] is efficient with no worst than O(log N) performance.
     */

    let merge: ('k => (option 'vAcc) => (option 'v) => (option 'vAcc)) => (t 'k 'vAcc) => (t 'k 'v) => (t 'k 'vAcc);
    /** [merge f acc next] return a PersistentMap that is the result of reducing [acc] with [next].
     *  The callback [f] is applied to the union of keys from [acc] and [next], with the values
     *  associated with each key, or None. If [f] returns None, the associated key/value pair is
     *  removed from the accumulator. If [f] returns Some, the associated key/value pair is
     *  added or update to the accumulator.
     *
     *  By contract, [merge] is efficient with no worst than O(N log N) performance.
     */

    let put: 'k => 'v => (t 'k 'v) => (t 'k 'v);
    /** [put key value map] returns a PersistentMap with an association
     *  from [key] to [value] added [map].
     *
     *  By contract, [put] is efficient with no worst than O(log N) performance.
     */

    let putAll: (KeyedIterator.t 'k 'v) => (t 'k 'v) => (t 'k 'v);
    /** [putAll keyedIter map] returns a PersistentMap, adding associations from all key/value pairs
     *  in [keyedIter] to [map],
     *
     *  By contract, [putAll] is efficient with no worst than O(N log N) performance.
     */

    let putAllEntries: (Iterator.t ('k, 'v)) => (t 'k 'v) => (t 'k 'v);
    /** [putAllEntries iter map] returns a PersistentMap, adding associations from all key/value pairs
     *  in [iter] to [map].
     *
     *  By contract, [putAllEntries] is efficient with no worst than O(N log N) performance.
     */
  };
};

let module TransientMap: {
  /** Module types implemented by transiently mutable maps. */

  module type S1 = {
    /** TransientMap module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include TransientKeyedCollection.S1 with type k := k and type t 'v := t 'v;

    let alter: k => (option 'v => option 'v) => (t 'v) => (t 'v);
    /** [alter key f transient] applies the mutation function to the value
     *  associated with key or None if no association exists. If [f] returns Some, the value
     *  associated with key is either added or updated to [transient]. If [f] returns None,
     *  the value associated with key is removed if an association previously existed in [transient].
     *
     *  By contract, [alter] is efficient with no worst than O(log N) performance.
     */

    let empty: unit => (t 'v);
    /** [empty ()] returns a new empty TransientMap. */

    let get: k => (t 'v) => (option 'v);
    /** [get key transient] returns the value associated with [key] in [transient] or None */

    let getOrRaise: k => (t 'v) => 'v;
    /** [getOrRaise key transient] returns the value associated with [key] in [transient] or raises an exception */

    let put: k => 'v => (t 'v) => (t 'v);
    /** [put key value transient] adds or replaces an association [key] to [value] in [transient].
     *
     *  By contract, [put] is efficient with no worst than O(log N) performance.
     */

    let putAll: (KeyedIterator.t k 'v) => (t 'v) => (t 'v);
    /** [putAll keyedIter transient] adds associations from all key/value pairs in [keyedIter] to [transient].
     *
     *  By contract, [putAll] is efficient with no worst than O(N log N) performance.
     */

    let putAllEntries: (Iterator.t (k, 'v)) => (t 'v) => (t 'v);
    /** [putAll keyedIter transient] adds associations from all key/value pairs in [keyedIter] to [transient].
     *
     *  By contract, [putAll] is efficient with no worst than O(N log N) performance.
     */
  };

  module type S2 = {
    type t 'k 'v;

    include TransientKeyedCollection.S2 with type t 'k 'v := t 'k 'v;

    let alter: 'k => (option 'v => option 'v) => (t 'k 'v) => (t 'k 'v);
    /** [alter key f transient] applies the mutation function to the value
     *  associated with key or None if no association exists. If [f] returns Some, the value
     *  associated with key is either added or updated to [transient]. If [f] returns None,
     *  the value associated with key is removed if an association previously existed in [transient].
     *
     *  By contract, [alter] is efficient with no worst than O(log N) performance.
     */

    let get: 'k => (t 'k 'v) => (option 'v);
    /** [get key transient] returns the value associated with [key] in [transient] or None */

    let getOrRaise: 'k => (t 'k 'v) => 'v;
    /** [getOrRaise key transient] returns the value associated with [key] in [transient] or raises an exception */

    let put: 'k => 'v => (t 'k 'v) => (t 'k 'v);
    /** [put key value transient] adds or replaces an association [key] to [value] in [transient].
     *
     *  By contract, [put] is efficient with no worst than O(log N) performance.
     */

    let putAll: (KeyedIterator.t 'k 'v) => (t 'k 'v) => (t 'k 'v);
    /** [putAll keyedIter transient] adds associations from all key/value pairs in [keyedIter] to [transient].
     *
     *  By contract, [putAll] is efficient with no worst than O(N log N) performance.
     */

    let putAllEntries: (Iterator.t ('k, 'v)) => (t 'k 'v) => (t 'k 'v);
    /** [putAll keyedIter transient] adds associations from all key/value pairs in [keyedIter] to [transient].
     *
     *  By contract, [putAll] is efficient with no worst than O(N log N) performance.
     */
  };
};

let module NavigableMap: {
  /*  Module types implemented by NavigableMap that supports navigation operations. */

  module type S1 = {
    /** NavigableMap module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include NavigableKeyedCollection.S1 with type k := k and type t 'v := t 'v;
    include Map.S1 with type k := k and type t 'v := t 'v;
  };
};

let module PersistentNavigableMap: {
  /** Module types implemented by NavigableMaps supporting persistent mutations.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S1 = {
    /** PersistentNavigableMap module type signature for types with a parametric type arity of 1. */

    type k;
    type t 'v;

    include NavigableMap.S1 with type k := k and type t 'v := t 'v;
    include PersistentMap.S1 with type k := k and type t 'v := t 'v;

    let removeFirstOrRaise: (t 'v) => (t 'v);
    /** [removeFirstOrRaise map] returns a PersistentNavigableMap without
     *  the first value or raises an exception if [map] is empty.
     */

    let removeLastOrRaise: (t 'v) => (t 'v);
    /** [removeLastOrRaise map] returns a PersistentNavigableMap without
     *  the last value or raises an exception if [map] is empty.
     */
  };
};

let module Indexed: {
  /** Collections that support efficient indexed access to values.
   *
   *  By contract, all functions must be efficient, with no worst than O(log N) performance.
   */

  module type S1 = {
    /** Indexed module type signature for types with a parametric type arity of 1. */

    type t 'a;

    include NavigableCollection.S1 with type t 'a := t 'a;

    let get: int => (t 'a) => (option 'a);
    /** [get index indexed] returns the value at [index] or None if [index] is out of bounds. */

    let getOrRaise: int => (t 'a) => 'a;
    /** [getOrRaise index indexed] returns the value at [index] or
     *  raises an exception if [index] is out of bounds.
     */

    let toKeyedIterator: (t 'a) => (KeyedIterator.t int 'a);
    /** [toKeyedIterator indexed] returns a KeyedIterator that can be used to iterate over
     *  the index/value pairs in [indexed].
     */

    let toKeyedIteratorRight: (t 'a) => (KeyedIterator.t int 'a);
    /* [toKeyedIteratorRight indexed] returns an KeyedIterator that can be used to iterate over
     * the index/value pairs in [indexed] from right to left.
     */

    let toMap: (t 'a) => (Map.t int 'a);
    /** [toMap indexed] returns a Map view of [indexed]. */
  };
};

let module rec Deque: {
  /** A double-ended queue with efficient appends [addLast], prepends [addFirst]
   *  and removals from either end of the queue [removeFirstOrRaise] [removeLastOrRaise].
   */

  type t 'a;

  include PersistentNavigableCollection.S1 with type t 'a := t 'a;

  let mutate: (t 'a) => (TransientDeque.t 'a);
  /** [mutate deque] returns a TransientDeque containing the same values as [deque]. */

  let reverse: (t 'a) => (t 'a);
  /** [reverse deque] returns a new Deque with [deque]'s values reversed.
   *
   *  Complexity: O(1)
   */

  let module ReducerRight: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Deques which reduces right. */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Deques. */
}

and TransientDeque: {
  /** A temporarily mutable Deque. Once persisted, any further operations on a
   *  TransientDeque instance will raise exceptions. Intended for implementing bulk
   *  mutation operations efficiently.
   */

  type t 'a;

  include TransientNavigableCollection.S1 with type t 'a := t 'a;
  /** The TransientDeque type. */

  let persist: (t 'a) => (Deque.t 'a);
  /** [persist transient] persists [transient] returning a Deque. Further attempts
   *  to access or mutate [transient] will raise exceptions.
   */

  let reverse: (t 'a) => (t 'a);
  /** [reverse transient] reverse [transient]'s values.
   *
   *  Complexity: O(1)
   */
};

let module rec HashMap: {
  /** A PersistentMap implemented using hashing and a comparator function to resolve hash conflicts.
   *  HashMap is implemented as a bitmapped trie of AVLTrees. Most map operations have a computational
   *  complexity of O(log32 N).
   */

  type t 'k 'v;

  include PersistentMap.S2 with type t 'k 'v := t 'k 'v;

  let emptyWith: hash::(Hash.t 'k) => comparator::(Comparator.t 'k) => (HashMap.t 'k 'v);
  /** [emptyWith hash comparator] returns an empty HashMap which uses [hash] to hash
   *  keys, and [comparator] to resolve collisions.
   */

  let fromWith: hash::(Hash.t 'k) => comparator::(Comparator.t 'k) => (KeyedIterator.t 'k 'v) => (HashMap.t 'k 'v);
  /** [fromWith hash comparator keyedIter] returns a HashMap containing all the key/value
   *  pairs in [keyedIter]. The returned HashMap uses [hash] to hash keys, and [comparator]
   *  to resolve collisions.
   */

  let fromEntriesWith: hash::(Hash.t 'k) => comparator::(Comparator.t 'k) => (Iterator.t ('k, 'v)) => (t 'k 'v);
  /** [fromEntriesWith hash comparator iter] returns a HashMap containing all the key/value
   *  pairs in [iter]. The returned HashMap uses [hash] to hash keys, and [comparator]
   *  to resolve collisions.
   */

  let mutate: (t 'k 'v) => (TransientHashMap.t 'k 'v);
  /** [mutate map] returns a TransientHashMap containing the same key/values pairs as [map]. */

  let module KeyedReducer: KeyedReducer.S2 with type t 'k 'v := t 'k 'v;
  /* KeyedReducer module for HashMaps. */
}

and TransientHashMap: {
  /** A temporarily mutable HashMap. Once persisted, any further operations on a
   *  TransientHashMap instance will throw. Intended for implementing bulk mutation
   *  operations efficiently.
   */

  type t 'k 'v;

  include TransientMap.S2 with type t 'k 'v := t 'k 'v;

  let emptyWith: hash::(Hash.t 'k) => comparator::(Comparator.t 'k) => unit => (TransientHashMap.t 'k 'v);
  /** [emptyWith hash comparator ()] returns an empty TransientHashMap which uses [hash] to hash
   *  keys, and [comparator] to resolve collisions.
   */

  let persist: (t 'k 'v) => (HashMap.t 'k 'v);
  /** [persist transient] persists [transient] returning a HashMap. Further attempts
   *  to access or mutate [transient] will raise exceptions.
   */
};

let module rec HashSet: {
  /** A PersistentSet implemented using hashing and a comparator function to resolve hash conflicts.
   *  HashSet are implemented as bitmapped tries. Most set operations have a computational
   *  complexity of O(log32 N).
   */

  type t 'a;
  /** The HashSet type. */

  include PersistentSet.S1 with type t 'a := t 'a;

  let emptyWith: hash::(Hash.t 'a) => comparator::(Comparator.t 'a) => (HashSet.t 'a);
  /** [emptyWith hash comparator] returns an empty HashSet which uses [hash] to hash
   *  keys, and [comparator] to resolve collisions.
   */

  let fromWith: hash::(Hash.t 'a) => comparator::(Comparator.t 'a) => (Iterator.t 'a) => (HashSet.t 'a);
  /** [fromWith hash comparator iter] returns a HashSet containing all the values in [iter].
   *  The returned HashSet uses [hash] to hash keys, and [comparator] to resolve collisions.
   */

  let hash: Hash.t (t 'a);
  /** An hashing function for HashSet instances. */

  let mutate: (t 'a) => (TransientHashSet.t 'a);
  /** [mutate set] returns a TransientHashSet containing the same values as [set]. */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for HashSets. */
}

and TransientHashSet: {
  /** A temporarily mutable HashSet. Once persisted, any further operations on a
   *  TransientHashMap instance will throw. Intended for implementing bulk mutation
   *  operations efficiently.
   */

  type t 'a;

  include TransientSet.S1 with type t 'a := t 'a;

  let emptyWith: hash::(Hash.t 'a) => comparator::(Comparator.t 'a) => unit => (TransientHashSet.t 'a);
  /** [emptyWith hash comparator ()] returns an empty TransientHashSet which uses [hash] to hash
   *  keys, and [comparator] to resolve collisions.
   */

  let persist: (t 'a) => (HashSet.t 'a);
  /** [persist transient] persists [transient] returning a HashSet. Further attempts
   *  to access or mutate [transient] will raise exceptions.
   */
};

let module rec IntMap: {
  /** A Map optimized for integer keys. IntMap is implemented as a bitmapped trie.
   *  Most map operations have a computational complexity of O(log32 N).
   */

  type k = int;
  type t 'v;

  include PersistentMap.S1 with type k := k and type t 'v := t 'v;

  let mutate: (t 'v) => (TransientIntMap.t 'v);
  /** [mutate map] returns a TransientIntMap containing the same key/values pairs as [map]. */

  let module KeyedReducer: KeyedReducer.S1 with type k = k and type t 'v := t 'v;
  /* KeyedReducer module for IntMaps. */
}

and TransientIntMap: {
  type k = int;
  type t 'v;

  include TransientMap.S1 with type k := k and type t 'v := t 'v;

  let persist: (t 'v) => (IntMap.t 'v);
  /** [persist transient] persists [transient] returning a IntMap. Further attempts
   *  to access or mutate [transient] will raise exceptions.
   */
};

let module IntRange: {
  /** A contiguous Set of discrete integers */

  type a = int;
  type t;

  include NavigableSet.S with type a := a and type t := t;
  include Comparable.S with type t := t;
  include Hashable.S with type t := t;

  let create: start::int => count::int => t;
  /** [create start count] returns an IntRange startint at [start] with [count].
   *  [start] may be any positive or negative integer. [count] must be greater
   *  than or equal to 0.
   */

  let module ReducerRight: Reducer.S with type a := a and type t := t;
    /* Reducer module for IntRanges which reduces right. */

  let module Reducer: Reducer.S with type a := a and type t := t;
  /* Reducer module for IntRanges. */
};

let module rec IntSet: {
  /** A PersistentSet optimized for integer values. IntSets are implemented as
   *  bitmapped tries. Most set operations have a computational complexity of O(log32 N).
   */

  type a = int;
  type t;
  /** The IntSet type. */

  include PersistentSet.S with type a := a and type t := t;

  let mutate: t => TransientIntSet.t;
  /** [mutate set] returns a TransientIntSet containing the same values as [set]. */

  let module Reducer: Reducer.S with type a = a and type t := t;
  /* Reducer module for IntSets. */
}

and TransientIntSet: {
  /** A temporarily mutable IntSet. Once persisted, any further operations on a
   *  TransientIntSet instance will throw. Intended for implementing bulk mutation
   *  operations efficiently.
   */

  type a = int;
  type t;
  /** The TransientIntSet type. */

  include TransientSet.S with type a := a and type t := t;

  let empty: unit => t;
  /** [empty ()] return a new empty TransientIntSet. */

  let persist: t => IntSet.t;
  /** [persist transient] returns a persisted IntSet. Further attempts to access or mutate [transient]
   *  will throw.
   */
};

let module List: {
  /** OCaml singly-linked list */

  type t 'a = list 'a;

  include Iterable.S1 with type t 'a := t 'a;

  let addFirst: 'a => (t 'a) => (t 'a);
  /** [addFirst value list] returns a List with [value] prepended.
   *
   *  Complexity: O(1)
   */

  let addFirstAll: (Iterator.t 'a) => (t 'a) => (t 'a);
  /** [addFirstAll iter list] returns a List with the values in [iter] prepended. */

  let empty: unit => (t 'a);
  /** [empty ()] returns an empty List. */

  let first: t 'a => (option 'a);
  /** [first list] returns first value in [list] or None.
   *
   *  Complexity: O(1)
   */

  let firstOrRaise: t 'a => 'a;
  /** [first seq] returns the first value in [list] or raises an exception.
   *
   *  Complexity: O(1)
   */

  let fromReverse: (Iterator.t 'a) => (t 'a);
  /** [fromReverse iter] returns a new List containing the values in [iter]
   *  in reverse order.
   *
   * Complexity: O(N) the number of values in [iter].
   */

  let removeAll: (t 'a) => (t 'a);
  /** [removeAll list] returns the empty List.
   *
   *  Complexity: O(1)
   */

  let removeFirstOrRaise: (t 'a) => (t 'a);
  /** [removeFirstOrRaise list] returns a List without the first value.
   *
   *  Complexity: O(1)
   */

  let return: 'a => (t 'a);
  /** [return value] returns a new List containing a single value, [value]. */

  let toSequence: (t 'a) => (Sequence.t 'a);
  /** [toSequence list] returns a Sequence of the values in [list] in order. */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Lists. */
};

let module ReadOnlyArray: {
  /** Opaque wrapper around an underlying array instance that provides read only semantics */

  type t 'a;

  include Indexed.S1 with type t 'a := t 'a;

  let empty: unit => t 'a;
  /* [empty ()] returns an empty ReadOnlyArray. */

  let init: int => (int => 'a) => (t 'a);
  /* [init count f] returns a ReadOnlyArray with size [count]. The callback [f] is called
   * for each index to initialize the value at the respective index.
   */

  let ofUnsafe: (array 'a) => (t 'a);
  /** [unsafe arr] returns a ReadOnlyArray backed by [arr]. Note, it is the caller's
   *  responsibility to ensure that [arr] is not subsequently mutated.
   */

  let module ReducerRight: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for ReadOnlyArray which reduces right. */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for ReadOnlyArrays. */
};

let module Stack: {
  /** A singly-linked stack with an O(1) count operation. */

  type t 'a;

  include PersistentSequentialCollection.S1 with type t 'a := t 'a;

  let fromList: (list 'a) => (t 'a);
  /** [fromList list] returns a Stack backed by [list].
   *
   *  Complexity: O(N)
   */

  let toList: (t 'a) => (list 'a);
  /** [toList stack] returns the underlying List backing the stack */

  let module Reducer: Reducer.S1 with type t 'a := t 'a;
  /* Reducer module for Stacks. */
};

let module SortedMap: {
  /** Sorted map implemented as an AVL tree. Most set operations
   *  have a computational complexity of O(log N).
   */

  module type S1 = {
    type k;

    type t +'v;
    /** The SortedMap type. */

    include PersistentNavigableMap.S1 with type k := k and type t 'v := t 'v;

    let module KeyedReducerRight: KeyedReducer.S1 with type k := k and type t 'v := t 'v;
    /* KeyedReducer module for SortedMaps which reduces right. */

    let module KeyedReducer: KeyedReducer.S1 with type k := k and type t 'v := t 'v;
    /* KeyedReducer module for SortedMaps. */
  };

  let module Make1: (Comparable: Comparable.S) => S1 with type k = Comparable.t;
  /** Module function to create a SortedMap. */
};

let module SortedSet: {
  /** Sorted set implemented as an AVL tree. Most set operations
   *  have a computational complexity of O(log N).
   */
  module type S = {
    type a;
    type t;

    include Comparable.S with type t := t;
    include PersistentNavigableSet.S with type a := a and type t := t;

    let module ReducerRight: Reducer.S with type a:= a and type t:= t;
    /* Reducer module for SortedSets which reduces right. */

    let module Reducer: Reducer.S with type a := a and type t := t;
    /* Reducer module for SortedSets. */
  };

  let module Make: (Comparable: Comparable.S) => S with type a = Comparable.t;
  /** Module function to create a SortedSet. */
};

let module rec Vector: {
  /** An Indexed supporting efficient prepend, appends, indexing, conctentation,
   *  and splits. Vectors are implemented as relaxed radix balanced trees. Computational
   *  is O(log32 N) for most operations, with optimizations for effection O(1) access to
   *  first and last values.
   */

  type t 'a;

  include PersistentNavigableCollection.S1 with type t 'a := t 'a;
  include Indexed.S1 with type t 'a := t 'a;

  let concat: (list (t 'a)) => (t 'a);
  /** [concat vecs] returns a Vector, concatenating together the Vectors in [vec].
  *
  *  WARNING: Not implemented
  *
  *  Complexity: O(log32 N * m)
  */

  let init: int => (int => 'a) => (t 'a);
  /* [init count f] returns a Vector with size [count]. The callback [f] is called
   * for each index to initialize the value at the respective index.
   */

  let insertAt: int => 'a => (t 'a) => (t 'a);
  /** [insertAt index value vec] returns a Vector with [value] inserted at [index].
   *
   *  WARNING: Not implemented
   *
   *  Complexity: O(log32 N)
   */

  let removeAt: int => (t 'a) => (t 'a);
  /** [removeAt index vec] returns a Vector with the value at [index] removed.
   *
   *  WARNING: Not implemented
   *
   *  Complexity: O(log32 N)
   */

  let skip: int => (t 'a) => (t 'a);
  /** [skip count vec] returns a new Vector that removes the first [count] values in [vec].
   *
   *  WARNING: Known issues for Vectors with count > 1024.
   *
   *  Complexity: O(log32 N)
   */

  let slice: start::int? => end_::int? => (t 'a) => (t 'a);

  let take: int => (t 'a) => (t 'a);
  /** [take count vec] returns a Vector that includes the first [count] values in [vec]. */

  let update: int => 'a => (t 'a) => (t 'a);
  /** [update index value cow] returns a Vector with [value]
   *  replacing the value at [index].
   *
   *  Complexity: O(log32 N)
   */

  let updateAll: (int => 'a => 'a) => (t 'a) => (t 'a);
  /** [updateAll f vec] returns a Vector updating each value
   *  in [vec] with result of applying the function [f] to each index/value pair.
   *
   *  Complexity: O(N)
   */

  let updateWith: int => ('a => 'a) => (t 'a) => (t 'a);
  /** [updateWith index f cow] returns a Vector updating the value
   *  at [index] with the result of applying the function [f] to the value.
   *
   *  Complexity: O(log32 N)
   */

  let mutate: (t 'a) => (TransientVector.t 'a);
  /** [mutate vector] returns a TransientVector containing the same values as [set]. */
  
  let map: ('a => 'b) => t 'a => t 'b;
  /** [map func vector] iterates over [vector] and calls [func] on each element returning a new vector of type [t 'b] composed of elements returned by [func]. */
  
  let mapWithIndex: (int => 'a => 'b) => t 'a => t 'b;
  /** [mapWithIndex func vector] same as [map] but also passes the index of the element to [func]. */
  
  let module ReducerRight: Reducer.S1 with type t 'a := t 'a;
  let module Reducer: Reducer.S1 with type t 'a := t 'a;
}

and TransientVector: {
  /** A temporarily mutable Vector. Once persisted, any further operations on a
   *  TransientVector instance will throw. Intended for implementing bulk mutation
   *  operations efficiently.
   */

  type t 'a;

  include TransientNavigableCollection.S1 with type t 'a := t 'a;

  let get: int => (t 'a) => (option 'a);
  /** [get index transient] returns the value at [index] or None if [index] is out of bounds. */

  let getOrRaise: int => (t 'a) => 'a;
  /** [getOrRaise index transient] returns the value at [index] or
   *  raises an exception if [index] is out of bounds.
   */

  let insertAt: int => 'a => (t 'a) => (t 'a);
  /** [insertAt index value transient] inserts value into [transient] at [index].
   *
   *  WARNING: Not implemented
   *
   *  Complexity: O(log32 N)
   */

  let removeAt: int => (t 'a) => (t 'a);
  /** [removeAt index transient] removes the value at [index].
   *
   *  WARNING: Not implemented
   *
   *  Complexity: O(log32 N)
   */

  let update: int => 'a => (t 'a) => (t 'a);
  /** [update index value transient] replaces the value at [index] with [value].
   *
   *  Complexity: O(log32 N)
   */

  let updateAll: (int => 'a => 'a) => (t 'a) => (t 'a);
  /** [updateAll f transient] updates each value in [transient] with result of applying
   *  the function [f] to each index/value pair.
   *
   *  Complexity: O(N)
   */

  let updateWith: int => ('a => 'a) => (t 'a) => (t 'a);
  /** [updateWith index f transient] updates the value at [index] with the result
   *  of applying the function [f] to the value.
   *
   *  Complexity: O(log32 N)
   */

  let persist: (t 'a) => (Vector.t 'a);
  /** [persist transient] returns a persisted Vector. Further attempts to access or mutate [transient]
  *  will throw.
  */
};
