/** Some common optic */

let id: Iso.t('a, 'a) = {get: x => x, reverseGet: x => x};

let stringBool: Prism.t(string, bool) =
  Prism.make(bool_of_string_opt, string_of_bool);

let stringInt: Prism.t(string, int) =
  Prism.make(int_of_string_opt, string_of_int);

module type VARIANT = {
  type t;

  let tToJs: t => string;

  let tFromJs: string => option(t);
};

let variant: type a. (module VARIANT with type t = a) => Prism.t(string, a) =
  (module Variant: VARIANT with type t = a) =>
    Prism.make(Variant.tFromJs, Variant.tToJs);

module Array = {
  let index: 'a. int => Optional.t(array('a), 'a) =
    i =>
      Optional.make(
        x => x->Array.get(i),
        (c, x) => Array.mapWithIndex(x, (i', entry) => i != i' ? entry : c),
      );

  let indexExn: 'a. int => Lens.t(array('a), 'a) =
    i =>
      Lens.make(
        x => x->Array.get(i)->Option.getExn,
        (c, x) => Array.mapWithIndex(x, (i', entry) => i != i' ? entry : c),
      );
};

let listArray: Iso.t(list('a), array('a)) = {
  Iso.get: List.toArray,
  reverseGet: List.fromArray,
};

module Option = {
  let optional: Optional.t(option('a), 'a) = {
    getOption: x => x,
    set: (x, _) => Some(x),
  };
};

module Result = {
  let optional: 'a 'e. Optional.t(result('a, 'e), 'a) = {
    Optional.getOption: x =>
      switch (x) {
      | Error(_) => None
      | Ok(x) => Some(x)
      },
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
