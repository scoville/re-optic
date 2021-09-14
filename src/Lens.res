@@ocaml.doc("Lenses are basically meant to be used to \"get\" or \"extract\" a value from a structure.
In other word, we can consider that a lens with pseudo type \"a -> b\" will get, or extract, b from a.

For example, a lens of type \"t<array<string>, string>\" will get an element from an array.

They are a safe version of Optionals.
")

@deriving(accessors)
type t<'a, 'b> = {
  get: 'a => 'b,
  set: ('b, 'a) => 'a,
}

let make: 'a 'b. ('a => 'b, ('b, 'a) => 'a) => t<'a, 'b> = (get, set) => {get: get, set: set}

@ocaml.doc("Compose 2 Lenses")
let compose: 'a 'b 'c. (t<'a, 'b>, t<'b, 'c>) => t<'a, 'c> = (
  {get: outerGet, set: outerSet},
  {get: innerGet, set: innerSet},
) => make(x => x->outerGet->innerGet, (c, a) => a->outerGet->innerSet(c, _)->outerSet(a))

@ocaml.doc("Map over a Lens value")
let modify: 'a 'b. (t<'a, 'b>, 'b => 'b, 'a) => 'a = ({get, set}, f, x) => x->get->f->set(x)

@ocaml.doc("Makes a Lens from an Iso")
let fromIso: 'a 'b. Iso.t<'a, 'b> => t<'a, 'b> = iso => make(iso.get, (b, _) => iso.reverseGet(b))
