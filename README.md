# LSON (Low Serialize Object Notation)

LSON is a representation for serializing numbers, booleans, arrays, and associative arrays. similar to JSON, LSON is intended to be represented as text data that is a bit smaller than what a person would write.

It is intended to be used for storing serialized objects in databases and the like.

## Features

1. By omitting redundant symbols in JSON, data size can be reduced by 10-30% on average.
2. Even if the compression does not work, the data will not be larger than the original data.
3. Integers with large digits have especially small data size.
4. Since it is text data, it can be read by humans.
5. JSON <=> LSON convertible.

## Restriction

SON is intended to serialize data in the smallest possible size. As such, it is not designed to be read or written by humans, and therefore cannot contain spaces, line breaks, or annotations.

## lson.h

JSON5 supported.


## Build

```sh
cc src/lson.c -o lson
clang src/lson.c -o lson
cl.exe src\lson.c
```

## Command

```sh
# JSON to LSON
lson -jl JSONFILE

# LSON to JSON
lson -lj JSONFILE
```

## Example

JSON
```json
["Hello","world",false,true,{b1:1,b2:12345678890},{a6:null},"foo","bar"]
```
LSON
```
[Hello'world><{b1+1'b2*kbhy8pL{a6~}foo'bar]
```
