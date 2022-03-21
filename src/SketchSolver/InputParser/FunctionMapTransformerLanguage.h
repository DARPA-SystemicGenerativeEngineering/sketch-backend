//
// Created by kliment on 3/20/22.
//

#ifndef SKETCH_SOURCE_FUNCTIONMAPTRANSFORMERLANGUAGE_H
#define SKETCH_SOURCE_FUNCTIONMAPTRANSFORMERLANGUAGE_H


#include "SolverLanguageLexAndYaccHeader.h"
#include "FunctionMap.h"

namespace FMTL{

    class FunctionMapTransformerState
    {
        SL::CodeBlock* root = nullptr;
    public:
        FunctionMap& function_map;
        explicit FunctionMapTransformerState(FunctionMap& _function_map): function_map(_function_map) {};

        void add_root(SL::CodeBlock* code_block) {
            root = code_block;
        }

        FunctionMap *eval() {
            AssertDebug(false, "TODO: implement state-polymorphic implementation of the AST.")
//            root->run(this);
        }
    };

    void parse_function_map_transformer_program(FunctionMapTransformerState* _state, string solver_program_file);
}

typedef void* yyscan_t;
void yyerror(yyscan_t scanner, FMTL::FunctionMapTransformerState* state, string s);



#endif //SKETCH_SOURCE_FUNCTIONMAPTRANSFORMERLANGUAGE_H
