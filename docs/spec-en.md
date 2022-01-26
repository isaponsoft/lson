# LSON specification.

## Format

|Name|Sym|Value|
|--|--|--|
|LSON||Fval|
|Symbol||```+-*\|'"<>~[]{}^=-```|
|Val||$Array or $Assoc or $Str or $Num or $E32base or $Bool|
|Fval||$Val or $Fstr or $Fnum|
|Array|```[```, ```]```|```[```$Fval...```]```|
|Assoc|```{```, ```}```|```{```$Fval$Val...```}```|
|Bool|```<```, ```>```|```<``` or ```>```|
|Str|```'```|```'```[^$Symbol]... or ```"```...```"```|
|Fstr||[^0-9_$Symbol]...|
|Num|```+```, ```-```|```+```[0-9.]... or ```-```[0-9.]...|
|Fnum||[0-9]...|
|E32base|```*```, ```\|```|```*```[a-z+]...[A-Z-] or ```\|```[a-z+]...[A-Z-]|

## Basic architecture

1. It reads values from the beginning of the buffer and treats them as values up to the point where the Symbol appears.
2. E32base, strings, arrays, and associative arrays enclosed in ``"```` have a value terminator.
3. The associative array terminator ```}``` can be omitted if the array contains a sequence of associative arrays.
4. For the first value of an array, the first key of an associative array, or the value immediately following the value in (3), Symbol can be omitted for integers beginning with ```0-9``` and strings that do not contain Symbol.



## Symbol

Indicates the starting position of the value. The symbols are listed as follows.

|Char|Type|
|--|--|
|```+```|Decimal plus|
|```-```|Decimal minus|
|```*```|E32base plus|
|```\|```|E32base minus|
|```'```|String|
|```"```, ```"```|String begin and string end|
|```<```|True|
|```>```|False|
|```~```|null|
|```[```, ```]```|Array begin and array end|
|```{```, ```}```|Assoc begin and assoc end|
|```^```, ```=```, ```-```|reserved|


## E32base (Ended 32 base)

A 32-bit integer. The order of the data is little-endian.

The characters to be used are as follows

|Char|Num|
|--|--|
|```a-z```|0 - 25|
|```0-4```|26 - 30|
|```+```|31|
|```A-Z```|Terminate 0 - 25|
|```5-9```|Terminate 26 - 30|
|```-```|Terminate 31|


For example, ```123456``` becomes ```iqah```. For example, ```123456``` becomes ```iqah```, but only the last digit is Terminated to detect the end position. For example, ```123456``` becomes ```iqah```, but only the last digit is terminated to detect the end position, i.e., ```iqaH```.

This is followed by the data identifier, so ```123456``` becomes ```*iqaH```. A negative number ```-123456``` is ```*iqaH```, and a negative number ```-123456``` is ```|iqaH```.

Translated with www.DeepL.com/Translator (free version)

# Case stady

## Integer

Given the string ```123```, LSON will append ```+``` to indicate that the subsequent value is an integer.

```
+123
```

You can exceptionally exclude ```+``` if there is no other value before it, or if the previous value has a terminator. In that case, it will look like this

```
123
```

Compare with JSON.

```json
JSON=123
LSON=123
```

```json
JSON=+123
LSON=123
```

```json
JSON=-123
LSON=-123
```

In the case of an array, Symbol can be omitted only for the first element in the array.

```json
JSON=[1,2,3]
LSON=[1+2+3]
```


## Integer (E32base)

For integers with four or more digits, E32base can be used instead; for integers with three or fewer digits, E32base will not be used for integers with three or fewer digits because the size will remain the same or increase.

```json
JSON=12345
LSON=*zbM
JSON=-12345
LSON=|zbM
```

## String

Basically, the value you want to store as a string is enclosed in ```"``` (double quotation marks).

```json
"Hello world"
```

If the string does not contain any Symbols, you can use ```'``` instead of ```"``` and omit the terminating ```"```.

```json
'Hello world
```

If the string contains a Symbol, use ```"```.

```json
"Hello world+"	// JSON
"Hello world+"	// OK
'Hello world+	// NG!!
```

As with Integer, you can exceptionally exclude ```'``` if there is no other value before it, or if the previous value has a terminator. In that case, it will look like this

```
Hello
```

However, for integers beginning with ```-``` and ```0-9```, ```'``` cannot be omitted, since they cannot be identified as integers.

```json
"1Hello"	// JSON
"1Hello"	// OK
'1Hello		// OK
1Hello		// NG!!
```

In the case of an array, it can be represented as follows

```json
["Hello","World","!!"]		// JSON
["Hello""World""!!"]		// OK
["Hello"World"!!"]			// OK
["Hello"'World'!!]			// OK
[Hello'World'!!]			// OK (best!!)
```


## Array

An array starts with ```[``` and ends with ```]```. An array can contain zero or more values.

For example, an array with zero elements looks like this

```json
[]		// JSON
[]		// LSON
```

A single array looks like the following. You can omit the symbol for the first value.

```json
["Hello"]		// JSON
["Hello"]		// OK
['Hello]		// OK
[Hello]			// OK (best!!)
```

The two arrays will look like this. You can omit the symbol for the first value.

```json
["Hello","World"]		// JSON
["Hello""World"]		// OK
['Hello'World]			// OK
[Hello'World]			// OK (best!!)
```

The values immediately after the array can also omit the symbol.

```json
[[],"Hello"]			// JSON
[[]"Hello""World"]		// OK
[[]'Hello'World]		// OK
[[]Hello'World]			// OK (best!!)
```

## Assosiative

An associative array starts with ```{``` and ends with ```}```. An associative array must start with ```{``` and end with ```}```, and there must be at least zero and at least an even number of values in the array. The odd number is used as the key of the associative array, and the even number is the value.

If there are no elements, the array will look like this

```json
{}		// JSON
{}		// OK
```
 
A single key/value combination looks like this

```json
{a:1}		// JSON
{"a"+1}		// OK
{'a+1}		// OK
{a+1}		// OK (best!!)
{"a"}		// NG!! (Has not value)
```

An example of a combination of two or more is shown below.

```json
{"a":"1","b":"2"}	// JSON
{"a""1""b""2"}		// OK
{'a'1'b'2}			// OK
{a'1'b'2}			// OK (best!!)
```

You can omit ```}``` if there is a sequence of associative arrays or arrays immediately following an associative array in an array.

```json
[{a:1},{b:2}]	// JSON
[{a+1}{b+2}]    // OK
[{a+1{b+2]      // OK (best!!)
```

```json
[{},{}]		// JSON
[{}{}]		// OK
[{{]		// OK (best!!)
```

```json
[{},[]]		// JSON
[{}[]]		// OK
[{[]]		// OK (best!!)
```

## True, False, Null

Boolean True, False, and null, which indicates that the element is empty, are each represented by a single character.

```json
true		// JSON
<			// OK
false		// JSON
>			// OK
null		// JSON
~			// OK
```

When combined with the associative array, the result is as follows

```json
{v1:true,v2:false,v3:null}	// JSON
{v1<v2>v3~}					// OK
```
