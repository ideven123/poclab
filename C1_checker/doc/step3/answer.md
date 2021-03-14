### 具体设计以及核心代码实现

1. 设计思路

   > 面对这个问题 ，首先的想法就是，根据AST语法树 的 遍历过程 在每一个结点进行相应的检查 。先在`Assembly` ,然后进入 `VarDef`  和  `FunDef`  。在	`VarDef` 中建立变量表 ，在`FunDef` 中进入到 `BlockStmt`   在`BlockStmt` 中 遍历其中的各个语句块 。然后就是对各种具体类型语句的检查。在这个过程中，类型信息，和常量的值信息的提取是关键。

2. 设计实现 

   + 对常量表达式信息的提取 与传递

     这部分的实现主要在`Expr` 的几个子类里面实现 。分别是`BinaryExpr`  ，`UnaryExpr`  ,`LVal` ,`Literal`  在这几个类里面，具体检查的东西较少 ，关键是表达式类型，和常量表达式值的计算。

      + `BinaryExpr`  

        ```c++
          node.lhs->accept(*this);
          node.rhs->accept(*this);
          node.is_const = node.lhs->is_const && node.rhs->is_const ;
          node.type = node.lhs->type > node.rhs->type ? node.lhs->type : node.rhs->type ; 
        ```

        递归地检查左式，检查右式 ，然后根据左右信息 ，和操作符信息确定 该表达式信息。

      + `UnaryExpr`  与`BinaryExpr`  同一个思路。

      + `LVal` 

        这个计算类型和值比较麻烦，是因为Lval 可以表示变量，常变量，数组。这里要分情况讨论通过`node.name` 查表来获取其信息。(另外在建变量表时，也加入了value域，记录值信息)

         + 如果是常量。

           ```c++
            if(L_var->is_const){ //常变量
                         node.is_const = 1;
                         switch (L_var->btype)
                         {
                         case SyntaxTree::Type::INT:
                             node.type = 0;
                             node.int_value = L_var->int_val ;
                             break;
                         case SyntaxTree::Type::FLOAT:
                             node.type = 1 ;
                             node.float_value = L_var->float_val;
                             break;
                         default:
                             node.type = 10;
                             break;
                         }
                     }
           ```

           

         + 如果是变量，可轻松获取类型信息，不用求值

         + 如果是数组，这里需要对数组进行一系列的检查 ，才能正确获取信息 。将在下文介绍

        

     + `Literal`  字符串型常量，可以根据结点的信息直接读取。

     

   + 各个具体语句的检查 。 `FuncDef`    , `VarDef `   `AssignStmt ` , `ReturnStmt`   ,   `FuncCallStmt` , 

      + `FuncDef` 此时要建立函数名表 。( 函数名表的创建和查找，类比变量名表进行实现 )然后分别检查函数的返回类型，函数名是否重复 ，函数体检查三部分 。

        ```c++
            // 函数名检查
            if (declare_function(node.name, node.ret_type));
            else {
                err.error(node.loc, "02_redefine of function " + node.name);
                exit(1);
            }
            //函数体检查
            if(node.body->body.empty()){
                 err.error(node.loc, "03_empty fuction " + node.name);
                exit(1); 
            }
            else
            {
                 node.body->accept(*this);
            } 
        ```

        

      +  `VarDef `  此时对不同类型的变量 ，要作不同的检查 。

         + 常变量型 。此时检查初始化是否为常量表达式

           ```c++
           if(node.is_constant){//检查是否constant
                   if(node.is_inited){
                       node.initializers.front()->accept(*this);
                       if(!node.initializers.front()->is_const){
                           err.error(node.loc, "16_wrong type to const " + node.name);
                           exit(1);
                       }
                   }
               }
           ```

           

         + 数组类型 ，此时检查各维是不是非负常量表达式 ，以及初始化是否长度匹配 。

           ```c++
            if(!node.array_length.empty()){//是数组          
                       std::vector<int> array_length_int ;
                       int size = 1;
                       for(auto length : node.array_length){             
                           length->accept(*this);//先分析表达式，确定其类型和值，再检查
                           if(length->is_const && length->type == 0 ) {
                               array_length_int.push_back(length->int_value);
                               size = size * length->int_value;
                            }    
                           else
                           {
                               err.error(node.loc, "05_unkonw length of array " + node.name);
                               exit(1);
                           }            
                       }
                       if (declare_variable(node.name, node.is_constant, node.btype , 0 , 0  ,array_length_int))
                       ;
                       else {
                       err.error(node.loc, "04_redefinition of var " + node.name);
                       exit(1);
                       }
                         //是否初始化
                       if(node.is_inited){
                           if(node.initializers.size() > size){
                               err.error(node.loc, "15_inital data too much " + node.name);
                               exit(1);
                           }          
                       }           
               }
           ```

            

        + 普通变量。 正常建表 ，检查是否重复声明。

     + `AssignStmt ` 这里重点检查 数组的索引 代码结构为

       ```c++
           //检查左
            node.target->accept(*this);
           //检查右 
             node.value->accept(*this);
            //赋值
            switch  (node.value->type )
            {
            case 0:
               node.target->int_value = node.value->float_value ;
                break;
            case 1:
               node.target->float_value = node.value->float_value;
                break;
            default:
                break;
            }
       ```

     + `ReturnStmt`  考虑到 return 的检查 ，需要知道函数信息以及return信息 ，而我又不会在 `ReturnStmt` 类里面向  `FuncDef`   获取信息 ，所以我把 return 的检查放在了 `BlockStmt` 模块。具体检查比较简单 ，只需要对比预期返回类型 ，和实际返回类型 。

     + `FuncCallStmt`  这里也是简单的检查有没有 函数名重复 ，以及确定该表达式 类型。

