// Copyright 2010 Google Inc, Shannon Weyrick <weyrick@mozek.us>

#include "Incompletes.h"

#include <llvm/LLVMContext.h>
#include "BTypeDef.h"
#include "BFuncDef.h"
#include "LLVMBuilder.h"
#include "Utils.h"

#include <map>

#include "llvm/GlobalValue.h"  // XXX for getting a module
#include "llvm/Module.h"  // XXX for getting a module

using namespace llvm;
using namespace model;
using namespace std;
using namespace builder::mvll;

// XXX defined in LLVMBuilder.cc
extern const Type * llvmIntType;

namespace {
    // utility
    Value *narrowToAncestor(IRBuilder<> &builder,
                            Value *receiver,
                            const TypeDef::AncestorPath &path
                            ) {
        for (TypeDef::AncestorPath::const_iterator iter = path.begin();
        iter != path.end();
        ++iter
                )
            receiver =
                    builder.CreateStructGEP(receiver, iter->index);

        return receiver;
    }
} // namespace

// IncompleteInstVarRef
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteInstVarRef, Value);

void * IncompleteInstVarRef::operator new(size_t s) {
    return User::operator new(s, 1);
}

IncompleteInstVarRef::IncompleteInstVarRef(const Type *type,
                                           Value *aggregate,
                                           BFieldDefImpl *fieldImpl,
                                           BasicBlock *parent
                                           ) :
    PlaceholderInstruction(
            type,
            parent,
            OperandTraits<IncompleteInstVarRef>::op_begin(this),
            OperandTraits<IncompleteInstVarRef>::operands(this)
            ),
    fieldImpl(fieldImpl) {
    Op<0>() = aggregate;
}

IncompleteInstVarRef::IncompleteInstVarRef(const Type *type,
                                           Value *aggregate,
                                           BFieldDefImpl *fieldImpl,
                                           Instruction *insertBefore
                                           ) :
    PlaceholderInstruction(
            type,
            insertBefore,
            OperandTraits<IncompleteInstVarRef>::op_begin(this),
            OperandTraits<IncompleteInstVarRef>::operands(this)
            ),
    fieldImpl(fieldImpl) {
    Op<0>() = aggregate;
}

Instruction * IncompleteInstVarRef::clone_impl() const {
    return new IncompleteInstVarRef(getType(), Op<0>(), fieldImpl.get());
}

void IncompleteInstVarRef::insertInstructions(IRBuilder<> &builder) {
    replaceAllUsesWith(fieldImpl->emitFieldRef(builder, getType(), Op<0>()));
}

// IncompleteInstVarAssign
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteInstVarAssign, Value);
 void * IncompleteInstVarAssign::operator new(size_t s) {
     return User::operator new(s, 2);
 }

 IncompleteInstVarAssign::IncompleteInstVarAssign(const Type *type,
                                                  Value *aggregate,
                                                  BFieldDefImpl *fieldDefImpl,
                                                  Value *rval,
                                                  BasicBlock *parent
                                                  ) :
     PlaceholderInstruction(
             type,
             parent,
             OperandTraits<IncompleteInstVarAssign>::op_begin(this),
             OperandTraits<IncompleteInstVarAssign>::operands(this)
             ),
     fieldDefImpl(fieldDefImpl) {
     Op<0>() = aggregate;
     Op<1>() = rval;
 }

 IncompleteInstVarAssign::IncompleteInstVarAssign(const Type *type,
                                                  Value *aggregate,
                                                  BFieldDefImpl *fieldDefImpl,
                                                  Value *rval,
                                                  Instruction *insertBefore
                                                  ) :
     PlaceholderInstruction(
             type,
             insertBefore,
             OperandTraits<IncompleteInstVarAssign>::op_begin(this),
             OperandTraits<IncompleteInstVarAssign>::operands(this)
             ),
     fieldDefImpl(fieldDefImpl) {
     Op<0>() = aggregate;
     Op<1>() = rval;
 }

