////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2012-2013 by authors (see AUTHORS.txt)                  //
//                                                                        //
//  This file is part of IOC.                                             //
//                                                                        //
//  IOC is free software; you can redistribute it and//or modify it under //
//  the terms of the IOC Public License(LGPL License) as published by     //
//  masol.lee.                                                            //
//                                                                        //
//  IOC is distributed in the hope that it will be useful,but WITHOUT     //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                  //
//                                                                        //
//  You should have received a copy of the IOC License along with this    //
//  program.  If not, see <http://www.masols.com/licenses>.               //
//                                                                        //
//  IOC website: http://www.masols.com                                    //
////////////////////////////////////////////////////////////////////////////

#ifndef  IOC_FRONTEND_AST_H
#define  IOC_FRONTEND_AST_H


/**
 * @file        ast.h
 * @brief       抽象语法树是一个轻量级的ioc中间表达形式。负责对上层语言做隔离，以便可以最终编译为本地代码。
 *                 所有的节点都在一个独立的隔离空间分配，这允许我们快速分配并以常数时间销毁全部语法数。
 * @relate      测试程序ast_test.cpp
 **/
/**
 * @defgroup varSurvival
 **/


#include "utils/zone.h"
#include "utils/exception.h"
#include "frontend/asttypes.h"
#include <boost/unordered_map.hpp>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <sstream>

namespace ioc{

/// @brief Data type used in SAST.
typedef enum {
    T_INVALID,
    T_VARIANT,
    T_VOID,
    T_BOOLEAN,        // bool
    T_I2,
    T_I4,
    T_UCHAR,
    T_CHAR,            // char
    T_USHORT,
    T_SHORT,        // short
    T_UINT,
    T_INT,            // int
    T_ULONG,
    T_LONG,            // long
    T_ULONGLONG,
    T_LONGLONG,        // long long
    T_FLOAT,        // float
    T_DOUBLE,        // double
    T_LONG_DOUBLE,
    T_STRING,
    T_STRUCT,
    T_ARRAY,
    T_POINTER,
    T_ARGLIST,
    T_FUNCTION,
    T_EXTERNAL,
    T_I_N,
    T_UI128,
    T_I128,
    T_ENUM,
    T_UNION,
    T_VECTOR,
    T_OPAQUE
} VariableType;

std::string getTystringByIntTy(int Ty);
#define DEF_FORWARD_DECLARATION(type) class type;
    AST_NODE_LIST(DEF_FORWARD_DECLARATION)
#undef DEF_FORWARD_DECLARATION

class AstVisitor;
class Statement;
class Expression;


///@brief 每个自AstNode派生的类需要使用次宏来正确设置AsXXX和node_type派生方法。
#define AST_DECLARE_REQUIRED_HEADER(className)                                 \
    public:                                                                    \
        virtual IOCASTTYPES node_type() const { return IocAst_k##className; }  \
        virtual className* As##className() {return this; }                     \
        virtual const className* As##className()const {return this; }          \
        virtual const std::string& printable_type_name() const {               \
            static    std::string lv_printable_type_name = #className;         \
            return lv_printable_type_name ;                                    \
        }                                                                      \
        virtual    AstNode* createNode()                                       \
        {                                                                      \
            return new className();                                            \
        }                                                                      \
        className(const className& ref)                                        \
        {                                                                      \
            this->assignFrom(&ref);                                            \
        }


///@brief A node, his child count can be changed, needs this macro body.
#define AST_CONTAINER_NODE_REQUIRED_HEADER                                    \
    protected:                                                                \
        ZoneVector<AstNode*> m_container;                                     \
    public:                                                                   \
        virtual size_t childrenCount(void) const{                             \
            return m_container.size();                                        \
        }                                                                     \
        virtual AstNode* getChildren(size_t idx) const {                      \
            return (m_container.size() > idx) ? m_container[idx] : NULL;      \
        }                                                                     \
        void setChildren(size_t idx, AstNode* node)                           \
        {                                                                     \
            if(m_container.size() < idx+1)                                    \
                m_container.resize(idx+1);                                    \
            m_container[idx] = node;                                          \
        }                                                                     \
        void push_back(AstNode* next){                                        \
            if(next)                                                          \
                m_container.push_back(next);                                  \
        }                                                                     \
        void pop_back()                                                       \
        {                                                                     \
            m_container.pop_back();                                           \
        }                                                                     \
        void insert( ZoneVector<AstNode*>::iterator start,                    \
                    ZoneVector<AstNode*>::iterator end )                      \
        {                                                                     \
            m_container.insert(m_container.begin(), start, end);              \
        }                                                                     \
        void insert(AstNode *tmp)                                             \
        {                                                                     \
            m_container.insert(m_container.begin(), tmp);                     \
        }                                                                     \
        void clear()                                                          \
        {                                                                     \
            m_container.clear();                                              \
        }

    /*ioc ast 基类 */
    class AstNode: public ZoneObject {
    protected:
        friend class AstVisitor;
        /// @brief 实现了通用的遍历方法。
        /// @details 派生类可以以类似方式实现Traversal并根据AstVisitor的类型重定向到AstNode的不同方法上去——比如codegen.
        virtual void Traversal(AstVisitor* v);

        AstNode(int l,int c,const std::string& filename) :
            id_(GetNextId()),
            line_(l), column_(c),sourceName_(filename)
        {
        }
        AstNode() :
            id_(GetNextId()),
            line_(-1), column_(-1)
        {
        }
    public:
        //virtual void Traversal(AstVisitor* v, int num);
        virtual ~AstNode() {}

        /// @brief print node type name
        virtual const std::string& printable_type_name () const = 0;

        /// @brief 虚函数，用于维护属性信息。
        virtual size_t attributeCount(void) const{
            return 0;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            return false;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            return false;
        }

        virtual bool  selfEqual(const AstNode* pNode)const{
            return ((pNode != NULL) && (this->node_type() == pNode->node_type()));
        }

        inline bool  logicEqual(const AstNode* pNode)const{
            bool bEqual = false;
            if(selfEqual(pNode) && this->childrenCount() == pNode->childrenCount())
            {
                bEqual = true;
                for(size_t i = 0 ; i < this->childrenCount(); i++)
                {
                    AstNode* pc1 = this->getChildren(i);
                    AstNode* pc2 = pNode->getChildren(i);
                    if( pc1 == NULL || pc2 == NULL)
                    {
                        if(pc1 != pc2)
                        {
                            bEqual = false;
                            break;
                        }
                    }else{
                        if(!pc1 || !pc1->logicEqual(pc2))
                        {
                            bEqual = false;
                            break;
                        }
                    }
                }
            }
            return bEqual;
        }

        inline bool operator == (const AstNode& Ref)const
        {
            return logicEqual(&Ref);
        }


        /// @brief 虚函数，用于返回孩子数量。
        virtual    size_t childrenCount(void) const{
            return 0;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            return NULL;
        }

        virtual    AstNode* createNode()
        {
            return NULL;
        }

        /// @brief 用于深度克隆一个相同节点(含子节点)。
        AstNode* clone()
        {
            AstNode    *pRet = this->createNode();
            pRet->assignFrom(this);
            size_t i;
            for(i = 0; i < this->childrenCount(); i++)
            {
                if(this->getChildren(i))
                {
                    pRet->setChildren(i, this->getChildren(i)->clone());
                }
                else
                {
                    pRet->setChildren(i, NULL);
                }
            }
            return pRet;
        }

        virtual void    setChildren(size_t idx,AstNode* node) {}
        virtual void    push_back(AstNode* next){}
        virtual void    pop_back(){}
        virtual void    insert(AstNode* tmp){}
        virtual void    insert(ZoneVector<AstNode*>::iterator start,
                    ZoneVector<AstNode*>::iterator end ){}
        virtual void    clear(){}

        /// @brief 派生类需要重载这个方法，并返回正确的类型。这由宏AST_DECLARE_REQUIRED_HEADER自动实现。
        virtual IOCASTTYPES node_type() const { return IocAst_kInvalid; }

        // Type testing & conversion functions overridden by concrete subclasses.
        // 缺省返回NULL，允许子类只派生As自己类型()返回reinterpret_cast后的指针就可以了。这由宏AST_DECLARE_REQUIRED_HEADER自动实现。
        #define DECLARE_NODE_FUNCTIONS(type)                      \
          virtual type* As##type() { return NULL; }               \
          virtual const type* As##type()const { return NULL; }

          AST_NODE_LIST(DECLARE_NODE_FUNCTIONS)
        #undef DECLARE_NODE_FUNCTIONS

        unsigned id() const { return id_; }
        static void ResetIds() { current_id_ = 0; }
        /**********************************************************************
         * Debug support info
         *********************************************************************/

        inline int line(void) const
        {
            return line_;
        }

        inline int pos(void) const
        {
            return column_;
        }

        inline void line(int line)
        {
            line_ = line;
        }

        inline void pos(int col)
        {
            column_ = col;
        }

        /**
         * @brief get source file name
         * @return
         **/
        inline const std::string& getSourceName(void) const
        {
            return sourceName_;
        }

        /**
         * @brief set souce file name
         * @param name
         **/
        inline void setSourceName(const std::string& name)
        {
            sourceName_ = name;
        }
    protected:
        static unsigned GetNextId() {
            return current_id_++;
        }
        static unsigned ReserveIdRange(int n) {
            unsigned tmp = current_id_;
            current_id_ += n;
            return tmp;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            if(!psrc)
            {
                IOC_THROW("Invalid AstNode Pointer");
            }
            this->id_ = psrc->id_;
            this->line_ = psrc->line_;
            this->column_ = psrc->column_;
            this->sourceName_ = psrc->sourceName_;
        }
    private:
        static unsigned current_id_;
        unsigned id_;
        // Location of code fragment in source-code file.
        int     line_;
        int     column_;
        /// @brief the name of source file, with full path.
        std::string sourceName_;
    };

    class Statement : public AstNode{
        AST_DECLARE_REQUIRED_HEADER(Statement)
    public:
        Statement(){}
    };

    class Expression: public Statement {
        AST_DECLARE_REQUIRED_HEADER(Expression)
    public:
        Expression(){}
    };

    /**
     * @brief `withStatement` parser rule in grammar.
     * @details JavaScript Example: NULL
     **/
     class WithStatement: public Statement {
        AST_DECLARE_REQUIRED_HEADER(WithStatement)
    private:
        AstNode* m_expression;//孩子节点指针
        AstNode* m_statement;
    public:

        WithStatement() :
            m_expression(NULL), m_statement(NULL) {
        }

        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_expression;
            case 1:
                return m_statement;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            case 1:
                m_statement = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }
        inline AstNode*    statement(void)const {return m_statement;}
        inline     void    statement(AstNode* s){
            m_statement = s;
        }
    };

