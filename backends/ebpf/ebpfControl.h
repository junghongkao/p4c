/*
Copyright 2013-present Barefoot Networks, Inc.

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

#ifndef _BACKENDS_EBPF_EBPFCONTROL_H_
#define _BACKENDS_EBPF_EBPFCONTROL_H_

#include "ebpfObject.h"
#include "ebpfTable.h"

namespace EBPF {

class EBPFControl;

class ControlBodyTranslator : public CodeGenInspector {
    const EBPFControl* control;
    std::set<const IR::Parameter*> toDereference;
    std::vector<cstring> saveAction;
    P4::P4CoreLibrary& p4lib;
    std::map<const IR::Parameter*, const IR::Parameter*> substitution;
 public:
    ControlBodyTranslator(const EBPFControl* control, CodeBuilder* builder);
    void substitute(const IR::Parameter* p, const IR::Parameter* with);

    // handle the packet_out.emit method
    void compileEmitField(const IR::Expression* expr, cstring field,
                          unsigned alignment, EBPFType* type);
    void compileEmit(const IR::Vector<IR::Expression>* args);
    void processMethod(const P4::ExternMethod* method);
    void processApply(const P4::ApplyMethod* method);

    bool preorder(const IR::PathExpression* expression) override;
    bool preorder(const IR::MethodCallExpression* expression) override;
    bool preorder(const IR::ExitStatement*) override;
    bool preorder(const IR::ReturnStatement*) override;
    bool preorder(const IR::IfStatement* statement) override;
    bool preorder(const IR::SwitchStatement* statement) override;
};

class EBPFControl : public EBPFObject {
 public:
    const EBPFProgram*      program;
    const IR::ControlBlock* controlBlock;
    const IR::Parameter*    headers;
    const IR::Parameter*    accept;
    const IR::Parameter*    parserHeaders;
    // replace references to headers with references to parserHeaders
    cstring                 hitVariable;

    std::set<const IR::Parameter*> toDereference;
    std::map<cstring, EBPFTable*>  tables;
    std::map<cstring, EBPFCounterTable*>  counters;

    EBPFControl(const EBPFProgram* program, const IR::ControlBlock* block,
                const IR::Parameter* parserHeaders);
    virtual void emit(CodeBuilder* builder);
    void emitDeclaration(const IR::Declaration* decl, CodeBuilder *builder);
    void emitTables(CodeBuilder* builder);
    virtual bool build();
    EBPFTable* getTable(cstring name) const {
        auto result = get(tables, name);
        BUG_CHECK(result != nullptr, "No table named %1%", name);
        return result; }
    EBPFCounterTable* getCounter(cstring name) const {
        auto result = get(counters, name);
        BUG_CHECK(result != nullptr, "No counter named %1%", name);
        return result; }

 protected:
    void scanConstants();
};

}  // namespace EBPF

#endif /* _BACKENDS_EBPF_EBPFCONTROL_H_ */
