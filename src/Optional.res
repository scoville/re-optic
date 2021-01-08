@@ocaml.doc(
  `Optionals are basically meant to be used to "get" or "extract" a value from a structure.
In other word, we can consider that an optional with pseudo type "a -> b" will get, or extract, b from a.
For example, an optional of type "t<array<string>, string>" will get an element from an array.
Notice that, unlike Lenses, Optionals can fail, and in the above example,
one would get a option<string>, not a string.
They are an unsafe version of Lenses.`
)

@deriving(accessors)
type t<'a, 'b> = {
  getOption: 'a => option<'b>,
  set: ('b, 'a) => 'a,
}

let make: 'a 'b. ('a => option<'b>, ('b, 'a) => 'a) => t<'a, 'b> = (getOption, set) => {
  getOption: getOption,
  set: set,
}

@ocaml.doc("Compose 2 Optionals")
let compose: 'a 'b 'c. (t<'a, 'b>, t<'b, 'c>) => t<'a, 'c> = (
  {getOption: outerGetOption, set: outerSet},
  {getOption: innerGetOption, set: innerSet},
) =>
  make(
    a =>
      switch outerGetOption(a) {
      | Some(x) => innerGetOption(x)
      | None => None
      },
    (c, a) =>
      outerGetOption(a)->Option.map(x => c->innerSet(x)->outerSet(a))->Option.getWithDefault(a),
  )

@ocaml.doc("Map over an Optional value")
let modify: 'a 'b. (t<'a, 'b>, 'b => 'b, 'a) => option<'a> = ({getOption, set}, f, a) =>
  a->getOption->Option.map(x => x->f->set(a))

@ocaml.doc("Map over an Optional value, returns a default value if None")
let modifyWithDefault: 'a 'b. (t<'a, 'b>, 'b => 'b, 'a) => 'a = (optional, f, x) =>
  modify(optional, f, x)->Option.getWithDefault(x)

@ocaml.doc("Compose a Lens and an Optional")
let composeLens: 'a 'b 'c. (t<'a, 'b>, Lens.t<'b, 'c>) => t<'a, 'c> = (
  {getOption: optionalGetOption, set: optionalSet},
  {get: lensGet, set: lensSet},
) =>
  make(
    a =>
      switch a->optionalGetOption {
      | Some(b) => Some(lensGet(b))
      | None => None
      },
    (c, a) =>
      a
      ->optionalGetOption
      ->Option.map(x => c->lensSet(x)->optionalSet(a))
      ->Option.getWithDefault(a),
  )

@ocaml.doc("Makes an Optional from a Lens")
let fromLens: 'a 'b. Lens.t<'a, 'b> => t<'a, 'b> = lens => make(a => Some(lens.get(a)), lens.set)
