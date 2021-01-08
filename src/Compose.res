@@ocaml.doc("Some convenient compose functions")

let isoWithLens: 'a 'b 'c. (Iso.t<'a, 'b>, Lens.t<'b, 'c>) => Lens.t<'a, 'c> = (
  {get: isoGet} as iso,
  {get: lensGet, set: lensSet},
) => Lens.make(x => x->isoGet->lensGet, c => iso->Iso.modify(lensSet(c)))

let isoWithOptional: 'a 'b 'c. (Iso.t<'a, 'b>, Optional.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {get: isoGet} as iso,
  {getOption: optionalGetOption, set: optionalSet},
) => Optional.make(x => x->isoGet->optionalGetOption, c => iso->Iso.modify(optionalSet(c)))

let isoWithPrism: 'a 'b 'c. (Iso.t<'a, 'b>, Prism.t<'b, 'c>) => Prism.t<'a, 'c> = (
  {get: isoGet, reverseGet: isoReverseGet},
  {getOption: prismGetOption, reverseGet: prismReverseGet},
) => Prism.make(x => x->isoGet->prismGetOption, x => x->prismReverseGet->isoReverseGet)

let lensWithIso: 'a 'b 'c. (Lens.t<'a, 'b>, Iso.t<'b, 'c>) => Lens.t<'a, 'c> = (
  {get: lensGet, set: lensSet},
  {get: isoGet, reverseGet: isoReverseGet},
) => Lens.make(x => x->lensGet->isoGet, c => c->isoReverseGet->lensSet)

let lensWithOptional: 'a 'b 'c. (Lens.t<'a, 'b>, Optional.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {get: lensGet} as lens,
  {getOption: optionalGetOption, set: optionalSet},
) => Optional.make(x => x->lensGet->optionalGetOption, c => lens->Lens.modify(optionalSet(c)))

let lensWithPrism: 'a 'b 'c. (Lens.t<'a, 'b>, Prism.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {get: lensGet, set: lensSet},
  {getOption: prismGetOption, reverseGet: prismReverseGet},
) => Optional.make(x => x->lensGet->prismGetOption, c => c->prismReverseGet->lensSet)

let prismWithIso: 'a 'b 'c. (Prism.t<'a, 'b>, Iso.t<'b, 'c>) => Prism.t<'a, 'c> = (
  {reverseGet: prismReverseGet, getOption: prismGetOption},
  {reverseGet: isoReverseGet, get: isoGet},
) => Prism.make(x => x->prismGetOption->Option.map(isoGet), x => x->isoReverseGet->prismReverseGet)

let prismWithLens: 'a 'b 'c. (Prism.t<'a, 'b>, Lens.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {getOption: prismGetOption} as prism,
  {get: lensGet, set: lensSet},
) =>
  Optional.make(
    x => x->prismGetOption->Option.map(lensGet),
    c => prism->Prism.modifyWithDefault(lensSet(c)),
  )

let prismWithOptional: 'a 'b 'c. (Prism.t<'a, 'b>, Optional.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {getOption: prismGetOption} as prism,
  {getOption: optionalGetOption, set: optionalSet},
) =>
  Optional.make(
    x => x->prismGetOption->Option.flatMap(optionalGetOption),
    c => prism->Prism.modifyWithDefault(optionalSet(c)),
  )

let optionalWithIso: 'a 'b 'c. (Optional.t<'a, 'b>, Iso.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {getOption: optionalGetOption, set: optionalSet},
  {get: isoGet, reverseGet: isoReverseGet},
) =>
  Optional.make(x => x->optionalGetOption->Option.map(isoGet), c => optionalSet(isoReverseGet(c)))

let optionalWithLens: 'a 'b 'c. (Optional.t<'a, 'b>, Lens.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {getOption: optionalGetOption} as optional,
  {get: lensGet, set: lensSet},
) =>
  Optional.make(
    x => x->optionalGetOption->Option.map(lensGet),
    c => optional->Optional.modifyWithDefault(lensSet(c)),
  )

let optionalWithPrism: 'a 'b 'c. (Optional.t<'a, 'b>, Prism.t<'b, 'c>) => Optional.t<'a, 'c> = (
  {getOption: optionalGetOption, set: optionalSet},
  {getOption: prismGetOption, reverseGet: prismReverseGet},
) =>
  Optional.make(
    x => x->optionalGetOption->Option.flatMap(prismGetOption),
    c => optionalSet(prismReverseGet(c)),
  )
