[toc]





简介：



本实训项目旨在让你熟悉LALR分析器的自动生成工具[Bison](https://www.gnu.org/software/bison/)（[使用手册](https://www.gnu.org/software/bison/manual/html_node/index.html)），掌握用[Bison](https://www.gnu.org/software/bison/)和[Flex](https://github.com/westes/flex)（[使用手册](http://westes.github.io/flex/manual/)）协作为实验型编程语言`L-expr`编写不同的文法和动作代码，构造包含语法制导翻译的解析器，完成对`L-expr`程序的解析、求值和表达式转换等功能。

你需要结合实训，体会：

 1）Bison如何与Flex协作；

 2）结合对具体程序的词法语法分析加深对LALR分析的理解； 

 3）语言的文法（如二义和非二义）对生成的分析表的状态空间、语法制导翻译等的影响；

 4）对于存在错误的程序，解析器如何识别错误并能从错误中恢复。

你需要在自己的系统上安装GCC、Flex、Bison，例如， 在OpenEuler系统上： `yum install -y flex bison` 在Ubuntu系统上： `apt-get install flex bison`

本实训项目的在线实践环境使用flex v2.6.1, bison v3.5。







## 第一关



### 任务描述

本关任务：了解表达式语言`L-expr`，编写2个`L-expr`小程序并完善`L-expr`的词法描述以形成`L-expr`的解析器。

### 相关知识

为了完成本关任务，你需要巩固对Flex词法描述以及命令行选项的理解和运用，补齐`L-expr`的词法描述。

#### `L-expr`语言简介

`L-expr`语言是简单的算术表达式语言，它包含数值以及算术表达式，支持加(`+`)、减(`-`)、乘(`*`)、除(`/`)、乘幂(`**`，或称乘方）五种双目运算符和负号(`-`)这种单目运算符。算符的优先级由高到低依次为：负号、乘方、乘和除、加和减；加、减、乘、除这些双目运算符满足自左向右结合，乘方运算符和负号都是自右向左结合。

用扩展的BNF(Backus-Naur Form)范式ls可以将`L-expr`语言的语法表示如下：

```
  input ::= ε  
| input lineline ::= EOL 
| expr EOLexpr ::= NUMBER        
| expr PLUS expr     # PLUS  – ‘+’加号       
| expr MINUS expr    # MINUS – ‘-’减号       
| expr MULT expr     # MULT  – ‘*’乘号       
| expr DIV expr      # DIV   – ‘/’除号       
| MINUS expr         # MINUS – ‘-’负号       
| expr EXPON expr    # EXPON – ‘**’乘方       
| LB expr RB         # LB, RB – ‘(’, ’)’ 左右括号
```



![image-20201116083419831](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083419831.png)



**注意**：

1. 该文法是二义的，需要利用算符的优先级和结合性来消除二义性，参见`labBison/config/expr.y`，我们将在下一关详细说明。
2. 上述文法中大写字母组成的符号串是**终结符**，词法描述文件 `labBison/config/expr.lex`给出了除`EXPON`之外的定义，你需要补充增加对乘方运算符的定义，它返回记号`EXPON`完整。

#### `L-expr`程序举例

在`labBison/test/expr.in`给出了一个`L-expr`语言示例程序。你可以再编写2个示例程序，并用Python3检查其中各表达式的运算结果。 例如，`3**2**3`的运算结果是什么呢？ 你可以在命令行下执行`python3`，进入交互式执行：

```
>>>3**2**36561>>> 3+-3-3-3>>>
```

![image-20201116083440435](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083440435.png)



注意：乘方是自右向左的结合，而不是把`3**2**3`当成`(3**2)**3`，得到`729`

#### L-expr的词法描述

[Flex](https://github.com/westes/flex) (Fast lexical analyzer generator)是词法分析器生成器lex的变种，它接受扩展名为`.lex`或`.l`的词法描述文件，产生对应的词法分析器的 C 源代码。 `L-expr`的词法描述文件见版本库的`labBison/config/expr.lex`, 下面依次解释该文件由`%%`分隔的三段内容。

##### 1. 声明部分

`labBison/config/expr.lex`中第1个`%%`之前的部分是声明，其中`%{...%}`括起的代码会被Flex直接复制到生成的词法分析器的 C 源码`*.lex.c`中，但不检查其合法性:

```
%{#include "expr.tab.h"#ifdef DEBUG#define debug(a) printf("%s", a)#else#define debug(a) #endifvoid yyerror(const char *);%}
```

![image-20201116083456726](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083456726.png)



说明：

 1）`expr.tab.h`是由Bison根据语法描述文件自动生成的，其中包含记号类型`enum yytokentype`以及各类记号的符号常量(如`NUMBER`、`PLUS`等)等。 

2）带参的宏`debug`是用于调试以控制是否输出词法分析识别出的记号串。 

3）声明函数`yyerror`以便使用（避免编译时产生警告）。

##### 2. 规则部分

在词法描述文件中介于两个`%%`之间的是词法规则部分。`labBison/config/expr.lex`中的词法规则如下：

```
[0-9]+     { yylval.val = atol(yytext);             debug(yytext);             return(NUMBER);           }[0-9]+\.[0-9]+ {             debug(yytext);             sscanf(yytext,"%f",&yylval.val);             return(NUMBER);           }"+"        { debug("+"); yylval.op = "+";             return(PLUS);}"-"        { debug("-"); yylval.op = "-"; return(MINUS);}"*"        { debug("*"); yylval.op = "*"; return(MULT);}"/"        { debug("/"); yylval.op = "/"; return(DIV);}"("        { debug("("); return(LB);}")"        { debug(")"); return(RB);}\n         { debug(""); return(EOL);}[\t ]*     /* throw away whitespace */.          { yyerror("Illegal character");             return(EOL);           }
```



![image-20201116083511988](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083511988.png)

说明： 1）[`yylval`](https://www.gnu.org/software/bison/manual/html_node/Token-Values.html)是存储记号的语义值的全局变量，它是在`labBison/src/expr.tab.h`中声明的类型为`YYSTYPE`的外部变量。 2）`yylval.val`和`yylval.op`分别表示记号类型为`NUMBER`以及算符的语义值类型，它是由Bison根据文法描述文件`labBison/config/expr.y`的`%union`声明生成的（你将在第2关对此有更深入的认识）。 3）全局变量`yytext`指向当前匹配的记号的文本，其长度保存在全局变量`yyleng`中。

##### 3. 代码部分

在词法描述文件的第2个`%%`之后是C代码部分，这部分代码将被Flex直接复制到生成的词法分析器源码的尾部。例如，这部分代码可以是

```
void getsym(){    sym = yylex();}
```

![image-20201116083526066](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083526066.png)

其中`yylex()`是flex生成的词法分析器的总控函数，它从输入流识别记号并返回指示记号类型的值。

不过在`labBison/config/expr.lex`中，第2个`%%`之后是空缺的，因为在后面的关卡中，它将配合Bison生成一个包含词法分析和语法分析的完整的解析器，Bison生成的解析器源码会调用`yylex()`进行词法分析。

##### Makefile

本实训项目主要使用版本库的`labBison/Makefile`来生成和编译词法分析、语法分析的解析代码。这里对Makefile中使用的自动变量等进行简要说明。

```
exp%:        mkdir -p $(SRC)        mkdir -p $(BIN)        $(YACC) -d -y -r solved -b $@ -o $(SRC)/$@.tab.c $(CONF)/$@.y        $(LEX) -i -I -o $(SRC)/expr.lex.c $(CONF)/$(EXPRLEX).lex        $(CC) -DDEBUG -o $(BIN)/$@ $(SRC)/expr.lex.c $(SRC)/$@.tab.c -lfl -lm
```

![image-20201116083600073](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083600073.png)



在Makefile中,有：

- 名为`exp%`的编译目标，`%`是模式字符，用来通配任意个字符。
- `$@`: 表示规则的目标文件名。如果目标是一个文档文件（Linux 中，一般成 .a 文件为文档文件，也成为静态的库文件），那么它代表这个文档的文件名。在多目标模式规则中，它代表的是触发规则被执行的文件名。
- `$%`: 当目标文件是一个静态库文件时，代表静态库的一个成员名。
- `$<`: 规则的第一个依赖的文件名。如果是一个目标文件使用隐含的规则来重建，则它代表由隐含规则加入的第一个依赖文件。
- `$?`: 所有比目标文件更新的依赖文件列表，空格分隔。如果目标文件时静态库文件，代表的是库文件（.o 文件）。
- `$^`: 代表的是所有依赖文件列表，使用空格分隔。如果目标是静态库文件，它所代表的只能是所有的库成员（.o 文件）名。一个文件可重复的出现在目标的依赖中，变量“$^”只记录它的第一次引用的情况。就是说变量“$^”会去掉重复的依赖文件。
- `$+`: 类似“$^”，但是它保留了依赖文件中重复出现的文件。主要用在程序链接时库的交叉引用场合。
- `$*`: 在模式规则和静态模式规则中，代表“茎”。“茎”是目标模式中“%”所代表的部分（当文件名中存在目录时，“茎”也包含目录部分）。



#### gcc任务要求

1. 请给`L-expr`语言编写至少2个`L-expr`程序。
2. 修改`labBison/config/expr.lex`，增加对乘方`**`的识别，得到记号`EXPON`。
3. 在`labBison/`目录下执行`make expr`，它将依次执行`bison`为`config/expr.y`产生语法分析器源码、执行`flex`为`labBison/config/expr.lex`产生词法分析器源码、执行`gcc`编译得到可执行的`L-expr`解析器`expr`。
4. 在`labBison/`目录下用生成的解析器`bin/expr`对`L-expr`程序（包括你自行编写的）进行分析和求值。例如，执行 `./run.sh expr expr.in` 或者 `bin/expr <test/expr.in`。
5. 请简述你对上述1、2的设计思路；对于上述第3步，请说明执行`make expr`时各步命令产生的结果，并说明命令的前后次序是否可以调整，为什么？请将你的回答写到`tasks/answer-s1.md`。

请按如下目录结构来组织本关卡的提交文件，目录结构如下：



![cat ](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116083623995.png)

```
<your repo>  |-- labBison>>  |-- bin/     # 临时存放可执行程序>>  |-- config/  # 修改expr.lex，增加对乘方的支持>>  |-- src/     # 其中包含Flex和Bison生成的分析器源文件>>  |-- test/    # 增加你编写的L-expr程序，文件名依次命名为e1.in、e2.in。注意不同程序需要体现不同的语言及处理特征>>  |-- tasks    # 增加 `answer-s1.md` 文档或其他以`answer-s1`为名的文档
```

**注意**：助教会借助脚本来实现对实训项目的半自动检查，请严格按照本节要求组织实训项目的目录和文件，否则会影响本次实验成绩。

= 1.39968e+06 at line 2













## 第二关



### 任务描述

**本关任务**：理解用[Bison](https://www.gnu.org/software/bison/)和[Flex](https://github.com/westes/flex)协作生成具有C源码的`L-expr`语言解析器(parser)的主要过程、命令行选项以及生成的文件及其内容要点，并回答问题。

### 相关知识

为了完成本关任务，你需要在第1关任务的基础之上，进一步掌握：

1. Bison的文法描述文件格式及命令行选项，
2. Bison与Flex如何协作，
3.  冲突的处理方法。

#### Bison 支持的文法

[Bison](https://www.gnu.org/software/bison/)是YACC（Yet another compiler compiler）的变种，YACC以LALR(1)分析技术为基础。

##### Bison 支持的分析表构造算法

由于历史原因，Bison缺省地构造LALR(1)分析表，但LALR不能处理所有的LR文法。不过，针对非LALR文法在冲突时可能发生的不确定行为，Bison提供简单的方法来解决冲突。你只需要使用`%define lr.type <type>`指令来激活功能更强大的分析表构造算法，其中参数`<type>`缺省地为`lalr`，还可以为`ielr`(Inadequacy Elimination LR）或`canonical-lr`。有关Bison的LR分析表构造的说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/LR-Table-Construction.html)。在本实验中，仍以LALR分析表构造算法为基础。

##### Bison 支持的文法

Bison允许输入的文法是二义的(ambiguous)，如版本库中提供的`labBison/config/expr.y`，或者非二义但是不确定的(nondeterministic)，即没有固定向前看(lookahead)的记号数使得能明确要应用的文法规则。 通过引入适当的声明（如`%glr-parser`、`%expect n`、`%expect-rr n`、`%edprec`、`%merge`），Bison能够使用`GLR`(Generalized IR)分析技术来分析处理更一般的上下文无关文法，对这些文法而言，任何给定字符串的可能解析次数都是有限的。Bison的GLR分析参见 [这里](https://www.gnu.org/software/bison/manual/html_node/Generalized-LR-Parsing.html)。

##### 冲突及冲突的解决

对于某些文法，按Bison的分析表构造算法构造出的分析表存在有不确定的单元项（某个单元格有不止一种动作选项），这时称为存在移进-归约冲突([Shift/Reduce Conflicts](https://www.gnu.org/software/bison/manual/html_node/Shift_002fReduce.html))或者归约-归约冲突([Reduce/Reduce Conflicts](https://www.gnu.org/software/bison/manual/html_node/Reduce_002fReduce.html))。当文法存在冲突时，你需要了解Bison对冲突采取的处理策略，或者尝试修改文法以减少冲突，或者让Bison自动处理冲突，但是你需要知道Bison的自动处理策略。

###### 移进-归约冲突的解决

为避免Bison发出移进-归约冲突的警告，可以使用`%expect n`声明，只要移进-归约冲突的数量不超过`n`，Bison就不报这些警告（参见[这里](https://www.gnu.org/software/bison/manual/html_node/Expect-Decl.html)）。==Bison会按**优先移进**来解决这种冲突==。 不过，一般不建议使用`%expect`声明（`%expect 0`除外），而是建议使用优先级（`%precedence`）和结合性（`%left`、`%right`、`%noassoc`）的指令去显式声明记号的优先级和结合性，或者进一步对某条文法规则用`%prec <symbol>`声明按给定符号`<symbol>`的优先级和结合性来处理该条规则。有关Bison提供显式修复的优先级和结合性声明方法，参见[对非算子使用优先级](https://www.gnu.org/software/bison/manual/html_node/Non-Operators.html)、[算子优先级](https://www.gnu.org/software/bison/manual/html_node/Precedence.html)。

###### 归约-归约冲突的解决

归约-归约冲突发生在对输入的相同序列可以应用2条或多条文法规则的时候。可以在使用GLR分析技术时，用`%expect-rr n`声明指定期望的归约-归约冲突的次数。==Bison会按**选择文法中首先出现的规则**来解决在期望范围内的冲突==。同样地，也不建议使用`%expect-rr n`，而是希望用优先级和结合性指令来显式消除。

#### L-expr的语法描述

[Bison](https://www.gnu.org/software/bison/)接受扩展名为`.y`的文法描述文件，产生对应的解析器的 C 源代码。 `L-expr`语言的语法的一种简单直接的文法描述文件见版本库的`labBison/config/expr.y`，这是一个二义文法，同时声明了算符的优先级和结合性。下面依次解释该文法描述文件由`%%`分隔的三段内容。

##### 1. 声明部分

`labBison/config/expr.y`中第1个`%%`之前的部分是声明。

###### C代码声明

`%{...%}`括起的代码会被Bison直接复制到生成的语法分析器的 C 源码`*.tab.c`中，但不检查其合法性。这部分主要将要用到的头文件包含进来，另外声明了`yylex`和`yyerror`两个函数（去掉这两个声明时，编译用`bison`和`flex`生成的分析器源码时将会产生warning）。

```
%{#include <stdio.h>#include <math.h>int yylex();void yyerror(const char *s);%}
```

###### 语义值类型的定义



![image-20201116132232857](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132232857.png)

`%union`用来定义文法符号的语义值类型，这里声明了`val`和`op`两个域：

```
%union {   float val;   char *op;}
```

它们分别表示数值和运算符的语义值，在Bison生成的解析器源码中对应为`union YYSTYPE`共用体类型。有关Bison的记号声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Union-Decl.html)。

###### 记号的声明

`%token`用来声明记号（终结符）的种类及语义值类型（可以空缺）。有关Bison的记号声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Token-Decl.html)。

```
%token <val> NUMBER%token <op> PLUS MINUS MULT DIV EXPON%token EOL%token LB RB
```

![image-20201116132255437](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132255437.png)

上面第1行声明`NUMBER`的语义值为`val`，而运算符的语义值为`op`。

###### 优先级与结合性的声明

`%left`、`%right`、`%nonassoc`用来声明记号的结合性，依次为自左向右的结合、自右向左的结合和无结合性。==记号的优先级是由记号结合性声明所在行的先后次序来决定的，先出现的优先级较低；在同一行声明的记号具有相同的优先级==。 在下面四行优先级和结合性声明中，负号`UMINUS`的优先级最高。有关Bison的优先级声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Precedence-Decl.html)。

![image-20201116132308854](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132308854.png)

```
%left  MINUS PLUS %left  MULT DIV%right EXPON%right UMINUS
```

###### 文法符号的语义值类型声明

在早期的Bison版本中，`%type`用来声明终结符和非终结符的语义值类型；但是在后来的Bison版本(如本在线实训环境使用的Bison v3.5)中，==只能用`%type`和`%nterm`声明非终结符的语义值类型==。

下例中给出了在早期Bison版本支持的声明，即这里声明了非终结符`exp`和终结符 `NUMBER`的语义值类型均为`%union`中的`val`项，即为`float`类型。

```
%type  <val> exp NUMBER
```

==但是在较新的Bison版本中（如Bison v3.5），终结符和非终结符的语义值类型需要用不同的声明指令声明==，即：



![image-20201116132327644](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132327644.png)

```
%token  <val> NUMBER...%type  <val> exp
```

对非终结符的语义值类型的声明还可以使用`%nterm`替代`%type`。 有关Bison的文法符号的语义值类型声明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Type-Decl.html)。

##### 2. 规则部分

在语法描述文件中介于两个`%%`之间的是文法规则部分。`labBison/config/expr.y`中的文法规则如下：

![image-20201116132342076](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132342076.png)

```
input   :        | input line        ;line    : EOL        | exp EOL { printf(" = %g\n",$1);}exp     : NUMBER                 { $$ = $1;        }        | exp PLUS  exp          { $$ = $1 + $3;   }        | exp MINUS exp          { $$ = $1 - $3;   }        | exp MULT  exp          { $$ = $1 * $3;   }        | exp DIV   exp          { $$ = $1 / $3;   }        | MINUS  exp %prec UMINUS { $$ = -$2;       }        | exp EXPON exp          { $$ = pow($1,$3);}        | LB exp RB              { $$ = $2;        }        ;
```

###### 文法规则

每条规则以分号结尾，冒号左边的符号是要定义的非终结符，冒号右边的是文法符号和动作组成的序列。有关Bison的文法规则说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Rules-Syntax.html)。

###### 动作

动作由花括号括起的C代码组成，可以放在文法规则右端（right-hand side）的任意位置。绝大多数的规则只包含置于规则尾部的动作，如上面`labBison/config/expr.y`中的规则。有关Bison的动作说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Actions.html)。

动作中的C代码可以通过`$n`引用与该规则匹配的第`n`个组成元素的语义值。`$$`表示该规则正在定义的文法符号的语义值。文法符号的语义值还可以用名字来访问，如`$name`或`$[name]`。例如：

```
exp[result]  : ...        | exp[left] PLUS  exp[right]   { $result = $left + $right;  }
```

![image-20201116132354952](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132354952.png)

###### 上下文相关的优先级

在文法规则部分，可以使用`%prec`声明所作用的规则的优先级为所指定的符号的优先级。通过这种声明，可以解决负号与减号文本相同但含义不同的冲突问题。有关Bison的上下文相关的优先级说明详见[这里](https://www.gnu.org/software/bison/manual/html_node/Contextual-Precedence.html)。 **注意**：负号和减号的冲突是在语法分析阶段解决的，而词法分析阶段对`-`识别后返回的都是记号`MINUS`。

在`config/expr.y`中存在如下代码片段，

```
%left  MINUS PLUS...%right UMINUS...%%...exp     : ...        | MINUS  exp %prec UMINUS { $$ = -$2;       }
```

![image-20201116132409307](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132409307.png)

其中：声明部分引入一个新的符号`UMINUS`用来声明负号的优先级和结合性；然后在文法规则部分，对`MINUS exp`增加声明`%prec UMINUS`，指出该表达式按`UMINUS`的优先级和结合性处理，这样通过bison生成的分析器就能正确识别和处理负号和减号的不同语义。

##### 3. 代码部分

在文法描述文件中第2个`%%`之后是C代码部分，例如：

```
void yyerror(const char *message){  printf("%s\n",message);}int main(int argc, char *argv[]){  yyparse();  return(0);}
```

![image-20201116132429098](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132429098.png)

**Bison约定**：由其生成的分析器会调用名字为[`yyerror()`](https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html)的函数来报告错误，而这个函数需要由用户提供定义。 函数[`yyparse()`](https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html)是Bison生成的分析器的总控函数。

#### 用Bison和Flex协作构造解析器的流程

你可以用Bison和Flex为某种编程语言构造解析器，其主要流程如下：

1. 编写语言的词法描述文件和语法描述文件，如`labBison/config/expr.lex`和`labBison/config/expr.y`

2. 进入文件夹`labBison/`(若是在线实验，则进入`/data/workspace/myshishun/labBison`)

3. 用bison分析处理语法描述文件，如`config/expr.y`，产生解析器源码，如`src/expr.tab.c`和`src/expr.tab.h`（`*.h`在有`-d`选项或者文法描述文件中有`%defines`时产生），使用的命令如下： `bison -d -y -r solved -b expr -o src/expr.tab.c config/expr.y` Bison的选项详见[这里](https://www.gnu.org/software/bison/manual/html_node/Bison-Options.html)。上述命令使用的选项说明如下：

   - `-y`或者`--yacc`：表示像传统的`yacc`命令那样执行，缺省地，分析器源文件命名为`y.tab.c`和`y.tab.h`
   - `-d`或者`--defines`：它假设在文法描述文件中指定了`%defines`，这将产生一个额外的输出文件，如这里的`expr.tab.h`，其中包含文法中声明的各个记号的记号类型定义以及符号常量等其他一些声明（详见[Bison声明总结](https://www.gnu.org/software/bison/manual/html_node/Decl-Summary.html)部分）
   - `-r solved`：它指示bison产生`*.output`文件输出关于文法和分析器的详细信息，包含所解决的冲突信息，该选项主要用来调试文法用，可以在调试成功后去掉
   - `-b expr`或`--file-prefix=expr`：它假设指定了`%file-prefix`，即指定Bison输出文件名的前缀，这里将缺省的`yy`修改为`expr`
   - `-o file`：指定输出的分析器源文件名

   其他有用的选项有：

   - `-v`或`--verbose`：将输出`*.output`文件，其中包含有关文法和分析器的详细信息
   - `-r things`或`--report=things`，指定包含在`*.output`文件中的额外要输出的内容，其中`things`可由若干通过逗号分隔的选项组成（参见[这里](https://www.gnu.org/software/bison/manual/html_node/Output-Files.html)）。例如`-r solved`将输出根据优先级和结合性指令解决的冲突情况。

1. 用flex分析处理

   ```
   config/expr.lex
   ```

   ，产生词法分析器源码

   ```
   src/expr.lex.c
   ```

   ，使用的命令如下：

    

   ```
   flex -i -I -o src/expr.lex.c config/expr.lex
   ```

    

   Flex的使用手册见

   这里

   ，其选项详见

   这里

   ，上述命令使用的选项说明如下：

   - `-i`或者`--case-insensitive`或者`%option case-insensitiv`：指示flex生成不区分大小写的词法分析程序（扫描器scanner）
   - `-I`或者`--interactive`或者 `%option interactive`：指示flex生成交互式扫描器，它会向前看足够的符号来确定匹配的记号

1. 用C编译器编译解析器的源程序，如： `gcc -g -Iinclude -o bin/expr src/expr.lex.c src/expr.tab.c -lfl -lm ` 这里`-l`选项指示要链接的库，包括libfl和libm，分别是flex和math库。 **注意**：有的flex库是命名为libl，这时就需要将`-lfl`修改为`-ll`。
2. 执行解析器分析`L-expr`程序，可以使用`./run.sh expr expr.in`指定要运行的解析器和`L-expr`程序，它等同于`bin/expr <test/expr.in`

##### 注意

先执行bison的原因是为了产生语法分析和词法分析共用的记号类型及记号的符号常量等，它们定义在`*.tab.h`中。这时`bison`的命令行选项必须要增加`-d`或者在文法描述文件中必须使用`%defines`。

#### 位置的跟踪

在实际进行编译器开发时，往往希望编译器能跟踪被分析程序在源代码的位置信息，这一方面有助于调试正在研制的编译器，另一方面也便于编译器能在识别出程序故障时报告警告或错误的位置信息。 下面以版本库`labBison`目录下的`config/expr.lex`和`config/expr.y`为例，说明Bison如何与Flex协作来跟踪程序的位置信息。

##### 1. 文法描述文件expr.y中设置位置跟踪

在文法描述文件中增加如下指令设置位置跟踪： `%locations ` Bison会在生成的定义文件(如`expr.tab.h`)中包含YYLTYPE类型的定义，它可以是用户自定义的，如果用户没有定义，则bison会缺省地给出如下定义：

```
/* Location type.  */#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLAREDtypedef struct YYLTYPE YYLTYPE;struct YYLTYPE{  int first_line;  int first_column;  int last_line;  int last_column;};# define YYLTYPE_IS_DECLARED 1# define YYLTYPE_IS_TRIVIAL 1#endif
```

![image-20201116132449208](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132449208.png)

然后会进一步定义`YYLTYPE yylloc`全局变量，保存当前的位置信息。需要注意的是，Bison并不会自动进行位置的跟踪，如果要跟踪程序的分析位置，还需要如下flex词法分析的配合。

##### 2. 词法描述文件expr.lex中的处理

首先要增加如下选项声明，来让词法分析器跟踪行号的变化，当前程序解析所处的行号保存在全局变量`yylineno`中。 `%option yylineno ` 然后在声明部分增加对宏`YY_USER_ACTION`的定义，bison会在生成的解析器代码中在执行归约后调用它；此外，还声明了yycolumn变量用来跟踪列的位置。

```
/* handle locations */int yycolumn = 1;#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \    yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \    yycolumn += yyleng;
```

![image-20201116132505822](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132505822.png)

注意：你需要在词法描述中设置`yycolumn`在每行开始时的初值，如在`expr.lex`中识别出换行后处理：

 `\n         { yycolumn = 1; debug(""); return(EOL);} `

##### 3. 文法描述文件expr.y中使用位置信息

在文法描述文件的规则部分，你可以使用`@$`、`@n`来访问规则左部符号、右部第`n`个符号对应的位置信息，该位置值的类型是上面的`YYLTYPE`，缺省地为包含起止行列位置的结构体。

![image-20201116132517900](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132517900.png)

```
line    : EOL  { printf("\n");}        | exp EOL { printf(" = %g at line %d\n",$1, @$.last_line);}
```



![image-20201116164334332](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116164334332.png)







### 实践并回答问题

请将以下解答提交到`labBison/tasks/answer-s2.md`文档以及`labBison/config/expr1-s2.y`。

1、请用`-r solved`选项执行bison ，将在`src`目录输出文件`expr.output`，其中`State 9`的定义如下：

```
State 9
6 exp: exp . PLUS exp
7    | exp . MINUS exp    
8    | exp . MULT exp    
9    | exp . DIV exp   
10    | MINUS exp .   
11    | exp . EXPON exp    
$default  reduce using rule 10 (exp)    
Conflict between rule 10 and token PLUS resolved as reduce (PLUS < UMINUS).    
Conflict between rule 10 and token MINUS resolved as reduce (MINUS < UMINUS).    
Conflict between rule 10 and token MULT resolved as reduce (MULT < UMINUS).    
Conflict between rule 10 and token DIV resolved as reduce (DIV < UMINUS).    
Conflict between rule 10 and token EXPON resolved as reduce (EXPON < UMINUS).
```

![image-20201116132549590](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116132549590.png)

该状态包含5条冲突，请解释这些冲突产生的原因以及Bison对这些冲突的解决。

1. 阅读bison产生的分析器源码`expr.tab.c`，回答：

   + 指出以下符号义：`yyss`、`yyvs`、`yyn`、`yytoken`、`yyval`、`yydefact`、`yydefgoto`、`yytable`、`yycheck`、`yypact`、`yypgoto`；

   - 简述`yyparse()`的主要流程，指出标号`yysetstate`、`yybackup`、`yydefault`、`yyreduce`处的代码主要在做什么方面的处理

1. 请阅读`labBison/config/expr1.y`，并在`labBison/`下执行`make expr1`，然后检查输出的`src/expr1.output`，你将看到其中`State 11`和`State 24`还包含有冲突，请说明冲突的原因。
2. 请编写`labBison/config/expr1-s2.y`（已提供的内容与`labBison/config/expr1.y`一样），请修改文法使得用Bison处理后的`src/expr2.output`不再出现任何Conflict，在文档中说明你的解题思路。
3. 请自行用一个句子解释用生成的expr分析器分析的过程，分别基于`expr.y`和`expr1-s2.y`文法文件构造的分析器。

– 开始你的练习和分析思考吧





![image-20201116165605923](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201116165605923.png)



























## 第三关



### 任务描述

**本关任务**：通过例子和练习，掌握用Bison构造中缀表达式到波兰式（前缀表达式）和逆波兰式（后缀表达式）的转换器，并回答问题。

### 基础知识

波兰表示法（Polish notation）和逆波兰表示法（Reverse Polish notation，RPN）是由波兰数学家扬·武卡谢维奇1920年引入的数学表达式方式。

在波兰记法中，所有操作符置于其对应操作数的前面，因此也称做前缀表示法；波兰式是一种逻辑、算术和代数表示方法，用于**简化命题逻辑**。例如 `56+78*3+--8+-2`将转换为`+ + + 56 * 78 3 - - 8 - 2`。

在逆波兰记法中，所有操作符置于操作数的后面，因此也被称为后缀表示法。逆波兰记法不需要括号来标识操作符的优先级，在编译系统中常用来作为**表达式求值**的表示方法。例如 `56+78*3+--8+-2`将转换为`56 78 3 * + 8 - - + 2 - +`。

### 实践并回答问题

请将以下解答提交到`labBison/tasks/answer-s3.*`文档，并将你针对第3题编写的将中缀式转换成波兰式的文法描述文件提交到`labBison/config/exprL-pre-s3.y`。

#### 1. 理解规则中嵌入的动作，是继承属性还是综合属性？

在版本库提供的文法描述文件`labBison/config/exprL.y`中，增加了表示行号的全局变量`lineno`，并且`input`规则定义如下：

```
input :     { lineno ++; }    
      |       input     { printf("Line %d (%d):\t", lineno++, @1.last_line); }     line     ;
```

在`labBison/`目录下执行`make exprL`并进行测试和分析，如运行`./run.sh exprL expr.in`，将输出：

```
3Line 1 (1):    **2**3 = 6561 at line 1Line 2 (2):    Line 3 (3):    48Line 4 (4):    +2**2**3 = 304 at line 456Line 5 (5):    +78*3+--8+-2 = 296 at line 5
```

![image-20201117111242237](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201117111242237.png)

请问：

1. 上述`input`规则中第2行的动作与第5行的动作分别相当于继承属性还是综合属性，Bison对这两个嵌入动作是怎么处理的？请查阅bison生成的`src/exprL.output`进行说明。,

2. 根据`./run.sh exprL expr.in`的输出结果，请说明词法分析和语法分析的交替过程，解释输出的第1个行中`3`在`Line 1:`之前的原因。

3. 请修改`config/exprL-s3.y`，使之能在解析每行前输出行号信息，即先后执行`make exprL-s3`和`./run.sh exprL-s3 expr.in`后，输出：

   ```
   Line 1 (1):    3 2 3  **  **  = 6561 at line 1Line 2 (2):    Line 3 (3):    Line 4 (4):    48 2 2 3  **  **  +  = 304 at line 4Line 5 (5):    56 78 3  *  + 8  -  -  + 2  -  +  = 296 at line 5
   ```

   或

   ```
   Line 1 (1):    3 2 3  **  **  = 6561 at line 1Line 2 (2):    Line 3 (3):    Line 4 (4):    48 2 2 3  **  **  +  = 304 at line 4Line 5 (5):    56 78 3  *  + 8  -  -  + 2  -  +  = 296 at line 5Line 6 (6):    
   ```

   ![image-20201117111302206](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201117111302206.png)

   请在`tasks/answer-s3.md`中说明你的解答是上述两种中的哪一种，简述你的解决思路；如果你的解答是后一种，请分析其中的难点是哪里，可能的改动是什么。

   > 注意：本小题对应评测的前四个测试样例。上述俩种输出格式均可通过评测机评测。请参考公开样例的预期输出的格式完成代码，否则评测机可能无法正确识别。对于选用前一种输出格式的同学，可以忽略预期输出的最后一行。



#### 2. 使用并理解用Bison构造中缀表达式到逆波兰式的转换

在版本库提供的文法描述文件`labBison/config/exprL-post.y`中，规则的语义动作除了包含对表达式的求值，还增加了将中缀表达式转换成逆波兰式并打印输出。

```
exp  : NUMBER                 { $$ = $1;  printf("%g ", $1);     }        | exp PLUS  exp          { $$ = $1 + $3;  printf(" %s ", $2); }        ...        | MINUS  exp %prec UMINUS { $$ = -$2;  printf(" %s ", $1);      }        | exp EXPON exp          { $$ = pow($1,$3);  printf(" %s ", $2);}        | LB exp RB              { $$ = $2;        }
```

![image-20201117111320392](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201117111320392.png)

请分析为什么在二义文法下，这里仍能正确输出逆波兰式。





#### 3. 分析用Bison构造中缀表达式到波兰式转换中的问题

在版本库提供的文法描述文件`labBison/config/exprL-pre.y`中，规则的语义动作除包含对表达式的求值，还试图增加将中缀表达式转换成波兰式的动作代码，但是执行`make exprL-pre`或者 `bison -d -y -b exprL-pre -o src/exprL-pre.tab.c config/exprL-pre.y ` 将会输出如下警告信息：

```
config/exprL-pre.y: 警告: 26 项偏移/归约冲突 [-Wconflicts-sr]config/exprL-pre.y: 警告: 169 项归约/归约冲突 [-Wconflicts-rr]config/exprL-pre.y:51.11-26: 警告: 由于冲突，解析器中的规则不起作用 [-Wother]   51 |         | {printf(" - ");} exp MINUS exp          { $$ = $2 - $4;   }      |           ^~~~~~~~~~~~~~~~config/exprL-pre.y:52.11-26: 警告: 由于冲突，解析器中的规则不起作用 [-Wother]   52 |         | {printf(" * ");} exp MULT  exp          { $$ = $2 * $4;   }      |           ^~~~~~~~~~~~~~~~config/exprL-pre.y:53.11-26: 警告: 由于冲突，解析器中的规则不起作用 [-Wother]   53 |         | {printf(" / ");} exp DIV   exp          { $$ = $2 / $4;   }      |           ^~~~~~~~~~~~~~~~config/exprL-pre.y:54.11-26: 警告: 由于冲突，解析器中的规则不起作用 [-Wother]   54 |         | {printf(" - ");} MINUS  exp %prec UMINUS { $$ = -$3;       }      |           ^~~~~~~~~~~~~~~~config/exprL-pre.y:55.11-27: 警告: 由于冲突，解析器中的规则不起作用 [-Wother]   55 |         | {printf(" ** ");}exp EXPON exp          { $$ = pow($2,$4);}      |           ^~~~~~~~~~~~~~~~~
```

![image-20201117111336967](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201117111336967.png)

运行生成的`bin/exprL-pre <test/expr.in`,将输出如下错误信息：

```
Line 1:    syntax error
```

请分析：

1）产生这些冲突的原因；

2）为什么还能生成可执行文件`bin/exprL-pre`；

3）解释报语法错误的原因。







#### 4. 用Bison构造中缀表达式到波兰式转换器

请编写`config/exprL-pre-s3.y`，完成中缀式到波兰式的转换，并用多个测试程序进行测试。请将你的主要构造思路写到`tasks/answer-s3.md`中。 **输出格式的说明**：每个算符在输出时左右两边各加一个空格，`NUMBER`按`"%g "`格式输出。例如，针对如下程序：

```
3**2**348+2**2**356+78*3+--8+-2
```

转换成波兰式后，将输出：

```
Line 1:     ** 3  ** 2 3  = 6561 at line 1Line 2:Line 3:Line 4:     + 48  ** 2  ** 2 3  = 304 at line 4Line 5:     +  +  + 56  * 78 3  -  - 8  - 2  = 296 at line 5
```

![image-20201117111409852](/Users/zhangwanlin/Library/Application Support/typora-user-images/image-20201117111409852.png)

> 注意：本小题对应评测的后四个测试样例，后四个样例和前四个样例的输入是相同的。请参考公开样例的预期输出的格式完成代码，否则评测机可能无法正确识别。



#### 评测说明

本关任务中，有难度的是第4项，你重点思考文法二义性以及继承属性在这里的影响。

– 实践、思考吧！你会对基于自下而上分析的语法制导翻译有更深入的领悟。





