Instruction *IncompleteInstVarAssign::clone_impl() const {
     return new IncompleteInstVarAssign(getType(), Op<0>(), fieldDefImpl.get(),
                                        Op<1>()
                                        );
}

void IncompleteInstVarAssign::insertInstructions(IRBuilder<> &builder) {
    fieldDefImpl->emitFieldAssign(builder, Op<0>(), Op<1>());
}

// IncompleteCatchSelector
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteCatchSelector, Value);
void *IncompleteCatchSelector::operator new(size_t s) {
    return User::operator new(s, 0);
}

IncompleteCatchSelector::IncompleteCatchSelector(Value *ehSelector,
                                                 Value *exception, 
                                                 Value *personalityFunc,
                                                 BasicBlock *parent
                                                 ) :
    PlaceholderInstruction(
        Type::getInt32Ty(getGlobalContext()),
        parent,
        OperandTraits<IncompleteCatchSelector>::op_begin(this),
        OperandTraits<IncompleteCatchSelector>::operands(this)
    ),
    ehSelector(ehSelector),
    exception(exception),
    personalityFunc(personalityFunc),
    typeImpls(0) {
}

IncompleteCatchSelector::IncompleteCatchSelector(Value *ehSelector,
                                                 Value *exception,
                                                 Value *personalityFunc,
                                                 Instruction *insertBefore
                                                 ) :
    PlaceholderInstruction(
        Type::getInt32Ty(getGlobalContext()),
        insertBefore,
        OperandTraits<IncompleteCatchSelector>::op_begin(this),
        OperandTraits<IncompleteCatchSelector>::operands(this)
    ),
    ehSelector(ehSelector),
    exception(exception),
    personalityFunc(personalityFunc),
    typeImpls(0) {
}

IncompleteCatchSelector::~IncompleteCatchSelector() {
}

Instruction *IncompleteCatchSelector::clone_impl() const {
    return new IncompleteCatchSelector(ehSelector, exception, personalityFunc);
}

void IncompleteCatchSelector::insertInstructions(IRBuilder<> &builder) {
    vector<Value *> args(3 + typeImpls->size());
    args[0] = exception;
    args[1] = personalityFunc;
    int i;
    for (i = 0; i < typeImpls->size(); ++i)
        args[i + 2] = (*typeImpls)[i];
    args[i + 2] = Constant::getNullValue(builder.getInt8Ty()->getPointerTo());
    replaceAllUsesWith(
        builder.CreateCall(ehSelector, args.begin(), args.end())
    );
}    

// IncompleteNarrower
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteNarrower, Value);
void * IncompleteNarrower::operator new(size_t s) {
    return User::operator new(s, 1);
}

IncompleteNarrower::IncompleteNarrower(Value *aggregate,
                                       BTypeDef *startType,
                                       BTypeDef *ancestor,
                                       BasicBlock *parent
                                       ) :
    PlaceholderInstruction(
            ancestor->rep,
            parent,
            OperandTraits<IncompleteNarrower>::op_begin(this),
            OperandTraits<IncompleteNarrower>::operands(this)
            ),
    startType(startType),
    ancestor(ancestor) {
    Op<0>() = aggregate;
}

IncompleteNarrower::IncompleteNarrower(Value *aggregate,
                                       BTypeDef *startType,
                                       BTypeDef *ancestor,
                                       Instruction *insertBefore
                                       ) :
    PlaceholderInstruction(
            ancestor->rep,
            insertBefore,
            OperandTraits<IncompleteNarrower>::op_begin(this),
            OperandTraits<IncompleteNarrower>::operands(this)
            ),
    startType(startType),
    ancestor(ancestor) {
    Op<0>() = aggregate;
}

Instruction * IncompleteNarrower::clone_impl() const {
    return new IncompleteNarrower(Op<0>(), startType, ancestor);
}

