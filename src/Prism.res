@@ocaml.doc("Prisms are roughly used to \"cast\" or \"convert\" values of different (or identical) types.
A prism \"a -> b\" means that b can always be casted to a, and a can sometimes be casted to b.

For example, a prism of type \"t(string, int)\" will always convert an int to a string,
but can fail converting a string to an int.

They are an unsafe version of Isos.
")

@deriving(accessors)
type t<'a, 'b> = {
  getOption: 'a => option<'b>,
  reverseGet: 'b => 'a,
}

let make: 'a 'b. ('a => option<'b>, 'b => 'a) => t<'a, 'b> = (getOption, reverseGet) => {
  getOption: getOption,
  reverseGet: reverseGet,
}

@ocaml.doc("Compose 2 Prisms")
let compose: 'a 'b 'c. (t<'a, 'b>, t<'b, 'c>) => t<'a, 'c> = (
  {getOption: outerGetOption, reverseGet: outerReverseGet},
  {getOption: innerGetOption, reverseGet: innerReverseGet},
) =>
  make(
    x =>
      switch x->outerGetOption {
      | Some(y) => y->innerGetOption
      | None => None
      },
    x => x->innerReverseGet->outerReverseGet,
  )

@ocaml.doc("Map over a Prism value")
let modify: 'a 'b. (t<'a, 'b>, 'b => 'b, 'a) => option<'a> = ({getOption, reverseGet}, f, a) =>
  a->getOption->Option.map(x => x->f->reverseGet)

@ocaml.doc("Map over a Prism value, returns a default value if None")
let modifyWithDefault: 'a 'b. (t<'a, 'b>, 'b => 'b, 'a) => 'a = (prism, f, x) =>
  prism->modify(f, x)->Option.getWithDefault(x)

let covers: 'a 'b. (t<'a, 'b>, 'a) => bool = ({getOption}, x) =>
  switch x->getOption {
  | Some(_) => true
  | None => false
  }

@ocaml.doc("Compose a Prism and an Iso")
let composeIso: 'a 'b 'c. (t<'a, 'b>, Iso.t<'b, 'c>) => t<'a, 'c> = (
  {getOption: prismGetOption, reverseGet: prismReverseGet},
  {get: isoGet, reverseGet: isoReverseGet},
) =>
  make(
    x =>
      switch x->prismGetOption {
      | Some(y) => Some(isoGet(y))
      | None => None
      },
    x => x->isoReverseGet->prismReverseGet,
  )

@ocaml.doc("Makes a Prism from an Iso")
let fromIso: 'a 'b. Iso.t<'a, 'b> => t<'a, 'b> = ({get, reverseGet}) =>
  make(x => x->get->Some, reverseGet)

module Result = {
  @@ocaml.doc("An alternative to common Prisms that uses the Result type under the hood.")

  @deriving(accessors)
  type t<'a, 'b, 'e> = {
    getResult: 'a => result<'b, 'e>,
    reverseGet: 'b => 'a,
  }

  let make: 'a 'b 'e. ('a => result<'b, 'e>, 'b => 'a) => t<'a, 'b, 'e> = (
    getResult,
    reverseGet,
  ) => {getResult: getResult, reverseGet: reverseGet}

  @ocaml.doc("Compose 2 Prisms")
  let compose: 'a 'b 'c 'e. (t<'a, 'b, 'e>, t<'b, 'c, 'e>) => t<'a, 'c, 'e> = (
    {getResult: outerGetResult, reverseGet: outerReverseGet},
    {getResult: innerGetResult, reverseGet: innerReverseGet},
  ) =>
    make(
      x =>
        switch x->outerGetResult {
        | Ok(y) => y->innerGetResult
        | Error(_) as error => error
        },
      x => x->innerReverseGet->outerReverseGet,
    )

  @ocaml.doc("Map over a Prism value")
  let modify: 'a 'b 'e. (t<'a, 'b, 'e>, 'b => 'b, 'a) => result<'a, 'e> = (
    {getResult, reverseGet},
    f,
    a,
  ) => a->getResult->Result.map(x => x->f->reverseGet)

  @ocaml.doc("Map over a Prism value, returns a default value if None")
  let modifyWithDefault: 'a 'b 'e. (t<'a, 'b, 'e>, 'b => 'b, 'a) => 'a = (prism, f, x) =>
    prism->modify(f, x)->Result.getWithDefault(x)

  let is: 'a 'b 'e. (t<'a, 'b, 'e>, 'a) => bool = ({getResult}, x) =>
    switch x->getResult {
    | Ok(_) => true
    | Error(_) => false
    }
}
