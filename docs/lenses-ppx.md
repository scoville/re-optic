# Lenses PPX

While [lenses](lens.md) are very useful, it can be cumbersome to write down all the lenses you need for each type you define... To help you, re-optic comes with an `@lenses` ppx that will generate all the lenses of a record for you.

## Usage

Let's create a very simple record that uses the lenses ppx:

```rescript
@lenses
type user = {
  age: int,
  city: string,
  loggedIn: bool,
  name: string,
}
```

The generated code will look like this:

```rescript
// The type is preserved
type user = {
  age: int,
  city: string,
  loggedIn: bool,
  name: string,
}

let age = Lens.make(
  x => x.age,
  (newAge, x) => {...x, age: newAge},
)

let city = Lens.make(
  x => x.city,
  (newCity, x) => {...x, city: newCity},
)

let loggedIn = Lens.make(
  x => x.loggedIn,
  (newLoggedIn, x) => {...x, loggedIn: newLoggedIn},
)

let name = Lens.make(
  x => x.name,
  (newName, x) => {...x, name: newName},
)
```

## Nesting

Nesting will work just fine, and will allow you to compose and easily access nested values:

```rescript
@lenses
type address = {city: string}

@lenses
type user = {
  name: string,
  address: address,
}

let user = {name: "Foobar", address: {city: "Paris"}}

address->Optic.Lens.compose(city)->Optic.Lens.get(user)->Js.log // Will display Paris
```

## Shadowing

If 2 records have one field sharing the same name, the first one will be shadowed by the latter!

```rescript
@lenses
type x = {name: string}

@lenses
type y = {name: int}

let _ = name.get({name: "Foobar"}) // Doesn't compile
```

An alternative to the above is to wrap your records in modules:

```rescript
module X = {
  @lenses
  type t = {name: string}
}

module Y = {
  @lenses
  type t = {name: int}
}

let _ = X.name.get({name: "Foobar"}) // Works just fine
```

## Drawbacks

- Works only with records for now
- Doesn't work in interfaces files!
