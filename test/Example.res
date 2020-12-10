module Address = {
  @lenses
  type t = {
    city: string,
    country: string,
  }
}

module User = {
  @lenses
  type t = {
    address: Address.t,
    age: int,
    name: string,
  }
}

let user = {
  User.address: {
    city: "Paris",
    country: "France",
  },
  age: 42,
  name: `Kévin`,
}

// Lenses

{
  open Lens

  let city' = User.address->compose(Address.city)

  Js.log(User.name.get(user))

  let _ = User.address->compose(Address.city)->set("f", user)

  let newUser = User.name.set("Kevin", user)

  Js.log(User.name.get(newUser))

  Js.log(User.age.get(newUser))

  Js.log(city'.set("Lyon", user))

  let newUser' = User.name->modify(name => `My name is ${name}`, user)

  Js.log(User.name.get(newUser'))
}

// Prisms

{
  open Prism

  let nonEmptyString = make(s => Js.String.length(s) > 0 ? Some(s) : None, s => s)

  let empty = nonEmptyString.getOption("")

  switch empty {
  | None => Js.log("Indeed, the string is empty!!")
  | Some(_) => Js.log("Should not happen")
  }

  let nonEmpty = nonEmptyString.getOption("foo")

  switch nonEmpty {
  | None => Js.log("Should not happen")
  | Some(s) => Js.log(`The string is not empty: ${s}`)
  }
}

// Both

{
  let nonEmptyString = Prism.make(s => Js.String.length(s) > 0 ? Some(s) : None, s => s)

  let composed = User.name->Compose.lensWithPrism(nonEmptyString)

  switch composed.getOption(user) {
  | None => Js.log("Should not happen")
  | Some(s) => Js.log(`The name is valid and is: ${s}`)
  }
}

module PositiveInteger = {
  // Under the hood, a positive integer is an integer.
  // ReScript's module system is powerful enough to allow us not to include
  // the PositiveInteger constructor, but it's slightly out of scope
  type t = PositiveInteger(int)

  // We also need a constructor, it returns an option, since it can fail
  let make = i => i > 0 ? Some(PositiveInteger(i)) : None

  // We can also declare our prism.
  // Here, we are using the String' module in order to build a Prism that will "return"
  // a result('a, string), that is, in our example a result(t, string).
  let prism = Prism.make(make, (PositiveInteger(i)) => i)
}

module PositiveEvenInteger = {
  type t = PositiveEvenInteger(PositiveInteger.t)

  let make = (PositiveInteger.PositiveInteger(i)) =>
    mod(i, 2) == 0 ? Some(PositiveEvenInteger(PositiveInteger.PositiveInteger(i))) : None

  let prism = Prism.make(make, (PositiveEvenInteger(i)) => i)
}

{
  open Prism

  switch PositiveInteger.prism.getOption(10) {
  | Some(x) => Js.log(PositiveInteger.prism.reverseGet(x))
  | None => ()
  }
}

{
  open Prism

  switch PositiveInteger.prism.getOption(-10) {
  | Some(x) => Js.log(j`$x is indeed negative`)
  | None => ()
  }
}

let composedPrism = PositiveInteger.prism->Prism.compose(PositiveEvenInteger.prism)

// Docs - Lens

type user = {
  age: int,
  name: string,
}

{
  open Lens

  let thien = {age: 23, name: "Thien"}

  let ageLens = Lens.make(user => user.age, (newAge, user) => {...user, age: newAge})

  assert (ageLens.get(thien) == 23)

  assert (ageLens.set(24, thien) == {...thien, age: 24})

  assert (ageLens->modify(age => age + 1, thien) == {...thien, age: 24})
}

// Docs - Prism

{
  open Prism

  Js.log(PositiveInteger.prism.getOption(11))

  Js.log(PositiveInteger.prism.getOption(-1))

  let x = 30->PositiveInteger.make->Option.getExn->PositiveInteger.prism.reverseGet

  Js.log(x)
}
