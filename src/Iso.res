@@ocaml.doc("Isos are roughly used to safely \"cast\" or \"convert\" values of different (or identical) types.
An iso \"a -> b\" means that b can always be casted to a, and a can always be casted to b.

For example, an iso of type \"t<list<int>, array<int>>\" will always convert a list of strings to an array
of strings, and an array of strings to a list of strings, without loss.

They are a safe version of Prisms.
")

@deriving(accessors)
type t<'a, 'b> = {
  get: 'a => 'b,
  reverseGet: 'b => 'a,
}

let make: 'a 'b. ('a => 'b, 'b => 'a) => t<'a, 'b> = (get, reverseGet) => {
  get: get,
  reverseGet: reverseGet,
}

@ocaml.doc("Makes a reversed Iso out of the provided one")
let reverse: 'a 'b. t<'a, 'b> => t<'b, 'a> = ({reverseGet, get}) => make(reverseGet, get)

@ocaml.doc("Compose 2 Isos")
let compose: 'a 'b 'c. (t<'a, 'b>, t<'b, 'c>) => t<'a, 'c> = (
  {get: outerGet, reverseGet: outerReverseGet},
  {get: innerGet, reverseGet: innerReverseGet},
) => make(x => x->outerGet->innerGet, x => x->innerReverseGet->outerReverseGet)

@ocaml.doc("Map over an Iso value")
let modify: 'a 'b. (t<'a, 'b>, 'b => 'b, 'a) => 'a = ({reverseGet, get}, f, x) =>
  x->get->f->reverseGet
