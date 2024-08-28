# Collaps

Collaps is a experimental programming language to prove that a stack based high level language is possible.

**DISCLAIMER**: work in progress.

## The collaps algorithm

Collaps is less of an interpreted scripting language and more of a stack based virtual machine.
But virtual machines are low level. This is where the `collaps algorithm` comes in.
When a function is parsed, it is placed on the stack as a first class object and whenever the correct number of arguments
sorround it, the function is invoked. This results in interesting syntax-quirks like:

```collaps
1 2 + 3 ==      # typical stack based (postfix) languages would look like this
1 + 2 == 3      # this would work only in higher level languages
== + 1 2 3      # typical prefix languages would look like this
```

With `collaps` though, all of these will result in a `true` value. Of course, one would favor the middle one.

Most assembly languages have a concept of base pointer. This is the address of the top of the stack.
In collaps the base pointer is altered dinamically by parentheses and commas.

```collaps
print(1 + 2)
```

If the parentheses were not there, print would be eagerly invoked when 1 is pushed, so only that would be printed.
But `(` creates a new base pointer, `,` sets the last base pointer to the current top of the stack, 
and `)` resets the base pointer to the previous one.

Parsing lists is easily implemented with this concept. The only difference is that `[` creates a completely new stack,
and `]` pushes the current stack as a list to the previous one.

## Build

Collaps uses the [`cuilt`](https://github.com/coddra/cuilt), so to build it just run:

```sh
cc project.c -o project # or any other c compiler. You won't need to compile `project.c` ever again
./project build
```

This produces a binary that can be executed by either `./bin/debug/collaps` or `./project run`.

## Test

```sh
./project test
```

For now, see the `test` folder for examples too.

## Usage

```sh
collaps [INPUT]
```

`INPUT` is the file to evaluate. The extension of the file containing `collaps` code should be `.laps`.
When `INPUT` is not provided, `collaps` enters interactive mode.

```sh
./project run       # or ./bin/debug/collaps
> "Hello, World!"
> print
Hello, World!
> 
```