    /**
     * @brief `caseBlock` parser rule in grammar.
     * @details
     * JavaScript Example: <pre>case 1: a=1;break;</pre> in:
     * @code
     *   switch(c){case 1: a=1;break;}
     * @endcode
     **/
    class CaseBlock : public AstNode{
        AST_DECLARE_REQUIRED_HEADER(CaseBlock)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        CaseBlock() {
        }
    };

    /**
     * @brief `caseClause` parser rule in grammar.
     * @details JavaScript Example: "case 1: a=1;break;" in "case 1: a=1;break; case 2: b=2;break;"
     **/
    class CaseClause : public Statement{
        AST_DECLARE_REQUIRED_HEADER(CaseClause)
    protected:
        AstNode* m_expression;
        AstNode* m_statement;
    public:
        CaseClause() :
            m_expression(NULL), m_statement(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_expression;
            case 1:
                return m_statement;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            case 1:
                m_statement = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* expression(void)const {return m_expression;}
        inline void expression(AstNode* e){
            m_expression = e;
        }
        inline AstNode* statement(void)const {return m_statement;}
        inline void statement(AstNode* s){
            m_statement = s;
        }
    };

    /**
     * @brief `defaultClause` parser rule in grammar.
     * @details JavaScript Example: "default: c=3;break;" in "case 1: a=1;break; case 2: b=2;break; default: c=3;break;"
     **/
    class DefaultClause : public Statement{
        AST_DECLARE_REQUIRED_HEADER(DefaultClause)
    protected:
        AstNode* m_statement;
    public:
        DefaultClause() :
            m_statement(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 1;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_statement;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_statement = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* statement(void)const {return m_statement;}
        inline void statement(AstNode* s){
            m_statement = s;
        }
    };

    /**
     * @brief `forInStatement` parser rule in grammar.
     * @details JavaScript Example: "for(i in arr){echo arr[i]}"
     **/
    class ForInStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(ForInStatement)
    protected:
        AstNode* m_init;
        AstNode* m_enumerable;
        AstNode* m_body;
    public:
        ForInStatement() :
            m_init(NULL), m_enumerable(NULL), m_body(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 3;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_init;
            case 1:
                return m_enumerable;
            case 2:
                return m_body;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_init = node;
                break;
            case 1:
                m_enumerable = node;
                break;
            case 2:
                m_body = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* init(void)const {return m_init;}
        inline void init(AstNode* i){
            m_init = i;
        }
        inline AstNode* enumerable(void)const {return m_enumerable;}
        inline void enumerable(AstNode* e){
            m_enumerable = e;
        }
        inline AstNode* body(void)const {return m_body;}
        inline void body(AstNode* b){
            m_body = b;
        }
    };

    /**
     * @brief `newExpression` parser rule in grammar.
     * @details JavaScript Example: "new Object()"
     **/
    class NewExpression : public Expression{
        AST_DECLARE_REQUIRED_HEADER(NewExpression)
    private:
        AstNode* m_expression;
        AstNode* m_argument;
    public:
        NewExpression(void) :
            m_expression(NULL),m_argument(NULL)
        {
        }
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx){
            case 0:
                return m_expression;
            case 1:
                return m_argument;
            default:
                break;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            case 1:
                m_argument = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }

        inline AstNode*    argument(void)const {return m_argument;}
        inline     void    argument(AstNode* e){
            m_argument = e;
        }
    };