3. 设计总结

   总体上分为两步 。第一步，先解决信息的传递 ，在进行递归分析的过程中 ，实现需要的信息就在当前 的结点内 。第二步 是开始针对各种情形进行检测。

### 遇到的困难及解决

>  `VarDef`  类型  如果是一个Const结点，如何获取其值。

解决 ： 在创建符号表的时候多加两个参数接口 ，传递其值 。

```c++
bool declare_variable(std::string name, bool is_const, Type btype, int int_val ,float float_val, std::vector<int> array_length)
```



> 如何在`ReturnStmt` 中得之函数 声明的返回类型 。

解决 ： 避开向上获取信息 ，将返回类型检查已到上层``BlockStmt`` 检查  。另外再给`BlockStmt`  一个命名为`expt_ret_type` 来存函数声明时 ，的返回类型 。实现如下。

```c++
struct RET
{
    bool is_ret;
    int count ; 
    Type ret_type;
};
RET retstmt;
```

定义数据结构

```c++
 node.body->expt_ret_type = node.ret_type;
```

函数声明时 存  返回类型

```
if(retstmt.is_ret){ //返回语句检查
            switch (node.expt_ret_type)
            {
            case SyntaxTree::Type::INT:
                if(retstmt.ret_type == SyntaxTree::Type::VOID){
                    err.error(node.loc, "12_unmatched return type " );
                    exit(1);
                }
                /* code */
                break;
            case SyntaxTree::Type::FLOAT:
                if(retstmt.ret_type == SyntaxTree::Type::VOID){
                    err.error(node.loc, "12_unmatched return type " );
                    exit(1);
                }
                /* code */
                break;
            case SyntaxTree::Type::VOID:
                if(retstmt.ret_type != SyntaxTree::Type::VOID){
                    err.error(node.loc, "12_unmatched return type " );
                    exit(1);
                }
                /* code */
                break;    
            default:
                break;
            }
        retstmt.count++ ;    
        retstmt.is_ret = false;
        }
```

进行检查 。



### 吐槽 

希望把需要检查的 种类类型 ，写的更清晰易懂 一点虽然已经很详细了 ，但是还是会 有一些迷惑的点 ，比如对return的检测 ，类型的转化，还有左值表达式中进行索引时，被索引的值不是数组 。







