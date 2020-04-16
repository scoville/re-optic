// Optionals are basically meant to be used to "get" or "extract" a value from a structure.
// In other word, we can consider that an optional with pseudo type `a -> b` will get, or extract, b from a.
// For example, an optional of type `t(array(string), string)` will get an element from an array.
// Notice that, unlike Lenses, Optionals can fail, and in the above example,
// one would get a result(string, 'e), not a string.
// They are an unsafe version of Lenses.

open BsAbstract;
open Function.Infix;

// TODO: Should be a functor to allow for custom error type

module Type = {
  type t('a, 'b) = {
    getResult: 'a => result('b, string),
    set: ('b, 'a) => 'a,
  };
};

include Type;

let make: 'a 'b. ('a => result('b, string), ('b, 'a) => 'a) => t('a, 'b) =
  (getResult, set) => {getResult, set};

module Semigroupoid: Interface.SEMIGROUPOID with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let compose = (inner, outer) =>
    make(
      a =>
        switch (outer.getResult(a)) {
        | Ok(x) => inner.getResult(x)

        | Error(_) as error => error
        },
      (c, a) =>
        outer.getResult(a)
        |> Belt.Result.map(_, inner.set(c) >. Function.flip(outer.set, a))
        |> Belt.Result.getWithDefault(_, a),
    );
};

let overResult: 'a 'b. (t('a, 'b), 'b => 'b, 'a) => result('a, string) =
  (optional, f, a) =>
    optional.getResult(a)
    |> Belt.Result.map(_, f >. Function.flip(optional.set, a));

module Overable': Overable.OVERABLE with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let over = (optional, f, x) =>
    overResult(optional, f, x) |> Belt.Result.getWithDefault(_, x);
};

let view: 'a 'b. (t('a, 'b), 'a) => result('b, string) =
  ({getResult, _}) => getResult;

let set: 'a 'b. (t('a, 'b), 'b, 'a) => 'a = ({set, _}) => set;

let composeLens: 'a 'b 'c. (Lens.t('b, 'c), t('a, 'b)) => t('a, 'c) =
  (lens, optional) =>
    make(
      a =>
        switch (optional.getResult(a)) {
        | Ok(b) => Ok(lens.get(b))
        | Error(_) as error => error
        },
      (c, a) =>
        optional.getResult(a)
        |> Belt.Result.map(
             _,
             lens.set(c) >. Function.flip(optional.set, a),
           )
        |> Belt.Result.getWithDefault(_, a),
    );

let fromLens: 'a 'b. Lens.t('a, 'b) => t('a, 'b) =
  lens => make(a => Ok(lens.get(a)), lens.set);

module Infix = {
  include Infix.Semigroupoid(Semigroupoid);
  include Overable.Infix(Overable');

  let ( *?- ): 'a 'b. ('a, t('a, 'b)) => result('b, string) =
    (b, {getResult, _}) => getResult(b);

  let ( *?= ): 'a 'b. (t('a, 'b), 'b, 'a) => 'a = set;
};
