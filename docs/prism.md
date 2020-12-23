# Prism

## What's a prism, and how does it differ from a lens?

Unlike lenses, prisms are not about getting or setting value in structures.

Said simply, a `lens<'a, 'b>` means that a value of type `'a` **has** a "`'b`", when a `prism<'a, 'b>` means
that a value of type `'a` **is** (can be) a `'b`.

## API

### Creation

The easiest way to create a Prism is to use the `make` function provided by the Optic package:

```rescript
Optic.Prism.make(...)
```

The function takes two arguments: the first one will _try_ to convert a value of type `a` to a value of type `b`, and return an `option`, the second one will always convert a value of type `b` to a value of type `a`.

We actually use prisms quite often without even thinking about it, for instance, when you need to parse a string to an int, or when you "stringify" this int back, you are actually "using" a prism.

Let's see what such a prism may look like:

```rescript
let stringInt: Optic.Prism.t<string, int> =
  Optic.Prism.make(
    int_of_string_opt, // Can fail, not all strings are ints
    string_of_int,     // Never fails, all ints can be "stringified"
  )
```

Notice that this prism is so very common, that you can find it in the `Optic.Common` module.

### Usage

Now we have a prism, but how do we use it? And what can we do with it?

First, we can transform a value using an optic, here a string to an int:

```rescript
stringInt.getOption("10")     // Returns Some(10)
stringInt.getOption("foobar") // Returns None
```

We can "reverse" the transformation:

```rescript
stringInt.reverseGet(10) // Returns "10"
```

But prisms are actually more powerful:

```rescript
// You can check a value is "covered" by a prism:
stringInt->Optic.Prism.covers("10")  // true
stringInt->Optic.Prism.covers("foo") // false

// You can read and update the value:
stringInt->Optic.Prism.modify("10", 10) // Returns Some("20")
```

## Example

### Numbers and integers

All the integers **are** numbers, but **not** all the numbers **are** integers,
consequently, coarcing an integer into a number always succeeds, but the opposite can fail.

If the above statement sounds simple to you, you already get the idea of what a prism is.

### Integers, and stricly positive integers

_In this example we also convert the value from type a to type b, but prisms can also preserve the type_

Leveraging our previous example, let's try to implement a positive integer type, and a prism to play with it:

```rescript
module PositiveInteger = {
  // Under the hood, a positive integer is an integer.
  type t = PositiveInteger(int)

  // We also need a constructor, it returns an option, since it can fail
  let make: int => option<t> =
    i => i > 0 ? Some(PositiveInteger(i)) : None

  // We can now declare our prism.
  let prism: Prism.t<int, t> = Prism.make(
    make,
    (PositiveInteger(i)) => i,
  )
}
```

## Composition

Prisms can also be composed together, let's say we have 2 prisms `positiveIntegerPrism` and `oddIntegerPrism`, we can compose them and get a `oddPositivieIntegerPrism`:

```rescript
let oddPositivieIntegerPrism = positiveIntegerPrism
->Prism.compose(oddIntegerPrism)
```

Prisms can also be composed with lenses, isos, and optionals!

### Usage with [lenses](lens.md)

Composing prisms with lenses work as expected.

There are two things to notice when composing lenses and prisms:

1. You can only compose a prism "on top" of a lens
2. The returned value is a an optional
