1. 冲突产生的原因 , 及解决

   ​    这里的冲突是 移进—归约冲突 。状态9是

        exp: exp . PLUS exp
           | exp . MINUS exp
           | exp . MULT exp
           | exp . DIV exp
           | MINUS exp .
           | exp . EXPON exp
   ​          ` ·`  表示 目前状态机处于的状态。当向前看是` PLUS ， MINUS ，MULT ，DIV，EXPON `，时可以进行移进，但是也可以按照 `exp : MINUS exp `进行归约。所以产生了归约。

   ​           bison按照优先级处理，由于优先级 `PLUS ， MINUS ，MULT ，DIV，EXPON`  都低于`UMINUS`,a所以先进行归约操作。

   

2. **下列符号的含义：**

   > `yyss`   ： 状态栈的栈底

   > `yyvs`     ：  语义(符号属性)栈的栈底

   > `yyn`   优势代表一个状态值，有时代表第几条规则

   > `yytoken` 当前读入的符号，映射到一个数字。

   > `yyval`   : 前瞻符号的语义值。  

   > `yydefact`  ：是一个数组 。yydefact[state_num]存储该状态下产生式规则的序号  （rule number）

   > `yydefgoto`  ： yydefgoto表示每个非终结符号的默认goto 但要与yypgoto核实后才能使用。它的元素数量为语法中非终结符的数量。元素的值表示每个非终结符要跳转的state。索引方式为rule number减去非终结符的数目`yystate = yydefgoto[yyn - YYNTOKENS];`

   > `yytable`  是每个状态下动作的高度压缩。结合yycheck , yypact , yypgoto 决定下一个动作是移入还是归约。

   > `yycheck`  ：作用是标志，选择归约还是移入，还选择使用哪一个跳转表yydefgoto 还是yypgoto

   > `yypact`  ： 定义了初始状态下的部分yytable。如果yypact[cur-state] = YYPACT_NINF， 意味着使用yydefact来归约。也即在这种状态下，只有归约没有移入的操作。

   > `yypgoto`  : 记录与GOTO公共表不同的情况 。因为同一个非终结符，根据之前状态的不同，其跳转的状态也是不同的。

   

   **简述`yyparse()`的主要流程**

   ​          yyparse()主要工作概括：  yyprase()  维护两个栈，一个是状态栈yyss，一个是符号属性栈     yyvs。当执行移进时 ,会将下一个状态入栈，并且将对应符号的属性值yyval入栈。而当执行归约时，会将相应表达式右部对应长度(yylen)的状态出栈，并且属性栈的相应长度(yylen) 的值出栈。再把归约的结果入栈。直至最后达到接受状态。类似与手动分析的过程

   ​            _流程_ :

   ​                先把状态0 入状态栈，然后开始从 yybackup开始解析。在yybackup部分，通过根据当前状态的 yytable , yystate , yypact 等量 以及向前看的符号 yytoken的情况，分析出下一步要做什么操作。如果出错会跳到yydefault ,如果归约，需要跳到yyreduce ，如果只是移进，则在yybackup部分完成，只需相应的属性进栈即可，并通过yysetstate ,将状态进栈。 每个分支操作执行完后，还会跳回yybackup继续解析。

   + yybackup()部分是核心的解析过程,分析下一步是移进还是归约。
   + yyfault()是遇到特殊情况的一个处理，当栈溢出或者出现其他异常时会跳到yydefault进行异常处理
   + yysetstate() 就是把栈顶的状态设置为新值。
   + yyreduce() 就是执行归约操作

   

   

3. `State 11`和`State 24`还包含有冲突的原因：

   > State 11

   ``` 
   fact: MINUS fact .
         | fact . EXPON fact
   ```

   当行前看的是EXPON时移进，但是同时也可以按照` fact: MINUS fact `归约 。于是产生了移进归约冲突，

   > State24 

   ```
   fact: fact . EXPON fact
       | fact EXPON fact .
   ```

   当行前看的是EXPON时移进，但是同时也可以按照` fact: fact EXPON fact `归约 。于是产生了移进归约冲突，

   

   

4. 解题思路 。

   ​       仍然出现移进归约冲突的原因是，展开不够充分。进一步把  ` fact : fact EXPON fact`  展开即可 ，展开方法如下

   ``` 
   fact : fact1 EXPON fact
        | fact
   ```

   * 值的注意的是，由于乘方运算的右结合性，所以要先进行移进后归约 ，所以用产生式`fact : fact1 EXPON fact`   而不是 `fact : fact EXPON fact1`  后者先归约后移进。

   + 另外是 `UMINUS` 和`EXPON`  优先级的问题。这里采用了和expr1一致的优先级。而不是和 expr一致

     

5. **请自行用一个句子解释用生成的expr分析器分析的过程，分别基于`expr.y`和`expr1-s2.y`文法文件构造的分析器**

   > 分析句子`48+2**2**3` ，  输入流是NUMBER ，PLUS ，NUMBER ，EXPON ，NUMBER ，EXPON，NUMBER EOL。

   为说明方便 ，将输入流标记为 N1 ，PLUS ，N2，EXPON1 ，N3，EXPON2，N4, EOL

      + expr.y的文法分析过程：

              + N1 归约为 exp1 , PLUS 入栈
              + N2 归约为 exp2 ，EXPON1 入栈
              + N3 归约为 exp3 ，EXPON2 入栈
              + N4 归约为 exp4 ,
              + exp3  EXPON2 exp4 归约为 exp5
              + exp2  EXPON1 exp5 归约为 exp6
              + exp1  PLUS  exp6 归约为 exp7
              + exp7 EOL 此时会输出 `printf(" = %g at line %d\n",$1, @$.last_line)`  ,并归约为line ，到达可接受状态

        exp7 的值即为所求结果

      + expr1-s2.y的文法分析过程：

           + N1 归约为 fact1 , 
           + fact1 归约为 eon1
           + eon1 归约为 next1
           + next1  归约为 term1 
           + term1 归约为 exp1 ,PLUS 入栈
           + N2 归约为 fact2 ，EXPON1 入栈
           + N3 归约为 fact3 ，EXPON2 入栈
           + N4 归约为 fact4 ,
           + fact3  EXPON2 fact4 归约为 eon2
           + fact1  EXPON1 eon2 归约为 eon3
           + eon3 归约为 next2
           + next2 归约为 term2 
           + exp1  PLUS  term2  归约为 exp2
           + exp2 EOL 此时会输出 `printf(" = %g at line %d\n",$1, @$.last_line)`  ,并归约为line ，到达可接受状态

        exp2 的值即为所求结果









​          



