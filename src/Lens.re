// Lenses are basically meant to be used to "get" or "extract" a value from a structure.
// In other word, we can consider that a lens with pseudo type `a -> b` will get, or extract, b from a.
// For example, a lens of type `t(array(string), string)` will get an element from an array.
// They are a safe version of Optionals.

open BsAbstract;
open Function.Infix;

module Type = {
  type t('a, 'b) = {
    get: 'a => 'b,
    set: ('b, 'a) => 'a,
  };
};

include Type;

let make: 'a 'b. ('a => 'b, ('b, 'a) => 'a) => t('a, 'b) =
  (get, set) => {get, set};

module Semigroupoid: Interface.SEMIGROUPOID with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let compose = (inner, outer) =>
    make(outer.get >. inner.get, (c, a) =>
      outer.get(a) |> inner.set(c) |> (b => outer.set(b, a))
    );
};

module Overable': Overable.OVERABLE with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let over = ({get, set}, f, x) => x |> get |> f |> set(_, x);
};

let view: 'a 'b. (t('a, 'b), 'a) => 'b = ({get, _}) => get;

let set: 'a 'b. (t('a, 'b), 'b, 'a) => 'a = ({set, _}) => set;

let fromIso: 'a 'b. Iso.t('a, 'b) => t('a, 'b) =
  iso => make(iso.get, (b, _) => iso.reverseGet(b));

module Infix = {
  include Infix.Semigroupoid(Semigroupoid);
  include Overable.Infix(Overable');

  let ( *- ): 'a 'b. ('a, t('a, 'b)) => 'b = (b, {get, _}) => get(b);

  let ( *= ): 'a 'b. (t('a, 'b), 'b, 'a) => 'a = set;
};
