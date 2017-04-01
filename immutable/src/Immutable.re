/**
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

let module Equality = Equality;

let module Ordering = Ordering;

let module Comparator = Comparator;

let module Hash = Hash;

let module Equatable = Equatable;

let module Comparable = Comparable;

let module Hashable = {
  module type S = {
    type t;

    let hash: Hash.t t;
  };
};

let module Streamable = {
  module type S1 = {
    type t 'a;

    let buffer: count::int => skip::int => (t 'a) => (t (list 'a));
    let concat: (list (t 'a)) => (t 'a);
    let defer: (unit => t 'a) => (t 'a);
    let distinctUntilChangedWith: (Equality.t 'a) => (t 'a) => (t 'a);
    let doOnNext: ('a => unit) => (t 'a) => (t 'a);
    let empty: unit => (t 'a);
    let filter: ('a => bool) => (t 'a) => (t 'a);
    let flatMap: ('a => t 'b) => (t 'a) => (t 'b);
    let flatten: (t (t 'a)) => (t 'a);
    let generate: ('a => 'a) => 'a => (t 'a);
    let map: ('a => 'b) => (t 'a) => (t 'b);
    let return: 'a => (t 'a);
    let scan: ('acc => 'a => 'acc) => 'acc => (t 'a) => (t 'acc);
    let skip: int => (t 'a) => (t 'a);
    let skipWhile: ('a => bool) => (t 'a) => (t 'a);
    let startWith: 'a => (t 'a) => (t 'a);
    let take: int => (t 'a) => (t 'a);
    let takeWhile: ('a => bool) => (t 'a) => (t 'a);
  };
};

let module Iterable = Iterable;

let module Sequence = Sequence;

let module Collection = {
  include Collection;

  module type S = {
    type a;
    type t;

    include Iterable.S with type a := a and type t := t;
    include Equatable.S with type t := t;

    let count: t => int;
    let empty: t;
    let isEmpty: t => bool;
    let isNotEmpty: t => bool;
    let toCollection: t => Collection.t a;
    let toSequence: t => (Sequence.t a);
  };

  module type S1 = {
    type t 'a;

    include Iterable.S1 with type t 'a := t 'a;

    let count: (t 'a) => int;
    let isEmpty: (t 'a) => bool;
    let isNotEmpty: (t 'a) => bool;
    let toCollection: (t 'a) => (Collection.t 'a);
    let toSequence: (t 'a) => (Sequence.t 'a);
  };

  let module Persistent = {
    module type S = {
      type a;
      type t;

      include S with type a := a and type t := t;

      let removeAll: t => t;
    };

    module type S1 = {
      type t 'a;

      include S1 with type t 'a := t 'a;

      let removeAll: t 'a => t 'a;
    };
  };

  let module Transient = {
    module type S = {
      type a;
      type t;

      let count: t => int;
      let empty: unit => t;
      let isEmpty: t => bool;
      let isNotEmpty: t => bool;
      let removeAll: t => t;
    };

    module type S1 = {
      type t 'a;

      let count: (t 'a) => int;
      let isEmpty: (t 'a) => bool;
      let isNotEmpty: (t 'a) => bool;
      let removeAll: (t 'a) => (t 'a);
    };
  };

};

let module SequentialCollection = {
  include SequentialCollection;

  module type S = {
    type a;
    type t;

    include Collection.S with type a := a and type t := t;

    let first: t => (option a);
    let firstOrRaise: t => a;
    let toSequentialCollection: t => (SequentialCollection.t a);
  };

  module type S1 = {
    type t 'a;

    include Collection.S1 with type t 'a := t 'a;

    let first: (t 'a) => (option 'a);
    let firstOrRaise: (t 'a) => 'a;
    let toSequentialCollection: (t 'a) => (SequentialCollection.t 'a);
  };

  let module Persistent = {
    module type S1 = {
      type t 'a;

      include Collection.Persistent.S1 with type t 'a := t 'a;
      include S1 with type t 'a := t 'a;

      let addFirst: 'a => (t 'a) => (t 'a);
      let addFirstAll: (Iterable.t 'a) => (t 'a) => (t 'a);
      let empty: unit => (t 'a);
      let fromReverse: (Iterable.t 'a) => (t 'a);
      let return: 'a => (t 'a);
      let removeFirstOrRaise: (t 'a) => (t 'a);
    };
  };

  let module Transient = {
    module type S1 = {
      type t 'a;

      include Collection.Transient.S1 with type t 'a := t 'a;

      let addFirst: 'a => (t 'a) => (t 'a);
      let addFirstAll: (Iterable.t 'a) => (t 'a) => (t 'a);
      let empty: unit => (t 'a);
      let first: (t 'a) => option 'a;
      let firstOrRaise: (t 'a) => 'a;
      let removeFirstOrRaise: (t 'a) => (t 'a);
    };
  };
};

let module NavigableCollection = {
  include NavigableCollection;

  module type S = {
    type a;
    type t;

    include SequentialCollection.S with type a := a and type t := t;

    let last: t => (option a);
    let lastOrRaise: t => a;
    let reduceRight: while_::('acc => a => bool)? => ('acc => a => 'acc) => 'acc => t => 'acc;
    let toIterableRight: t => (Iterable.t a);
    let toNavigableCollection: t => (NavigableCollection.t a);
    let toSequenceRight: t => (Sequence.t a);
  };

  module type S1 = {
    type t 'a;

    include SequentialCollection.S1 with type t 'a := t 'a;

    let last: (t 'a) => (option 'a);
    let lastOrRaise: (t 'a) => 'a;
    let reduceRight: while_::('acc => 'a => bool)? => ('acc => 'a => 'acc) => 'acc => (t 'a) => 'acc;
    let toIterableRight: t 'a => (Iterable.t 'a);
    let toNavigableCollection: (t 'a) => (NavigableCollection.t 'a);
    let toSequenceRight: (t 'a) => (Sequence.t 'a);
  };

  let module Persistent = {
    module type S1 = {
      type t 'a;

      include S1 with type t 'a := t 'a;
      include SequentialCollection.Persistent.S1 with type t 'a := t 'a;

      let addLast: 'a => (t 'a) => (t 'a);
      let addLastAll: (Iterable.t 'a) => (t 'a) => (t 'a);
      let from: (Iterable.t 'a) => (t 'a);
      let removeLastOrRaise: (t 'a) => (t 'a);
    };
  };

  let module Transient = {
    module type S1 = {
      type t 'a;

      include SequentialCollection.Transient.S1 with type t 'a := t 'a;

      let addLast: 'a => (t 'a) => (t 'a);
      let last: (t 'a) => option 'a;
      let lastOrRaise: (t 'a) => 'a;
      let removeLastOrRaise: (t 'a) => (t 'a);
    };
  };

};

let module Set = {
  include ImmSet;

  module type S = {
    type a;
    type t;

    include Collection.S with type a := a and type t := t;

    let contains: a => t => bool;
    let toSet: t => ImmSet.t a;
  };

  module type S1 = {
    type t 'a;

    include Collection.S1 with type t 'a := t 'a;

    let contains: 'a => (t 'a) => bool;
    let equals: Equality.t (t 'a);
    let toSet: (t 'a) => ImmSet.t 'a;
  };

  let module Persistent = {
    module type S = {
      type a;
      type t;

      include S with type a := a and type t := t;
      include Collection.Persistent.S with type a := a and type t := t;

      let add: a => t => t;
      let addAll: (Iterable.t a) => t => t;
      let from: (Iterable.t a) => t;
      let intersect: t => t => t;
      let remove: a => t => t;
      let subtract: t => t => t;
      let union: t => t => t;
    };

    module type S1 = {
      type t 'a;

      include S1 with type t 'a := t 'a;
      include Collection.Persistent.S1 with type t 'a := t 'a;

      let add: 'a => (t 'a) => (t 'a);
      let addAll: (Iterable.t 'a) => (t 'a) => (t 'a);
      let intersect: (t 'a) => (t 'a) => (t 'a);
      let remove: 'a => (t 'a) => (t 'a);
      let subtract: (t 'a) => (t 'a) => (t 'a);
      let union: (t 'a) => (t 'a) => (t 'a);
    };
  };

  let module Transient = {
    module type S = {
      type a;
      type t;

      include Collection.Transient.S with type a := a and type t := t;

      let add: a => t => t;
      let addAll: (Iterable.t a) => t => t;
      let contains: a => t => bool;
      let remove: a => t => t;
    };

    module type S1 = {
      type t 'a;

      include Collection.Transient.S1 with type t 'a := t 'a;

      let add: 'a => (t 'a) => (t 'a);
      let addAll: (Iterable.t 'a) => (t 'a) => (t 'a);
      let contains: 'a => (t 'a) => bool;
      let remove: 'a => (t 'a) => (t 'a);
    };
  };
};

let module NavigableSet = {
  include NavigableSet;

  module type S = {
    type a;
    type t;

    include Set.S with type a := a and type t := t;
    include NavigableCollection.S with type a := a and type t := t;

    let toNavigableSet: t => NavigableSet.t a;
  };

  let module Persistent = {
    module type S = {
      type a;
      type t;

      include S with type a := a and type t := t;
      include Set.Persistent.S with type a := a and type t := t;

      let removeFirstOrRaise: t => t;

      let removeLastOrRaise: t => t;
    };
  };
};

let module KeyedStreamable = {
  module type S2 = {
    type t 'k 'v;

    let concat: (list (t 'k 'v)) => (t 'k 'v);
    let defer: (unit => t 'k 'v) => (t 'k 'v);
    let distinctUntilChangedWith: keyEquals::(Equality.t 'k) => valueEquals::(Equality.t 'v) => (t 'k 'v) => (t 'k 'v);
    let doOnNext: ('k => 'v => unit) => (t 'k 'v) => (t 'k 'v);
    let empty: unit => (t 'k 'v);
    let filter: ('k => 'v => bool) => (t 'k 'v) => (t 'k 'v);
    let flatMap: ('kA => 'vA => t 'kB 'vB) => (t 'kA 'vA) => (t 'kB 'vB);
    let generate: genKey::('k => 'v => 'k) => genValue::('k => 'v => 'v) => 'k => 'v => (t 'k 'v);
    let map: keyMapper::('kA => 'vA => 'kB) => valueMapper::('kA => 'vA => 'vB) => (t 'kA 'vA) => (t 'kB 'vB);
    let mapKeys: ('a => 'v => 'b) => (t 'a 'v) => (t 'b 'v);
    let mapValues: ('k => 'a => 'b) => (t 'k 'a) => (t 'k 'b);
    let return: 'k => 'v => (t 'k 'v);
    let scan: ('acc => 'k => 'v => 'acc) => 'acc => (t 'k 'v) => (Iterable.t 'acc);
    let skip: int => (t 'k 'v) => (t 'k 'v);
    let skipWhile: ('k => 'v => bool) => (t 'k 'v) => (t 'k 'v);
    let startWith: 'k => 'v => (t 'k 'v) => (t 'k 'v);
    let take: int => (t 'k 'v) => (t 'k 'v);
    let takeWhile: ('k => 'v => bool) => (t 'k 'v) => (t 'k 'v);
  };
};

let module KeyedIterable = KeyedIterable;

let module KeyedCollection = {
  module type S1 = {
    type k;
    type t 'v;

    include KeyedIterable.S1 with type k := k and type t 'v := t 'v;

    let containsKey: k => t 'v => bool;
    let count: t 'v => int;
    let isEmpty: (t 'v) => bool;
    let isNotEmpty: (t 'v) => bool;
    let keys: (t 'v) => (Set.t k);
    let toSequence: (t 'v) => (Sequence.t (k, 'v));
  };

  module type S2 = {
    type t 'k 'v;

    include KeyedIterable.S2 with type t 'k 'v := t 'k 'v;

    let containsKey: 'k => t 'k 'v => bool;
    let count: t 'k 'v => int;
    let isEmpty: (t 'k 'v) => bool;
    let isNotEmpty: (t 'k 'v) => bool;
    let keys: (t 'k 'v) => (Set.t 'k);
    let toSequence: (t 'k 'v) => (Sequence.t ('k, 'v));
  };
  let module Persistent = {
    module type S1 = {
      type k;
      type t 'v;

      include S1 with type k := k and type t 'v := t 'v;

      let remove: k => (t 'v) => (t 'v);

      let removeAll: (t 'v) => (t 'v);
    };

    module type S2 = {
      type t 'k 'v;

      include S2 with  type t 'k 'v := t 'k 'v;

      let remove: 'k => (t 'k 'v) => (t 'k 'v);

      let removeAll: (t 'k 'v) => (t 'k 'v);
    };
  };

  let module Transient = {
    module type S1 = {
      type k;
      type t 'v;

      let containsKey: k => (t 'v) => bool;

      let count: (t 'v) => int;
      let isEmpty: (t 'v) => bool;
      let isNotEmpty: (t 'v) => bool;
      let remove: k => (t 'v) => (t 'v);
      let removeAll: (t 'v) => (t 'v);
    };

    module type S2 = {
      type t 'k 'v;

      let containsKey: 'k => (t 'k 'v) => bool;

      let count: (t 'k 'v) => int;
      let isEmpty: (t 'k 'v) => bool;
      let isNotEmpty: (t 'k 'v) => bool;
      let remove: 'k => (t 'k 'v) => (t 'k 'v);
      let removeAll: (t 'k 'v) => (t 'k 'v);
    };
  };

};

let module NavigableKeyedCollection = {
  module type S1 = {
    type k;
    type t 'v;

    include KeyedCollection.S1 with type k := k and type t 'v := t 'v;

    let first: (t 'v) => (option (k, 'v));
    let firstOrRaise: (t 'v) => (k, 'v);
    let last: (t 'v) => (option (k, 'v));
    let lastOrRaise: (t 'v) => (k, 'v);
    let reduceRight: while_::('acc => k => 'v => bool)? => ('acc => k => 'v => 'acc) => 'acc => (t 'v) => 'acc;
    let toIterableRight: t 'v => Iterable.t (k, 'v);
    let toKeyedIterableRight: t 'v => KeyedIterable.t k 'v;
    let toSequenceRight: (t 'v) => (Sequence.t (k, 'v));
  };
};

let module Map = {
  include ImmMap;

  module type S1 = {
    type k;
    type t 'v;

    include KeyedCollection.S1 with type k := k and type t 'v := t 'v;

    let get: k => (t 'v) => (option 'v);
    let getOrRaise: k => (t 'v) => 'v;
    let toMap: (t 'v) => ImmMap.t k 'v;
  };

  module type S2 = {
    type t 'k 'v;

    include KeyedCollection.S2 with type t 'k 'v := t 'k 'v;

    let get: 'k => (t 'k 'v) => (option 'v);
    let getOrRaise: 'k => (t 'k 'v) => 'v;
    let toMap: (t 'k 'v) => ImmMap.t 'k 'v;
  };

  let module Persistent = {
    module type S1 = {
      type k;
      type t 'v;

      include KeyedCollection.Persistent.S1 with type k := k and type t 'v := t 'v;
      include S1 with type k := k and type t 'v := t 'v;

      let alter: k => (option 'v => option 'v) => (t 'v) => (t 'v);

      let empty: unit => (t 'v);
      let from: (KeyedIterable.t k 'v) => (t 'v);
      let fromEntries: (Iterable.t (k, 'v)) => (t 'v);
      let merge: (k => (option 'vAcc) => (option 'v) => (option 'vAcc)) => (t 'vAcc) => (t 'v) => (t 'vAcc);
      let put: k => 'v => (t 'v) => (t 'v);
      let putAll: (KeyedIterable.t k 'v) => (t 'v) => (t 'v);
      let putAllEntries: (Iterable.t (k, 'v)) => (t 'v) => (t 'v);
    };

    module type S2 = {
      type t 'k 'v;

      include KeyedCollection.Persistent.S2 with type t 'k 'v := t 'k 'v;
      include S2 with type t 'k 'v := t 'k 'v;

      let alter: 'k => (option 'v => option 'v) => (t 'k 'v) => (t 'k 'v);
      let merge: ('k => (option 'vAcc) => (option 'v) => (option 'vAcc)) => (t 'k 'vAcc) => (t 'k 'v) => (t 'k 'vAcc);
      let put: 'k => 'v => (t 'k 'v) => (t 'k 'v);
      let putAll: (KeyedIterable.t 'k 'v) => (t 'k 'v) => (t 'k 'v);
      let putAllEntries: (Iterable.t ('k, 'v)) => (t 'k 'v) => (t 'k 'v);
    };
  };

  let module Transient = {
    module type S1 = {
      type k;

      type t 'v;

      include KeyedCollection.Transient.S1 with type k := k and type t 'v := t 'v;

      let alter: k => (option 'v => option 'v) => (t 'v) => (t 'v);
      let empty: unit => (t 'v);
      let get: k => (t 'v) => (option 'v);
      let getOrRaise: k => (t 'v) => 'v;
      let put: k => 'v => (t 'v) => (t 'v);
      let putAll: (KeyedIterable.t k 'v) => (t 'v) => (t 'v);
      let putAllEntries: (Iterable.t (k, 'v)) => (t 'v) => (t 'v);
    };

    module type S2 = {
      type t 'k 'v;

      include KeyedCollection.Transient.S2 with type t 'k 'v := t 'k 'v;

      let alter: 'k => (option 'v => option 'v) => (t 'k 'v) => (t 'k 'v);
      let get: 'k => (t 'k 'v) => (option 'v);
      let getOrRaise: 'k => (t 'k 'v) => 'v;
      let put: 'k => 'v => (t 'k 'v) => (t 'k 'v);
      let putAll: (KeyedIterable.t 'k 'v) => (t 'k 'v) => (t 'k 'v);
      let putAllEntries: (Iterable.t ('k, 'v)) => (t 'k 'v) => (t 'k 'v);
    };
  };

};

let module NavigableMap = {
  module type S1 = {
    type k;
    type t 'v;

    include NavigableKeyedCollection.S1 with type k := k and type t 'v := t 'v;
    include Map.S1 with type k := k and type t 'v := t 'v;
  };

  let module Persistent = {
    module type S1 = {
      type k;
      type t 'v;

      include S1 with type k := k and type t 'v := t 'v;
      include Map.Persistent.S1 with type k := k and type t 'v := t 'v;

      let removeFirstOrRaise: (t 'v) => (t 'v);
      let removeLastOrRaise: (t 'v) => (t 'v);
    };
  };

};

let module Indexed = {
  module type S1 = {
    type t 'a;

    include NavigableCollection.S1 with type t 'a := t 'a;

    let get: int => (t 'a) => (option 'a);
    let getOrRaise: int => (t 'a) => 'a;
    let toKeyedIterable: (t 'a) => (KeyedIterable.t int 'a);
    let toKeyedIterableRight: (t 'a) => (KeyedIterable.t int 'a);
    let toMap: (t 'a) => (Map.t int 'a);
  };
};

let module Deque = Deque;

let module HashMap = HashMap;

let module HashSet = HashSet;

let module IntMap = IntMap;

let module IntRange = IntRange;

let module IntSet = IntSet;

let module List = {
  include ImmList;

  let addFirstAll = Iterable.listAddFirstAll;
  let fromReverse = Iterable.listFromReverse;
  let toIterable = Iterable.ofList;
  let toSequence = Sequence.ofList;

  let module Reducer = Iterable.ListReducer;
};

let module ReadOnlyArray = CopyOnWriteArray;

let module SortedMap = SortedMap;

let module SortedSet = SortedSet;

let module Stack = ImmStack;

let module Vector = Vector;
