# Collaps

Collaps is a experimental programming language to prove that a stack based high level language is possible.

**DISCLAIMER**: work in progress.

## The collapse algorithm

Collaps is a not-interpreted scripting language. It means it has no backend, but is evaluated instantly at parse-time.
This alone would make it a disturbingly low level language. This is where the `collaps algorithm` comes in.
When a function is parsed, it is placed on the stack as a first class object and whenever the correct number of arguments
sorround it, the function is invoked. This results in interesting syntax-quirks like:

```collaps
1 2 + 3 ==      # typical stack based (postfix) languages would look like this
1 + 2 == 3      # this would work only in higher level languages
== + 1 2 3      # typical prefix languages would look like this
```

With `collaps` though, all of these will result in a `true` value. Of course, one would favor the middle one.

## Build

Collaps uses the [`cuilt`](https://github.com/coddra/cuilt), so to build it just run:

```sh
cc project.c -o project # or any other c compiler
./project build
```

This produces a binary that can be executed with either `./collaps` or `./project run`.

## Test

```sh
./project test
```

For now, see the `test` folder for examples too.

## Usage

```sh
collaps INPUT
```

`INPUT` is a file to evaluate. The extension of the file containing `collaps` code should be `.laps`.