/* FIXME: I'm fairly certain the BitmapTrie functions can be changed to properly sort IntSet */
let module BitmapTrieIntSet = {
  type t =
    | Level int32 (array t) Transient.Owner.t
    | Entry int
    | Empty;

  type updateLevelNode = Transient.Owner.t => int => t => t => t;

  let rec add
      (updateLevelNode: updateLevelNode)
      (owner: Transient.Owner.t)
      (depth: int)
      (value: int)
      (set: t): t => switch set {
    | Level bitmap nodes _ =>
        let bit = BitmapTrie.bitPos value depth;
        let index = BitmapTrie.index bitmap bit;

        if (BitmapTrie.containsNode bitmap bit) {
          let childNode = nodes.(index);
          let newChildNode = childNode |> add updateLevelNode owner (depth + 1) value;

          if (childNode === newChildNode) set
          else (updateLevelNode owner index newChildNode set)
        } else {
          let entry = Entry value;
          let nodes = nodes |> CopyOnWriteArray.insertAt index entry;
          Level (Int32.logor bitmap bit) nodes owner;
        }
    | Entry entryValue =>
      if (value == entryValue) set
      else {
        let bitmap = BitmapTrie.bitPos entryValue depth;
        Level bitmap [| set |] owner |> add updateLevelNode owner depth value;
      }
    | Empty => Entry value;
  };

  let rec contains
      (depth: int)
      (value: int)
      (set: t): bool => switch set {
    | Level bitmap nodes _ =>
        let bit = BitmapTrie.bitPos value depth;
        let index = BitmapTrie.index bitmap bit;

        (BitmapTrie.containsNode bitmap bit) &&
        (contains (depth + 1) value nodes.(index));
    | Entry entryValue => value == entryValue
    | Empty => false;
  };

  let rec remove
      (updateLevelNode: updateLevelNode)
      (owner: Transient.Owner.t)
      (depth: int)
      (value: int)
      (set: t): t => switch set {
    | Level bitmap nodes _ =>
        let bit = BitmapTrie.bitPos value depth;
        let index = BitmapTrie.index bitmap bit;

        if (BitmapTrie.containsNode bitmap bit) {
          let childNode = nodes.(index);
          let newChildNode = childNode |> remove updateLevelNode owner (depth + 1) value;

          if (newChildNode === childNode) set
          else if (newChildNode === Empty) {
            let nodes = nodes |> CopyOnWriteArray.removeAt index;

            if (CopyOnWriteArray.count nodes > 0) (Level (Int32.logxor bitmap bit) nodes owner)
            else Empty;
          } else (updateLevelNode owner index newChildNode set);
        } else set;
    | Entry entryValue when value == entryValue =>
        Empty;
    | _ => set
  };

  let rec toSeq (set: t): (Seq.t int) => switch set {
    | Level _ nodes _ => nodes |> CopyOnWriteArray.toSeq |> Seq.flatMap toSeq
    | Entry entryValue => Seq.return entryValue;
    | Empty => Seq.empty;
  };

  let updateLevelNodePersistent
      (_: Transient.Owner.t)
      (index: int)
      (childNode: t)
      (Level bitmap nodes _: t): t =>
    Level bitmap (CopyOnWriteArray.update index childNode nodes) Transient.Owner.none;

  let updateLevelNodeTransient
      (owner: Transient.Owner.t)
      (index: int)
      (childNode: t)
      (Level bitmap nodes nodeOwner as node: t): t =>
    if (nodeOwner === owner) {
      nodes.(index) = childNode;
      node
    }
    else Level bitmap (CopyOnWriteArray.update index childNode nodes) owner;
};

type t = {
  count: int,
  root: BitmapTrieIntSet.t,
};

let add (value: int) ({ count, root } as set: t): t => {
  let newRoot = root |> BitmapTrieIntSet.add
    BitmapTrieIntSet.updateLevelNodePersistent
    Transient.Owner.none
    0
    value;

  if (newRoot === root) set
  else { count: count + 1, root: newRoot };
};

let contains (value: int) ({ root }: t): bool =>
  root |> BitmapTrieIntSet.contains 0 value;

let count ({ count }: t): int => count;

let empty: t = { count: 0, root: BitmapTrieIntSet.Empty };

let isEmpty ({ count }: t): bool => count == 0;

let isNotEmpty ({ count }: t): bool => count != 0;

let remove (value: int) ({ count, root } as set: t): t => {
  let newRoot = root |> BitmapTrieIntSet.remove
    BitmapTrieIntSet.updateLevelNodePersistent
    Transient.Owner.none
    0
    value;

  if (newRoot === root) set
  else { count: count - 1, root: newRoot };
};

let removeAll (_: t): t =>
  empty;

let toSeq ({ root }: t): (Seq.t int) =>
  root |> BitmapTrieIntSet.toSeq;

let every (f: int => bool) (set: t): bool =>
  set |> toSeq |> Seq.every f;

let find (f: int => bool) (set: t): int =>
  set |> toSeq |> Seq.find f;

