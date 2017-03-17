/**
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

open ReUnit.Test;

ReUnit.run (describe "Immutable.re" [
  CopyOnWriteArrayTest.test,
  DequeTest.test,
  FunctionsTest.test,/*
  HashBiMapTest.test,
  HashMapTest.test,
  HashMultisetTest.test,*/
  HashSetTest.test,/*
  HashSetMultimapTest.test,
  IntMapTest.test,*/
  MapTest.test,
  OptionTest.test,
  SequenceTest.test,
  SetTest.test,/*
  SortedMapTest.test,*/
  SortedSetTest.test,
  StackTest.test,/*
  StackMultimapTest.test,
  TableTest.test,*/
  TransientTest.test,
  VectorTest.test,
]);
