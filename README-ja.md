# LSON (Low Serialize Object Notation)

LSON は数値、ブーリアン、配列、連想配列をシリアライズするための表現方法です。JSONに似ていますが、LSONは人が書くことよりも少しでも小さいテキストデータとして表現することが目的となっています。

データベースなどにシリアライズしたオブジェクトを格納するのに使用することを意図しています。

## 特徴

1. JSONの冗長な記号を省略しデータサイズを平均１０～３０％ほど縮小できます。
2. 圧縮がうまくいかない場合でも元データよりもデータが大きくなることはありません。
3. 大きな桁の整数は特にデータサイズが小さくなります。
4. テキストデータなので人間でも良一応読むことが可能です。
5. JSON <=> LSON と相互にコンバート可能です。

## 制限

LSONはなるべく小さいサイズでデータをシリアライズすることを目的としています。そのため人が読み書きすることは考慮していませんので、空白や改行や注釈を含めることはできません。


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
