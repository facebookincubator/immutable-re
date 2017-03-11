open Option.Operators;

let module VectorImpl = {
  module type VectorBase = {
    type t 'a;

    let addFirst: (option Transient.Owner.t) => 'a => (t 'a) => (t 'a);
    let addLast: (option Transient.Owner.t) => 'a => (t 'a) => (t 'a);
    let count: (t 'a) => int;
    let empty: (t 'a);
    let getUnsafe: int => (t 'a) => 'a;
    let removeFirst: (option Transient.Owner.t) => (t 'a) => (t 'a);
    let removeLast: (option Transient.Owner.t) => (t 'a) => (t 'a);
    let updateUnsafe: (option Transient.Owner.t) => int => 'a => (t 'a) => (t 'a);
    let updateWithUnsafe: (option Transient.Owner.t) => int => ('a => 'a) => (t 'a) => (t 'a);
  };

  module type S = {
    type t 'a;

    let addFirst: (option Transient.Owner.t) => 'a => (t 'a) => (t 'a);
    let addFirstAll: (option Transient.Owner.t) => (Seq.t 'a) => (t 'a) => (t 'a);
    let addLast: (option Transient.Owner.t) => 'a => (t 'a) => (t 'a);
    let addLastAll: (option Transient.Owner.t) => (Seq.t 'a) => (t 'a) => (t 'a);
    let count: (t 'a) => int;
    let empty: (t 'a);
    let first: (t 'a) => 'a;
    let get: int => (t 'a) => 'a;
    let isEmpty: (t 'a) => bool;
    let isNotEmpty: (t 'a) => bool;
    let last: (t 'a) => 'a;
    let removeAll: (t 'a) => (t 'a);
    let removeFirst: (option Transient.Owner.t) => (t 'a) => (t 'a);
    let removeLast: (option Transient.Owner.t) => (t 'a) => (t 'a);
    let tryFirst: (t 'a) => (option 'a);
    let tryGet: int => (t 'a) => (option 'a);
    let tryLast: (t 'a) => (option 'a);
    let update: (option Transient.Owner.t) => int => 'a => (t 'a) => (t 'a);
    let updateWith: (option Transient.Owner.t) => int => ('a => 'a) => (t 'a) => (t 'a);
  };

  let module Make = fun (X: VectorBase) => {
    type t 'a = X.t 'a;

    let addFirstAll (owner: option Transient.Owner.t) (seq: Seq.t 'a) (vector: t 'a): (t 'a) => seq
      |> Seq.reduce (fun acc next => acc |> X.addFirst owner next) vector;

    let addLastAll (owner: option Transient.Owner.t) (seq: Seq.t 'a) (vector: t 'a): (t 'a) => seq
      |> Seq.reduce (fun acc next => acc |> X.addLast owner next) vector;

    let addFirst = X.addFirst;
    let addLast = X.addLast;
    let count = X.count;

    let get (index: int) (vector: t 'a): 'a => {
      Preconditions.failIfOutOfRange (X.count vector) index;
      X.getUnsafe index vector;
    };

    let empty: (t 'a) = X.empty;

    let first (vector: t 'a): 'a => get 0 vector;

    let isEmpty (vector: t 'a): bool =>
      (X.count vector) == 0;

    let isNotEmpty (vector: t 'a): bool =>
      (X.count vector) != 0;

    let last (vector: t 'a): 'a => get ((X.count vector) - 1) vector;

    let removeAll (_: t 'a): (t 'a) => X.empty;

    let removeFirst = X.removeFirst;

    let removeLast = X.removeLast;

    let tryGet (index: int) (vector: t 'a): (option 'a) => {
      let trieCount = count vector;
      Preconditions.noneIfIndexOutOfRange trieCount index (Functions.flip X.getUnsafe vector);
    };

    let tryFirst (vector: t 'a): (option 'a) => tryGet 0 vector;

    let tryLast (vector: t 'a): (option 'a) => tryGet ((X.count vector) - 1) vector;

    let update (owner: option Transient.Owner.t) (index: int) (value: 'a) (vector: t 'a): (t 'a) => {
      Preconditions.failIfOutOfRange (X.count vector) index;
      X.updateUnsafe owner index value vector;
    };

    let updateWith (owner: option Transient.Owner.t) (index: int) (f: 'a => 'a) (vector: t 'a): (t 'a) => {
      Preconditions.failIfOutOfRange (X.count vector) index;
      X.updateWithUnsafe owner index f vector;
    };
  };
};

type t 'a = {
  left: array 'a,
  middle: IndexedTrie.t 'a,
  right: array 'a,
};

let module PersistentVector = VectorImpl.Make {
  type nonrec t 'a = t 'a;

  let tailIsFull (arr: array 'a): bool => (CopyOnWriteArray.count arr) == IndexedTrie.width;
  let tailIsNotFull (arr: array 'a): bool => (CopyOnWriteArray.count arr) != IndexedTrie.width;

  let count ({ left, middle, right }: t 'a): int => {
    let leftCount = CopyOnWriteArray.count left;
    let middleCount = IndexedTrie.count middle;
    let rightCount = CopyOnWriteArray.count right;

    leftCount + middleCount + rightCount;
  };

  let empty = {
    left: [||],
    middle: IndexedTrie.empty,
    right: [||],
  };



  let addFirst (_: option Transient.Owner.t) (value: 'a) ({ left, middle, right }: t 'a): (t 'a) =>
    (tailIsFull left) && (CopyOnWriteArray.isNotEmpty right) ? {
      left: [| value |],
      middle: IndexedTrie.addFirstLeafUsingMutator IndexedTrie.updateLevelPersistent None left middle,
      right,
    } :

    (tailIsFull left) && (CopyOnWriteArray.isEmpty right) ? {
      left: [| value |],
      middle,
      right: left,
    } :

    {
      left: left |> CopyOnWriteArray.addFirst value,
      middle,
      right,
    };

  let addLast (_: option Transient.Owner.t) (value: 'a) ({ left, middle, right }: t 'a): (t 'a) =>
    /* If right is empty, then middle is also empty */
    (tailIsNotFull left) && (CopyOnWriteArray.isEmpty right) ? {
      left: left |> CopyOnWriteArray.addLast value,
      middle,
      right,
    } :

    (tailIsNotFull right) ? {
      left,
      middle,
      right: right |> CopyOnWriteArray.addLast value,
    } :

    {
      left,
      middle: IndexedTrie.addLastLeafUsingMutator IndexedTrie.updateLevelPersistent None right middle,
      right: [| value |],
    };

  let removeFirst (_: option Transient.Owner.t) ({ left, middle, right }: t 'a): (t 'a) => {
    let leftCount = CopyOnWriteArray.count left;
    let middleCount = IndexedTrie.count middle;
    let rightCount = CopyOnWriteArray.count right;

    leftCount > 1 ? {
      left: CopyOnWriteArray.removeFirst left,
      middle,
      right,
    } :

    middleCount > 0 ? {
      let (IndexedTrie.Leaf _ left, middle) =
        IndexedTrie.removeFirstLeafUsingMutator IndexedTrie.updateLevelPersistent None middle;
      { left, middle, right };
    } :

    rightCount > 0 ? {
      left: right,
      middle,
      right: [||],
    } :

    leftCount == 1 ? empty :

    failwith "vector is empty";
  };

  let removeLast (_: option Transient.Owner.t) ({ left, middle, right }: t 'a): (t 'a) => {
    let leftCount = CopyOnWriteArray.count left;
    let middleCount = IndexedTrie.count middle;
    let rightCount = CopyOnWriteArray.count right;

    rightCount > 1 ? {
      left,
      middle,
      right: CopyOnWriteArray.removeLast right,
    } :

    middleCount > 0 ? {
      let (middle, IndexedTrie.Leaf _ right) =
        IndexedTrie.removeLastLeafUsingMutator IndexedTrie.updateLevelPersistent None middle;
      { left, middle, right };
    } :

    rightCount == 1 ? {
      left,
      middle,
      right: [||],
    } :

    leftCount > 0 ? {
      left: CopyOnWriteArray.removeLast left,
      middle,
      right,
    } :

    failwith "vector is empty";
  };

  let getUnsafe (index: int) ({ left, middle, right }: t 'a): 'a => {
    let leftCount = CopyOnWriteArray.count left;
    let middleCount = IndexedTrie.count middle;

    let rightIndex = index - middleCount - leftCount;

    index < leftCount ? left.(index) :
    rightIndex >= 0 ? right.(rightIndex) :
    {
      let index = index - leftCount;
      middle |> IndexedTrie.get index;
    }
  };

  let updateUnsafe
      (_: option Transient.Owner.t)
      (index: int)
      (value: 'a)
      ({ left, middle, right }: t 'a): (t 'a) => {
    let leftCount = CopyOnWriteArray.count left;
    let middleCount = IndexedTrie.count middle;

    let rightIndex = index - middleCount - leftCount;

    index < leftCount ? {
      left: left |>  CopyOnWriteArray.update index value,
      middle,
      right,
    } :

    rightIndex >= 0 ? {
      left,
      middle,
      right: right |> CopyOnWriteArray.update rightIndex value,
    } :

    {
      let index = (index - leftCount);
      let middle = middle
        |> IndexedTrie.updateUsingMutator IndexedTrie.updateLevelPersistent IndexedTrie.updateLeafPersistent index value;
      { left, middle, right }
    };
  };

  let updateWithUnsafe
      (_: option Transient.Owner.t)
      (index: int)
      (f: 'a => 'a)
      ({ left, middle, right }: t 'a): (t 'a) => {
    let leftCount = CopyOnWriteArray.count left;
    let middleCount = IndexedTrie.count middle;

    let rightIndex = index - middleCount - leftCount;

    index < leftCount ? {
      left: left |>  CopyOnWriteArray.updateWith index f,
      middle,
      right,
    } :

    rightIndex >= 0 ? {
      left,
      middle,
      right: right |> CopyOnWriteArray.updateWith rightIndex f,
    } :

    {
      let index = (index - leftCount);
      let middle = middle
        |> IndexedTrie.updateWithUsingMutator IndexedTrie.updateLevelPersistent IndexedTrie.updateLeafPersistent index f;

      { left, middle, right }
    };
  };
};

type transientVectorImpl 'a = {
  left: array 'a,
  leftCount: int,
  middle: IndexedTrie.t 'a,
  right: array 'a,
  rightCount: int,
};

let tailCopyAndExpand (arr: array 'a): (array 'a) => {
  let arrCount = CopyOnWriteArray.count arr;
  let retval = Array.make IndexedTrie.width arr.(0);
  Array.blit arr 0 retval 0 (min arrCount IndexedTrie.width);
  retval;
};

let module TransientVectorImpl = VectorImpl.Make {
  type t 'a = transientVectorImpl 'a;

  let tailIsEmpty (count: int): bool => count == 0;
  let tailIsFull (count: int): bool => count == IndexedTrie.width;
  let tailIsNotEmpty (count: int): bool => count != 0;
  let tailIsNotFull (count: int): bool => count != IndexedTrie.width;

  let tailAddFirst (value: 'a) (arr: array 'a): (array 'a) => {
    let arr = (CopyOnWriteArray.count arr) == 0
      ? Array.make IndexedTrie.width value
      : arr;

    let rec loop index => index > 0 ? {
      arr.(index) = arr.(index - 1);
      loop (index - 1);
    } : ();

    loop (CopyOnWriteArray.lastIndex arr);
    arr.(0) = value;
    arr;
  };

  let tailRemoveFirst (arr: array 'a): (array 'a) => {
    let countArr = CopyOnWriteArray.count arr;
    let rec loop index => index < countArr ? {
      arr.(index - 1) = arr.(index);
      loop (index + 1);
    } : arr;

    loop 1;
  };

  let tailUpdate (index: int) (value: 'a) (arr: array 'a): (array 'a) => {
    let arr = (CopyOnWriteArray.count arr) == 0
      ? Array.make IndexedTrie.width value
      : arr;

    arr.(index) = value;
    arr;
  };

  let count ({ leftCount, middle, rightCount }: transientVectorImpl 'a): int => {
    let middleCount = IndexedTrie.count middle;
    leftCount + middleCount + rightCount;
  };

  let empty = {
    left: [||],
    leftCount: 0,
    middle: IndexedTrie.empty,
    right: [||],
    rightCount: 0,
  };

  let addFirst
      (owner: option Transient.Owner.t)
      (value: 'a)
      ({
        left,
        leftCount,
        middle,
        right,
        rightCount,
      }: transientVectorImpl 'a): (transientVectorImpl 'a) =>
    (tailIsFull leftCount) && (tailIsNotEmpty rightCount) ? {
      left: Array.make IndexedTrie.width value,
      leftCount: 1,
      middle: IndexedTrie.addFirstLeafUsingMutator (IndexedTrie.updateLevelTransient @@ Option.first @@ owner) owner left middle,
      right,
      rightCount,
    } :

    (tailIsFull leftCount) && (tailIsEmpty rightCount) ? {
      left: Array.make IndexedTrie.width value,
      leftCount: 1,
      middle,
      right: left,
      rightCount: leftCount,
    } :

    {
      left: left |> tailAddFirst value,
      leftCount: leftCount + 1,
      middle,
      right,
      rightCount,
    };

  let addLast
      (owner: option Transient.Owner.t)
      (value: 'a)
      ({
        left,
        leftCount,
        middle,
        right,
        rightCount,
      }: transientVectorImpl 'a): (transientVectorImpl 'a) =>
    /* If right is empty, then middle is also empty */
    (tailIsNotFull leftCount) && (tailIsEmpty rightCount) ? {
      left: left |> tailUpdate leftCount value,
      leftCount: leftCount + 1,
      middle,
      right,
      rightCount,
    } :

    (tailIsNotFull rightCount) ? {
      left,
      leftCount,
      middle,
      right: right |> tailUpdate rightCount value,
      rightCount: rightCount + 1,
    } :

    {
      left,
      leftCount,
      middle: IndexedTrie.addLastLeafUsingMutator (IndexedTrie.updateLevelTransient @@ Option.first @@ owner) owner right middle,
      right: Array.make IndexedTrie.width value,
      rightCount: 1,
    };

  let removeFirst
      (owner: option Transient.Owner.t)
      ({
        left,
        leftCount,
        middle,
        right,
        rightCount,
      }: transientVectorImpl 'a): (transientVectorImpl 'a) =>
    leftCount > 1 ? {
      left: tailRemoveFirst left,
      leftCount: leftCount - 1,
      middle,
      right,
      rightCount,
    } :

    (IndexedTrie.count middle) > 0 ? {
      let (IndexedTrie.Leaf leftOwner left, middle) = middle
        |> IndexedTrie.removeFirstLeafUsingMutator (IndexedTrie.updateLevelTransient @@ Option.first @@ owner) owner;
      let leftCount = CopyOnWriteArray.count left;

      let owner = Option.first owner;
      let left = switch leftOwner {
        | Some leftOwner when leftOwner === owner && leftCount == IndexedTrie.width => left
        | _ => tailCopyAndExpand left
      };

      {
        left,
        leftCount,
        middle,
        right,
        rightCount,
      };
    } :

    rightCount > 0 ? {
      left: right,
      leftCount: rightCount,
      middle,
      right: Array.make IndexedTrie.width right.(0),
      rightCount: 0,
    } :

    leftCount == 1 ? {
      left,
      leftCount: 0,
      middle,
      right,
      rightCount,
    } :

    failwith "vector is empty";

  let removeLast
      (owner: option Transient.Owner.t)
      ({
        left,
        leftCount,
        middle,
        right,
        rightCount,
      }: transientVectorImpl 'a): (transientVectorImpl 'a) =>
    rightCount > 1 ? {
      left,
      leftCount,
      middle,
      right,
      rightCount: rightCount - 1,
    } :

    (IndexedTrie.count middle) > 0 ? {
      let (middle, IndexedTrie.Leaf rightOwner right) = middle
        |> IndexedTrie.removeLastLeafUsingMutator (IndexedTrie.updateLevelTransient @@ Option.first @@ owner) owner;
      let rightCount = CopyOnWriteArray.count right;

      let owner = Option.first owner;
      let right = switch rightOwner {
        | Some rightOwner when rightOwner === owner && rightCount == IndexedTrie.width => right
        | _ => tailCopyAndExpand right
      };

      { left, leftCount, middle, right, rightCount };
    } :

    rightCount == 1 ? {
      left,
      leftCount,
      middle,
      right,
      rightCount: 0,
    } :

    leftCount > 0 ? {
      left,
      leftCount: leftCount - 1,
      middle,
      right,
      rightCount,
    } :

    failwith "vector is empty";

  let getUnsafe
      (index: int)
      ({
        left,
        leftCount,
        middle,
        right,
        _,
      }: transientVectorImpl 'a): 'a => {
    let middleCount = IndexedTrie.count middle;
    let rightIndex = index - middleCount - leftCount;

    index < leftCount ? left.(index) :

    rightIndex >= 0 ? right.(rightIndex) :

    {
      let index = index - leftCount;
      middle |> IndexedTrie.get index;
    }
  };

  let updateUnsafe
      (owner: option Transient.Owner.t)
      (index: int)
      (value: 'a)
      ({
        left,
        leftCount,
        middle,
        right,
        rightCount,
      }: transientVectorImpl 'a): (transientVectorImpl 'a) => {
    let middleCount = IndexedTrie.count middle;

    let rightIndex = index - middleCount - leftCount;

    index < leftCount ? {
      left: left |> tailUpdate index value,
      leftCount,
      middle,
      right,
      rightCount,
    } :

    rightIndex >= 0 ? {
      left,
      leftCount,
      middle,
      right: right |> tailUpdate rightIndex value,
      rightCount,
    } :

    {
      let index = (index - leftCount);
      let middle = middle |> IndexedTrie.updateUsingMutator
        (IndexedTrie.updateLevelTransient @@ Option.first @@ owner)
        (IndexedTrie.updateLeafTransient @@ Option.first @@ owner)
        index
        value;

      { left, leftCount, middle, right, rightCount }
    };
  };

  let updateWithUnsafe
      (owner: option Transient.Owner.t)
      (index: int)
      (f: 'a => 'a)
      ({
        left,
        leftCount,
        middle,
        right,
        rightCount,
      }: transientVectorImpl 'a): (transientVectorImpl 'a) => {
    let middleCount = IndexedTrie.count middle;

    let rightIndex = index - middleCount - leftCount;

    index < leftCount ? {
      left: left |> tailUpdate index (f left.(index)),
      leftCount,
      middle,
      right,
      rightCount,
    } :

    rightIndex >= 0 ? {
      left,
      leftCount,
      middle,
      right: right |> tailUpdate rightIndex (f right.(rightIndex)),
      rightCount,
    } :

    {
      let index = (index - leftCount);
      let middle = middle |> IndexedTrie.updateWithUsingMutator
        (IndexedTrie.updateLevelTransient @@ Option.first @@ owner)
        (IndexedTrie.updateLeafTransient @@ Option.first @@ owner)
        index
        f;

      { left, leftCount, middle, right, rightCount }
    };
  };
};

let addFirst value => PersistentVector.addFirst None value;
let addLast value => PersistentVector.addLast None value;
let count = PersistentVector.count;
let empty = PersistentVector.empty;
let first = PersistentVector.first;
let get = PersistentVector.get;
let isEmpty = PersistentVector.isEmpty;
let isNotEmpty = PersistentVector.isNotEmpty;
let last = PersistentVector.last;
let removeAll = PersistentVector.removeAll;
let removeFirst vector => PersistentVector.removeFirst None vector;
let removeLast vector => PersistentVector.removeLast None vector;
let tryFirst = PersistentVector.tryFirst;
let tryGet = PersistentVector.tryGet;
let tryLast = PersistentVector.tryLast;
let update index => PersistentVector.update None index;
let updateWith index => PersistentVector.updateWith None index;

module TransientVector = {
  type vector 'a = t 'a;
  type t 'a = Transient.t (transientVectorImpl 'a);

  let mutate ({ left, middle, right }: vector 'a): (t 'a) => Transient.create {
    left: (CopyOnWriteArray.count left > 0) ? tailCopyAndExpand left : [||],
    leftCount: CopyOnWriteArray.count left,
    middle,
    right: (CopyOnWriteArray.count right > 0) ? tailCopyAndExpand right : [||],
    rightCount: CopyOnWriteArray.count right,
  };

  let addFirst (value: 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.addFirst (Some owner) value);

  let addFirstAll (seq: Seq.t 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.addFirstAll (Some owner) seq);

  let addLast (value: 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.addLast (Some owner) value);

  let addLastAll (seq: Seq.t 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.addLastAll (Some owner) seq);

  let count (transient: t 'a): int =>
    transient |> Transient.get |> TransientVectorImpl.count;

  let empty () => empty |> mutate;

  let isEmpty (transient: t 'a): bool =>
    transient |> Transient.get |> TransientVectorImpl.isEmpty;

  let isNotEmpty (transient: t 'a): bool =>
    transient |> Transient.get |> TransientVectorImpl.isNotEmpty;

  let tailCompress (count: int) (arr: array 'a): (array 'a) => {
    let arrCount = CopyOnWriteArray.count arr;

    arrCount == count ? arr :
    arrCount > 0 ? {
      let retval = Array.make count arr.(0);
      Array.blit arr 0 retval 0 count;
      retval;
    } : [||];
  };

  let persist (transient: t 'a): (vector 'a) => {
    let {
      left,
      leftCount,
      middle,
      right,
      rightCount,
    } = transient |> Transient.persist;

    {
      left: left |> tailCompress leftCount,
      middle,
      right: right |> tailCompress rightCount,
    }
  };

  let removeAll (transient: t 'a): (t 'a) =>
      transient |> Transient.update (fun _ => TransientVectorImpl.removeAll);

  let removeFirst (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.removeFirst (Some owner));

  let removeLast (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.removeLast (Some owner));

  let get (index: int) (transient: t 'a): 'a =>
    transient |> Transient.get |> TransientVectorImpl.get index;

  let first (transient: t 'a): 'a =>
    transient |> Transient.get |> TransientVectorImpl.first;

  let last (transient: t 'a): 'a =>
    transient |> Transient.get |> TransientVectorImpl.last;

  let reverse (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner vector => {
      let count = TransientVectorImpl.count vector;
      let lastIndex = count - 1;

      let rec loop indexFirst indexLast => indexFirst < indexLast ? {
        let first = vector |> TransientVectorImpl.get indexFirst;
        let last = vector |> TransientVectorImpl.get indexLast;

        vector
          |> TransientVectorImpl.update (Some owner) indexFirst first
          |> TransientVectorImpl.update (Some owner) indexLast last
          |> ignore;

        loop (indexFirst + 1) (indexLast - 1)
      }: vector;

      loop 0 lastIndex;
    });

  let tryGet (index: int) (transient: t 'a): (option 'a) =>
    transient |> Transient.get |> TransientVectorImpl.tryGet index;

  let tryFirst (transient: t 'a): (option 'a) =>
    transient |> Transient.get |> TransientVectorImpl.tryFirst;

  let tryLast (transient: t 'a): (option 'a) =>
    transient |> Transient.get |> TransientVectorImpl.tryLast;

  let update (index: int) (value: 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.update (Some owner) index value);

  let updateAll (f: int => 'a => 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner ({ left, leftCount, middle, right, rightCount }) => {
      let index = ref 0;
      let updater value => {
        let result = f !index value;
        index := !index + 1;
        result;
      };

      for i in 0 to (leftCount - 1) { left.(i) = updater left.(i) };

      let middle = middle |> IndexedTrie.updateAllUsingMutator
        (IndexedTrie.updateLevelTransient owner)
        (IndexedTrie.updateLeafTransient owner)
        updater;

      for i in 0 to (rightCount - 1) { right.(i) = updater right.(i) };

      { left, leftCount, middle, right, rightCount }
    });

  let updateWith (index: int) (f: 'a => 'a) (transient: t 'a): (t 'a) =>
    transient |> Transient.update (fun owner => TransientVectorImpl.updateWith (Some owner) index f);

  /* Unimplemented functions */
  let insertAt (index: int) (value: 'a) (transient: t 'a): (t 'a) =>
    failwith "Not Implemented";

  let removeAt (index: int) (transient: t 'a): (t 'a) =>
    failwith "Not Implemented";
};

let mutate = TransientVector.mutate;

let addFirstAll (seq: Seq.t 'a) (vec: t 'a): (t 'a) => vec
  |> mutate
  |> TransientVector.addFirstAll seq
  |> TransientVector.persist;

let addLastAll (seq: Seq.t 'a) (vec: t 'a): (t 'a) => vec
  |> mutate
  |> TransientVector.addLastAll seq
  |> TransientVector.persist;

let every (f: 'a => bool) ({ left, middle, right }: t 'a): bool =>
  (CopyOnWriteArray.every f left) && (IndexedTrie.every f middle) && (CopyOnWriteArray.every f right);

let everyWithIndex (f: int => 'a => bool) (vec: t 'a): bool => {
  /* kind of a hack, but a lot less code to write */
  let index = ref 0;
  let f next => {
    let result = f !index next;
    index := !index + 1;
    result
  };

  every f vec;
};

let equalsWith
    (valueEquals: Equality.t 'a)
    ({ left: thisLeft, middle: thisMiddle, right: thisRight } as this: t 'a)
    ({ left: thatLeft, middle: thatMiddle, right: thatRight } as that: t 'a): bool =>
  this === that ? true :
  (count this) != (count that) ? false :
  CopyOnWriteArray.equalsWith valueEquals thisLeft thatLeft &&
  /* Perhaps could make this more efficient by avoiding use of Seq */
  Seq.equalsWith valueEquals (IndexedTrie.toSeq thisMiddle) (IndexedTrie.toSeq thatMiddle) &&
  CopyOnWriteArray.equalsWith valueEquals thisRight thatRight;

let equals (this: t 'a) (that: t 'a): bool =>
  equalsWith Equality.structural this that;

let find (f: 'a => bool) ({ left, middle, right }: t 'a): 'a =>
  /* FIXME: Add an operator to Option for this use case */
  switch (left |> CopyOnWriteArray.tryFind f) {
    | Some v => v
    | _ => switch (middle |> IndexedTrie.tryFind f) {
      | Some v => v
      | _ => right |> CopyOnWriteArray.find f
    }
  };

let findWithIndex (f: int => 'a => bool) (vec: t 'a): 'a => {
  /* kind of a hack, but a lot less code to write */
  let index = ref 0;
  let f next => {
    let result = f !index next;
    index := !index + 1;
    result
  };

  find f vec;
};

let fromSeq (seq: Seq.t 'a): (t 'a) =>
  empty |> addLastAll seq;

let fromSeqReversed (seq: Seq.t 'a): (t 'a) =>
  empty|> addFirstAll seq;

let indexOf (f: 'a => bool) (vec: t 'a): int => {
  /* kind of a hack, but a lot less code to write */
  let index = ref (-1);
  findWithIndex (fun i v => {
    let result = f v;
    if result { index := i };
    result;
  }) vec;

  !index >= 0 ? !index : failwith "not found";
};

let indexOfWithIndex (f: int => 'a => bool) (vec: t 'a): int => {
  /* kind of a hack, but a lot less code to write */
  let index = ref (-1);
  findWithIndex (fun i v => {
    let result = f i v;
    if result { index := i };
    result;
  }) vec;

  !index >= 0 ? !index : failwith "not found";
};

let init (count: int) (f: int => 'a): (t 'a) => Seq.inRange 0 (Some count) 1
  |> Seq.reduce (fun acc next => acc |> TransientVector.addLast (f next)) (mutate empty)
  |> TransientVector.persist;

let none (f: 'a => bool) ({ left, middle, right }: t 'a): bool =>
  (CopyOnWriteArray.none f left) && (IndexedTrie.none f middle) && (CopyOnWriteArray.none f right);

let noneWithIndex (f: int => 'a => bool) (vec: t 'a): bool => {
  /* kind of a hack, but a lot less code to write */
  let index = ref 0;
  let f next => {
    let result = f !index next;
    index := !index + 1;
    result
  };

  none f vec;
};

let some (f: 'a => bool) ({ left, middle, right }: t 'a): bool =>
  (CopyOnWriteArray.some f left) || (IndexedTrie.some f middle) || (CopyOnWriteArray.some f right);

let containsWith (valueEquals: Equality.t 'a) (value: 'a) (vec: t 'a): bool =>
  some (valueEquals value) vec;

let contains (value: 'a) (vec: t 'a): bool =>
  containsWith Equality.structural value vec;

let someWithIndex (f: int => 'a => bool) (vec: t 'a): bool => {
  /* kind of a hack, but a lot less code to write */
  let index = ref 0;
  let f next => {
    let result = f !index next;
    index := !index + 1;
    result
  };

  some f vec;
};

let reduce (f: 'acc => 'a => 'acc) (acc: 'acc) ({ left, middle, right }: t 'a): 'acc => {
  let acc = left |> CopyOnWriteArray.reduce f acc;
  let acc = middle |> IndexedTrie.reduce f acc;
  let acc = right |> CopyOnWriteArray.reduce f acc;
  acc;
};

let forEach (f: 'a => unit) (vec: t 'a): unit =>
  vec |> reduce (fun _ next => f next) ();

let reduceWithIndex (f: 'acc => int => 'a => 'acc) (acc: 'acc) (vec: t 'a): 'acc => {
  /* kind of a hack, but a lot less code to write */
  let index = ref 0;
  let reducer acc next => {
    let acc = f acc !index next;
    index := !index + 1;
    acc
  };

  reduce reducer acc vec;
};

let forEachWithIndex (f: int => 'a => unit) (vec: t 'a): unit =>
  vec |> reduceWithIndex (fun _ index next => f index next) ();

let reduceRight (f: 'acc => 'a => 'acc) (acc: 'acc) ({ left, middle, right }: t 'a): 'acc => {
  let acc = right |> CopyOnWriteArray.reduceRight f acc;
  let acc = middle |> IndexedTrie.reduceRight f acc;
  let acc = left |> CopyOnWriteArray.reduceRight f acc;
  acc;
};

let forEachReverse (f: 'a => unit) (vec: t 'a): unit =>
  vec |> reduceRight (fun _ next => f next) ();

let reduceRightWithIndex (f: 'acc => int => 'a => 'acc) (acc: 'acc) (vec: t 'a): 'acc => {
  /* kind of a hack, but a lot less code to write */
  let index = ref (count vec - 1);
  let reducer acc next => {
    let acc = f acc !index next;
    index := !index - 1;
    acc
  };

  reduceRight reducer acc vec;
};

let forEachReverseWithIndex (f: int => 'a => unit) (vec: t 'a): unit =>
  vec |> reduceRightWithIndex (fun _ index next => f index next) ();

let hashWith (hash: Hash.t 'a) (vec: t 'a): int =>
  vec |> reduce (Hash.reducer hash) Hash.initialValue;

let hash (vec: t 'a): int =>
  hashWith Hash.structural vec;

let map (f: 'a => 'b) (vector: t 'a): (t 'b) => vector
  |> reduce
    (fun acc next => acc |> TransientVector.addLast @@ f @@ next)
    (mutate empty)
  |> TransientVector.persist;

let mapWithIndex (f: int => 'a => 'b) (vector: t 'a): (t 'b) => vector
  |> reduceWithIndex
    (fun acc index next => acc |> TransientVector.addLast @@ f index @@ next)
    (mutate empty)
  |> TransientVector.persist;

let mapReverse (f: 'a => 'b) (vector: t 'a): (t 'b) => vector
  |> reduceRight
    (fun acc next => acc |> TransientVector.addLast @@ f @@ next)
    (mutate empty)
  |> TransientVector.persist;

let mapReverseWithIndex (f: int => 'a => 'b) (vector: t 'a): (t 'b) => vector
  |> reduceWithIndex
    (fun acc index next => acc |> TransientVector.addFirst @@ f index @@ next)
    (mutate empty)
  |> TransientVector.persist;

let return (value: 'a): (t 'a) =>
  empty |> addLast value;

let reverse (vector: t 'a): (t 'a) => vector
  |> reduceRight
    (fun acc next => acc |> TransientVector.addLast next)
    (mutate empty)
  |> TransientVector.persist;

let skip (skipCount: int) ({ left, middle, right } as vec: t 'a): (t 'a) => {
  let vectorCount = count vec;
  let leftCount = CopyOnWriteArray.count left;
  let middleCount = IndexedTrie.count middle;

  skipCount >= vectorCount ? empty :

  skipCount < leftCount ? {
    left: left |> CopyOnWriteArray.skip skipCount,
    middle,
    right,
  } :

  skipCount == leftCount ? {
    let (IndexedTrie.Leaf _ left, middle) =
      IndexedTrie.removeFirstLeafUsingMutator IndexedTrie.updateLevelPersistent None middle;
    { left, middle, right }
  } :

  skipCount - leftCount < middleCount ? {
    let skipCount = skipCount - leftCount;
    let (left, middle) = IndexedTrie.skip None skipCount middle;
    { left, middle, right }
  } :

  {
    let skipCount = skipCount - leftCount - middleCount;
    {
      left:  right |> CopyOnWriteArray.skip skipCount,
      middle: IndexedTrie.empty,
      right: [||],
    }
  }
};

let take (takeCount: int) ({ left, middle, right } as vec: t 'a): (t 'a) => {
  let vectorCount = count vec;
  let leftCount = CopyOnWriteArray.count left;
  let middleCount = IndexedTrie.count middle;

  takeCount >= vectorCount ? vec :
  takeCount <= leftCount ? {
    left: left |> CopyOnWriteArray.take takeCount,
    middle: IndexedTrie.empty,
    right: [||],
  } :

  takeCount - leftCount < middleCount ? {
    let takeCount = takeCount - leftCount;
    let (middle, right) = IndexedTrie.take None takeCount middle;
    { left, middle, right }
  } :

  takeCount - leftCount == middleCount ? {
    let (middle, IndexedTrie.Leaf _ right) =
      IndexedTrie.removeLastLeafUsingMutator IndexedTrie.updateLevelPersistent None middle;
    { left, middle, right }
  } :

  {
    let takeCount = takeCount - leftCount - middleCount;
    { left, middle, right: right |> CopyOnWriteArray.take takeCount }
  }
};

/* FIXME: Likely could be made more efficient with a custom implementation */
let range (startIndex: int) (takeCount: option int) (vec: t 'a): (t 'a) =>
   vec |> skip startIndex |> take (takeCount |? (count vec));

let toSeq ({ left, middle, right }: t 'a): (Seq.t 'a) => Seq.concat [
  CopyOnWriteArray.toSeq left,
  IndexedTrie.toSeq middle,
  CopyOnWriteArray.toSeq right,
];

let compareWith
    (compareValue: Comparator.t 'a)
    (this: t 'a)
    (that: t 'a): Ordering.t =>
  this === that ? Ordering.equal : Seq.compareWith compareValue (toSeq this) (toSeq that);

let compare (this: t 'a) (that: t 'a): Ordering.t =>
  compareWith Comparator.structural this that;

let toSeqReversed ({ left, middle, right }: t 'a): (Seq.t 'a) => Seq.concat [
  CopyOnWriteArray.toSeqReversed right,
  IndexedTrie.toSeqReversed middle,
  CopyOnWriteArray.toSeqReversed left,
];

let tryFind (f: 'a => bool) ({ left, middle, right }: t 'a): (option 'a) =>
  /* FIXME: Add an operator to Option for this use case */
  switch (left |> CopyOnWriteArray.tryFind f) {
    | Some _ as v => v
    | _ => switch (middle |> IndexedTrie.tryFind f) {
      | Some _ as v => v
      | _ => right |> CopyOnWriteArray.tryFind f
    }
  };

let tryFindWithIndex (f: int => 'a => bool) (vec: t 'a): (option 'a) => {
  /* kind of a hack, but a lot less code to write */
  let index = ref 0;
  let f next => {
    let result = f !index next;
    index := !index + 1;
    result
  };

  tryFind f vec;
};

let tryIndexOf (f: 'a => bool) (vec: t 'a): (option int) => {
  /* kind of a hack, but a lot less code to write */
  let index = ref (-1);
  tryFindWithIndex (fun i v => {
    let result = f v;
    if result { index := i };
    result;
  }) vec |> ignore;

  !index >= 0 ? Some !index : None;
};

let tryIndexOfWithIndex (f: int => 'a => bool) (vec: t 'a): (option int) => {
  /* kind of a hack, but a lot less code to write */
  let index = ref (-1);
  tryFindWithIndex (fun i v => {
    let result = f i v;
    if result { index := i };
    result;
  }) vec |> ignore;

  !index >= 0 ? Some !index : None;
};

let toMap (vec: t 'a): (ImmMap.t int 'a) => {
  containsWith: fun equals index value => index >= 0 && index < count vec
    ? equals (get index vec) value
    : false,
  containsKey: fun index => index >= 0 && index < count vec,
  count: count vec,
  every: fun f => everyWithIndex f vec,
  find: fun f => {
    let index = indexOfWithIndex f vec;
    (index, vec |> get index)
  },
  forEach: fun f => forEachWithIndex f vec,
  get: fun index => get index vec,
  none: fun f => noneWithIndex f vec,
  reduce: fun f acc => reduceWithIndex f acc vec,
  some: fun f => someWithIndex f vec,
  toSeq: Seq.zip2 (Seq.inRange 0 (Some (count vec)) 1) (toSeq vec),
  tryFind: fun f => tryIndexOfWithIndex f vec >>| fun index => (index, vec |> get index),
  tryGet: fun i => tryGet i vec,
  values: toSeq vec,
};

let updateAll (f: int => 'a => 'a) (vec: t 'a): (t 'a) => vec
  |> mutate
  |> TransientVector.updateAll f
  |> TransientVector.persist;

/* Unimplemented functions */
let concat (vectors: list (t 'a)): (t 'a) =>
  failwith "Not Implemented";

let insertAt (index: int) (value: 'a) (vec: t 'a): (t 'a) =>
  failwith "Not Implemented";

let removeAt (index: int) (vec: t 'a): (t 'a) =>
  failwith "Not Implemented";