    /**
     * @brief `arguments` parser rule in grammar.
     * @details JavaScript Example: "dateString" in "new Date(dateString)"
     **/
    class Arguments : public Expression{
        AST_DECLARE_REQUIRED_HEADER(Arguments)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        Arguments() {
        }
    };

    /**
     * @brief Multi property accessor, split by "."
     * @details JavaScript Example: "a.b.c.fn().d.e.f.fn2().g"
     **/
    class MultiPropertyAccessor : public Expression {
        AST_DECLARE_REQUIRED_HEADER(MultiPropertyAccessor)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        MultiPropertyAccessor() {
        }
    };

    /**
     * @brief `indexSuffix` parser rule in grammar.
     * @details JavaScript Example: "[i]" in "a[i]"
     **/
    class IndexSuffix : public Expression{
        AST_DECLARE_REQUIRED_HEADER(IndexSuffix)
    public:
        IndexSuffix(void) :
            m_expression(NULL) {
        }

        virtual    size_t childrenCount(void) const{
            return 1;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx){
            case 0:
                return m_expression;
            default:
                break;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }
    protected:
        AstNode* m_expression;
    };

    /**
     * @brief The property of a object, containing his name string.
     * @details
     * Javascript:
     *   `propertyReferenceSuffix` parser rule in grammar.
     *   src: "property" in "Object.property"
     **/
    class PropertyReferenceSuffix : public Expression{
        typedef Expression  inherit;
        AST_DECLARE_REQUIRED_HEADER(PropertyReferenceSuffix)
    public:
        PropertyReferenceSuffix(void) :
            m_expression(NULL)
        {
        }

        virtual    size_t childrenCount(void) const{
            return 1;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx){
            case 0:
                return m_expression;
            default:
                break;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }

        inline const std::string& name(void) const { return m_name; }
        void name(const std::string &str) { m_name = str; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_name_name;
              value = m_name.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_name_name) == 0)
            {
               m_name = value;
               bSetOK = true;
            }
            return bSetOK;
        }

    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const PropertyReferenceSuffix* pSelf = pNode->AsPropertyReferenceSuffix();
			    return (m_name == pSelf->m_name);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const PropertyReferenceSuffix* pPropertyReferenceSuffix = psrc->AsPropertyReferenceSuffix();
            if(pPropertyReferenceSuffix)
            {
                this->m_name = pPropertyReferenceSuffix->m_name;
            }
            inherit::assignFrom(psrc);
        }
    private:
        static  const char* sv_name_name;
        std::string m_name;
        AstNode* m_expression;
    };

    /**
     * @brief Ternary operation
     **/
    class Conditional : public Expression{
        /// @brief conditional expression
        AstNode * condition_;

        /// @brief expression when value is true.
        AstNode * valueIfTrue_;

        /// @brief expression when value is false.
        AstNode * valueIfFalse_;

        AST_DECLARE_REQUIRED_HEADER(Conditional)
    public:
        Conditional(void) :
            condition_(NULL), valueIfTrue_(NULL), valueIfFalse_(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 3;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx){
            case 0:
                return condition_;
            case 1:
                return valueIfTrue_;
            case 2:
                return valueIfFalse_;
            default:
                break;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                condition_ = node;
                break;
            case 1:
                valueIfTrue_ = node;
                break;
            case 2:
                valueIfFalse_ = node;
                break;
            default:
                break;
            }
        }

        /// @brief get the conditional expression.
        inline AstNode*    condition(void) const { return condition_; }

        /// @brief get the expression if true.
        inline AstNode*    valueIfTrue(void) const { return valueIfTrue_; }

        /// @brief get the expression if false.
        inline AstNode*    valueIfFalse(void) const { return valueIfFalse_; }

        /// @brief get the conditional expression.
        inline void condition(AstNode* e) { condition_ = e; }

        /// @brief set the expression if true.
        inline void valueIfTrue(AstNode* e) { valueIfTrue_ = e; }

        /// @brief set the expression if false.
        inline void valueIfFalse(AstNode* e) { valueIfFalse_ = e; }

    };

    /**
     * @brief `objectLiteral` parser rule in grammar.
     * @details JavaScript Example: "{a:1,b:2,c:3}"
     **/
    class ObjectLiteral : public Expression{
        AST_DECLARE_REQUIRED_HEADER(ObjectLiteral)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        ObjectLiteral() {
        }
    };

    /**
     * @brief `propertyNameAndValue` parser rule in grammar.
     * @details JavaScript Example: "a:1" in "{a:1,b:2,c:3}"
     **/
    class PropertyNameAndValue : public Expression{
        AST_DECLARE_REQUIRED_HEADER(PropertyNameAndValue)
    public:
        PropertyNameAndValue(void) :
            m_name(NULL), m_expression(NULL)
        {
        }
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx){
            case 0:
                return m_name;
            case 1:
                return m_expression;
            default:
                break;
            }
            return NULL;
        }
        /// @brief 虚函数，用于设置孩子的指针。
        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_name = node;
                break;
            case 1:
                m_expression = node;
                break;
            default:
                break;
            }
        }
        inline AstNode*    name(void)const {return m_name;}
        inline     void    name(AstNode* n){
            m_name = n;
        }
        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }
    protected:
        AstNode* m_name;
        AstNode* m_expression;
    };


    ///@brief this is like LLVM::Module.
    class SourceElements : public AstNode{
        AST_DECLARE_REQUIRED_HEADER(SourceElements)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        SourceElements()
        {
        }
    };

    /**
     * @brief Describ a empty statement, with nothing to do.
     * @details JavaScript Example: ";"
     **/
    class EmptyStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(EmptyStatement)
    public:
        EmptyStatement() {
        }
    };

    /**
     * @brief The continue statement.
     * @details JavaScript Example: "continue i;" OR "continue;"
     **/
    class ContinueStatement: public Statement {
        typedef Statement  inherit;
        AST_DECLARE_REQUIRED_HEADER(ContinueStatement)
    public:
        ContinueStatement()
        {
        }
        virtual size_t childrenCount(void) const{
            return 0;
        }
        virtual AstNode* getChildren(size_t idx) const{
            return NULL;
        }

        inline const std::string& identifier(void) const { return m_identifier; }
        inline void identifier(const std::string &i) { m_identifier = i; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_identifier_name;
              value = m_identifier.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_identifier_name) == 0)
            {
               m_identifier = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const ContinueStatement* pSelf = pNode->AsContinueStatement();
			    return (m_identifier == pSelf->m_identifier);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const ContinueStatement*    pContinueStatement = psrc->AsContinueStatement();
            if(pContinueStatement)
            {
                this->m_identifier = pContinueStatement->m_identifier;
            }
            inherit::assignFrom(psrc);
        }
    private:
        std::string m_identifier;
        static const char* sv_identifier_name;
    };

    /**
     * @brief The break statement.
     * @details JavaScript Example: "break i;" OR "break;"
     **/
    class BreakStatement: public Statement {
        typedef Statement     inherit;
        AST_DECLARE_REQUIRED_HEADER(BreakStatement)
    public:
        BreakStatement()
        {
        }
        virtual size_t childrenCount(void) const{
            return 0;
        }
        virtual AstNode* getChildren(size_t idx) const{
            return NULL;
        }
        inline const std::string& identifier(void) const { return m_identifier; }
        inline void identifier(const std::string &i) { m_identifier = i; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_identifier_name;
              value = m_identifier.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_identifier_name) == 0)
            {
               m_identifier = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const BreakStatement* pSelf = pNode->AsBreakStatement();
			    return (m_identifier == pSelf->m_identifier);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const BreakStatement*    pBreakStatement = psrc->AsBreakStatement();
            if(pBreakStatement)
            {
                this->m_identifier = pBreakStatement->m_identifier;
            }
            inherit::assignFrom(psrc);
        }
    private:
        std::string m_identifier;
        static const char* sv_identifier_name;
    };

    /**
     * @brief The return statement.
     * @details JavaScript Example: "return i=1;"
     **/
    class ReturnStatement: public Statement {
        AST_DECLARE_REQUIRED_HEADER(ReturnStatement)
    public:
        ReturnStatement() :
            m_expression(NULL)
        {
        }
        virtual    size_t childrenCount(void) const{
            return 1;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_expression;
            }
            return NULL;
        }

        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_expression = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* i){
            m_expression = i;
        }
    private:
        AstNode* m_expression;//孩子节点指针
    };

    /**
     * @brief A label statement in Javascript.
     * @details JavaScript Example: "label:i=1" OR "label:if(){}"
     **/
    class LabelledStatement: public Statement {
        typedef  Statement  inherit;
        AST_DECLARE_REQUIRED_HEADER(LabelledStatement)
    public:
        LabelledStatement() :
            m_statement(NULL)
        {
        }

        virtual    size_t childrenCount(void) const{
            return 1;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_statement;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_statement = node;
                break;
            default:
                break;
            }
        }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_identifier_name;
              value = m_identifier.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_identifier_name) == 0)
            {
               m_identifier = value;
               bSetOK = true;
            }
            return bSetOK;
        }

        inline const std::string& identifier(void)const { return m_identifier; }
        inline void identifier(const std::string &i){ m_identifier = i; }

        inline AstNode*    statement(void) const { return m_statement; }
        inline void statement(AstNode* s){ m_statement = s; }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const LabelledStatement* pSelf = pNode->AsLabelledStatement();
			    return (m_identifier == pSelf->m_identifier);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const LabelledStatement*    pLabelledStatement = psrc->AsLabelledStatement();
            if(pLabelledStatement)
            {
                this->m_identifier = pLabelledStatement->m_identifier;
            }
            inherit::assignFrom(psrc);
        }
    private:
        static const char* sv_identifier_name;
        std::string m_identifier;//节点名称
        AstNode* m_statement;//孩子节点指针
    };

    /**
     * @brief `switchStatement` parser rule in grammar.
     * @details JavaScript Example: "switch(c){case 1: a=1;break}"
     **/
    class SwitchStatement: public Statement {
        AST_DECLARE_REQUIRED_HEADER(SwitchStatement)
    public:
        SwitchStatement() :
            m_expression(NULL), m_case(NULL) {
        }

        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_expression;
            case 1:
                return m_case;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            case 1:
                m_case = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }
        inline AstNode*    caseBlock(void)const {return m_case;}
        inline     void    caseBlock(AstNode* c){
            m_case = c;
        }
    protected:
        AstNode* m_expression;//孩子节点指针,指向switch后()中的表达式节点
        AstNode* m_case;
    };

    /**
     * @brief `doWhileStatement` parser rule in grammar.
     * @details JavaScript Example: "do{b=1}while(a==1)"
     **/
    class DoWhileStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(DoWhileStatement)
    public:
        DoWhileStatement() :
            m_body(NULL), m_condition(NULL)
        {
        }
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_body;
            case 1:
                return m_condition;
            }
            return NULL;
        }
        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_body = node;
                break;
            case 1:
                m_condition = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* body(void)const {return m_body;}
        inline void body(AstNode* d){
            m_body = d;
        }
        inline AstNode* condition(void)const {return m_condition;}
        inline void condition(AstNode* c){
            m_condition = c;
        }
    private:
        AstNode* m_body;
        AstNode* m_condition;
    };

    /**
     * @brief `whileStatement` parser rule in grammar.
     * @details JavaScript Example: "while(a==1){b=1}"
     **/
    class WhileStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(WhileStatement)
    public:
        WhileStatement() :
            m_condition(NULL), m_body(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_condition;
            case 1:
                return m_body;
            }
            return NULL;
        }
        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_condition = node;
                break;
            case 1:
                m_body = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* condition(void)const {return m_condition;}
        inline void condition(AstNode* c){
            m_condition = c;
        }
        inline AstNode* body(void)const {return m_body;}
        inline void body(AstNode* b){
            m_body = b;
        }
    private:
        AstNode* m_condition;
        AstNode* m_body;
    };

    /**
     * @brief `forStatement` parser rule in grammar.
     * @details JavaScript Example: "for(i=0;i<10;i++){echo i}"
     **/
    class ForStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(ForStatement)
    public:
        ForStatement() :
            m_init(NULL), m_condition(NULL), m_next(NULL), m_body(NULL)
        {
        }
        virtual    size_t childrenCount(void) const{
            return 4;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_init;
            case 1:
                return m_condition;
            case 2:
                return m_next;
            case 3:
                return m_body;
            }
            return NULL;
        }
        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_init = node;
                break;
            case 1:
                m_condition = node;
                break;
            case 2:
                m_next = node;
                break;
            case 3:
                m_body = node;
                break;
            default:
                break;
            }
        }
        inline AstNode* init(void)const {return m_init;}
        inline void init(AstNode* i){
            m_init = i;
        }
        inline AstNode* condition(void)const {return m_condition;}
        inline void condition(AstNode* c){
            m_condition = c;
        }
        inline AstNode* next(void)const {return m_next;}
        inline void next(AstNode* n){
            m_next = n;
        }
        inline AstNode* body(void)const {return m_body;}
        inline void body(AstNode* b){
            m_body = b;
        }
    private:
        AstNode* m_init;    //@example中的i=0;
        AstNode* m_condition;    //@example中的i<10;
        AstNode* m_next;    //@example中的i++
        AstNode* m_body;    //@example中的{echo i}
    };

    /**
     * @brief `tryStatement` parser rule in grammar.
     * @details JavaScript Example: "try{...}catch(err){...}finally{...}"
     **/
    class TryStatement : public Statement
    {
        AST_DECLARE_REQUIRED_HEADER(TryStatement)
    public:
        TryStatement() :
            m_statement(NULL), m_catchClause(NULL), m_finallyClause(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 3;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_statement;
            case 1:
                return m_catchClause;
            case 2:
                return m_finallyClause;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_statement = node;
                break;
            case 1:
                m_catchClause = node;
                break;
            case 2:
                m_finallyClause = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* statement(void)const {return m_statement;}
        inline void statement(AstNode* s){
            m_statement = s;
        }
        inline AstNode* catchClause(void)const {return m_catchClause;}
        inline void catchClause(AstNode* c){
            m_catchClause = c;
        }
        inline AstNode* finallyClause(void)const {return m_finallyClause;}
        inline void finallyClause(AstNode* f){
            m_finallyClause = f;
        }
    private:
        AstNode* m_statement;
        AstNode* m_catchClause;
        AstNode* m_finallyClause;
    };

    /**
     * @brief `catchClause` parser rule in grammar.
     * @details JavaScript Example: "catch{...}" in "try{...}catch(err){...}finally{...}"
     **/
    class TryCatchStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(TryCatchStatement)
    public:
        TryCatchStatement() :
            m_identifier(NULL), m_statement(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_identifier;
            case 1:
                return m_statement;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_identifier = node;
                break;
            case 1:
                m_statement = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* init(void)const {return m_identifier;}
        inline void init(AstNode* i){
            m_identifier = i;
        }
        inline AstNode* statement(void)const {return m_statement;}
        inline void statement(AstNode* s){
            m_statement = s;
        }
    private:
        AstNode* m_identifier;
        AstNode* m_statement;
    };

    /**
     * @brief `finallyClause` parser rule in grammar.
     * @details JavaScript Example: "finally{...}" in "try{...}catch(err){...}finally{...}"
     **/
    class TryFinallyStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(TryFinallyStatement)
    public:
        TryFinallyStatement() :
            m_statement(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 1;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_statement;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_statement = node;
                break;
            default:
                break;
            }
        }

        inline AstNode* statement(void)const {return m_statement;}
        inline void statement(AstNode* s){
            m_statement = s;
        }
    private:
        AstNode* m_statement;
    };

    /**
     * @brief If Then Else statement.
     * @details JavaScript Example: "if(a==1){b=1}else{b=2}"
     **/
    class IfStatement : public Statement{
        AST_DECLARE_REQUIRED_HEADER(IfStatement)
    public:
        IfStatement() :
            m_condition(NULL), m_then(NULL), m_else(NULL) {
        }
        virtual    size_t childrenCount(void) const{
            return 3;
        }

        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_condition;
            case 1:
                return m_then;
            case 2:
                return m_else;
            }
            return NULL;
        }

        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_condition = node;
                break;
            case 1:
                m_then = node;
                break;
            case 2:
                m_else = node;
            default:
                break;
            }
        }
        inline AstNode* conditionExpression(void)const {return m_condition;}
        inline void conditionExpression(AstNode* c){
            m_condition = c;
        }
        inline AstNode* thenStatement(void)const {return m_then;}
        inline void thenStatement(AstNode* t){
            m_then = t;
        }
        inline AstNode* elseStatement(void)const {return m_else;}
        inline void elseStatement(AstNode* e){
            m_else = e;
        }
    private:
        AstNode* m_condition;
        AstNode* m_then;
        AstNode* m_else;
    };

    /**
     * @class Assignment
     * @details \
     * Grammars:
     *   javascript.g: `assignmentExpression`
     *   java.g: ``
     * Examples:
     *   JavaScript: `a=1`
     *   Java: `a=1`
     **/
    class Assignment : public AstNode{
        typedef AstNode inherit;
        AST_DECLARE_REQUIRED_HEADER(Assignment)
    public:
        Assignment(void) : m_operator(T_INVALID)
        {
          m_left = NULL;
          m_right = NULL;
        }

        /// @brief assignment operation token
        /// @details about shift op:
        /// wikipedia:Arithmetic_shift
        /// wikipedia:Logical_shift
        /// https://developer.mozilla.org/en/JavaScript/Reference/operators/bitwise_operators#.3e.3e_(Sign-propagating_right_shift)
        enum {
            T_INVALID,
            T_EQU,            // `=`
            T_MUL,            // `*=`
            T_DIV,            // `/=`
            T_MOD,            // `%=`
            T_ADD,            // `+=`
            T_SUB,            // `-=`
            T_SHL,            // `<<=` logical/arithmetic shift left
            T_SHR,            // `>>=` arithmetic shift right
            T_SHR_ZEROFILL,    // `>>>=` logical shift right
            T_AND,            // `&=`
            T_XOR,            // `^=`
            T_OR            // `|=`
        };

        char print_value(void) const
        {
            return m_operator;
        }

        virtual    size_t childrenCount(void) const
        {
            return 2;
        }

        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode* getChildren(size_t idx) const
        {
            switch(idx){
            case 0:
                return m_left;
            case 1:
                return m_right;
            default:
                break;
            }
            return NULL;
        }

        virtual void setChildren(size_t idx, AstNode* node)
        {
            switch(idx) {
            case 0:
                m_left = node;
                break;
            case 1:
                m_right = node;
                break;
            default:
                break;
            }
        }
        /** Get section */
        /// @brief Get the left child.
        inline AstNode*    left(void) const { return m_left; }
        /// @brief Set a node pointer to the left child
        inline void left(AstNode* l) { m_left = l; }

        /// @brief Get the right child.
        inline AstNode*    right(void) const { return m_right; }
        /// @brief Set a node pointer to the right child
        inline void right(AstNode* r) { m_right = r; }

        /// @brief Get the operator of node.(NOT SETTER MEMBER).
        inline char operater(void) const { return m_operator; }


        /// @brief Get the operator string of node.
        inline const std::string& opName(void) const { return opName_; }
        /// @brief Set the operator string of node.
        void opName(const std::string &name);

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_operator_name;
              value = opName_.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_operator_name) == 0)
            {
               opName(value);
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const Assignment* pSelf = pNode->AsAssignment();
			    return ((opName_ == pSelf->opName_) && (m_operator == pSelf->m_operator));
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const Assignment*    pAssignment = psrc->AsAssignment();
            if(pAssignment)
            {
                this->m_operator = pAssignment->m_operator;
                this->opName_ = pAssignment->opName_;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /// @brief 左孩子
        AstNode* m_left;

        /// @brief 右孩子
        AstNode* m_right;

        /// @brief 上述枚举中的操作符EQU等
        int m_operator;

        /// @brief the string of operator of node.
        std::string opName_;

        static const char* sv_operator_name;
    };

    /**
     * @brief We are calling a function.
     * @details JavaScript Example: \
     *   "fn()"
     *   OR
     *   "fn()" in "bar=fn();"
     **/
    class Call : public Expression {
        typedef    Expression    inherit;
        AST_DECLARE_REQUIRED_HEADER(Call)
    public:
        Call(void) :
            m_expression(NULL), m_arguments(NULL)
        {
        }

        virtual    size_t childrenCount(void) const{
            return 2;
        }
        /// @brief 虚函数，用于返回孩子的指针。
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx){
            case 0:
                return m_expression;
            case 1:
                return m_arguments;
            default:
                break;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            case 1:
                m_arguments = node;
                break;
            default:
                break;
            }
        }

        /// @brief Give you the name of the function which you are calling.
        inline const std::string& name(void) const { return m_name; }
        /// @brief Give me the function name.
        inline void name(const std::string &n) {
            m_name = n;
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }

        inline AstNode*    arguments(void)const {return m_arguments;}
        inline     void    arguments(AstNode* a){
            m_arguments = a;
        }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_name_name;
              value = m_name.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_name_name) == 0)
            {
               m_name = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const Call* pSelf = pNode->AsCall();
			    return (m_name == pSelf->m_name);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const Call*    pCall = psrc->AsCall();
            if(pCall)
            {
                this->m_name = pCall->m_name;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /**
         * @brief Function name.
         * * If a javascript function, his function name is a expression,
         * this is left as empty string.
         * @details JavaScript Example:
         * You give a "fn(){...}", and his function name is "fn".
         **/
        std::string m_name;
        /**
         * @brief Javascript function name expression
         * * The name of js function may be a expression.
         * @details
         * JavaScript Example: `(function(){alert(1);})();`
         **/
        AstNode* m_expression;
        /**
         * @brief Argument list of function
         **/
        AstNode* m_arguments;

        /** @brief attribute name.
        **/
        static const char* sv_name_name;
    };

    /**
     * @brief `arrayLiteral` parser rule in grammar.
     * @details JavaScript Example: "[a,b,c]"
     **/
    class ArrayLiteral : public Expression{
        AST_DECLARE_REQUIRED_HEADER(ArrayLiteral)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        ArrayLiteral() {
        }
    };

    /**
     * @brief Compare operation
     * @details JavaScript Example: \
     *   "a<b"
     *   OR
     *   "a==b"
     **/
    class CompareOperation: public Expression{
        typedef    Expression inherit;
        AST_DECLARE_REQUIRED_HEADER(CompareOperation)
    public:
        CompareOperation() :
            m_operator(T_INVALID), m_left(NULL), m_right(NULL)
        {
        }

        enum{
            T_INVALID,
            T_LT,                // `<`
            T_GT,                // `>`
            T_EQU,                // `==`
            T_NOTEQU,            // `!=`
            T_LE,                // `<=`
            T_GE,                // `>=`
            T_EXEQU,            // `===` js only
            T_EXNOTEQU,            // `!==` js only
            T_INSTANCEOF,        // `instanceof` js only
            T_IN,                // `in` js only
            T_INIT              //':'
        };

        virtual    size_t childrenCount(void) const{
            return 2;
        }
        int print_value() const{
            return m_operator;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_left;
            case 1:
                return m_right;
            }
            return NULL;
        }

        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_left=node;
                break;
            case 1:
                m_right=node;
                break;
            default:
                break;
            }
        }

        /// @brief ...
        inline AstNode*    left(void)const {return m_left;}
        /// @brief ...
        inline void left(AstNode* l) { m_left = l; }

        /// @brief ...
        inline AstNode*    right(void)const {return m_right;}
        /// @brief ...
        inline void right(AstNode* r) { m_right = r; }

        /// @brief ...
        inline int operater(void) const { return m_operator; }

        /// @brief Get the operation name string.
        inline const std::string& opName(void) const { return opName_; }
        /// @brief Set the operation name string.
        void opName(const std::string &name);

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_operater_name;
              value = opName_.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_operater_name) == 0)
            {
               opName(value);
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const CompareOperation* pSelf = pNode->AsCompareOperation();
			    return ((opName_ == pSelf->opName_) && (m_operator == pSelf->m_operator));
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const CompareOperation*    pCompareOperation = psrc->AsCompareOperation();
            if(pCompareOperation)
            {
                this->opName_ = pCompareOperation->opName_;
                this->m_operator = pCompareOperation->m_operator;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /// @brief Save the op as a string, only for debug when printing AST.
        std::string opName_;
        /// @brief 上述枚举中的操作符等
        int  m_operator;
        AstNode *m_left;
        AstNode    *m_right;
        static const char* sv_operater_name;
    };

    /**
     * .g:
     * additiveExpression
     * multiplicativeExpression
     * eg:
     *  1+2 OR "a=1,b=2" in "if(a=1,b=2){}"
     **/
    class BinaryOperation: public Expression{
        typedef    Expression inherit;
        AST_DECLARE_REQUIRED_HEADER(BinaryOperation)
    public:
        BinaryOperation() :
            m_operator(T_INVALID), m_left(NULL), m_right(NULL)
        {
        }

        //char getchildType(AstNode *child);
        enum{
            T_INVALID,
            // Binary operation.
            T_ADD,            // `+`
            T_SUB,            // `-`
            T_MUL,            // `*`
            T_DIV,            // `/`
            T_MOD,            // `%`
            T_BIT_OR,        // `|`
            T_BIT_XOR,        // `^`
            T_BIT_AND,        // `&`
            T_SHL,            // `<<`
            T_SHR,            // `>>`
            T_SAR,            // `>>>`
            T_OR,            // `||`
            T_AND,            // `&&`
            // Special operation.
            T_COMMA,        // `,`
            T_MEMBER        //'.|->'
        };
        virtual    size_t childrenCount(void) const{
            return 2;
        }
        int print_value() const{
            return m_operator;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_left;
            case 1:
                return m_right;
            }
            return NULL;
        }

        virtual void    setChildren(size_t idx,AstNode* node){
            switch(idx){
            case 0:
                m_left=node;
                break;
            case 1:
                m_right=node;
                break;
            default:
                break;
            }
        }

        /// @brief ...
        inline AstNode*    left(void)const {return m_left;}
        /// @brief ...
        inline void left(AstNode* l) { m_left = l; }

        /// @brief ...
        inline AstNode*    right(void)const {return m_right;}
        /// @brief ...
        inline void right(AstNode* r) { m_right = r; }

        /// @brief ...
        inline char operater(void) const { return m_operator; }

        /// @brief Get the op name string.
        inline const std::string& opName(void) const { return opName_; }
        /// @brief Set the op name string.
        void opName(const std::string &name);

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_operater_name;
              value = opName_.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_operater_name) == 0)
            {
               opName(value);
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const BinaryOperation* pSelf = pNode->AsBinaryOperation();
			    return ((opName_ == pSelf->opName_) && (m_operator == pSelf->m_operator));
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const BinaryOperation*    pBinaryOperation = psrc->AsBinaryOperation();
            if(pBinaryOperation)
            {
                this->opName_ = pBinaryOperation->opName_;
                this->m_operator = pBinaryOperation->m_operator;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /// @brief 二元操作符标示 enum数据
        int m_operator;

        /// @brief Save the op as a string, only for debug when printing AST.
        std::string opName_;

        /// @brief 左操作数
        AstNode *m_left;

        /// @brief 右操作数
        AstNode    *m_right;

        /// @brief 属性名。
        static const char* sv_operater_name;
    };

    /**
     * variable name
     **/
    class VariableProxy: public Expression {
        typedef    Expression inherit;
        AST_DECLARE_REQUIRED_HEADER(VariableProxy)
    public:
        VariableProxy() : m_is_declaration(false)
        {
        }
        /**
         * Get Action
         **/

        ///@brief Get the name string of variable.
        inline const std::string& js_identifier(void) const { return m_js_identifier; }
        ///@brief Set the name string of variable.
        inline void js_identifier(const std::string &i) { m_js_identifier = i; }

        ///@brief Check if this node is a declaration.
        inline bool isDeclaration(void) const { return m_is_declaration; }
        ///@brief Set this node as a declaration.
        inline void isDeclaration(bool b) { m_is_declaration = b; }

        virtual size_t attributeCount(void) const{
            return 2;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_is_declar_name;
              value = m_is_declaration ? sv_true_value : sv_false_value;
              bGetOK = true;
              break;
            case 1:
              name = sv_identify_name;
              value = m_js_identifier.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_is_declar_name) == 0)
            {
               if(strcmp(value,sv_false_value) == 0){
                    m_is_declaration = false;
               }else{
                    m_is_declaration = true;
               }
               bSetOK = true;
            }else if(strcmp(name,sv_identify_name) == 0)
            {
            m_js_identifier = value;
            bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const VariableProxy* pSelf = pNode->AsVariableProxy();
			    return ((m_js_identifier == pSelf->m_js_identifier) && (m_is_declaration == pSelf->m_is_declaration));
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const VariableProxy*    pVariableProxy = psrc->AsVariableProxy();
            if(pVariableProxy)
            {
                this->m_js_identifier = pVariableProxy->m_js_identifier;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /**
         * Javascript feature.
         **/
        /// @brief name string of variable
        std::string m_js_identifier;
        /// @brief 只有当变量声明时为true。如：jsval a;/javal b = 1;
        bool m_is_declaration;

        static const char* sv_is_declar_name;
        static const char* sv_identify_name;
        static const char* sv_true_value;
        static const char* sv_false_value;
    };

    /**
     * @brief A string.
     *   example "abc"
     **/
    class StringLiteral : public Expression{
        typedef    Expression    inherit;
        AST_DECLARE_REQUIRED_HEADER(StringLiteral)
    public:
        StringLiteral() {
        }
        inline const std::string &str(void) const { return m_str; }
        inline void str(const std::string &s) { m_str = s; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_literal_name;
              value = m_str.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_literal_name) == 0)
            {
               m_str = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const StringLiteral* pSelf = pNode->AsStringLiteral();
			    return (m_str == pSelf->m_str);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const StringLiteral*    pStringLiteral = psrc->AsStringLiteral();
            if(pStringLiteral)
            {
                this->m_str = pStringLiteral->m_str;
            }
            inherit::assignFrom(psrc);
        }
    private:
        //
        std::string m_str;

        static const char* sv_literal_name;
    };

    /**
     * @brief A number
     * @details \
     *   JavaScript Example: "123" in "num=123;"
     **/
    class NumberLiteral : public Expression{
        typedef    Expression inherit;
        AST_DECLARE_REQUIRED_HEADER(NumberLiteral)
    public:
        NumberLiteral() {
        }
        inline const std::string& num(void) { return m_num; }
        inline void num(const std::string &n) { m_num = n; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_literal_name;
              value = m_num.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_literal_name) == 0)
            {
               m_num = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const NumberLiteral* pSelf = pNode->AsNumberLiteral();
			    return (m_num == pSelf->m_num);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const NumberLiteral*    pNumberLiteral = psrc->AsNumberLiteral();
            if(pNumberLiteral)
            {
                this->m_num = pNumberLiteral->m_num;
            }
            inherit::assignFrom(psrc);
        }
    private:
        std::string m_num;
        static const char* sv_literal_name;
    };

    /**
     * @brief A Boolean.
     * @details \
     *   Javascript : "true" in "a=true;"
     **/
    class BooleanLiteral : public Expression{
        typedef    Expression    inherit;
        AST_DECLARE_REQUIRED_HEADER(BooleanLiteral)
    public:
        BooleanLiteral() : m_isTrue(false)
        {
        }
        void setValue(bool value) { m_isTrue = value;}
        bool getValue(){return m_isTrue;}

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_literal_name;
              value = m_isTrue ? sv_true_value : sv_false_value;
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_literal_name) == 0)
            {
               if(strcmp(value,sv_false_value) == 0)
               {
             m_isTrue = false;
               }else{
             m_isTrue = true;
               }
               bSetOK = true;
            }
            return bSetOK;
        }
        static inline const char* trueValue(){return sv_true_value;}
        static inline const char* falseValue(){return sv_false_value;}
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const BooleanLiteral* pSelf = pNode->AsBooleanLiteral();
			    return (m_isTrue == pSelf->m_isTrue);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const BooleanLiteral*    pBooleanLiteral = psrc->AsBooleanLiteral();
            if(pBooleanLiteral)
            {
                this->m_isTrue = pBooleanLiteral->m_isTrue;
            }
            inherit::assignFrom(psrc);
        }
    private:
        bool m_isTrue;
        static const char* sv_literal_name;
        static const char* sv_true_value;
        static const char* sv_false_value;
    };

    /**
     * @brief Null value.
     * @details \
     *   Javascript : "null" in "a=null;"
     **/
    class NullLiteral : public Expression{
        typedef Expression inherit;
        AST_DECLARE_REQUIRED_HEADER(NullLiteral)
    public:
        NullLiteral()
        {
        }
        inline const std::string& value(){return m_value;}
        inline void value(const std::string& t){m_value = t;}
        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_literal_name;
              value = m_value.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_literal_name) == 0)
            {
               m_value = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const NullLiteral* pSelf = pNode->AsNullLiteral();
			    return (m_value == pSelf->m_value);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const NullLiteral*    pNullLiteral = psrc->AsNullLiteral();
            if(pNullLiteral)
            {
                this->m_value = pNullLiteral->m_value;
            }
            inherit::assignFrom(psrc);
        }
    private:
        std::string m_value;

        static const char* sv_literal_name;
    };

    /**
     * @brief `functionDeclaration` parser rule in grammar.
     * @details JavaScript Example: "function fn(a){a=1};"
     **/
    class FunctionDeclaration : public Statement{
        typedef Statement inherit;
        AST_DECLARE_REQUIRED_HEADER(FunctionDeclaration)
    public:
        FunctionDeclaration() :
            m_param(NULL), m_body(NULL)
        {
        }

        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_param;
            case 1:
                return m_body;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_param = node;
                break;
            case 1:
                m_body = node;
                break;
            default:
                break;
            }
        }

        ///@brief get the function name string.
        inline const std::string& name(void) const { return m_name; }
        ///@brief set the name of function.
        inline void name(const std::string &str) { m_name = str; }

        inline AstNode*    param(void) const {return m_param;}
        inline void param(AstNode* p) { m_param = p; }

        inline AstNode*    body(void) const {return m_body;}
        inline void body(AstNode* b) { m_body = b; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_name_name;
              value = m_name.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_name_name) == 0)
            {
               m_name = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const FunctionDeclaration* pSelf = pNode->AsFunctionDeclaration();
			    return (m_name == pSelf->m_name);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const FunctionDeclaration*    pFunctionDeclaration = psrc->AsFunctionDeclaration();
            if(pFunctionDeclaration)
            {
                this->m_name = pFunctionDeclaration->m_name;
            }
            inherit::assignFrom(psrc);
        }
    private:
        AstNode    *m_param;
        AstNode    *m_body;
        ///@brief the name of function.
        std::string m_name;

        static const char* sv_name_name;
    };

    /**
     * @brief `functionExpression` parser rule in grammar.
     * @details JS example "function(a){a=1}" in "var fn=function(a){a=1};"
     **/
    class FunctionExpression : public Statement
    {
        typedef    Statement    inherit;
        AST_DECLARE_REQUIRED_HEADER(FunctionExpression)
    public:
        FunctionExpression() :
            m_param(NULL), m_body(NULL)
        {
        }

        virtual    size_t childrenCount(void) const{
            return 2;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_param;
            case 1:
                return m_body;
            }
            return NULL;
        }

        inline AstNode*    param(void)const {return m_param;}
        inline     void    param(AstNode* p){
            m_param = p;
        }

        inline AstNode*    body(void)const {return m_body;}
        inline    void    body(AstNode* b){
            m_body = b;
        }

        ///@brief get the function name string.
        inline const std::string& name(void) const { return m_name; }
        ///@brief set the name of function.
        inline void name(const std::string &str) { m_name = str; }

        virtual size_t attributeCount(void) const{
            return 1;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_name_name;
              value = m_name.c_str();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_name_name) == 0)
            {
               m_name = value;
               bSetOK = true;
            }
            return bSetOK;
        }
    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const FunctionExpression* pSelf = pNode->AsFunctionExpression();
			    return (m_name == pSelf->m_name);
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const FunctionExpression*    pFunctionExpression = psrc->AsFunctionExpression();
            if(pFunctionExpression)
            {
                this->m_name = pFunctionExpression->m_name;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /// @brief a function-expression may have no name(identifier).
        AstNode    *m_param;
        AstNode    *m_body;
        std::string m_name;

        static const char* sv_name_name;
    };

    /**
     * @brief `formalParameterList` parser rule in grammar.
     * @details JavaScript Example: "(a,b,c)" in "function(a,b,c)"
     **/
    class FormalParameterList : public AstNode{
        AST_DECLARE_REQUIRED_HEADER(FormalParameterList)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        FormalParameterList()
        {
        }
    };

    class UnaryOperation : public Expression{
        typedef    Expression    inherit;
        AST_DECLARE_REQUIRED_HEADER(UnaryOperation)
    public:
        UnaryOperation() :
            m_operator(T_INVALID), m_expression(NULL),m_isFrontOp(false)
        {
        }

        enum{
            T_INVALID,
            T_ADDADD,        // `++` //PRE_INC//POST_INC
            T_SUBSUB,        // `--` //PRE_DEC//POST_DEC
            T_PLUS,            // `+`
            T_MINUS,        // `-`
            T_TILDE,        // `~`
            T_NOT,            // `!`
            K_DELETE,        // js delete keyword
            K_VOID,            // js void keyword
            K_TYPEOF        // js typeof keyword
        };

        virtual    size_t childrenCount(void) const{
            return 1;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_expression;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode* node){
            switch(idx){
            case 0:
                m_expression = node;
                break;
            default:
                break;
            }
        }
        /// @brief Get op
        inline int operater(void) const { return m_operator; }

        /// @brief Get the op name string.
        inline const std::string& opName(void) const { return opName_; }
        /// @brief Set the op name string.
        void opName(const std::string &name);

        /// @brief ...
        inline AstNode*    expression(void) const { return m_expression; }
        /// @brief ...
        inline void expression(AstNode* e) { m_expression = e; }

        /// @brief The op is in front of expression or not.
        inline bool isFrontOp(void) const { return m_isFrontOp; }
        /// @brief Set the op as in front of expression.
        inline void isFrontOp(bool isFront) { m_isFrontOp = isFront; }

        virtual size_t attributeCount(void) const{
            return 2;
        }
        virtual bool  getAttribute(int idx,const char* &name,const char* &value)
        {
            bool bGetOK = false;
            switch(idx)
            {
            case 0:
              name = sv_operator_name;
              value = opName_.c_str();
              bGetOK = true;
              break;
            case 1:
              name = sv_isfront_name;
              value = m_isFrontOp ? BooleanLiteral::trueValue() : BooleanLiteral::falseValue();
              bGetOK = true;
              break;
            }
            return bGetOK;
        }
        virtual bool  setAttribute(const char* name,const char* value)
        {
            bool bSetOK = false;
            if(strcmp(name,sv_operator_name) == 0)
            {
               opName(value);
               bSetOK = true;
            }else if(strcmp(name,sv_isfront_name) == 0)
            {
               if(strcmp(value,BooleanLiteral::trueValue()) == 0){
              m_isFrontOp = true;
               }else{
              m_isFrontOp = false;
               }
            bSetOK = true;
            }
            return bSetOK;
        }

    protected:
        virtual bool  selfEqual(const AstNode* pNode)const{
			if(inherit::selfEqual(pNode))
            {
			    const UnaryOperation* pSelf = pNode->AsUnaryOperation();
			    return ((opName_ == pSelf->opName_) && (m_operator == pSelf->m_operator) && (m_isFrontOp == pSelf->m_isFrontOp));
            }
            return false;
        }

        virtual    void    assignFrom(const AstNode* psrc)
        {
            const UnaryOperation*    pUnaryOperation = psrc->AsUnaryOperation();
            if(pUnaryOperation)
            {
                this->m_operator = pUnaryOperation->m_operator;
                this->opName_ = pUnaryOperation->opName_;
            }
            inherit::assignFrom(psrc);
        }
    private:
        /// @brief 操作符标示，enum数据
        int     m_operator;

        /// @brief Save the op as a string, only for debug when printing AST.
        std::string opName_;

        /// @brief 一元运算孩子节点
        AstNode    *m_expression;

        /// @brief True, if "++i"; False if "i++".
        bool     m_isFrontOp;

        static const char* sv_operator_name;
        static const char* sv_isfront_name;
    };

    /**
     * @brief `throwStatement` parser rule in grammar.
     * @details JavaScript Example: "throw a=1;"
     **/
    class Throw : public Expression
    {
        AST_DECLARE_REQUIRED_HEADER(Throw)
    public:
        Throw() : m_expression(NULL) {}

        virtual    size_t childrenCount(void) const{
            return 1;
        }
        virtual AstNode*    getChildren(size_t idx) const{
            switch(idx)
            {
            case 0:
                return m_expression;
            }
            return NULL;
        }

        /// @brief 虚函数，用于给孩子赋值。
        virtual void setChildren(size_t idx, AstNode * node)
        {
            switch(idx)
            {
            case 0:
                m_expression = node;
                break;
            default:
                break;
            }
        }

        inline AstNode*    expression(void)const {return m_expression;}
        inline     void    expression(AstNode* e){
            m_expression = e;
        }
    private:
        AstNode    *m_expression;
    };

    /**
     * @brief Declare a list of variables.
     * @details
     *   JavaScript: "a=1,b,c=2,d,e,f=3" in "var a=1,b,c=2,d,e,f=3"
     **/
    class VariableDeclarationList : public AstNode{
        AST_DECLARE_REQUIRED_HEADER(VariableDeclarationList)
        AST_CONTAINER_NODE_REQUIRED_HEADER
    public:
        VariableDeclarationList() {
        }
    };

} // namespace ioc

#endif //IOC_FRONTEND_AST_H
