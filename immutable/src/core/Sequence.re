/**
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

open Functions.Operators;
open Option.Operators;

type iterator 'a =
  | Next 'a (t 'a)
  | Completed

and t 'a = unit => iterator 'a;

let empty: (t 'a) = fun () => Completed;

let return (value: 'a): (t 'a) => fun () =>
  Next value empty;

let rec ofList (list: list 'a): (t 'a) => fun () => switch list {
  | [value] => Next value empty
  | [value, ...tail] => Next value (ofList tail)
  | [] => Completed
};

let rec compareWith (valueCompare: Comparator.t 'a) (this: t 'a) (that: t 'a): Ordering.t =>
  if (this === that) Ordering.equal
  else switch (this (), that ()) {
    | (Next thisValue thisNext, Next thatValue thatNext) =>
        let cmp = valueCompare thisValue thatValue;

        if (cmp === Ordering.equal) (compareWith valueCompare thisNext thatNext)
        else cmp
    | (Completed, Completed) => Ordering.equal
    | (Next _ _, Completed) => Ordering.greaterThan
    | (Completed, Next _ _) => Ordering.lessThan
  };

let compare (this: t 'a) (that: t 'a): Ordering.t =>
  compareWith Comparator.structural this that;

let flatten (seq: t (t 'a)): (t 'a) => {
  let rec continuedWith (continuation: t (t 'a)) (iter: (iterator 'a)): (iterator 'a) => switch (iter) {
    | Next value next =>
        Next value (next >> continuedWith continuation);
    | Completed => continuation () |> flattenIter
  }

  and flattenIter (iter: iterator (t 'a)): (iterator 'a) => switch iter {
    | Next value next => value () |> continuedWith next
    | Completed => Completed
  };

  fun () => seq () |> flattenIter;
};

let concat (seqs: list (t 'a)): (t 'a) =>
  seqs |> ofList |> flatten;

let rec containsWith (valueEquals: Equality.t 'a) (value: 'a) (seq: t 'a): bool => switch (seq ()) {
  | Next next _ when valueEquals next value => true
  | Next _ nextSequence => containsWith valueEquals value nextSequence
  | Completed => false
};

let contains (value: 'a) (seq: t 'a): bool =>
  containsWith Equality.structural value seq;

let defer (f: unit => (t 'a)): (t 'a) => fun () => f () ();

let rec equalsWith (equality: Equality.t 'a) (this: t 'a) (that: t 'a): bool =>
  (that === this) ||
  switch (that (), this ()) {
    | (Next thisValue thisNext, Next thatValue thatNext) =>
        if (equality thisValue thatValue) (equalsWith equality thisNext thatNext)
        else false
    | (Completed, Completed) => true
    | _ => false
  };

let rec filter (f: 'a => bool) (seq: t 'a): (t 'a) => {
  let rec filterIter (f: 'a => bool) (iter: iterator 'a): iterator 'b => switch iter {
    | Next value next =>
        if (f value) (Next value (filter f next))
        else next () |> filterIter f
    | Completed => Completed
  };

  fun () => seq () |> filterIter f
};

let first (seq: t 'a): (option 'a) => switch (seq ()) {
  | Next value _ => Some value
  | Completed => None
};

let firstOrRaise (seq: t 'a): 'a => switch (seq ()) {
  | Next value _ => value
  | Completed => failwith "empty"
};

let rec generate (f: 'acc => 'acc) (acc: 'acc): (t 'acc) => fun () =>
  Next acc (generate f (f acc));

let rec map (f: 'a => 'b) (seq: t 'a): (t 'b) => fun () => switch (seq ()) {
  | Next value next =>
      Next (f value) (map f next)
  | Completed => Completed
};

let doOnNext (f: 'a => unit) (seq: t 'a): (t 'a) =>
  seq |> map (fun next => { f next; next });

let flatMap (f: 'a => (t 'b)) (seq: t 'a): (t 'b) =>
  seq |> map f |> flatten;

let ofOption (opt: option 'a): (t 'a) => switch opt {
  | Some value => return value
  | None => empty
};

let rec reduce
    (reducer: 'acc => 'a => 'acc)
    (acc: 'acc)
    (seq: t 'a): 'acc => switch (seq ()) {
  | Next value next =>
      let acc = reducer acc value;
      reduce reducer acc next
  | Completed => acc
};

let rec reduceWhile
    (predicate: 'acc => 'a => bool)
    (reducer: 'acc => 'a => 'acc)
    (acc: 'acc)
    (seq: t 'a): 'acc => switch (seq ()) {
  | Next value next when predicate acc value =>
      let acc = reducer acc value;
      reduceWhile predicate reducer acc next
  | _ => acc
};

let repeat (value: 'a): (t 'a) => {
  let rec repeatForever value () =>
    Next value (repeatForever value);
  repeatForever value;
};

let rec scan
    (reducer: 'acc => 'a => 'acc)
    (acc: 'acc)
    (seq: t 'a): (t 'acc) => fun () => switch (seq ()) {
  | Next value next => {
      let acc = reducer acc value;
      Next acc (scan reducer acc next)
    }
  | Completed => Completed
};

let toIterator (seq: t 'a): (Iterator.t 'a) =>
  if (seq === empty) Iterator.empty
  else {
    reduceWhile: fun predicate f acc =>
      reduceWhile predicate f acc seq
  };

let buffer
    (count: int)
    (skip: int)
    (seq: t 'a): (t (list 'a)) => {
  if (count <= 0 || skip <= 0) (failwith "out of range");

  let rec recurse (lst: list 'a) (counted: int) (skipped: int) (seq: t 'a) => fun () => switch (seq ()) {
    | Next value next =>
        let nextSequence =
          if (counted < count && skipped < skip) (recurse [value, ...lst] (counted + 1) (skipped + 1) next)
          else if (skipped < skip) (recurse lst counted (skipped + 1) next)
          else if (counted < count) (recurse [value, ...lst] (counted + 1) skipped next)
          else if (skip < count) (recurse [value, ...(ImmList.take (count - skip) lst)] counted skipped next)
          else (recurse [value] 1 1 next);

        if (counted == count && skipped == skip) (Next lst nextSequence)
        else (nextSequence ())
    | Completed =>
        if (counted == count && skipped == skip) (Next lst empty)
        else Completed
  };

  recurse [] 0 0 seq;
};

let distinctUntilChangedWith
    (equality: Equality.t 'a)
    (seq: t 'a): (t 'a) => fun () => {
  let rec iter
      (equality: Equality.t 'a)
      (prevValue: 'a)
      (next: t 'a): (t 'a) => fun () => switch (next ()) {
    | Next value next =>
        if (equality prevValue value) (iter equality prevValue next ())
        else Next value (iter equality value next)
    | Completed => Completed
  };

  switch (seq ()) {
    | Next value next => Next value (iter equality value next)
    | Completed => Completed
  }
};

let distinctUntilChanged (seq: t 'a): (t 'a) =>
  seq |> distinctUntilChangedWith Equality.structural;

let skip (count: int) (seq: t 'a): (t 'a) => fun () => {
  let rec skipIter (count: int) (iter: iterator 'a): (iterator 'a) => switch iter {
    | Next _ next =>
        if (count > 0) { skipIter (count - 1) (next ()) }
        else iter
    | Completed => Completed
  };

  skipIter count (seq ())
};

let skipWhile (f: 'a => bool) (seq: t 'a): (t 'a) => fun () => {
  let rec skipIter f (iter: iterator 'a): (iterator 'a) => switch iter {
    | Next value next =>
        if (f value) { skipIter f (next ()) }
        else iter
    | Completed => Completed
  };

  skipIter f (seq ())
};

let startWith (value: 'a) (seq: t 'a): (t 'a) =>
  concat [return value, seq];

let rec takeWhile (f: 'a => bool) (seq: t 'a): (t 'a) => fun () => switch (seq ()) {
  | Next value next =>
      if (f value) (Next value (takeWhile f next))
      else Completed;
  | Completed => Completed
};

let rec take (count: int) (seq: t 'a): (t 'a) => fun () =>
  if (count > 0) (switch (seq ()) {
    | Next value next =>
        Next value (take (count - 1) next)
    | Completed => Completed
  })
  else if (count == 0) Completed
  else failwith "count must be greater or equal to 0";

let rec zip (seqs: list (t 'a)): (t (list 'a)) => fun () => {
  let iters = seqs |> ImmList.mapReverse Functions.call;

  let nextSequence: (t (list 'a)) = fun () =>
    (iters |> ImmList.mapReverse (fun next => switch next {
      | Next _ next => next
      | Completed => empty
    }) |> zip) ();

  iters |> ImmList.reduce (
    fun acc next  => switch (acc, next) {
      | (Some Completed      , _              ) => acc
      | (_                   , Completed      ) => Some Completed
      | (Some (Next values _), Next value _) =>
          Next [value, ...values] nextSequence |> Option.return
      | (None                , Next value _) =>
          Next [value] nextSequence |> Option.return
    }
  ) None |? Completed;
};

let rec zip2 (a: t 'a) (b: t 'b): (t ('a, 'b)) => fun () => switch (a (), b ()) {
  | (Next aValue aNext, Next bValue bNext) =>
      Next (aValue, bValue) (zip2 aNext bNext)
  | _ => Completed
};

let rec zip3 (a: t 'a) (b: t 'b) (c: t 'c): (t ('a, 'b, 'c)) => fun () => switch (a (), b (), c ()) {
  | (Next aValue aNext, Next bValue bNext, Next cValue cNext) =>
      Next (aValue, bValue, cValue) (zip3 aNext bNext cNext)
  | _ => Completed
};

let zipLongest (seqs: list (t 'a)): (t (list (option 'a))) => seqs
  |> ImmList.mapReverse (fun seq => concat [seq |> map Option.return, repeat None ])
  |> ImmList.reverse
  |> zip
  |> takeWhile (ImmList.some Option.isNotEmpty);

let rec zipLongest2 (a: t 'a) (b: t 'b): (t (option 'a, option 'b)) => fun () => switch (a (), b ()) {
  | (Next aValue aNext, Next bValue bNext) =>
      Next (Some aValue, Some bValue) (zipLongest2 aNext bNext)
  | (Next aValue aNext, Completed) =>
      Next (Some aValue, None) (zipLongest2 aNext empty)
  | (Completed, Next bValue bNext) =>
      Next (None, Some bValue) (zipLongest2 empty bNext)
  | _ => Completed
};

let rec zipLongest3
    (a: t 'a)
    (b: t 'b)
    (c: t 'c): (t (option 'a, option 'b, option 'c)) => fun () => switch (a (), b (), c ()) {
  | (Next aValue aNext, Next bValue bNext, Next cValue cNext) =>
      Next (Some aValue, Some bValue, Some cValue) (zipLongest3 aNext bNext cNext)
  | (Next aValue aNext, Next bValue bNext, Completed) =>
      Next (Some aValue, Some bValue, None) (zipLongest3 aNext bNext empty)
  | (Next aValue aNext, Completed, Next cValue cNext) =>
      Next (Some aValue, None, Some cValue) (zipLongest3 aNext empty cNext)
  | (Completed, Next bValue bNext, Next cValue cNext) =>
      Next (None, Some bValue, Some cValue) (zipLongest3 empty bNext cNext)
  | _ => Completed
};
