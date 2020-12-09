// Prisms are roughly used to "cast" or "convert" values of different (or identical) types.
// A prism `a -> b` means that b can always be casted to a, and a can sometimes be casted to b.
// For example, a prism of type `t(string, int)` will always convert an int to a string,
// but can fail converting a string to an int.
// They are an unsafe version of Isos.

[@deriving accessors]
type t('a, 'b) = {
  getOption: 'a => option('b),
  reverseGet: 'b => 'a,
};

let make: 'a 'b. ('a => option('b), 'b => 'a) => t('a, 'b) =
  (getOption, reverseGet) => {getOption, reverseGet};

/** Compose 2 Prisms */
let compose: 'a 'b 'c. (t('a, 'b), t('b, 'c)) => t('a, 'c) =
  (
    {getOption: outerGetOption, reverseGet: outerReverseGet},
    {getOption: innerGetOption, reverseGet: innerReverseGet},
  ) =>
    make(
      x =>
        switch (x->outerGetOption) {
        | Some(y) => y->innerGetOption
        | None => None
        },
      x => x->innerReverseGet->outerReverseGet,
    );

/** Map over a Prism value */
let modify: 'a 'b. (t('a, 'b), 'b => 'b, 'a) => option('a) =
  ({getOption, reverseGet}, f, a) =>
    a->getOption->Option.map(x => x->f->reverseGet);

/** Map over a Prism value, returns a default value if None */
let modifyWithDefault: 'a 'b. (t('a, 'b), 'b => 'b, 'a) => 'a =
  (prism, f, x) => prism->modify(f, x)->Option.getWithDefault(x);

let is: 'a 'b. (t('a, 'b), 'a) => bool =
  ({getOption}, x) =>
    switch (x->getOption) {
    | Some(_) => true
    | None => false
    };

/** Compose a Prism and an Iso */
let composeIso: 'a 'b 'c. (t('a, 'b), Iso.t('b, 'c)) => t('a, 'c) =
  (
    {getOption: prismGetOption, reverseGet: prismReverseGet},
    {get: isoGet, reverseGet: isoReverseGet},
  ) =>
    make(
      x =>
        switch (x->prismGetOption) {
        | Some(y) => Some(isoGet(y))
        | None => None
        },
      x => x->isoReverseGet->prismReverseGet,
    );

/** Makes a Prism from an Iso */
let fromIso: 'a 'b. Iso.t('a, 'b) => t('a, 'b) =
  ({get, reverseGet}) => make(x => x->get->Some, reverseGet);
