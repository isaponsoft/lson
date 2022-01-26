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

1. バッファの先頭から順に値を読み取り、 Symbol が現れたところまでを値として扱います。
2. E32base、 ```"``` で囲まれた文字列、配列、連想配列 は値の終端記号を持ちます。
3. 配列中に連想配列が連続した場合は連想配列の終端記号```}```を省略できます。
4. 配列の最初の値、連想配列の最初のキー、(3)の値の直後の値の場合、```0-9```で始まる整数と Symbol を含まない文字列の場合は Symbol を省略できます。


## Symbol

値の開始位置を示します。シンボルに一覧は以下のようになります。

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

32進数の整数です。データの並びはリトルエンディアンとなります。

使用する文字は次のようになります。

|Char|Num|
|--|--|
|```a-z```|0 - 25|
|```0-4```|26 - 30|
|```+```|31|
|```A-Z```|Terminate 0 - 25|
|```5-9```|Terminate 26 - 30|
|```-```|Terminate 31|


例えば ```123456``` は ```iqah``` になります。ただし終端位置を検出するために最後の桁のみ Terminated にします。すなわち```iqaH``` となります。

これにデータの識別子を付与するので ```123456``` は ```*iqaH``` となります。また、負の数 ```-123456``` であれば ```|iqaH``` となります。

# Case stady

## Integer

整数 ```123``` という文字列があった場合、LSONでは後続の値が整数であることを示す ```+``` を付与します。

```
+123
```

前に他の値がないか、直前の値が終端記号を持っている場合、例外的に ```+``` を除外できます。その場合は、以下のようになります。

```
123
```

JSONと比較してみます。

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

配列にした場合、配列内の最初の要素のみ Symbol が省略出来ます。

```json
JSON=[1,2,3]
LSON=[1+2+3]
```


## Integer (E32base)

４桁以上の整数の場合は E32base に置き換えることができます。３桁以下の場合は E32base にするとサイズが変わらないか増加してしまうため３桁以下はE32baseにしません。

```json
JSON=12345
LSON=*zbM
JSON=-12345
LSON=|zbM
```

## String

文字列として保存したい値は基本的には ```"``` (ダブルコーテーション)で囲います。

```json
"Hello world"
```

文字列中に Symbol が含まれない場合は```"```の代わりに```'```を使用し、終端の ```"``` を省略することができます。

```json
'Hello world
```

文字列中に Symbol が含まれる場合は ```"``` を使用します。

```json
"Hello world+"	// JSON
"Hello world+"	// OK
'Hello world+	// NG!!
```

Integer と同様に前に他の値がないか、直前の値が終端記号を持っている場合、例外的に ```'``` を除外できます。その場合は、以下のようになります。

```
Hello
```

ただし、```-```と```0-9```で始まる整数の場合は整数と判別できなくなるため ```'``` を省略できません。

```json
"1Hello"	// JSON
"1Hello"	// OK
'1Hello		// OK
1Hello		// NG!!
```

配列にした場合は以下のように表すことができます。

```json
["Hello","World","!!"]		// JSON
["Hello""World""!!"]		// OK
["Hello"World"!!"]			// OK
["Hello"'World'!!]			// OK
[Hello'World'!!]			// OK (best!!)
```


## Array

配列は ```[``` で始まり ```]``` で終わります。配列中には０個以上の値が格納できます。

例えば０個の要素の配列は以下のようになります。

```json
[]		// JSON
[]		// LSON
```

１個の配列は以下のようになります。最初の値はシンボルを省略出来ます。

```json
["Hello"]		// JSON
["Hello"]		// OK
['Hello]		// OK
[Hello]			// OK (best!!)
```

２個の配列は以下のようになります。最初の値はシンボルを省略出来ます。

```json
["Hello","World"]		// JSON
["Hello""World"]		// OK
['Hello'World]			// OK
[Hello'World]			// OK (best!!)
```

配列直後の値もシンボルを省略できます。

```json
[[],"Hello"]			// JSON
[[]"Hello""World"]		// OK
[[]'Hello'World]		// OK
[[]Hello'World]			// OK (best!!)
```

## Assosiative

連想配列は ```{``` で始まり、 ```}``` で終わります。また、連想配列中には０個以上、偶数個の値を持つ必要があります。奇数番目は連想配列のキーとして使用され、偶数番目は値となります。

要素が無いものは以下のようになります。

```json
{}		// JSON
{}		// OK
```
 
ひとつのキーと値の組み合わせは以下のようになります。

```json
{a:1}		// JSON
{"a"+1}		// OK
{'a+1}		// OK
{a+1}		// OK (best!!)
{"a"}		// NG!! (Has not value)
```

二つ以上の組み合わせの例は以下のようになります。

```json
{"a":"1","b":"2"}	// JSON
{"a""1""b""2"}		// OK
{'a'1'b'2}			// OK
{a'1'b'2}			// OK (best!!)
```

配列中に連想配列の直後に連想配列または配列が連続した場合は ```}``` を省略できます。

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

Boolean の True, False と要素が空であることを示す null はそれぞれ1文字で表します。

```json
true		// JSON
<			// OK
false		// JSON
>			// OK
null		// JSON
~			// OK
```

連想配列と組み合わせると以下のようになります。

```json
{v1:true,v2:false,v3:null}	// JSON
{v1<v2>v3~}					// OK
```
