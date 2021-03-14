#include "SyntaxTreeChecker.h"

using namespace SyntaxTree;
struct RET
{
    bool is_ret;
    int count ; 
    Type ret_type;
};
RET retstmt;

void SyntaxTreeChecker::visit(Assembly &node)
{
    enter_scope();
    for (auto def : node.global_defs) {
        def->accept(*this);
    }
    exit_scope();
    //有没有main ，以及main的 返回类型
    Type return_type;
    if((return_type=lookup_function("main")) == (Type)-1)
    {
        err.error(node.loc, " 13_no definition of main");
        exit(1);
    }else if(return_type != SyntaxTree::Type::INT)
    {
        err.error(node.loc, " 14_unsuported return type of main");
        exit(1);
    }
    

}

void SyntaxTreeChecker::visit(FuncDef &node) {
    // 基本类型检查

    /**/ 
    // 函数名检查
    if (declare_function(node.name, node.ret_type));
    else {
        err.error(node.loc, "02_redefine of function " + node.name);
        exit(1);
    }
    //函数体检查
    retstmt.count = 0;
    node.body->expt_ret_type = node.ret_type;
    if(node.body->body.empty()){
         err.error(node.loc, "03_empty fuction " + node.name);
        exit(1); 
    }
    else
    {
         node.body->accept(*this);
    } 
    //  可检查是否有return
     
}

void SyntaxTreeChecker::visit(BinaryExpr &node) {
    node.lhs->accept(*this);
    node.rhs->accept(*this);
    node.is_const = node.lhs->is_const && node.rhs->is_const ;
    node.type = node.lhs->type > node.rhs->type ? node.lhs->type : node.rhs->type ; 
    if(node.type == 0){
        switch (node.op)
        {
        case BinOp::PLUS:
            node.int_value = node.lhs->int_value + node.rhs->int_value;
            break;
        case BinOp::MINUS:
             node.int_value = node.lhs->int_value - node.rhs->int_value;
            break;
        case BinOp::MULTIPLY:
            node.int_value = node.lhs->int_value * node.rhs->int_value;
            break;
         case BinOp::DIVIDE:
             node.int_value = node.lhs->int_value / node.rhs->int_value;
            break; 
         case BinOp::MODULO:
              node.int_value = node.lhs->int_value % node.rhs->int_value;
            break;          
        default: 
            break;
        }
    } else if(node.type == 1)
    {
        if(node.lhs->type==0 && node.rhs->type == 1)
         switch (node.op)
        {
        case BinOp::PLUS:
            node.float_value = node.lhs->int_value + node.rhs->float_value;
            break;
        case BinOp::MINUS:
             node.float_value = node.lhs->int_value - node.rhs->float_value;
            break;
        case BinOp::MULTIPLY:
            node.float_value = node.lhs->int_value * node.rhs->float_value;
            break;
         case BinOp::DIVIDE:
             node.float_value = node.lhs->int_value / node.rhs->float_value;
            break;                
        default:
            break;
        }
        else if (node.lhs->type==1 && node.rhs->type == 0)
        {
            switch (node.op)
            {
            case BinOp::PLUS:
                node.float_value = node.lhs->float_value + node.rhs->int_value;
                break;
            case BinOp::MINUS:
                node.float_value = node.lhs->float_value - node.rhs->int_value;
                break;
            case BinOp::MULTIPLY:
                node.float_value = node.lhs->float_value * node.rhs->int_value;
                break;
            case BinOp::DIVIDE:
                node.float_value = node.lhs->float_value / node.rhs->int_value;
                break;       
            default: 
                break;
            }
        }else if (node.lhs->type==1 && node.rhs->type == 1)
        {
          switch (node.op)
            {
            case BinOp::PLUS:
                node.float_value = node.lhs->float_value + node.rhs->float_value;
                break;
            case BinOp::MINUS:
                node.float_value = node.lhs->float_value - node.rhs->float_value;
                break;
            case BinOp::MULTIPLY:
                node.float_value = node.lhs->float_value * node.rhs->float_value;
                break;
            case BinOp::DIVIDE:
                node.float_value = node.lhs->float_value / node.rhs->float_value;
                break;       
            default: 
                break;
            }
        }
    }
}