let forEach (f: int => unit) (set: t): unit =>
  set |> toSeq |> Seq.forEach f;

let none (f: int => bool) (set: t): bool =>
  set |> toSeq |> Seq.none f;

let reduce (f: 'acc => int => 'acc) (acc: 'acc) (set: t): 'acc =>
  set |> toSeq |> Seq.reduce f acc;

let some (f: int => bool) (set: t): bool =>
  set |> toSeq |> Seq.some f;

let tryFind (f: int => bool) (set: t): (option int) =>
  set |> toSeq |> Seq.tryFind f;

let toIterable (set: t): (Iterable.t int) =>
  if (isEmpty set) Iterable.empty
  else { reduce: fun f acc => reduce f acc set };

let toKeyedIterable (set: t): (KeyedIterable.t int int) =>
  if (isEmpty set) KeyedIterable.empty
  else { reduce: fun f acc => set |> reduce
    (fun acc next => f acc next next)
    acc
  };

let toSet (set: t): (ImmSet.t int) =>
  if (isEmpty set) ImmSet.empty
  else {
    contains: fun v => contains v set,
    count: count set,
    every: fun f => set |> every f,
    find: fun f => set |> find f,
    forEach: fun f => set |> forEach f,
    none: fun f => set |> none f,
    reduce: fun f acc => set |> reduce f acc,
    some: fun f => set |> some f,
    toSeq: toSeq set,
    tryFind: fun f => set |> tryFind f,
  };

let equals (this: t) (that: t): bool =>
  ImmSet.equals (toSet this) (toSet that);

let hash (set: t): int =>
  set |> toSet |> ImmSet.hash;

let toMap (set: t): (ImmMap.t int int) =>
  set |> toSet |> ImmMap.ofSet;

let module TransientIntSet = {
  type intSet = t;

  type t = Transient.t intSet;

  let mutate (set: intSet): t =>
    Transient.create set;

  let addImpl
      (owner: Transient.Owner.t)
      (value: int)
      ({ count, root } as set: intSet): intSet => {
    if (set |> contains value) set
    else {
      let newRoot = root |> BitmapTrieIntSet.add
        BitmapTrieIntSet.updateLevelNodeTransient
        owner
        0
        value;

      { count: count + 1, root: newRoot };
    }
  };

  let add (value: int) (transient: t): t =>
    transient |> Transient.update1 addImpl value;

  let addAllImpl
      (owner: Transient.Owner.t)
      (iter: Iterable.t int)
      ({ count, root } as set: intSet): intSet => {
    let newCount = ref count;

    let newRoot = iter |> Iterable.reduce (fun acc value => {
      if (acc |> BitmapTrieIntSet.contains 0 value) acc
      else  {
        let newRoot = acc |> BitmapTrieIntSet.add
          BitmapTrieIntSet.updateLevelNodeTransient
          owner
          0
          value;

        newCount := !newCount + 1;
        newRoot
      }
    }) root;

    if (!newCount == count) set
    else { count: !newCount, root: newRoot };
  };

  let addAll (iter: Iterable.t int) (transient: t): t =>
    transient |> Transient.update1 addAllImpl iter;

  let contains (value: int) (transient: t): bool =>
    transient |> Transient.get |> contains value;

  let count (transient: t): int =>
    transient |> Transient.get |> count;

  let persistentEmpty = empty;

  let empty (): t =>
    empty |> mutate;

  let isEmpty (transient: t): bool =>
    transient |> Transient.get |> isEmpty;

  let isNotEmpty (transient: t): bool =>
    transient |> Transient.get |> isNotEmpty;

  let persist (transient: t): intSet =>
    transient |> Transient.persist;

  let removeImpl
      (owner: Transient.Owner.t)
      (value: int)
      ({ count, root } as set: intSet): intSet => {
    let newRoot = root |> BitmapTrieIntSet.remove
      BitmapTrieIntSet.updateLevelNodeTransient
      owner
      0
      value;

    if (newRoot === root) set
    else { count: count - 1, root: newRoot };
  };

  let remove (value: int) (transient: t): t =>
    transient |> Transient.update1 removeImpl value;

  let removeAllImpl
      (_: Transient.Owner.t)
      (_: intSet): intSet => persistentEmpty;

  let removeAll (transient: t): t =>
    transient |> Transient.update removeAllImpl;
};

let mutate = TransientIntSet.mutate;

let addAll (iter: Iterable.t int) (set: t): t =>
  set |> mutate |> TransientIntSet.addAll iter |> TransientIntSet.persist;

let from (iter: Iterable.t int): t =>
  empty |> addAll iter;

let intersect (this: t) (that: t): t =>
  /* FIXME: Improve this implementation */
  ImmSet.intersect (toSet this) (toSet that) |> from;

let subtract (this: t) (that: t): t =>
  /* FIXME: Improve this implementation */
  ImmSet.subtract (toSet this) (toSet that) |> from;

let union (this: t) (that: t): t =>
  /* FIXME: Improve this implementation */
  ImmSet.union (toSet this) (toSet that) |> from;
