/**
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

open Immutable;
open ReUnit.Test;

let module Vector = {
  type t 'a = CopyOnWriteArray.t 'a;

  let addFirst = CopyOnWriteArray.addFirst;
  let addFirstAll = CopyOnWriteArray.addFirstAll;
  let addLast = CopyOnWriteArray.addLast;
  let addLastAll = CopyOnWriteArray.addLastAll;
  let compare = CopyOnWriteArray.compare;
  let compareWith = CopyOnWriteArray.compareWith;
  let concat = CopyOnWriteArray.concat;
  let contains = CopyOnWriteArray.contains;
  let containsWith = CopyOnWriteArray.containsWith;
  let count = CopyOnWriteArray.count;
  let empty = CopyOnWriteArray.empty;
  let equals = CopyOnWriteArray.equals;
  let equalsWith = CopyOnWriteArray.equalsWith;
  let every = CopyOnWriteArray.every;
  let everyWithIndex = CopyOnWriteArray.everyWithIndex;
  let find = CopyOnWriteArray.find;
  let findWithIndex = CopyOnWriteArray.findWithIndex;
  let first = CopyOnWriteArray.first;
  let forEach = CopyOnWriteArray.forEach;
  let forEachReverse = CopyOnWriteArray.forEachReverse;
  let forEachReverseWithIndex = CopyOnWriteArray.forEachReverseWithIndex;
  let forEachWithIndex = CopyOnWriteArray.forEachWithIndex;
  let from = CopyOnWriteArray.from;
  let fromReversed = CopyOnWriteArray.fromReversed;
  let get = CopyOnWriteArray.get;
  let hash = CopyOnWriteArray.hash;
  let hashWith = CopyOnWriteArray.hashWith;
  let indexOf = CopyOnWriteArray.indexOf;
  let indexOfWithIndex = CopyOnWriteArray.indexOfWithIndex;
  let init = CopyOnWriteArray.init;
  let insertAt = CopyOnWriteArray.insertAt;
  let last = CopyOnWriteArray.last;
  let isEmpty = CopyOnWriteArray.isEmpty;
  let isNotEmpty = CopyOnWriteArray.isNotEmpty;
  let map = CopyOnWriteArray.map;
  let mapWithIndex = CopyOnWriteArray.mapWithIndex;
  let mapReverse = CopyOnWriteArray.mapReverse;
  let mapReverseWithIndex = CopyOnWriteArray.mapReverseWithIndex;
  let none = CopyOnWriteArray.none;
  let noneWithIndex = CopyOnWriteArray.noneWithIndex;
  let range = CopyOnWriteArray.range;
  let reduce = CopyOnWriteArray.reduce;
  let reduceWithIndex = CopyOnWriteArray.reduceWithIndex;
  let reduceRight = CopyOnWriteArray.reduceRight;
  let reduceRightWithIndex = CopyOnWriteArray.reduceRightWithIndex;
  let removeAll = CopyOnWriteArray.removeAll;
  let removeAt = CopyOnWriteArray.removeAt;
  let removeFirst = CopyOnWriteArray.removeFirst;
  let removeLast = CopyOnWriteArray.removeLast;
  let return = CopyOnWriteArray.return;
  let reverse = CopyOnWriteArray.reverse;
  let skip = CopyOnWriteArray.skip;
  let some = CopyOnWriteArray.some;
  let someWithIndex = CopyOnWriteArray.someWithIndex;
  let take = CopyOnWriteArray.take;
  let toMap = CopyOnWriteArray.toMap;
  let toSequence = CopyOnWriteArray.toSequence;
  let toSequenceReversed = CopyOnWriteArray.toSequenceReversed;
  let tryFind = CopyOnWriteArray.tryFind;
  let tryFindWithIndex = CopyOnWriteArray.tryFindWithIndex;
  let tryFirst = CopyOnWriteArray.tryFirst;
  let tryGet = CopyOnWriteArray.tryGet;
  let tryIndexOf = CopyOnWriteArray.tryIndexOf;
  let tryIndexOfWithIndex = CopyOnWriteArray.tryIndexOfWithIndex;
  let tryLast = CopyOnWriteArray.tryLast;
  let update = CopyOnWriteArray.update;
  let updateAll = CopyOnWriteArray.updateAll;
  let updateWith = CopyOnWriteArray.updateWith;
};

let test = describe "CopyOnWriteArray" (List.fromReversed @@ Iterator.concat @@ [
  (VectorTester.test 10 (module Vector)) |> List.toIterator,
  (VectorTester.test 5000 (module Vector)) |> List.toIterator,
]);