void SyntaxTreeChecker::visit(UnaryExpr &node) {
    node.rhs->accept(*this);
    node.is_const = node.rhs->is_const;
    node.type = node.rhs->type ;
    if(node.is_const){
        if(node.rhs->type == 0)
        switch (node.op)
        {
        case UnaryOp::MINUS :
         node.int_value = 0 - node.rhs->int_value ; 
            break;
        case UnaryOp::PLUS :
         node.int_value = node.rhs->int_value ;
            break;
        default:
            break;
        }else if(node.rhs->type == 1)
        {
            switch (node.op)
            {
            case UnaryOp::MINUS :
            node.float_value = 0 - node.rhs->float_value ; 
                break;
            case UnaryOp::PLUS :
            node.float_value = node.rhs->float_value ;
                break;
            default:
                break;
            }
        }
    }
}
void SyntaxTreeChecker::visit(LVal &node) {
   //先判断是什么变量
     PtrVariable L_var ;
     if( (L_var=lookup_variable(node.name)) != nullptr ){
          //再Lval检查类型
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
          }else
          {
              node.is_const = 0;
              node.type = (int) L_var->btype;
              //是否数组 
              if(L_var->array_length.empty()){ // 非数组型 
                if(! node.array_index.empty()){
                    err.error(node.loc, "07_not an array " + node.name);
                    exit(1);
                }
               }
              // 越界检查
              if(!L_var->array_length.empty()){  //是数组
                   int i=0;
                   for(auto index : node.array_index){  //类型检查  .同时越界检查
                        index->accept(*this);
                        if(index->type != 0){
                                err.error(node.loc, "08_wrong type of array index " + node.name);
                                exit(1);
                        }
                        if(index->is_const){
                            if(index->int_value >= L_var->array_length[i]){
                                    err.error(node.loc, "09_array overstep " + node.name);
                                    exit(1);
                            }
                        }
                        i++ ;
                   }
                    if(node.array_index.size() > L_var->array_length.size()){ //维度检查
                        err.error(node.loc, "10_oevr dimenson of " + node.name);
                        exit(1);
                    }
              }else //不是数组
              {
                   //一般变量的处理
              }
              
          }
          
     }else{
        err.error(node.loc, "11_undefined variable " + node.name);
        exit(1);
     }
    
/*
PtrVariable L_var ;
    //先Lval检查是否被定义
     if( (L_var=lookup_variable(node.name)) != nullptr ){ //被定义

          if(L_var->is_const){ //再Lval检查类型 ,常量型
             err.error(node.loc, "02_Unable assign to " + node.name);
             exit(1);
          }
          if(L_var->array_length.empty()){ // 非数组型 
                if(! node.array_index.empty()){
                    err.error(node.loc, " 03_not array " + node.name);
                    exit(1);
                }
          }else //数组型
          {    int i=0;
               for(auto index : node.array_index){  //类型检查  .同时越界检查
                   index->accept(*this);
                   if(index->type != 0){
                        err.error(node.loc, " 04_wrong type of index " + node.name);
                        exit(1);
                   }
                   if(index->is_const){
                       if(index->int_value >= L_var->array_length[i]){
                            err.error(node.loc, " 06_array overstep " + node.name);
                            exit(1);
                       }
                   }
                   i++ ;
               }
               if(node.array_index.size() > L_var->array_length.size()){ //维度检查
                    err.error(node.loc, " 05_unmatched length of " + node.name);
                    exit(1);
               }

          }
          node.type = (int)L_var->btype;

     }else{ //no_被定义
        err.error(node.loc, "01_undefined variable " + node.name);
        exit(1);
     }
 */    
}

void SyntaxTreeChecker::visit(Literal &node) {
    node.is_const = true;
    if(node.is_int){
        node.type = 0;
        node.int_value = node.int_const ;
    }else
    {
        node.type = 1;
        node.float_value = node.float_const ;
    }
    
}

