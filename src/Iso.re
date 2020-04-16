// Isos are roughly used to safely "cast" or "convert" values of different (or identical) types.
// An iso `a -> b` means that b can always be casted to a, and a can always be casted to b.
// For example, an iso of type `t(list(int), array(int))` will always convert a list of strings to an array
// of strings, and an array of strings to a list of strings, without loss.
// They are a safe version of Prisms

open BsAbstract;
open Function.Infix;

module Type = {
  type t('a, 'b) = {
    get: 'a => 'b,
    reverseGet: 'b => 'a,
  };
};

include Type;

let make: 'a 'b. ('a => 'b, 'b => 'a) => t('a, 'b) =
  (get, reverseGet) => {get, reverseGet};

let reverse: 'a 'b. t('a, 'b) => t('b, 'a) =
  iso => make(iso.reverseGet, iso.get);

let preview: 'a 'b. (t('a, 'b), 'a) => 'b = ({get}) => get;

let re: 'a 'b. (t('a, 'b), 'b) => 'a = ({reverseGet}) => reverseGet;

module Semigroupoid: Interface.SEMIGROUPOID with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let compose = (inner, outer) =>
    make(outer.get >. inner.get, inner.reverseGet >. outer.reverseGet);
};

module Overable': Overable.OVERABLE with type t('a, 'b) = t('a, 'b) = {
  include Type;

  let over = (iso, f) => iso.get >. f >. iso.reverseGet;
};

module Infix = {
  include Infix.Semigroupoid(Semigroupoid);
  include Overable.Infix(Overable');

  let ( *- ): 'a 'b. ('a, t('a, 'b)) => 'b =
    (x, prism) => preview(prism, x);

  let ( *-- ): 'a 'b. ('b, t('a, 'b)) => 'a = (x, prism) => re(prism, x);
};
