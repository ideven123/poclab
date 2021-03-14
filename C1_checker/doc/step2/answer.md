1. **请阅读`include/SyntaxTree.h`,总结在该文件中声明了哪些类型和类，分别表示什么含义；进一步阅读`src/SyntaxTree.cpp`，指出其中定义的各个方法的作用**。

   >    `include/SyntaxTree.h`声明的类型和类：

    + `Type`        ：表示基本类型，INT，FLOAT，或VOID 

    + `BinOp `    ：表示双目运算符

    + `UnaryOp` ： 表示单目运算符

    + `Node`        ： 语法树的结点，是所有结点的基

    + `Assembley` ： 表示根结点

    + `GlobalDef`  ：表示全局定义， 是全局的变量，函数，定义的基 

    + `FuncDef`     ： 函数结点 

    + `Expr`            ： 表达式结点，是所有表达式类型的基

    + `BinaryExpr` ： lhs  op rhs 类型的表达式

    + `UnaryExpr`  ： op rhs 类型的表达式

    + `LVal`              :   ident 或 ident[exp] 类型的表达式

    + `Literal`         ：literal number 类型的表达式

    + `Stmt`             ： 表示语句的结点 ，是所有语句的基。

    + `VarDef`         ： 变量声明语句

    + `AssignStmt`  ： 任务语句

    + `FuncCallStmt` ： 函数调用语句

    + `ReturnStmt`   ：返回语句

    + `BlockStmt`      :  块语句

    + `EmptyStmt`    : 空语句

    + `Visitor`        :  定义抽象访问者类，定义了被访问的元素类型。

      

   > `src/SyntaxTree.cpp`  定义的各个方法的作用

   答 ： 其中定义的方法，(也就是各个抽象数据能接受的操作) ，accept 函数的实现是  `visitor.visit(*this);`  ，作用是执行visitor 里定义的visit函数。 如此定义的作用是，可以在visitor 的子类里简单地扩展，或者删除操作。

   

2. **请阅读`src/C1Driver.cpp`、`src/main.cpp`并浏览`grammar`目录下C1语言的词法描述文件`C1Parser.ll`和文法描述文件`C1Parser.yy`，简述`C1Driver`类与词法分析类和语法分析类之间的关系，词法分析类和语法分析类与`C1Parser.ll`和`C1Parser.yy`之间的对应关系**。

   > 简述`C1Driver`类与词法分析类和语法分析类之间的关系

   答：` C1Driver `类与词法分析类`C1FlexLexer`单向关联，`C1Driver`知道`C1FlexLexer`的存在，`C1FlexLexer`不知道`C1Driver`类的存在，这体现在 `C1FlexLexer`是`C1Driver`的一个成员变量。

   `C1Driver`  类与语法分析类`C1Parser`是依赖关系，`在C1Driver`类的`parse`方法的实现中用到了`C1Parser` 类的`parser`方法,如果`C1Parser`的parser方法变化，`C1Driver`的parse方法也会发生变化。

   > 词法分析类和语法分析类与`C1Parser.ll`和`C1Parser.yy`之间的对应关系

   答： 词法分析类`C1FlexLexer` 继承与基类 `yyFlexLexer` 依赖文件`C1Parser.ll`  并切提供了与`C1Paraer`  的接口 。 而语法分析类的实现主要依赖`C1Parser.yy`文件。

   

   

