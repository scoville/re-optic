// Some convenient compose functions

open BsAbstract;
open Function.Infix;

let isoWithLens: 'a 'b 'c. (Lens.t('b, 'c), Iso.t('a, 'b)) => Lens.t('a, 'c) =
  (inner, outer) =>
    Lens.make(outer.get >. inner.get, c =>
      Iso.Overable'.over(outer, inner.set(c))
    );

let isoWithOptional:
  'a 'b 'c.
  (Optional.t('b, 'c), Iso.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(outer.get >. inner.getResult, c =>
      Iso.Overable'.over(outer, inner.set(c))
    );

let isoWithPrism:
  'a 'b 'c.
  (Prism.t('b, 'c), Iso.t('a, 'b)) => Prism.t('a, 'c)
 =
  (inner, outer) =>
    Prism.make(
      outer.get >. inner.getResult,
      inner.reverseGet >. outer.reverseGet,
    );

let lensWithIso: 'a 'b 'c. (Iso.t('b, 'c), Lens.t('a, 'b)) => Lens.t('a, 'c) =
  (inner, outer) =>
    Lens.make(outer.get >. inner.get, c => outer.set(inner.reverseGet(c)));

let lensWithOptional:
  'a 'b 'c.
  (Optional.t('b, 'c), Lens.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(outer.get >. inner.getResult, c =>
      Lens.Overable'.over(outer, inner.set(c))
    );

let lensWithPrism:
  'a 'b 'c.
  (Prism.t('b, 'c), Lens.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(outer.get >. inner.getResult, c =>
      outer.set(inner.reverseGet(c))
    );

let prismWithIso:
  'a 'b 'c.
  (Iso.t('b, 'c), Prism.t('a, 'b)) => Prism.t('a, 'c)
 =
  (inner, outer) =>
    Prism.make(
      outer.getResult >. Belt.Result.map(_, inner.get),
      inner.reverseGet >. outer.reverseGet,
    );

let prismWithLens:
  'a 'b 'c.
  (Lens.t('b, 'c), Prism.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(outer.getResult >. Belt.Result.map(_, inner.get), c =>
      Prism.Overable'.over(outer, inner.set(c))
    );

let prismWithOptional:
  'a 'b 'c.
  (Optional.t('b, 'c), Prism.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(
      outer.getResult >. Belt.Result.flatMap(_, inner.getResult), c =>
      Prism.Overable'.over(outer, inner.set(c))
    );

let optionalWithIso:
  'a 'b 'c.
  (Iso.t('b, 'c), Optional.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(outer.getResult >. Belt.Result.map(_, inner.get), c =>
      outer.set(inner.reverseGet(c))
    );

let optionalWithLens:
  'a 'b 'c.
  (Lens.t('b, 'c), Optional.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(outer.getResult >. Belt.Result.map(_, inner.get), c =>
      Optional.Overable'.over(outer, inner.set(c))
    );

let optionalWithPrism:
  'a 'b 'c.
  (Prism.t('b, 'c), Optional.t('a, 'b)) => Optional.t('a, 'c)
 =
  (inner, outer) =>
    Optional.make(
      outer.getResult >. Belt.Result.flatMap(_, inner.getResult), c =>
      outer.set(inner.reverseGet(c))
    );
