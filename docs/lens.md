# Lens

## What's a lens?

A lens is a convenient type allowing for both accessing and setting a value in any kind of _"structure"_
(records, variants, arrays, etc...).

## API

### Creation

The easiest way to create a Lens is to use the `make` function provided by the Optic package:

```rescript
Optic.Lens.make(...)
```

The function takes two arguments: the first one will extract a value of type `b` from a value of type `a`, the second one will _update_ the value of type `a` with a new value of type `b`.

## First example: records

### Vanilla ReScript

Let's say you have this simple record type:

```rescript
type user = {
  age: int,
  name: string,
}
```

and the corresponding `foo` value:

```rescript
let foo: user = {
  age: 40,
  name: "foo",
}
```

to access the `age` value, one would type: ...

```rescript
foo.age
```

... and use this syntax to update the same field:

```rescript
{...foo, age: 25}
```

While pretty simple, the 2 syntaxes above are very hard to compose, not re-usable, and not dynamic.

### What about lenses?

First of all, we need to "make" a lens:

```rescript
let ageLens = Lens.make(
  user => user.age,                           // "getter"
  (newAge, user) => {...user, age: newAge},   // "setter"
)
```

We can now get the age value using the lens:

```rescript
ageLens.get(foo) // 24
```

and set the age:

```rescript
// will return the whole user object with age == 25
ageLens.set(25, foo)
```

This the Lens type comes with many helpers, you can update a field using the `modify` function:

```rescript
// will return the whole user object with age incremented
ageLens->Lens.modify(age => age + 1, foo)
```

### Arrays

Lenses are highly composable, and can even be composed with Isos, Prisms, or Optionals.

Having a dedicated syntax for record get/set operations, and functions to access arrays, results, or options values, relying in different API, has some drawbacks:

- The APIs don't always work well together
- It's hard to compose and to normalize the composition
- There is no way to safely and dynamically access record values

For instance, let's add an array of `skill` to our user:

```rescript
type skill = {
  level: int,
  name: string,
}

type user = {
  age: int,
  name: string,
  skills: array<skill>,
}
```

In order to access, let's say, the second skill's level, you will have to mix the different syntax as follow:

```rescript
// The following code is unsafe, it can raise an index out of bound exception

let level = foo.skills[1].level

// Update
foo.skills[1] = {...foo.skills[1], level: foo.skills[1].level + 1}
```

Lenses, on the other hand can be composed:

```rescript
let skills = ... // Define the skills lense, or generate it with the ppx
let level = ... // Define the level lense, or generate it with the ppx

let newLens = skills
->Lens.compose(Common.Array.indexExn(1))
->Lens.compose(level)
```

Not only is the code more consistent (and arguably more readable), but it's also more flexible, and easier to "compose".

## Dynamic nature of Lenses (and other optics)

An other big advantage of lenses over the traditional access/update syntaxes, is the highly dynamic nature of the former.

Let's say you need to dynamically access any values from a homogeneous records:

```rescript
// :warning: The following code is not valid :warning:
type config = {
  apiUrlTest: string,
  apiUrlStaging: string,
  apiUrlProduction: string,
}

let configSingleton = { ... }

// While pretty cool, this doesn't work:
let getUrl = (key: string): string => {
  configSingleton[key]
}
```

There is actually no way to achieve this, and you must change your implementation by using a variant for instance.

Lenses, on the other hand can help you achieve this:

```rescript
@lenses
type config = {
  apiUrlTest: string,
  apiUrlStaging: string,
  apiUrlProduction: string,
}

let configSingleton = { ... }

let getUrl = (lens: Lens.t<config, string>): string => {
  lens.get(configSingleton)
}

let productionUrl = getUrl(apiUrlProduction)
```

The above even works with heterogeneous records:

```rescript
@lenses
type user = {
  age: int,
  name: string,
  isLoggedIn: bool,
}

let user = {...}

let get = (type a, lens: Optic.Lens.t<user, a>): a => {
  lens.get(user)
}

let age = get(age) // int

let name = get(name) // string

let isLoggedIn = get(isLoggedIn) // bool
```

## A word on verbosity

While it seems very verbose to write lenses for all the types, in practice, it's actually rather lightweight. This library comes with some default lenses for most of the common structures (arrays, variants, results, options, etc...). It also comes with a [`@lenses` ppx](lenses-ppx.md).
