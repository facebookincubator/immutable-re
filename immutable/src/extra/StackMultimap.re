open Option.Operators;

type t 'k 'v = {
  count: int,
  map: HashMap.t 'k (Stack.t 'v),
};

let add (key: 'k) (value: 'v) ({ count, map }: t 'k 'v): (t 'k 'v) => {
  count: count + 1,
  map: map |> HashMap.alter key (fun stack => switch stack {
    | Some stack => stack
    | None => Stack.empty
  } |> Stack.addFirst value |> Option.return),
};

let addAllValues (key: 'k) (values: (Iterable.t 'v)) ({ count, map } as multimap: t 'k 'v): (t 'k 'v) => {
  let increment = ref 0;

  let newMap = map |> HashMap.alter key (fun stack => {
    switch stack {
    | Some stack =>
        let newStack = stack |> Stack.addFirstAll values;
        increment := (Stack.count newStack) - (Stack.count stack);
        Some newStack;
    | None =>
        let newStack = Stack.fromReversed values;
        if (Stack.isEmpty newStack) None else {
          increment := (Stack.count newStack);
          Some newStack;
        }
    }
  });

  if (newMap === map) multimap else {
    count: count + !increment,
    map: newMap,
  };
};

let containsWith
    (equals: Equality.t 'v)
    (key: 'k)
    (value: 'v)
    ({ map }: t 'k 'v): bool =>
  map |> HashMap.tryGet key >>| Stack.containsWith equals value |? false;

let contains (key: 'k) (value: 'v) (multimap: t 'k 'v): bool =>
  multimap |> containsWith Equality.structural key value;

let containsKey (key: 'k) ({ map }: t 'k 'v): bool =>
  map |> HashMap.containsKey key;

let count ({ count }: t 'k 'v) => count;

let empty: (t 'k 'v) = {
  count: 0,
  map: HashMap.empty,
};

let emptyWith (hashStrategy: HashStrategy.t 'k): (t 'k 'v) => {
  count: 0,
  map: HashMap.emptyWith hashStrategy,
};

let equalsWith
    (equals: Equality.t 'v)
    (this: t 'k 'v)
    (that: t 'k 'v): bool =>
  HashMap.equalsWith (Stack.equalsWith equals) this.map that.map;

let equals (this: t 'k 'v) (that: t 'k 'v): bool =>
  HashMap.equalsWith Stack.equals this.map that.map;

let every (f: 'k => 'v => bool) ({ map }: t 'k 'v): bool => {
  let f' k stack =>
    stack |> Stack.every (fun v => f k v);
  map |> HashMap.every f';
};

let forEach (f: 'k => 'v => unit) ({ map }: t 'k 'v): unit => {
  let f' k stack =>
    stack |> Stack.forEach (fun v => f k v);
  map |> HashMap.forEach f';
};

let get (key: 'k) ({ map }: t 'k 'v): (Stack.t 'v) =>
  map |> HashMap.tryGet key |? Stack.empty;

let hash ({ map }: t 'k 'v): int =>
  map |> HashMap.hashWith Stack.hash;

let hashWith (valueHash: Hash.t 'v) ({ map }: t 'k 'v): int =>
  map |> HashMap.hashWith (Stack.hashWith valueHash);

let isEmpty ({ map }: t 'k 'v): bool =>
  map |> HashMap.isEmpty;

let isNotEmpty ({ map }: t 'k 'v): bool =>
  map |> HashMap.isNotEmpty;

let keys ({ map }: t 'k 'v): (ImmSet.t 'k) =>
  map |> HashMap.keys;

let none (f: 'k => 'v => bool) ({ map }: t 'k 'v): bool => {
  let f' k stack =>
    stack |> Stack.none (fun v => f k v);
  map |> HashMap.every f';
};

let reduce (f: 'acc => 'k => 'v => 'acc) (acc: 'acc) ({ map }: t 'k 'v): 'acc => {
  let reducer acc key values =>
    values |> Stack.reduce (fun acc v => f acc key v) acc;

  map |> HashMap.reduce reducer acc;
};

let remove (key: 'k) ({ count, map } as stackMultimap: t 'k 'v): (t 'k 'v) =>
  map |> HashMap.tryGet key >>| (fun stack => ({
    count: count - (Stack.count stack),
    map: map |> HashMap.remove key,
  })) |? stackMultimap;

let removeAll ({ map }: t 'k 'v): (t 'k 'v) =>
  { count: 0, map: map |> HashMap.removeAll };

let some (f: 'k => 'v => bool) ({ map }: t 'k 'v): bool => {
  let f' k stack =>
    stack |> Stack.some (fun v => f k v);
  map |> HashMap.some f';
};

let toSeq ({ map }: t 'k 'v): (Seq.t ('k, 'v)) => map
  |> HashMap.toSeq
  |> Seq.flatMap (
    fun (k, stack) =>
      stack |> Stack.toSeq |> Seq.map (Pair.create k)
  );

let tryFind (f: 'k => 'v => bool) ({ map }: t 'k 'v): (option ('k, 'v)) => {
  let result = ref None;
  let f' k set => set |> Stack.tryFind (
    fun v => if (f k v) { result := Some (k, v); true } else false
  ) |> Option.isNotEmpty;
  map |> HashMap.tryFind f' |> ignore;
  !result
};

let find (f: 'k => 'v => bool) (multimap: t 'k 'v): ('k, 'v) =>
  multimap |> tryFind f |> Option.first;

let values ({ map }: t 'k 'v): (Iterable.t 'v) =>
  map |> HashMap.values |> Iterable.flatMap Stack.toIterable;
