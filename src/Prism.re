// Prisms are roughly used to "cast" or "convert" values of different (or identical) types.
// A prism `a -> b` means that b can always be casted to a, and a can sometimes be casted to b.
// For example, a prism of type `t(string, int)` will always convert an int to a string,
// but can fail converting a string to an int.
// They are an unsafe version of Isos.

open BsAbstract;
open Function.Infix;

// TODO: Should be a functor to allow for custom error type

module Type = {
  type t('a, 'b) = {
    getResult: 'a => result('b, string),
    reverseGet: 'b => 'a,
  };
};

include Type;

let make: 'a 'b. ('a => result('b, string), 'b => 'a) => t('a, 'b) =
  (getResult, reverseGet) => {getResult, reverseGet};

module Semigroupoid: Interface.SEMIGROUPOID with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let compose = (inner, outer) =>
    make(
      x =>
        switch (outer.getResult(x)) {
        | Ok(y) => inner.getResult(y)
        | Error(_) as error => error
        },
      inner.reverseGet >. outer.reverseGet,
    );
};

let overResult: 'a 'b. (t('a, 'b), 'b => 'b, 'a) => result('a, string) =
  (prism, f) => prism.getResult >. Belt.Result.map(_, f >. prism.reverseGet);

module Overable': Overable.OVERABLE with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let over = (prism, f, x) =>
    overResult(prism, f, x) |> Belt.Result.getWithDefault(_, x);
};

let preview: 'a 'b. (t('a, 'b), 'a) => result('b, string) =
  ({getResult}) => getResult;

let re: 'a 'b. (t('a, 'b), 'b) => 'a = ({reverseGet}) => reverseGet;

let is: 'a 'b. (t('a, 'b), 'a) => bool =
  (prism, x) =>
    switch (preview(prism, x)) {
    | Ok(_) => true
    | Error(_) => false
    };

let isn't: 'a 'b. (t('a, 'b), 'a) => bool = (prism, x) => !is(prism, x);

let composeIso: 'a 'b 'c. (Iso.t('b, 'c), t('a, 'b)) => t('a, 'c) =
  (inner, outer) =>
    make(
      x =>
        switch (outer.getResult(x)) {
        | Ok(y) => Ok(inner.get(y))
        | Error(_) as error => error
        },
      inner.reverseGet >. outer.reverseGet,
    );

let fromIso: 'a 'b. Iso.t('a, 'b) => t('a, 'b) =
  iso => make(iso.get >. (x => Ok(x)), iso.reverseGet);

module Infix = {
  include Infix.Semigroupoid(Semigroupoid);
  include Overable.Infix(Overable');

  let ( *?- ): 'a 'b. ('a, t('a, 'b)) => result('b, string) =
    (x, prism) => preview(prism, x);

  let ( *-- ): 'a 'b. ('b, t('a, 'b)) => 'a = (x, prism) => re(prism, x);
};
