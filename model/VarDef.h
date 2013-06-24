// Copyright 2009-2012 Google Inc.
// Copyright 2010-2011 Shannon Weyrick <weyrick@mozek.us>
// 
//   This Source Code Form is subject to the terms of the Mozilla Public
//   License, v. 2.0. If a copy of the MPL was not distributed with this
//   file, You can obtain one at http://mozilla.org/MPL/2.0/.
// 

#ifndef _model_VarDef_h_
#define _model_VarDef_h_

#include <set>
#include <vector>
#include "model/ResultExpr.h"
#include <spug/RCBase.h>
#include <spug/RCPtr.h>

namespace model {

class Context;
class Deserializer;
SPUG_RCPTR(Expr);
class Namespace;
class ModuleDef;
class ModuleDefMap;
SPUG_RCPTR(OverloadDef);
class Serializer;
SPUG_RCPTR(TypeDef);
SPUG_RCPTR(VarDefImpl);

SPUG_RCPTR(VarDef);

// Variable definition.  All names in a context (including functions and 
// types) are derived from VarDef's.
class VarDef : public virtual spug::RCBase {

    protected:
        Namespace *owner;
        mutable std::string fullName; // a cache, built in getFullName

        // Does all the work of serializing an extern.  parameterized types 
        // should pass their parameters in through 'params', other defs may 
        // set this to null.
        void serializeExternCommon(Serializer &serializer, 
                                   const std::vector<TypeDefPtr> *localDeps
                                   ) const;

        void serializeExternRef(Serializer &serializer, 
                                const std::vector<TypeDefPtr> *localDeps
                                ) const;

    public:
        
        TypeDefPtr type;
        std::string name;
        VarDefImplPtr impl;
        bool constant;
        
        // this flag is true if we've run replaceAllStubs() on this object and 
        // it is guaranteed to not reference any stubs.
        bool stubFree;

        VarDef(TypeDef *type, const std::string &name);
        virtual ~VarDef();
        
        ResultExprPtr emitAssignment(Context &context, Expr *expr);
        
        /**
         * Returns true if the definition type requires a slot in the instance 
         * variable.
         */
        virtual bool hasInstSlot();
        
        /**
         * Returns the instance variable slot of the variable (if any).  -1 if 
         * there is none.
         */
        virtual int getInstSlot() const;
        
        /**
         * Returns true if the definition is class static.
         */
        virtual bool isStatic() const;

        /**
         * Returns the fully qualified name of the definition.
         */        
        std::string getFullName() const;
        
        /**
         * Returns the display name of the definition.  This is the name to be 
         * displayed in error messages.  It is the same as the result of 
         * getFullName() except for builtins and symbols in the main module.
         */
        virtual std::string getDisplayName() const;
        
        /**
         * Set namespace owner
         */
        virtual void setOwner(Namespace *o) {
            owner = o;
            fullName.clear(); // must recache since owner changed
        }

        Namespace *getOwner() const { return owner; }
        
        /**
         * Return true if the variable is unassignable.
         */
        virtual bool isConstant();
        
        /**
         * Returns true if the variable can not be made visible to other 
         * modules.
         */
        bool isHidden() const;

        virtual
        void dump(std::ostream &out, const std::string &prefix = "") const;
        
        /**
         * Allow dumping from the debugger.
         */
        void dump() const;
        
        /**
         * Returns the module that owns this definition.
         */
        ModuleDef *getModule() const;
        
        /**
         * Returns true if the VarDef can be imported from the module.
         */
        virtual bool isImportableFrom(ModuleDef *module) const;
        
        /**
         * Returns true if the definition should be serialized when the owner
         * is being serialized.
         */
        virtual bool isSerializable(const Namespace *ns, 
                                    const std::string &name
                                    ) const;

        /** Keeps track of a set of externally managed VarDefs. */
        typedef std::set<const VarDef *> Set;
        
        /**
         * Add all of the modules that this definition (and all types it 
         * introduces) to the dependencies of the module.
         * @param added the set of TypeDef's that have already been added.
         *              We only have to keep track of types, these are the 
         *              only things that can be cyclic.
         */
        virtual void addDependenciesTo(ModuleDef *mod, Set &added) const;

        /**
         * Serialize an external definition. "Extern"
         */
        virtual void serializeExtern(Serializer &serializer) const;

        /**
         * Serialize the definition as an alias. "AliasDef"
         */
        void serializeAlias(Serializer &serializer, 
                            const std::string &alias
                            ) const;

        /**
         * Serialize the variable definition.
         * If 'writeKind' is true, write the kind of definition in front of 
         * the definition itself.  This is false when we can determine the 
         * kind of definiton from the context, as when we serialize a variable 
         * type.
         */
        virtual void serialize(Serializer &serializer, bool writeKind,
                               const Namespace *ns
                               ) const;

        /**
         * Deserialize the body of an alias, producing a TypeDef, OverloadDef 
         * or VarDef accordingly.
         */
        /** @{ */
        static TypeDefPtr deserializeTypeAliasBody(Deserializer &deser);
        static OverloadDefPtr deserializeOverloadAliasBody(Deserializer &deser);
        static VarDefPtr deserializeVarAliasBody(Deserializer &deser);
        /** @} */
        
        /**
         * Deserialize an alias reference.
         */
        static VarDefPtr deserializeAlias(Deserializer &serializer);
        
        /**
         * Deserialize a VarDef.
         */
        static VarDefPtr deserialize(Deserializer &deser);
        
        /**
         * Called on a module definition when a module is completely 
         * deserialized.  The module may choose to call this recursively on 
         * all definitions that it contains.
         */
        virtual void onDeserialized(Context &context) {}
        
        /**
         * Returns true if the definition is a stub.
         */
        virtual bool isStub() const { return false; }
        
        /**
         * Replace a stub object created during deserialization with the 
         * actual definition.  Returns the replacement if there is one, null 
         * if not.
         */
        virtual VarDefPtr replaceStub(Context &context) { return this; }
        
        /**
         * Recursively replace all stubs referenced by the definition.  
         * Returns the replaceStub() or this var (not null like replaceStub()).
         */
        virtual VarDefPtr replaceAllStubs(Context &context);
};

inline std::ostream &operator <<(std::ostream &out, const VarDef &def) {
    def.dump(out);
    return out;
}

} // namespace model

#endif