Value * IncompleteNarrower::emitGEP(IRBuilder<> &builder,
    BTypeDef *type,
    BTypeDef *ancestor,
    Value *inst
    ) {
    if (type == ancestor)
        return inst;

    int i = 0;
    for (TypeDef::TypeVec::iterator iter = type->parents.begin();
    iter != type->parents.end();
    ++iter, ++i
            )
        if ((*iter)->isDerivedFrom(ancestor)) {
        inst = builder.CreateStructGEP(inst, i);
        BTypeDef *base =
                BTypeDefPtr::arcast(*iter);
        return emitGEP(builder, base, ancestor, inst);
    }
    assert(false && "narrowing to non-ancestor!");
}

void IncompleteNarrower::insertInstructions(IRBuilder<> &builder) {
    replaceAllUsesWith(emitGEP(builder, startType, ancestor,
                               Op<0>()
                               )
                       );
}

// IncompleteVTableInit
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteVTableInit, Value);
void * IncompleteVTableInit::operator new(size_t s) {
    return User::operator new(s, 1);
}

IncompleteVTableInit::IncompleteVTableInit(BTypeDef *aggregateType,
                                           Value *aggregate,
                                           BTypeDef *vtableBaseType,
                                           BasicBlock *parent
                                           ) :
    PlaceholderInstruction(
            aggregateType->rep,
            parent,
            OperandTraits<IncompleteVTableInit>::op_begin(this),
            OperandTraits<IncompleteVTableInit>::operands(this)
            ),
    aggregateType(aggregateType),
    vtableBaseType(vtableBaseType) {
    Op<0>() = aggregate;
}

IncompleteVTableInit::IncompleteVTableInit(BTypeDef *aggregateType, Value *aggregate,
                                           BTypeDef *vtableBaseType,
                                           Instruction *insertBefore
                                           ) :
    PlaceholderInstruction(
            aggregateType->rep,
            insertBefore,
            OperandTraits<IncompleteVTableInit>::op_begin(this),
            OperandTraits<IncompleteVTableInit>::operands(this)
            ),
    aggregateType(aggregateType),
    vtableBaseType(vtableBaseType) {
    Op<0>() = aggregate;
}

Instruction * IncompleteVTableInit::clone_impl() const {
    return new IncompleteVTableInit(aggregateType, Op<0>(),
                                    vtableBaseType
                                    );
}

void IncompleteVTableInit::emitInitOfFirstVTable(IRBuilder<> &builder,
                                                 BTypeDef *btype,
                                                 Value *inst,
                                                 Constant *vtable
                                                 ) {

    TypeDef::TypeVec &parents = btype->parents;
    int i = 0;
    for (TypeDef::TypeVec::iterator ctxIter =
         parents.begin();
    ctxIter != parents.end();
    ++ctxIter, ++i
            ) {
        BTypeDef *base = BTypeDefPtr::arcast(*ctxIter);
        if (base == vtableBaseType) {
            inst = builder.CreateStructGEP(inst, i);

            // convert the vtable to {}*
            const PointerType *emptyStructPtrType =
                    cast<PointerType>(vtableBaseType->rep);
            const Type *emptyStructType =
                    emptyStructPtrType->getElementType();
            Value *castVTable =
                    builder.CreateBitCast(vtable, emptyStructType);

            // store the vtable pointer in the field.
            builder.CreateStore(castVTable, inst);
            return;
        }
    }

    assert(false && "no vtable base class");
}

