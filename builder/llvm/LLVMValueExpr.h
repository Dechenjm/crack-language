// Copyright 2011 Google Inc

#ifndef _builder_llvm_LLVMValueExpr_h_
#define _builder_llvm_LLVMValueExpr_h_

#include "model/Expr.h"

namespace llvm {
    class Value;
}

namespace builder { namespace mvll {

/**
 * Expression class for wrapping LLVMValues.
 */
class LLVMValueExpr : public model::Expr {
    private:
        llvm::Value *value;
    
    public:
        LLVMValueExpr(model::TypeDef *type, llvm::Value *value)  :
            Expr(type),
            value(value) {
        }

        virtual model::ResultExprPtr emit(model::Context &context);
        virtual void writeTo(std::ostream &out) const;
        
        /** Override isProductive(), LLVM values are not. */
        virtual bool isProductive() const;
};
    
}} // namespace builder::llvm

#endif
