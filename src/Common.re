// Some common optic

open BsAbstract;
open Function.Infix;

let id: Iso.t('a, 'a) = {get: Functions.id, reverseGet: Functions.id};

let stringBool: Prism.t(string, bool) =
  Prism.make(
    bool_of_string_opt
    >. Option.maybe(
         ~f=(x => Ok(x)) <. Functions.id,
         ~default=Error("Not a valid bool"),
       ),
    Bool.Show.show,
  );

let stringInt: Prism.t(string, int) =
  Prism.make(
    int_of_string_opt
    >. Option.maybe(
         ~f=(x => Ok(x)) <. Functions.id,
         ~default=Error("Not a valid int"),
       ),
    Int.Show.show,
  );

module type VARIANT = {
  type t;

  let tToJs: t => string;

  let tFromJs: string => option(t);
};

let variant:
  type a.
    (~onError: string=?, (module VARIANT with type t = a)) =>
    Prism.t(string, a) =
  (~onError as message="Bad value", module Variant: VARIANT with type t = a) =>
    Prism.make(
      Variant.tFromJs
      >. (
        fun
        | Some(value) => Ok(value)
        | None => Error(message)
      ),
      Variant.tToJs,
    );

// TODO: Should be functors to allow for custom error type

// Naming the following "Array" could lead to conflicts.
module Array' = {
  let ix: 'a. int => Optional.t(array('a), 'a) =
    i =>
      Optional.make(
        x =>
          switch (Belt.Array.get(x, i)) {
          | Some(x') => Ok(x')
          | None => Error("Index cannot be found")
          },
        (c, x) =>
          Belt.Array.mapWithIndex(x, (i', entry) => i != i' ? entry : c),
      );

  let ixExn: 'a. int => Lens.t(array('a), 'a) =
    i =>
      Lens.make(
        x => x |> Belt.Array.get(_, i) |> Belt.Option.getExn,
        (c, x) =>
          Belt.Array.mapWithIndex(x, (i', entry) => i != i' ? entry : c),
      );
};

let listArray: Iso.t(list('a), array('a)) = {
  Iso.get: Belt.List.toArray,
  reverseGet: Belt.List.fromArray,
};

module Option' = {
  let optional: Optional.t(option('a), 'a) = {
    Optional.getResult:
      fun
      | Some(x) => Ok(x)
      | None => Error("Value is None"),
    set: (x, _) => Some(x),
  };
};

module Result' = {
  let optional: 'a 'e. Optional.t(result('a, string), 'a) = {
    Optional.getResult: Functions.id,
    set: (x, _) => Ok(x),
  };
};

module Tuple2 = {
  let first: 'a 'b. Lens.t(('a, 'b), 'a) = {
    get: ((x, _)) => x,
    set: (a, (_, b)) => (a, b),
  };

  let second: 'a 'b. Lens.t(('a, 'b), 'b) = {
    get: ((_, x)) => x,
    set: (b, (a, _)) => (a, b),
  };
};

module Tuple3 = {
  let first: 'a 'b 'c. Lens.t(('a, 'b, 'c), 'a) = {
    get: ((x, _, _)) => x,
    set: (a, (_, b, c)) => (a, b, c),
  };

  let second: 'a 'b 'c. Lens.t(('a, 'b, 'c), 'b) = {
    get: ((_, x, _)) => x,
    set: (b, (a, _, c)) => (a, b, c),
  };

  let third: 'a 'b 'c. Lens.t(('a, 'b, 'c), 'c) = {
    get: ((_, _, x)) => x,
    set: (c, (a, b, _)) => (a, b, c),
  };
};

module Tuple4 = {
  let first: 'a 'b 'c 'd. Lens.t(('a, 'b, 'c, 'd), 'a) = {
    get: ((x, _, _, _)) => x,
    set: (a, (_, b, c, d)) => (a, b, c, d),
  };

  let second: 'a 'b 'c 'd. Lens.t(('a, 'b, 'c, 'd), 'b) = {
    get: ((_, x, _, _)) => x,
    set: (b, (a, _, c, d)) => (a, b, c, d),
  };

  let third: 'a 'b 'c 'd. Lens.t(('a, 'b, 'c, 'd), 'c) = {
    get: ((_, _, x, _)) => x,
    set: (c, (a, b, _, d)) => (a, b, c, d),
  };

  let fourth: 'a 'b 'c 'd. Lens.t(('a, 'b, 'c, 'd), 'd) = {
    get: ((_, _, _, x)) => x,
    set: (d, (a, b, c, _)) => (a, b, c, d),
  };
};