// emit the code to initialize all vtables in an object.
void IncompleteVTableInit::emitVTableInit(IRBuilder<> &builder, BTypeDef *btype,
                                          Value *inst
                                          ) {

    // if btype has a registered vtable, startClass gets
    // incremented so that we don't emit a parent vtable that
    // overwrites it.
    int startClass = 0;

    // check for the vtable of the current class
    map<BTypeDef *, Constant *>::iterator firstVTableIter =
            aggregateType->vtables.find(btype);
    if (firstVTableIter != aggregateType->vtables.end()) {
        emitInitOfFirstVTable(builder, btype, inst,
                              firstVTableIter->second
                              );
        startClass = 1;
    }

    // recurse through all other parents with vtables
    TypeDef::TypeVec &parents = btype->parents;
    int i = 0;
    for (TypeDef::TypeVec::iterator ctxIter =
         parents.begin() + startClass;
    ctxIter != parents.end();
    ++ctxIter, ++i
            ) {
        BTypeDef *base =
                BTypeDefPtr::arcast(*ctxIter);

        // see if this class has a vtable in the aggregate type
        map<BTypeDef *, Constant *>::iterator vtableIter =
                aggregateType->vtables.find(base);
        if (vtableIter != aggregateType->vtables.end()) {
            Value *baseInst =
                    builder.CreateStructGEP(inst, i);
            emitInitOfFirstVTable(builder, base, baseInst,
                                  vtableIter->second
                                  );
        } else if (base->hasVTable) {
            Value *baseInst =
                    builder.CreateStructGEP(inst, i);
            emitVTableInit(builder, base, baseInst);
        }
    }
}

void IncompleteVTableInit::insertInstructions(IRBuilder<> &builder) {
    emitVTableInit(builder, aggregateType, Op<0>());
}

// IncompleteVirtualFunc
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteVirtualFunc, Value);
Value * IncompleteVirtualFunc::getVTableReference(IRBuilder<> &builder,
                                                  BTypeDef *vtableBaseType,
                                                  const Type *finalVTableType,
                                                  BTypeDef *curType,
                                                  Value *inst
                                                  ) {

    // (the logic here looks painfully like that of
    // emitVTableInit(), but IMO converting this to an internal
    // iterator would just make the code harder to grok)

    if (curType == vtableBaseType) {

        // XXX this is fucked

        // convert the instance pointer to the address of a
        // vtable pointer.
        return builder.CreateBitCast(inst, finalVTableType);

    } else {
        // recurse through all parents with vtables
        TypeDef::TypeVec &parents = curType->parents;
        int i = 0;
        for (TypeDef::TypeVec::iterator baseIter = parents.begin();
        baseIter != parents.end();
        ++baseIter, ++i
                ) {
            BTypeDef *base = BTypeDefPtr::arcast(*baseIter);
            if (base->hasVTable) {
                Value *baseInst = builder.CreateStructGEP(inst, i);
                Value *vtable =
                    getVTableReference(builder, vtableBaseType,
                                       finalVTableType,
                                       base,
                                       baseInst
                                       );
                if (vtable)
                    return vtable;
            }

        }

        return 0;
    }
}

Value *IncompleteVirtualFunc::innerEmitCall(IRBuilder<> &builder,
                                            BTypeDef *vtableBaseType,
                                            BFuncDef *funcDef,
                                            Value *receiver,
                                            const vector<Value *> &args,
                                            BasicBlock *normalDest,
                                            BasicBlock *unwindDest
                                            ) {

    BTypeDef *receiverType =
            BTypeDefPtr::acast(funcDef->getReceiverType());
    assert(receiver->getType() == receiverType->rep);

    // get the underlying vtable
    Value *vtable =
            getVTableReference(builder, vtableBaseType,
                               receiverType->firstVTableType,
                               receiverType,
                               receiver
                               );
    assert(vtable && "virtual function receiver has no vtable");

    vtable = builder.CreateLoad(vtable);
    Value *funcFieldRef =
            builder.CreateStructGEP(vtable, funcDef->vtableSlot);
    Value *funcPtr = builder.CreateLoad(funcFieldRef);
    Value *result = builder.CreateInvoke(funcPtr, normalDest, unwindDest,
                                         args.begin(), 
                                         args.end()
                                         );
    return result;
}

