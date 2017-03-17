/**
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

open Immutable;
open Printf;
open ReUnit;
open ReUnit.Expect;
open ReUnit.Test;

module type Stack = {
  type t 'a;

  let addFirst: 'a => (t 'a) => (t 'a);
  let addFirstAll: (Iterator.t 'a) => (t 'a) => (t 'a);
  let compare: (Comparator.t (t 'a));
  let compareWith: (Comparator.t 'a) => (Comparator.t (t 'a));
  let contains: 'a => (t 'a) => bool;
  let containsWith: (Equality.t 'a) => 'a => (t 'a) => bool;
  let count: (t 'a) => int;
  let empty: (t 'a);
  let equals: (Equality.t (t 'a));
  let equalsWith: (Equality.t 'a) => (Equality.t (t 'a));
  let every: ('a => bool) => (t 'a) => bool;
  let find: ('a => bool) => (t 'a) => 'a;
  let first: (t 'a) => 'a;
  let forEach: ('a => unit) => (t 'a) => unit;
  let fromReversed: (Iterator.t 'a) => (t 'a);
  let hash: (Hash.t (t 'a));
  let hashWith: (Hash.t 'a) => (Hash.t (t 'a));
  let isEmpty: t 'a => bool;
  let isNotEmpty: t 'a => bool;
  let mapReverse: ('a => 'b) => (t 'a) => (t 'b);
  let none: ('a => bool) => (t 'a) => bool;
  let reduce: ('acc => 'a => 'acc) => 'acc => (t 'a) => 'acc;
  let removeAll: (t 'a) => (t 'a);
  let removeFirst: (t 'a) => (t 'a);
  let return: 'a => (t 'a);
  let reverse: (t 'a) => (t 'a);
  let some: ('a => bool) => (t 'a) => bool;
  let toSequence: (t 'a) => (Sequence.t 'a);
  let tryFind: ('a => bool) => (t 'a) => (option 'a);
  let tryFirst: (t 'a) => (option 'a);
};

let test (count: int) (module Stack: Stack): (list Test.t) => [
  it (sprintf "addFirst and removeFirst %i elements" count) (fun () => {
    let empty = Stack.empty;

    expect (Stack.isNotEmpty empty) |> toBeEqualToFalse;
    expect (Stack.isEmpty empty) |> toBeEqualToTrue;
    expect (Stack.count empty) |> toBeEqualToInt 0;
    defer (fun () => empty |> Stack.first) |> throws;
    expect (empty |> Stack.tryFirst) |> toBeEqualToNoneOfInt;

    let stack = IntRange.create 0 count |> IntRange.reduce (fun acc i => {
      let acc = acc |> Stack.addFirst i;

      expect (Stack.isNotEmpty acc) |> toBeEqualToTrue;
      expect (Stack.isEmpty acc) |> toBeEqualToFalse;
      expect (Stack.count acc) |> toBeEqualToInt (i + 1);
      expect (acc |> Stack.first) |> toBeEqualToInt i;
      expect (acc |> Stack.tryFirst) |> toBeEqualToSomeOfInt i;

      acc;
    }) empty;

    let shouldBeEmpty = IntRange.create 0 count |> IntRange.reduceRight (fun acc i => {
      expect (Stack.isNotEmpty acc) |> toBeEqualToTrue;
      expect (Stack.isEmpty acc) |> toBeEqualToFalse;
      expect (Stack.count acc) |> toBeEqualToInt (i + 1);
      expect (acc |> Stack.first) |> toBeEqualToInt i;
      expect (acc |> Stack.tryFirst) |> toBeEqualToSomeOfInt i;
      acc |> Stack.removeFirst;
    }) stack;

    expect (Stack.isNotEmpty shouldBeEmpty) |> toBeEqualToFalse;
    expect (Stack.isEmpty shouldBeEmpty) |> toBeEqualToTrue;
    expect (Stack.count shouldBeEmpty) |> toBeEqualToInt 0;
    defer (fun () => shouldBeEmpty |> Stack.first) |> throws;
    expect (shouldBeEmpty |> Stack.tryFirst) |>  toBeEqualToNoneOfInt;

    expect @@ Stack.toSequence @@ stack |> toBeEqualToSequenceOfInt (
      IntRange.create 0 count |> IntRange.toSequenceReversed
    );
  }),

  it (sprintf "every with %i elements" count) (fun () => {
    Sequence.concat [Sequence.return false, Sequence.repeat true |> Sequence.take (count - 1)]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.every (fun v => v)
      |> expect |> toBeEqualToFalse;

    Sequence.concat [Sequence.repeat true |> Sequence.take (count - 1), Sequence.return false]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.every (fun v => v)
      |> expect |> toBeEqualToFalse;

    Sequence.concat [
      Sequence.repeat true |> Sequence.take (count / 2 - 1),
      Sequence.return false,
      Sequence.repeat true |> Sequence.take (count / 2 - 1),
    ]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.every (fun v => v)
      |> expect |> toBeEqualToFalse;

    Sequence.repeat true
      |> Sequence.take count
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.every (fun v => v)
      |> expect |> toBeEqualToTrue;
  }),

  it (sprintf "none with %i elements" count) (fun () => {
    Sequence.concat [Sequence.repeat false |> Sequence.take (count - 1), Sequence.return true]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.none (fun v => v)
      |> expect |> toBeEqualToFalse;

    Sequence.concat [Sequence.return true, Sequence.repeat false |> Sequence.take (count - 1)]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.none (fun v => v)
      |> expect |> toBeEqualToFalse;

    Sequence.concat [
      Sequence.repeat false |> Sequence.take (count / 2 - 1),
      Sequence.return true,
      Sequence.repeat false |> Sequence.take (count / 2 - 1),
    ]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.none (fun v => v)
      |> expect |> toBeEqualToFalse;

    Sequence.repeat false
      |> Sequence.take count
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.none (fun v => v)
      |> expect |> toBeEqualToTrue;
  }),

  it (sprintf "some with %i elements" count) (fun () => {
    Sequence.concat [Sequence.repeat false |> Sequence.take (count - 1), Sequence.return true]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.some (fun v => v)
      |> expect |> toBeEqualToTrue;

    Sequence.concat [Sequence.return true, Sequence.repeat false |> Sequence.take (count - 1)]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.some (fun v => v)
      |> expect |> toBeEqualToTrue;

    Sequence.concat [
      Sequence.repeat false |> Sequence.take (count / 2 - 1),
      Sequence.return true,
      Sequence.repeat false |> Sequence.take (count / 2 - 1),
    ]
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.some (fun v => v)
      |> expect |> toBeEqualToTrue;

    Sequence.repeat false
      |> Sequence.take count
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.some (fun v => v)
      |> expect |> toBeEqualToFalse;
  }),

  it (sprintf "mapReverse %i elements" count) (fun () => {
    IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.mapReverse (fun i => i + 1)
      |> Stack.toSequence
      |> expect
      |> toBeEqualToSequenceOfInt (IntRange.create 1 count |> IntRange.toSequence);
  }),

  it (sprintf "reverse %i elements" count) (fun () => {
    let stack = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let reversed = stack |> Stack.reverse;

    reversed
      |> Stack.toSequence
      |> expect
      |> toBeEqualToSequenceOfInt (IntRange.create 0 count |> IntRange.toSequence);

    reversed
      |> Stack.reverse
      |> Stack.toSequence
      |> expect
      |> toBeEqualToSequenceOfInt (stack |> Stack.toSequence);
  }),

  it (sprintf "addFirst and removeAll %i elements" count) (fun () => {
    let stack = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    expect (Stack.isNotEmpty stack) |> toBeEqualToTrue;
    expect (Stack.isEmpty stack) |> toBeEqualToFalse;
    expect (Stack.count stack) |> toBeEqualToInt count;

    let shouldBeEmpty = Stack.removeAll stack;
    expect (Stack.isNotEmpty shouldBeEmpty) |> toBeEqualToFalse;
    expect (Stack.isEmpty shouldBeEmpty) |> toBeEqualToTrue;
    expect (Stack.count shouldBeEmpty) |> toBeEqualToInt 0;
  }),

  it (sprintf "reduce with %i elements" count) (fun () => {
    /* FIXME: This test could be better by not using a single repeated value. */
    Sequence.repeat 1
      |> Sequence.take count
      |> Sequence.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty
      |> Stack.reduce (fun acc i => acc + i) 0
      |> expect
      |> toBeEqualToInt count;
  }),

  it (sprintf "find and tryFind in %i elements" count) (fun () => {
    let stack = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let find0 i => i == 0;
    let findCountMinusOne i => i == (count - 1);
    let findCountDividedByTwo i => i == (count / 2);

    expect (Stack.find find0 stack) |> toBeEqualToInt 0;
    expect (Stack.find findCountMinusOne stack) |> toBeEqualToInt (count - 1);
    expect (Stack.find findCountDividedByTwo stack) |> toBeEqualToInt (count / 2);

    expect (Stack.tryFind find0 stack) |> toBeEqualToSomeOfInt 0;
    expect (Stack.tryFind findCountMinusOne stack) |> toBeEqualToSomeOfInt (count - 1);
    expect (Stack.tryFind findCountDividedByTwo stack) |> toBeEqualToSomeOfInt (count / 2);

    defer (fun () => Stack.find Functions.alwaysFalse stack) |> throws;
    expect (Stack.tryFind Functions.alwaysFalse stack) |> toBeEqualToNoneOfInt;
  }),

  it (sprintf "compare %i elements" count) (fun () => {
    let orderingToString (ord: Ordering.t): string =>
      if (ord === Ordering.equal) "Equals"
      else if (ord === Ordering.greaterThan) "GreaterThan"
      else "LesserThan";

    let stackCount = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCountPlusOne = IntRange.create 0 (count + 1)
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCountDup = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackGreaterCount = IntRange.create 1 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    expect (Stack.compare stackCount stackCount) |> toBeEqualTo orderingToString Ordering.equal;
    expect (Stack.compare stackCount stackCountDup) |> toBeEqualTo orderingToString Ordering.equal;
    expect (Stack.compare stackGreaterCount stackCount) |> toBeEqualTo orderingToString Ordering.greaterThan;
    expect (Stack.compare stackCount stackGreaterCount) |> toBeEqualTo orderingToString Ordering.lessThan;
    expect (Stack.compare stackCountPlusOne stackCount) |> toBeEqualTo orderingToString Ordering.greaterThan;
    expect (Stack.compare stackCount stackCountPlusOne) |> toBeEqualTo orderingToString Ordering.lessThan;
  }),

  it (sprintf "equals %i elements" count) (fun () => {
    let stackCountMinusOne = IntRange.create 0 (count - 1)
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCount =  IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCountDup =  IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCountPlusOne = IntRange.create 0 (count + 1)
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    expect (Stack.equals stackCount stackCount) |> toBeEqualToTrue;
    expect (Stack.equals stackCount stackCountDup) |> toBeEqualToTrue;
    expect (Stack.equals stackCount stackCountMinusOne) |> toBeEqualToFalse;
    expect (Stack.equals stackCount stackCountPlusOne) |> toBeEqualToFalse;
  }),

  it (sprintf "hash %i elements" count) (fun () => {
    let stackCountMinusOne = IntRange.create 0 (count - 1)
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCount = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCountDup = IntRange.create 0 count
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    let stackCountPlusOne = IntRange.create 0 (count + 1)
      |> IntRange.reduce (fun acc i => acc |> Stack.addFirst i) Stack.empty;

    expect (Stack.hash stackCount) |> toBeEqualToInt (Stack.hash stackCount);
    expect (Stack.hash stackCount) |> toBeEqualToInt (Stack.hash stackCountDup);
    expect ((Stack.hash stackCount) != (Stack.hash stackCountMinusOne)) |> toBeEqualToTrue;
    expect ((Stack.hash stackCount) != (Stack.hash stackCountPlusOne)) |> toBeEqualToTrue;
  }),

  it (sprintf "addFirstAll with %i elements" count) (fun () => {
    let result = IntRange.create 0 count
      |> IntRange.toIterator
      |> Stack.fromReversed;

    (Stack.toSequence result)
      |> Sequence.equals (IntRange.create 0 count |> IntRange.toSequenceReversed)
      |> expect
      |> toBeEqualToTrue;
  }),

  it (sprintf "fromSequenceReversed with %i elements" count) (fun () => {
    let result = IntRange.create 0 count
      |> IntRange.toIterator
      |> Stack.fromReversed;

    (Stack.toSequence result)
      |> Sequence.equals (IntRange.create 0 count |> IntRange.toSequenceReversed)
      |> expect
      |> toBeEqualToTrue;
  }),

  it (sprintf "forEach with %i elements" count) (fun () => {
    let counted = ref 0;
    let result = IntRange.create 0 count
      |> IntRange.toIteratorReversed
      |> Stack.fromReversed;

    result |> Stack.forEach (fun i => {
      expect i |> toBeEqualToInt !counted;
      counted := !counted + 1;
    });
  }),

  it "return" (fun () => {
    let result = Stack.return 0;
    expect (Stack.count result) |> toBeEqualToInt 1;
    expect (Stack.first result) |> toBeEqualToInt 0;
  }),

  it (sprintf "contains with %i elements" count) (fun () => {
    let result = IntRange.create 0 count
      |> IntRange.toIterator
      |> Stack.fromReversed;

    result
      |> Stack.contains (count / 2)
      |> expect
      |> toBeEqualToTrue;

    result
      |> Stack.contains count
      |> expect
      |> toBeEqualToFalse;
  }),
];
