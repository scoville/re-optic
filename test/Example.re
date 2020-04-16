module Address = {
  [@deriving lenses]
  type t = {
    city: string,
    country: string,
  };
};

module User = {
  [@deriving lenses]
  type t = {
    address: Address.t,
    age: int,
    name: string,
  };
};

let user = {
  User.address: {
    city: "Paris",
    country: "France",
  },
  age: 42,
  name: {j|Kévin|j},
};

// Lenses

{
  open Lens.Infix;

  let city' = User.address >. Address.city;

  Js.log(user *- User.name);

  let _ = user |> (User.address >. Address.city) *= "f";

  let newUser = user |> User.name *= "Kevin";

  Js.log(newUser *- User.name);

  Js.log(user *- User.age);

  Js.log(user |> city' *= "Lyon");

  let newUser' = user |> User.name *~ (++)("My name's ");

  Js.log(newUser' *- User.name);
};

// Prisms

{
  open Prism.Infix;

  let nonEmptyString =
    Prism.make(
      s => Js.String.length(s) <= 0 ? Error("String is empty") : Ok(s),
      s => s,
    );

  let empty = "" *?- nonEmptyString;

  switch (empty) {
  | Error(_) => Js.log("Indeed, the string is empty!!")
  | Ok(_) => Js.log("Should not happen")
  };

  let nonEmpty = "foo" *?- nonEmptyString;

  switch (nonEmpty) {
  | Error(_) => Js.log("Should not happen")
  | Ok(s) => Js.log("The string is not empty: " ++ s)
  };
};

// Both

{
  let nonEmptyString: Prism.t(string, string) =
    Prism.make(
      s => Js.String.length(s) <= 0 ? Error("String is empty") : Ok(s),
      s => s,
    );

  let composed = User.name |> Compose.lensWithPrism(nonEmptyString);

  switch (Optional.Infix.(user *?- composed)) {
  | Error(_) => Js.log("Should not happen")
  | Ok(s) => Js.log("The name is valid and is: " ++ s)
  };
};

module PositiveInteger = {
  // Under the hood, a positive integer is an integer.
  // OCaml's module system is powerful enough to allow us not to include
  // the PositiveInteger constructor, but it's slightly out of scope
  type t =
    | PositiveInteger(int);

  // We also need a constructor, it returns an option, since it can fail
  let make: int => option(t) = i => i > 0 ? Some(PositiveInteger(i)) : None;

  // We can also declare our prism.
  // Here, we are using the String' module in order to build a Prism that will "return"
  // a result('a, string), that is, in our example a result(t, string);
  let prism: Prism.t(int, t) =
    Prism.make(
      i =>
        switch (make(i)) {
        | None => Error("This integer is not strictly positive")
        | Some(pi) => Ok(pi)
        },
      (PositiveInteger(i)) => i // The positive integer to int "coerce" function, this one alway succeeds
    );
};

module PositiveEvenInteger = {
  type t =
    | PositiveEvenInteger(PositiveInteger.t);

  let make: PositiveInteger.t => option(t) =
    (PositiveInteger.PositiveInteger(i)) =>
      i mod 2 == 0
        ? Some(PositiveEvenInteger(PositiveInteger.PositiveInteger(i)))
        : None;

  let prism: Prism.t(PositiveInteger.t, t) =
    Prism.make(
      i =>
        switch (make(i)) {
        | None => Error("This integer is not even")
        | Some(pi) => Ok(pi)
        },
      (PositiveEvenInteger(i)) => i,
    );
};

Prism.Infix.(
  switch (10 *?- PositiveInteger.prism) {
  | Ok(x) => Js.log(x *-- PositiveInteger.prism)
  | Error(_) => ()
  }
);

switch (Prism.Infix.((-10) *?- PositiveInteger.prism)) {
| Error(error) => Js.log(error)
| Ok(_) => ()
};

let composedPrism = {
  open! Prism.Infix;

  PositiveInteger.prism >. PositiveEvenInteger.prism;
};

// Docs - Lens

type user = {
  age: int,
  name: string,
};

{
  open Lens.Infix;

  let thien = {age: 23, name: "Thien"};

  let ageLens =
    Lens.make(user => user.age, (newAge, user) => {...user, age: newAge});

  assert(thien |> Lens.view(ageLens) == 23);
  assert(thien *- ageLens == 23);

  assert(thien |> Lens.set(ageLens, 24) == {...thien, age: 24});
  assert(thien |> ageLens *= 24 == {...thien, age: 24});

  assert(
    thien |> Lens.Overable'.over(ageLens, (+)(1)) == {...thien, age: 24},
  );
  assert(thien |> ageLens *~ (+)(1) == {...thien, age: 24});
};

// Docs - Prism

{
  open Prism.Infix;

  Js.log(11 *?- PositiveInteger.prism);

  Js.log((-1) *?- PositiveInteger.prism);

  let x: int =
    (PositiveInteger.make(30) |> Belt.Option.getExn) *-- PositiveInteger.prism;

  Js.log(x);
};
