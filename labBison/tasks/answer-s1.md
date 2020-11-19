> 两个L_expr程序的设计思路

1. e1.in 主要展示该语言的结合结合性 。 
   	* 第一个式子 `3**2**3` 展示乘方的结合性。计算结果为6561 可以验证是右结合的
   	* 第二个式子`-2**4**2` 展示乘方和负号两个右结合的优先级。输出结果为65536可以发现是个正值，可见负号的优先级高于乘方。值的一提的是，这里的结果与python3的结果不同，python3 的结果为-65536 。可见两种语言定义的优先级不同。
   	* 第三个式子可以验证负号`-` 是右结合的。
2. e2.in 主要展示L_expr的各运算符的优先级
   	+ `7*2/-1+7/-1*2-2*2**3` 第一个式子验证乘除优先级同，乘方大于乘除。
   	+ `2**4/2-6*(0.1+1)` 第二个式子验证括号可以改变运算顺序，括号里的先计算
   	+ `2*4+4/2-1+(3-5+1)`  第三个式子验证加减同优先级，且小于乘除



> 请说明执行`make expr`时各步命令产生的结果

+ `mkdir -p src`  建一个文件夹src
+ `mkdir -p bin`  建一个文件夹 bin
+ `bison -d -y -r solved -b expr -o src/expr.tab.c config/expr.y`

​         生成 _expr.output_	_expr.tab.c_	_expr.tab.h_ 几个文件

+ `flex -i -I -o src/expr.lex.c config/expr.lex`

​         生成 _expr.lex.c_ 文件

+ `gcc -g -Isrc -DDEBUG -o bin/expr src/expr.lex.c src/expr.tab.c -ll -lm`

​        生成 _expr_ 可执行文件。



​      **注**  ：前两个创建文件夹的指令顺序无所谓可以换。后面的指令顺序不能调整,因为他们有依赖关系。 bison指令的生成文件_expr.tab.c_ 是flex指令的输入文件。 gcc的输入文件_expr.lex.c_是 flex的输出文件。 _expr.tab.c_ 是bison指令的输出文件。