/*
Copyright 2016 VMware, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _MIDEND_ELIMINATETUPLES_H_
#define _MIDEND_ELIMINATETUPLES_H_

#include "ir/ir.h"
#include "frontends/p4/typeChecking/typeChecker.h"

namespace P4 {

class ReplacementMap {
    ordered_map<const IR::Type*, const IR::Type_Struct*> replacement;
    std::set<const IR::Type_Struct*> inserted;
    const IR::Type* convertType(const IR::Type* type);
 public:
    P4::NameGenerator* ng;
    P4::TypeMap* typeMap;

    ReplacementMap(NameGenerator* ng, TypeMap* typeMap) : ng(ng), typeMap(typeMap)
    { CHECK_NULL(ng); CHECK_NULL(typeMap); }
    const IR::Type_Struct* getReplacement(const IR::Type_Tuple* tt);
    IR::IndexedVector<IR::Node>* getNewReplacements();
};

class DoReplaceTuples final : public Transform {
    ReplacementMap* repl;
 public:
    explicit DoReplaceTuples(ReplacementMap* replMap) : repl(replMap)
    { CHECK_NULL(repl); setName("DoReplaceTuples"); }
    const IR::Node* postorder(IR::Type_Tuple* type) override;
    const IR::Node* insertReplacements(const IR::Node* before);
    const IR::Node* postorder(IR::Type_Struct* type) override
    { return insertReplacements(type); }
    const IR::Node* postorder(IR::P4Parser* parser) override
    { return insertReplacements(parser); }
    const IR::Node* postorder(IR::P4Control* control) override
    { return insertReplacements(control); }
};

class EliminateTuples final : public PassManager {
 public:
    EliminateTuples(ReferenceMap* refMap, TypeMap* typeMap) {
        auto repl = new ReplacementMap(refMap, typeMap);
        passes.push_back(new TypeChecking(refMap, typeMap));
        passes.push_back(new DoReplaceTuples(repl));
        passes.push_back(new ClearTypeMap(typeMap));
        setName("EliminateTuples");
    }
};

}  // namespace P4

#endif /* _MIDEND_ELIMINATETUPLES_H_ */