3. **理解bison文法文件 `C1Parser.yy`  ，描述其中**至少3种非终结符的产生式定义**及其相关的**AST语法树生成的实现**，你需要从下列非终结符中挑选要描述的产生式定义以及AST语法树的生成：**

   - CompUnit、ConstDecl、ConstDef、VarDef、FuncDef、Stmt、Exp

   

   >  FuncDef

   产生式的定义：`FuncDef —> FuncType  Ident '('  ')' Block`  定义了由函数类型FuncTYpe，接函数名称Ident，接上一个括号'( )' 后面再跟一个块语句的 形式为一个 `FuncDef `，

   AST语法树的生成实现：当按`FuncDef:DefType IDENTIFIER LPARENTHESE RPARENTHESE Block`分析出`FuncDef`时，语法分析器先创建一个 `FuncDef`结点,并且将 `FuncDel` 的`ret_type`指针指向第一个结构 DefType 。将`name`  指针指向第二个结构 IDENTIFIER 。`body` 指向Block 语句块 。并将自己`loc`   填入自身的地址

   

   >ConstDef

   产生式定义 ： `ConstDef ——> Ideven '=' Exp`  描述了 由变量名，接赋值号，在接一个常量表达式的句子。

   AST语法树的生成实现： 当按`ConstDef:IDENTIFIER ASSIGN Exp` 分析出`ConstDef` 时 语法分析器先创建一个`varDef`  结点 并将`varDef->is_constant` 设置为true ,`varDef->is_inited` 设置为 true 。将``name`指向第一个结构`IDENTIFIER`  。 `loc `   域写上自身的地址。 将 `varDef->initializer`  指向第三个结构`Exp`的位置 。

   

   > ConstDecl

   产生式定义 ：`ConstDecl ——> 'const' BType ConstDef {,ConstDef }` 描述了由 `const`

   接上 基本类型`BType`  再接上由逗号隔开的 至少一个 `ConstDef`  构成的语句。

   AST语法树的生成实现 ：语法分析器不创建结点，而是修改第三个结构中各个结点的 `node->btype `  域 为第二结构代表的值。

   

4. **理解`include/SyntaxTreePrinter.h`和`src/SyntaxTreePrinter.cpp`文件，描述访问者模式在这个类中的体现： 1）请说明`SyntaxTreePrinter`和`SyntaxTree::Visitor`以及语法树节点三者之间的关系； 2）请说明`SyntaxTreePrinter`访问者是如何处理`VarDef`、`BinaryExpr`、`BlockStmt`、`FuncDef`这几类语法树节点的。**

   

   >  1）请说明`SyntaxTreePrinter`和`SyntaxTree::Visitor`以及语法树节点三者之间的关系；

   答 ： `SyntaxTreePrinter`  类 继承 `SyntaxTree::Visitor`  类并且对`Visitor`  提供的操作接口进行了实现 。 而`SyntaxTreePrinter ` 的实现的内容就是对语法树结点的做相应的操作。 `visitor` 提供的接口就是针对语法树结点操作的接口。

   > 请说明`SyntaxTreePrinter`访问者是如何处理`VarDef`、`BinaryExpr`、`BlockStmt`、`FuncDef`这几类语法树节点的。

   答： 

   + 对 `VarDef` 处理。 先输出相应的空格。  判断是否为constant 类型，是则打印` ”const  “`  不是则不处理 。然后输出 类型 和 名字。紧接着根据`length` 域的值 循环输出各维的长度和方括号 。最后判断是否 初始化，如果是 则输出 `={ init->accapt(*this) } ;`  其中大括号内为 初始化的数据，调用`init->accept(*this)` 函数获得。
   + 对`BinaryExpr`  处理。先输出'(' 再对输出左操作数 ，然后 输出运算符 ，接着输出右操作树，最后输出 ')' 。其中左右操作数通过递归调用`accept()`实现。
   + 对`BlockStmt` 处理 。 先打印相应的空格，然后输出'{'  空格默认数加4 。然后对语句块内的每一个body , 进行递归调用输出 。最后 空格默认数加4输出空格 ，在输出'}' ， 结束。
   + 对`FuncDef`  处理 。 输出返回值类型 接 函数名 接 '()' 然后对函数体 ，递归调用 `accept(*this)`  。最后将默认空格更新为零 ，实现 。

   

5. **请修改相应的代码并在`SyntaxTreePrinter`中进行支持。 注：需要保持打印的格式（命令行加上`-e`选项），函数调用前后不需要加上括号，单个函数调用表达式也不需要；其他非终结符的产生式定义一致。**

   实现： 修改文法后为 Stmt类加了一个ID属性用于标志是执行的何种归约，在printer中将输出空格的位置移到语句外面实现 。在遍历 `BlockStmt` 的 body域时再进行格式控制。



