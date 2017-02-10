open Immutable;
open ReUnit.Test;

let test = describe "Stack" (StackTester.test 10 (module {
  type t 'a = Stack.t 'a;

  let addFirst = Stack.addFirst;
  let addFirstAll = Stack.addFirstAll;
  let compare = Stack.compare;
  let compareWith = Stack.compareWith;
  let count = Stack.count;
  let empty = Stack.empty;
  let equals = Stack.equals;
  let equalsWith = Stack.equalsWith;
  let every = Stack.every;
  let find = Stack.find;
  let first = Stack.first;
  let forEach = Stack.forEach;
  let fromSeqReversed = Stack.fromSeqReversed;
  let hash = Stack.hash;
  let hashWith = Stack.hashWith;
  let isEmpty = Stack.isEmpty;
  let isNotEmpty = Stack.isNotEmpty;
  let mapReverse = Stack.mapReverse;
  let none = Stack.none;
  let reduce = Stack.reduce;
  let removeAll = Stack.removeAll;
  let removeFirst = Stack.removeFirst;
  let return = Stack.return;
  let reverse = Stack.reverse;
  let some = Stack.some;
  let toSeq = Stack.toSeq;
  let tryFind = Stack.tryFind;
  let tryFirst = Stack.tryFirst;
}));