void IncompleteVirtualFunc::init(Value *receiver, const vector<Value *> &args) {
    // fill in all of the operands
    assert(NumOperands == args.size() + 1);
    OperandList[0] = receiver;
    for (int i = 0; i < args.size(); ++i)
        OperandList[i + 1] = args[i];
}

IncompleteVirtualFunc::IncompleteVirtualFunc(
    BTypeDef *vtableBaseType,
    BFuncDef *funcDef,
    Value *receiver,
    const vector<Value *> &args,
    BasicBlock *parent,
    BasicBlock *normalDest,
    BasicBlock *unwindDest
) :
    PlaceholderInstruction(
            BTypeDefPtr::arcast(funcDef->returnType)->rep,
            parent,
            OperandTraits<IncompleteVirtualFunc>::op_end(this) -
            (args.size() + 1),
            args.size() + 1
            ),
    vtableBaseType(vtableBaseType),
    funcDef(funcDef),
    normalDest(normalDest),
    unwindDest(unwindDest) {
    init(receiver, args);
}

IncompleteVirtualFunc::IncompleteVirtualFunc(
    BTypeDef *vtableBaseType,
    BFuncDef *funcDef,
    Value *receiver,
    const vector<Value *> &args,
    BasicBlock *normalDest,
    BasicBlock *unwindDest,
    Instruction *insertBefore
) :
    PlaceholderInstruction(
            BTypeDefPtr::arcast(funcDef->returnType)->rep,
            insertBefore,
            OperandTraits<IncompleteVirtualFunc>::op_end(this) -
            (args.size() + 1),
            args.size() + 1
            ),
    vtableBaseType(vtableBaseType),
    funcDef(funcDef),
    normalDest(normalDest),
    unwindDest(unwindDest) {

    init(receiver, args);
}

IncompleteVirtualFunc::IncompleteVirtualFunc(
    BTypeDef *vtableBaseType,
    BFuncDef *funcDef,
    Use *operands,
    unsigned numOperands,
    BasicBlock *normalDest,
    BasicBlock *unwindDest
) :
    PlaceholderInstruction(
            BTypeDefPtr::arcast(funcDef->returnType)->rep,
            static_cast<Instruction *>(0),
            operands,
            numOperands
            ),
    vtableBaseType(vtableBaseType),
    funcDef(funcDef),
    normalDest(normalDest),
    unwindDest(unwindDest) {

    for (int i = 0; i < numOperands; ++i)
        OperandList[i] = operands[i];
}

Instruction * IncompleteVirtualFunc::clone_impl() const {
    return new(NumOperands) IncompleteVirtualFunc(vtableBaseType,
                                                  funcDef,
                                                  OperandList,
                                                  NumOperands,
                                                  normalDest,
                                                  unwindDest
                                                  );
}

void IncompleteVirtualFunc::insertInstructions(IRBuilder<> &builder) {
    vector<Value *> args(NumOperands - 1);
    for (int i = 1; i < NumOperands; ++i)
        args[i - 1] = OperandList[i];
    Value *callInst =
            innerEmitCall(builder, vtableBaseType, funcDef,
                          OperandList[0],
                          args,
                          normalDest,
                          unwindDest
                          );
    replaceAllUsesWith(callInst);
}

Value * IncompleteVirtualFunc::emitCall(Context &context,
                                        BFuncDef *funcDef,
                                        Value *receiver,
                                        const vector<Value *> &args,
                                        BasicBlock *normalDest,
                                        BasicBlock *unwindDest       
                                               ) {
    // do some conversions that we need to do either way.
    LLVMBuilder &llvmBuilder =
            dynamic_cast<LLVMBuilder &>(context.builder);
    BTypeDef *vtableBaseType =
            BTypeDefPtr::arcast(context.construct->vtableBaseType);
    BTypeDef *type = BTypeDefPtr::acast(funcDef->getOwner());

    // if this is for a complete class, go ahead and emit the code.
    // Otherwise just emit a placeholder.
    if (type->complete) {
        Value *val = innerEmitCall(llvmBuilder.builder,
                                   vtableBaseType,
                                   funcDef,
                                   receiver,
                                   args,
                                   normalDest,
                                   unwindDest
                                   );
        return val;
    } else {
        PlaceholderInstruction *placeholder =
                new(args.size() + 1) IncompleteVirtualFunc(
                        vtableBaseType,
                        funcDef,
                        receiver,
                        args,
                        llvmBuilder.builder.GetInsertBlock(),
                        normalDest,
                        unwindDest
                        );
        type->addPlaceholder(placeholder);
        return placeholder;
    }
}

