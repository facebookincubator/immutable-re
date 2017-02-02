open Immutable;
open Printf;
open ReUnit;
open ReUnit.Expect;
open ReUnit.Test;

module type Deque = {
  type t 'a;

  let addFirst: 'a => (t 'a) => (t 'a);
  let addLast: 'a => (t 'a) => (t 'a);
  let count: (t 'a) => int;
  let empty: (t 'a);
  let every: ('a => bool) => (t 'a) => bool;
  let first: (t 'a) => 'a;
  let isEmpty: t 'a => bool;
  let isNotEmpty: t 'a => bool;
  let last: (t 'a) => 'a;
  let map: ('a => 'b) => (t 'a) => (t 'b);
  let mapReverse: ('a => 'b) => (t 'a) => (t 'b);
  let none: ('a => bool) => (t 'a) => bool;
  let reduce: ('acc => 'a => 'acc) => 'acc => (t 'a) => 'acc;
  let reduceRight: ('acc => 'a => 'acc) => 'acc => (t 'a) => 'acc;
  let removeAll: (t 'a) => (t 'a);
  let removeFirst: (t 'a) => (t 'a);
  let removeLast: (t 'a) => (t 'a);
  let reverse: (t 'a) => (t 'a);
  let some: ('a => bool) => (t 'a) => bool;
  let toSeq: (t 'a) => (Seq.t 'a);
  let toSeqReversed: (t 'a) => (Seq.t 'a);
  let tryFirst: (t 'a) => (option 'a);
  let tryLast: (t 'a) => option 'a;
};

let test (count: int) (module Deque: Deque): (list Test.t) => {
  let stackTests = StackTester.test count (module {
    type t 'a = Deque.t 'a;

    let addFirst = Deque.addFirst;
    let count = Deque.count;
    let empty = Deque.empty;
    let every = Deque.every;
    let first = Deque.first;
    let isEmpty = Deque.isEmpty;
    let isNotEmpty = Deque.isNotEmpty;
    let mapReverse = Deque.mapReverse;
    let none = Deque.none;
    let reduce = Deque.reduce;
    let removeAll = Deque.removeAll;
    let removeFirst = Deque.removeFirst;
    let reverse = Deque.reverse;
    let some = Deque.some;
    let toSeq = Deque.toSeq;
    let tryFirst = Deque.tryFirst;
  });

  [
    it (sprintf "addLast and removeLast %i elements" count) (fun () => {
      let empty = Deque.empty;

      expect (Deque.isNotEmpty empty) |> toBeEqualToFalse;
      expect (Deque.isEmpty empty) |> toBeEqualToTrue;
      expect (Deque.count empty) |> toBeEqualToInt 0;
      defer (fun () => empty |> Deque.first) |> throws;
      defer (fun () => empty |> Deque.last) |> throws;
      expect (empty |> Deque.tryFirst) |> toBeEqualToNoneOfInt;
      expect (empty |> Deque.tryLast) |> toBeEqualToNoneOfInt;

      let deque = Seq.inRange 0 (Some count) 1 |> Seq.reduce (fun acc i => {
        let acc = acc |> Deque.addLast i;

        expect (Deque.isNotEmpty acc) |> toBeEqualToTrue;
        expect (Deque.isEmpty acc) |> toBeEqualToFalse;
        expect (Deque.count acc) |> toBeEqualToInt (i + 1);
        expect (acc |> Deque.first) |> toBeEqualToInt 0;
        expect (acc |> Deque.last) |> toBeEqualToInt i;
        expect (acc |> Deque.tryFirst) |> toBeEqualToSomeOfInt 0;
        expect (acc |> Deque.tryLast) |> toBeEqualToSomeOfInt i;

        acc;
      }) empty;

      let shouldBeEmpty = Seq.inRange (count - 1) (Some count) (-1) |> Seq.reduce (fun acc i => {
        expect (Deque.isNotEmpty acc) |> toBeEqualToTrue;
        expect (Deque.isEmpty acc) |> toBeEqualToFalse;
        expect (Deque.count acc) |> toBeEqualToInt (i + 1);
        expect (acc |> Deque.first) |> toBeEqualToInt 0;
        expect (acc |> Deque.last) |> toBeEqualToInt i;
        expect (acc |> Deque.tryFirst) |> toBeEqualToSomeOfInt 0;
        expect (acc |> Deque.tryLast) |> toBeEqualToSomeOfInt i;
        acc |> Deque.removeLast;
      }) deque;

      expect (Deque.isNotEmpty shouldBeEmpty) |> toBeEqualToFalse;
      expect (Deque.isEmpty shouldBeEmpty) |> toBeEqualToTrue;
      expect (Deque.count shouldBeEmpty) |> toBeEqualToInt 0;
      defer (fun () => shouldBeEmpty |> Deque.first) |> throws;
      defer (fun () => shouldBeEmpty |> Deque.last) |> throws;
      expect (shouldBeEmpty |> Deque.tryFirst) |>  toBeEqualToNoneOfInt;
      expect (shouldBeEmpty |> Deque.tryLast) |>  toBeEqualToNoneOfInt;

      let seqsEquality = Seq.equals (Deque.toSeq deque) (Seq.inRange 0 (Some count) 1);
      expect seqsEquality |> toBeEqualToTrue;

      let seqsEquality = Seq.equals (Deque.toSeqReversed deque) (Seq.inRange (count - 1) (Some count) (-1));
      expect seqsEquality |> toBeEqualToTrue;
    }),
    it (sprintf "addLast and removeFirst %i elements" count) (fun () => {
      let empty = Deque.empty;

      expect (Deque.isNotEmpty empty) |> toBeEqualToFalse;
      expect (Deque.isEmpty empty) |> toBeEqualToTrue;
      expect (Deque.count empty) |> toBeEqualToInt 0;
      defer (fun () => empty |> Deque.first) |> throws;
      defer (fun () => empty |> Deque.last) |> throws;
      expect (empty |> Deque.tryFirst) |> toBeEqualToNoneOfInt;
      expect (empty |> Deque.tryLast) |> toBeEqualToNoneOfInt;

      let deque = Seq.inRange 0 (Some count) 1 |> Seq.reduce (fun acc i => {
        let acc = acc |> Deque.addLast i;

        expect (Deque.isNotEmpty acc) |> toBeEqualToTrue;
        expect (Deque.isEmpty acc) |> toBeEqualToFalse;
        expect (Deque.count acc) |> toBeEqualToInt (i + 1);
        expect (acc |> Deque.first) |> toBeEqualToInt 0;
        expect (acc |> Deque.last) |> toBeEqualToInt i;
        expect (acc |> Deque.tryFirst) |> toBeEqualToSomeOfInt 0;
        expect (acc |> Deque.tryLast) |> toBeEqualToSomeOfInt i;

        acc;
      }) empty;

      let shouldBeEmpty = Seq.inRange 0 (Some count) 1 |> Seq.reduce (fun acc i => {
        expect (Deque.isNotEmpty acc) |> toBeEqualToTrue;
        expect (Deque.isEmpty acc) |> toBeEqualToFalse;
        expect (Deque.count acc) |> toBeEqualToInt (count - i);
        expect (acc |> Deque.first) |> toBeEqualToInt i;
        expect (acc |> Deque.last) |> toBeEqualToInt (count - 1);
        expect (acc |> Deque.tryFirst) |> toBeEqualToSomeOfInt i;
        expect (acc |> Deque.tryLast) |> toBeEqualToSomeOfInt (count - 1);
        acc |> Deque.removeFirst;
      }) deque;

      expect (Deque.isNotEmpty shouldBeEmpty) |> toBeEqualToFalse;
      expect (Deque.isEmpty shouldBeEmpty) |> toBeEqualToTrue;
      expect (Deque.count shouldBeEmpty) |> toBeEqualToInt 0;
      defer (fun () => shouldBeEmpty |> Deque.first) |> throws;
      defer (fun () => shouldBeEmpty |> Deque.last) |> throws;
      expect (shouldBeEmpty |> Deque.tryFirst) |>  toBeEqualToNoneOfInt;
      expect (shouldBeEmpty |> Deque.tryLast) |>  toBeEqualToNoneOfInt;

      let seqsEquality = Seq.equals (Deque.toSeq deque) (Seq.inRange 0 (Some count) 1);
      expect seqsEquality |> toBeEqualToTrue;

      let seqsEquality = Seq.equals (Deque.toSeqReversed deque) (Seq.inRange (count - 1) (Some count) (-1));
      expect seqsEquality |> toBeEqualToTrue;
    }),
    it (sprintf "addFirst and removeLast %i elements" count) (fun () => {
      let empty = Deque.empty;

      expect (Deque.isNotEmpty empty) |> toBeEqualToFalse;
      expect (Deque.isEmpty empty) |> toBeEqualToTrue;
      expect (Deque.count empty) |> toBeEqualToInt 0;
      defer (fun () => empty |> Deque.first) |> throws;
      defer (fun () => empty |> Deque.last) |> throws;
      expect (empty |> Deque.tryFirst) |> toBeEqualToNoneOfInt;
      expect (empty |> Deque.tryLast) |> toBeEqualToNoneOfInt;

      let deque = Seq.inRange 0 (Some count) 1 |> Seq.reduce (fun acc i => {
        let acc = acc |> Deque.addFirst i;

        expect (Deque.isNotEmpty acc) |> toBeEqualToTrue;
        expect (Deque.isEmpty acc) |> toBeEqualToFalse;
        expect (Deque.count acc) |> toBeEqualToInt (i + 1);
        expect (acc |> Deque.first) |> toBeEqualToInt i;
        expect (acc |> Deque.last) |> toBeEqualToInt 0;
        expect (acc |> Deque.tryFirst) |> toBeEqualToSomeOfInt i;
        expect (acc |> Deque.tryLast) |> toBeEqualToSomeOfInt 0;

        acc;
      }) empty;

      let shouldBeEmpty = Seq.inRange 0 (Some count) 1 |> Seq.reduce (fun acc i => {
        expect (Deque.isNotEmpty acc) |> toBeEqualToTrue;
        expect (Deque.isEmpty acc) |> toBeEqualToFalse;
        expect (Deque.count acc) |> toBeEqualToInt (count - i);
        expect (acc |> Deque.first) |> toBeEqualToInt (count - 1);
        expect (acc |> Deque.last) |> toBeEqualToInt i;
        expect (acc |> Deque.tryFirst) |> toBeEqualToSomeOfInt (count - 1);
        expect (acc |> Deque.tryLast) |> toBeEqualToSomeOfInt i;
        acc |> Deque.removeLast;
      }) deque;

      expect (Deque.isNotEmpty shouldBeEmpty) |> toBeEqualToFalse;
      expect (Deque.isEmpty shouldBeEmpty) |> toBeEqualToTrue;
      expect (Deque.count shouldBeEmpty) |> toBeEqualToInt 0;
      defer (fun () => shouldBeEmpty |> Deque.first) |> throws;
      defer (fun () => shouldBeEmpty |> Deque.last) |> throws;
      expect (shouldBeEmpty |> Deque.tryFirst) |>  toBeEqualToNoneOfInt;
      expect (shouldBeEmpty |> Deque.tryLast) |>  toBeEqualToNoneOfInt;

      let seqsEquality = Seq.equals (Deque.toSeq deque) (Seq.inRange (count - 1) (Some count) (-1));
      expect seqsEquality |> toBeEqualToTrue;

      let seqsEquality = Seq.equals (Deque.toSeqReversed deque) (Seq.inRange 0 (Some count) 1);
      expect seqsEquality |> toBeEqualToTrue;
    }),
    it (sprintf "map %i elements" count) (fun () => {
      Seq.inRange 0 (Some count) 1
        |> Seq.reduce (fun acc i => acc |> Deque.addLast i) Deque.empty
        |> Deque.map (fun i => i + 1)
        |> Deque.toSeq
        |> expect
        |> toBeEqualToSeq string_of_int (Seq.inRange 1 (Some count) 1);
    }),
    it (sprintf "reduceRight %i elements" count) (fun () => {
      /* FIXME: This test could be better by not using a single repeated value. */
      Seq.repeat 1 (Some count)
        |> Seq.reduce (fun acc i => acc |> Deque.addFirst i) Deque.empty
        |> Deque.reduceRight (fun acc i => acc + i) 0
        |> expect
        |> toBeEqualToInt count;
    }),
    ...stackTests
  ];
};