void SyntaxTreeChecker::visit(ReturnStmt &node) {
     retstmt.is_ret = true ;
     if(node.ret == nullptr) retstmt.ret_type = SyntaxTree::Type::VOID ;
     else
     {
         node.ret->accept(*this);
         retstmt.ret_type = (Type) node.ret->type;
     }   
    //返回语句检查 
}
void SyntaxTreeChecker::visit(VarDef &node)
{   //
    if(node.is_constant){//检查是否constant
        if(node.is_inited){
            node.initializers.front()->accept(*this);
            if(!node.initializers.front()->is_const){
                err.error(node.loc, "16_wrong type to const " + node.name);
                exit(1);
            }
        }
    }
    //先检查是否数组类型
    if(!node.array_length.empty()){//是数组          
            std::vector<int> array_length_int ;
            int size = 1;
            for(auto length : node.array_length){             
                length->accept(*this);//先分析表达式，确定其类型和值，再检查
                if(length->is_const && length->type == 0 ) {
                    array_length_int.push_back(length->int_value);
                    size = size * length->int_value;
                    if(length->int_value < 0){
                        err.error(node.loc, "17_illeagle length define of array " + node.name);
                        exit(1);
                    }
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
    else {//不是数组
        //是否初始化
        if(node.is_inited){//初始化
            node.initializers.front()->accept(*this);        
            if (declare_variable(node.name, node.is_constant, node.btype, node.initializers.front()->int_value  ,node.initializers.front()->float_value  , std::vector<int>()))
                ;
            else {
                err.error(node.loc, "04_redefinition of var " + node.name);
                exit(1);
            }        
        }else// no_初始化
        {
           if (declare_variable(node.name, node.is_constant, node.btype, 0,0 , std::vector<int>()))
                ;
            else {
                err.error(node.loc, "04_redefinition of var " + node.name);
                exit(1);
            }        
        }
    }
}
void SyntaxTreeChecker::visit(AssignStmt &node) {
 /*
    PtrVariable L_var ;
    //先Lval检查是否被定义
     if( (L_var=lookup_variable(node.target->name)) != nullptr ){ //被定义

          if(L_var->is_const){ //再Lval检查类型 ,常量型
             err.error(node.loc, "02_Unable assign to " + node.target->name);
             exit(1);
          }
          if(L_var->array_length.empty()){ // 非数组型 
                if(! node.target->array_index.empty()){
                    err.error(node.loc, " 03_not array " + node.target->name);
                    exit(1);
                }
          }else //数组型
          {
               for(auto index : node.target->array_index){
                   index->accept(*this);
                   if(index->type != 0){
                        err.error(node.loc, " 04_wrong type of index " + node.target->name);
                        exit(1);
                   }
               }
               if(node.target->array_index.size() > L_var->array_length.size()){
                    err.error(node.loc, " 05_unmatched length of " + node.target->name);
                    exit(1);
               }
          }

     }else{ //no_被定义
        err.error(node.loc, "01_undefined variable " + node.target->name);
        exit(1);
     }
*/   node.target->accept(*this);
    
     PtrVariable L_var ;
    //先Lval检查是否被定义
     if( (L_var=lookup_variable(node.target->name)) != nullptr ){ //被定义
          
          if(L_var->is_const){ //再Lval检查类型 ,常量型
             err.error(node.loc, "06_Unable assign to const " + node.target->name);
             exit(1);
          }
          if(L_var->array_length.empty()){ // 非数组型 
                if(! node.target->array_index.empty()){
                    err.error(node.loc, "07_not an array " + node.target->name);
                    exit(1);
                }
          }else //数组型
          {    int i=0;
               for(auto index : node.target->array_index){  //类型检查  .同时越界检查
                   index->accept(*this);
                   if(index->type != 0){
                        err.error(node.loc, "08_wrong type of index " + node.target->name);
                        exit(1);
                   }
                   if(index->is_const){
                       if(index->int_value >= L_var->array_length[i]){
                            err.error(node.loc, "09_array overstep " + node.target->name);
                            exit(1);
                       }
                   }
                   i++ ;
               }
               if(node.target->array_index.size() > L_var->array_length.size()){ //维度检查
                    err.error(node.loc, "05_unmatched length of " + node.target->name);
                    exit(1);
               }

          }
          node.target->type = (int)L_var->btype;

     }else{ //no_被定义
        err.error(node.loc, "11_undefined variable " + node.target->name);
        exit(1);
     }
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
}

void SyntaxTreeChecker::visit(FuncCallStmt &node) {
    // 函数调用 检查
    Type return_type;
    if((return_type=lookup_function(node.name)) == (Type)-1)
    {
        err.error(node.loc, "01_nodefinition of funtion" + node.name);
        exit(1);
    }
    node.is_const = 0;
    node.type = (int)return_type;
}

void SyntaxTreeChecker::visit(BlockStmt &node) {
     enter_scope();
     for (auto stmt : node.body) {
    //  std::cout << typeid(*this).name() ;
    //    std::cout<< "block_point" ;
        stmt->accept(*this);
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
    }
    exit_scope();
}

void SyntaxTreeChecker::visit(EmptyStmt &node) {
    //空语句检查
    
}