// IncompleteSpecialize
DEFINE_TRANSPARENT_OPERAND_ACCESSORS(IncompleteSpecialize, Value);
void * IncompleteSpecialize::operator new(size_t s) {
    return User::operator new(s, 1);
}

Instruction * IncompleteSpecialize::clone_impl() const {
    return new IncompleteSpecialize(getType(), value,
                                    ancestorPath
                                    );
}

IncompleteSpecialize::IncompleteSpecialize
        (const Type *type,
         Value *value,
         const TypeDef::AncestorPath &ancestorPath,
         Instruction *insertBefore
                                     ) :
    PlaceholderInstruction(
            type,
            insertBefore,
            OperandTraits<IncompleteSpecialize>::op_begin(this),
            OperandTraits<IncompleteSpecialize>::operands(this)
            ),
    value(value),
    ancestorPath(ancestorPath) {
    Op<0>() = value;
}

IncompleteSpecialize::IncompleteSpecialize
        (const Type *type,
         Value *value,
         const TypeDef::AncestorPath &ancestorPath,
         BasicBlock *parent
         ) :
    PlaceholderInstruction(
            type,
            parent,
            OperandTraits<IncompleteSpecialize>::op_begin(this),
            OperandTraits<IncompleteSpecialize>::operands(this)
            ),
    value(value),
    ancestorPath(ancestorPath) {
    Op<0>() = value;
}

Value * IncompleteSpecialize::emitSpecializeInner(
        IRBuilder<> &builder,
        const Type *type,
        Value *value,
        const TypeDef::AncestorPath &ancestorPath
        ) {
    // XXX won't work for virtual base classes

    // create a constant offset from the start of the derived
    // class to the start of the base class
    Value *offset =
            narrowToAncestor(builder,
                             Constant::getNullValue(type),
                             ancestorPath
                             );

    // convert to an integer and subtract from the pointer to the
    // base class.
    assert(llvmIntType && "integer type has not been initialized");
    offset = builder.CreatePtrToInt(offset, llvmIntType);
    value = builder.CreatePtrToInt(value, llvmIntType);
    Value *derived = builder.CreateSub(value, offset);
    Value *specialized =
            builder.CreateIntToPtr(derived, type);
    return specialized;
}

void IncompleteSpecialize::insertInstructions(IRBuilder<> &builder) {
    replaceAllUsesWith(emitSpecializeInner(builder,
                                           getType(),
                                           value,
                                           ancestorPath
                                           )
                       );
}

// Utility function - emits the specialize instructions if the
// target class is defined, emits a placeholder instruction if it
// is not.
Value *IncompleteSpecialize::emitSpecialize(
    Context &context,
    BTypeDef *type,
    Value *value,
    const TypeDef::AncestorPath &ancestorPath
) {
    LLVMBuilder &llvmBuilder =
        dynamic_cast<LLVMBuilder &>(context.builder);

    if (type->complete) {
        return emitSpecializeInner(llvmBuilder.builder, type->rep,
                                   value,
                                   ancestorPath
                                   );
    } else {
        PlaceholderInstruction *placeholder =
            new IncompleteSpecialize(type->rep, value,
                                     ancestorPath,
                                     llvmBuilder.builder.GetInsertBlock()
                                     );
        type->addPlaceholder(placeholder);
        return placeholder;
    }

}
