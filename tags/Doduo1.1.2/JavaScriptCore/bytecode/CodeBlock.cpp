/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Cameron Zwarich <cwzwarich@uwaterloo.ca>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "CodeBlock.h"

#include "JIT.h"
#include "JSValue.h"
#include "Interpreter.h"
#include "Debugger.h"
#include <stdio.h>
#include <wtf/StringExtras.h>

namespace JSC {

#if !defined(NDEBUG) || ENABLE(OPCODE_SAMPLING)

static UString escapeQuotes(const UString& str)
{
    UString result = str;
    int pos = 0;
    while ((pos = result.find('\"', pos)) >= 0) {
        result = result.substr(0, pos) + "\"\\\"\"" + result.substr(pos + 1);
        pos += 4;
    }
    return result;
}

static UString valueToSourceString(ExecState* exec, JSValue* val)
{
    if (val->isString()) {
        UString result("\"");
        result += escapeQuotes(val->toString(exec)) + "\"";
        return result;
    } 

    return val->toString(exec);
}

static CString registerName(int r)
{
    if (r == missingThisObjectMarker())
        return "<null>";

    return (UString("r") + UString::from(r)).UTF8String();
}

static CString constantName(ExecState* exec, int k, JSValue* value)
{
    return (valueToSourceString(exec, value) + "(@k" + UString::from(k) + ")").UTF8String();
}

static CString idName(int id0, const Identifier& ident)
{
    return (ident.ustring() + "(@id" + UString::from(id0) +")").UTF8String();
}

static UString regexpToSourceString(RegExp* regExp)
{
    UString pattern = UString("/") + regExp->pattern() + "/";
    if (regExp->global())
        pattern += "g";
    if (regExp->ignoreCase())
        pattern += "i";
    if (regExp->multiline())
        pattern += "m";

    return pattern;
}

static CString regexpName(int re, RegExp* regexp)
{
    return (regexpToSourceString(regexp) + "(@re" + UString::from(re) + ")").UTF8String();
}

static UString pointerToSourceString(void* p)
{
    char buffer[2 + 2 * sizeof(void*) + 1]; // 0x [two characters per byte] \0
    snprintf(buffer, sizeof(buffer), "%p", p);
    return buffer;
}

NEVER_INLINE static const char* debugHookName(int debugHookID)
{
    switch (static_cast<DebugHookID>(debugHookID)) {
        case DidEnterCallFrame:
            return "didEnterCallFrame";
        case WillLeaveCallFrame:
            return "willLeaveCallFrame";
        case WillExecuteStatement:
            return "willExecuteStatement";
        case WillExecuteProgram:
            return "willExecuteProgram";
        case DidExecuteProgram:
            return "didExecuteProgram";
        case DidReachBreakpoint:
            return "didReachBreakpoint";
    }

    ASSERT_NOT_REACHED();
    return "";
}

static int locationForOffset(const Vector<Instruction>::const_iterator& begin, Vector<Instruction>::const_iterator& it, int offset)
{
    return it - begin + offset;
}

static void printUnaryOp(int location, Vector<Instruction>::const_iterator& it, const char* op)
{
    int r0 = (++it)->u.operand;
    int r1 = (++it)->u.operand;

    printf("[%4d] %s\t\t %s, %s\n", location, op, registerName(r0).c_str(), registerName(r1).c_str());
}

static void printBinaryOp(int location, Vector<Instruction>::const_iterator& it, const char* op)
{
    int r0 = (++it)->u.operand;
    int r1 = (++it)->u.operand;
    int r2 = (++it)->u.operand;
    printf("[%4d] %s\t\t %s, %s, %s\n", location, op, registerName(r0).c_str(), registerName(r1).c_str(), registerName(r2).c_str());
}

static void printConditionalJump(const Vector<Instruction>::const_iterator& begin, Vector<Instruction>::const_iterator& it, int location, const char* op)
{
    int r0 = (++it)->u.operand;
    int offset = (++it)->u.operand;
    printf("[%4d] %s\t\t %s, %d(->%d)\n", location, op, registerName(r0).c_str(), offset, locationForOffset(begin, it, offset));
}

static void printGetByIdOp(int location, Vector<Instruction>::const_iterator& it, const Vector<Identifier>& m_identifiers, const char* op)
{
    int r0 = (++it)->u.operand;
    int r1 = (++it)->u.operand;
    int id0 = (++it)->u.operand;
    printf("[%4d] %s\t %s, %s, %s\n", location, op, registerName(r0).c_str(), registerName(r1).c_str(), idName(id0, m_identifiers[id0]).c_str());
    it += 4;
}

static void printPutByIdOp(int location, Vector<Instruction>::const_iterator& it, const Vector<Identifier>& m_identifiers, const char* op)
{
    int r0 = (++it)->u.operand;
    int id0 = (++it)->u.operand;
    int r1 = (++it)->u.operand;
    printf("[%4d] %s\t %s, %s, %s\n", location, op, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str(), registerName(r1).c_str());
    it += 4;
}

void CodeBlock::printStructure(const char* name, const Instruction* vPC, int operand) const
{
    unsigned instructionOffset = vPC - m_instructions.begin();
    printf("  [%4d] %s: %s\n", instructionOffset, name, pointerToSourceString(vPC[operand].u.structure).UTF8String().c_str());
}

void CodeBlock::printStructures(const Instruction* vPC) const
{
    Interpreter* interpreter = m_globalData->interpreter;
    unsigned instructionOffset = vPC - m_instructions.begin();

    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id)) {
        printStructure("get_by_id", vPC, 4);
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_self)) {
        printStructure("get_by_id_self", vPC, 4);
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_proto)) {
        printf("  [%4d] %s: %s, %s\n", instructionOffset, "get_by_id_proto", pointerToSourceString(vPC[4].u.structure).UTF8String().c_str(), pointerToSourceString(vPC[5].u.structure).UTF8String().c_str());
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_transition)) {
        printf("  [%4d] %s: %s, %s, %s\n", instructionOffset, "put_by_id_new", pointerToSourceString(vPC[4].u.structure).UTF8String().c_str(), pointerToSourceString(vPC[5].u.structure).UTF8String().c_str(), pointerToSourceString(vPC[6].u.structureChain).UTF8String().c_str());
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_chain)) {
        printf("  [%4d] %s: %s, %s\n", instructionOffset, "get_by_id_chain", pointerToSourceString(vPC[4].u.structure).UTF8String().c_str(), pointerToSourceString(vPC[5].u.structureChain).UTF8String().c_str());
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id)) {
        printStructure("put_by_id", vPC, 4);
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_replace)) {
        printStructure("put_by_id_replace", vPC, 4);
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_resolve_global)) {
        printStructure("resolve_global", vPC, 4);
        return;
    }

    // These m_instructions doesn't ref Structures.
    ASSERT(vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_generic) || vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_generic) || vPC[0].u.opcode == interpreter->getOpcode(op_call) || vPC[0].u.opcode == interpreter->getOpcode(op_call_eval) || vPC[0].u.opcode == interpreter->getOpcode(op_construct));
}

void CodeBlock::dump(ExecState* exec) const
{
    Vector<Instruction>::const_iterator begin = m_instructions.begin();
    Vector<Instruction>::const_iterator end = m_instructions.end();

    size_t instructionCount = 0;
    for (Vector<Instruction>::const_iterator it = begin; it != end; ++it)
        if (exec->interpreter()->isOpcode(it->u.opcode))
            ++instructionCount;

    printf("%lu m_instructions; %lu bytes at %p; %d parameter(s); %d callee register(s)\n\n",
        static_cast<unsigned long>(instructionCount),
        static_cast<unsigned long>(m_instructions.size() * sizeof(Instruction)),
        this, m_numParameters, m_numCalleeRegisters);
    
    for (Vector<Instruction>::const_iterator it = begin; it != end; ++it)
        dump(exec, begin, it);

    if (!m_identifiers.isEmpty()) {
        printf("\nIdentifiers:\n");
        size_t i = 0;
        do {
            printf("  id%u = %s\n", static_cast<unsigned>(i), m_identifiers[i].ascii());
            ++i;
        } while (i != m_identifiers.size());
    }

    if (!m_constantRegisters.isEmpty()) {
        printf("\nConstants:\n");
        unsigned registerIndex = m_numVars;
        size_t i = 0;
        do {
            printf("   r%u = %s\n", registerIndex, valueToSourceString(exec, m_constantRegisters[i].jsValue(exec)).ascii());
            ++i;
            ++registerIndex;
        } while (i < m_constantRegisters.size());
    }

    if (!m_unexpectedConstants.isEmpty()) {
        printf("\nUnexpected Constants:\n");
        size_t i = 0;
        do {
            printf("  k%u = %s\n", static_cast<unsigned>(i), valueToSourceString(exec, m_unexpectedConstants[i]).ascii());
            ++i;
        } while (i < m_unexpectedConstants.size());
    }
    
    if (!m_regexps.isEmpty()) {
        printf("\nm_regexps:\n");
        size_t i = 0;
        do {
            printf("  re%u = %s\n", static_cast<unsigned>(i), regexpToSourceString(m_regexps[i].get()).ascii());
            ++i;
        } while (i < m_regexps.size());
    }

    if (!m_globalResolveInstructions.isEmpty() || !m_propertyAccessInstructions.isEmpty())
        printf("\nStructures:\n");

    if (!m_globalResolveInstructions.isEmpty()) {
        size_t i = 0;
        do {
             printStructures(&m_instructions[m_globalResolveInstructions[i]]);
             ++i;
        } while (i < m_globalResolveInstructions.size());
    }
    if (!m_propertyAccessInstructions.isEmpty()) {
        size_t i = 0;
        do {
             printStructures(&m_instructions[m_propertyAccessInstructions[i].bytecodeIndex]);
             ++i;
        } while (i < m_propertyAccessInstructions.size());
    }
 
    if (!m_exceptionHandlers.isEmpty()) {
        printf("\nException Handlers:\n");
        unsigned i = 0;
        do {
            printf("\t %d: { start: [%4d] end: [%4d] target: [%4d] }\n", i + 1, m_exceptionHandlers[i].start, m_exceptionHandlers[i].end, m_exceptionHandlers[i].target);
            ++i;
        } while (i < m_exceptionHandlers.size());
    }
    
    if (!m_immediateSwitchJumpTables.isEmpty()) {
        printf("Immediate Switch Jump Tables:\n");
        unsigned i = 0;
        do {
            printf("  %1d = {\n", i);
            int entry = 0;
            Vector<int32_t>::const_iterator end = m_immediateSwitchJumpTables[i].branchOffsets.end();
            for (Vector<int32_t>::const_iterator iter = m_immediateSwitchJumpTables[i].branchOffsets.begin(); iter != end; ++iter, ++entry) {
                if (!*iter)
                    continue;
                printf("\t\t%4d => %04d\n", entry + m_immediateSwitchJumpTables[i].min, *iter);
            }
            printf("      }\n");
            ++i;
        } while (i < m_immediateSwitchJumpTables.size());
    }
    
    if (!m_characterSwitchJumpTables.isEmpty()) {
        printf("\nCharacter Switch Jump Tables:\n");
        unsigned i = 0;
        do {
            printf("  %1d = {\n", i);
            int entry = 0;
            Vector<int32_t>::const_iterator end = m_characterSwitchJumpTables[i].branchOffsets.end();
            for (Vector<int32_t>::const_iterator iter = m_characterSwitchJumpTables[i].branchOffsets.begin(); iter != end; ++iter, ++entry) {
                if (!*iter)
                    continue;
                ASSERT(!((i + m_characterSwitchJumpTables[i].min) & ~0xFFFF));
                UChar ch = static_cast<UChar>(entry + m_characterSwitchJumpTables[i].min);
                printf("\t\t\"%s\" => %04d\n", UString(&ch, 1).ascii(), *iter);
        }
            printf("      }\n");
            ++i;
        } while (i < m_characterSwitchJumpTables.size());
    }
    
    if (!m_stringSwitchJumpTables.isEmpty()) {
        printf("\nString Switch Jump Tables:\n");
        unsigned i = 0;
        do {
            printf("  %1d = {\n", i);
            StringJumpTable::StringOffsetTable::const_iterator end = m_stringSwitchJumpTables[i].offsetTable.end();
            for (StringJumpTable::StringOffsetTable::const_iterator iter = m_stringSwitchJumpTables[i].offsetTable.begin(); iter != end; ++iter)
                printf("\t\t\"%s\" => %04d\n", UString(iter->first).ascii(), iter->second.branchOffset);
            printf("      }\n");
            ++i;
        } while (i < m_stringSwitchJumpTables.size());
    }

    printf("\n");
}

void CodeBlock::dump(ExecState* exec, const Vector<Instruction>::const_iterator& begin, Vector<Instruction>::const_iterator& it) const
{
    int location = it - begin;
    switch (exec->interpreter()->getOpcodeID(it->u.opcode)) {
        case op_enter: {
            printf("[%4d] enter\n", location);
            break;
        }
        case op_enter_with_activation: {
            int r0 = (++it)->u.operand;
            printf("[%4d] enter_with_activation %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_create_arguments: {
            printf("[%4d] create_arguments\n", location);
            break;
        }
        case op_convert_this: {
            int r0 = (++it)->u.operand;
            printf("[%4d] convert_this %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_unexpected_load: {
            int r0 = (++it)->u.operand;
            int k0 = (++it)->u.operand;
            printf("[%4d] unexpected_load\t %s, %s\n", location, registerName(r0).c_str(), constantName(exec, k0, m_unexpectedConstants[k0]).c_str());
            break;
        }
        case op_new_object: {
            int r0 = (++it)->u.operand;
            printf("[%4d] new_object\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_new_array: {
            int dst = (++it)->u.operand;
            int argv = (++it)->u.operand;
            int argc = (++it)->u.operand;
            printf("[%4d] new_array\t %s, %s, %d\n", location, registerName(dst).c_str(), registerName(argv).c_str(), argc);
            break;
        }
        case op_new_regexp: {
            int r0 = (++it)->u.operand;
            int re0 = (++it)->u.operand;
            printf("[%4d] new_regexp\t %s, %s\n", location, registerName(r0).c_str(), regexpName(re0, m_regexps[re0].get()).c_str());
            break;
        }
        case op_mov: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] mov\t\t %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str());
            break;
        }
        case op_not: {
            printUnaryOp(location, it, "not");
            break;
        }
        case op_eq: {
            printBinaryOp(location, it, "eq");
            break;
        }
        case op_eq_null: {
            printUnaryOp(location, it, "eq_null");
            break;
        }
        case op_neq: {
            printBinaryOp(location, it, "neq");
            break;
        }
        case op_neq_null: {
            printUnaryOp(location, it, "neq_null");
            break;
        }
        case op_stricteq: {
            printBinaryOp(location, it, "stricteq");
            break;
        }
        case op_nstricteq: {
            printBinaryOp(location, it, "nstricteq");
            break;
        }
        case op_less: {
            printBinaryOp(location, it, "less");
            break;
        }
        case op_lesseq: {
            printBinaryOp(location, it, "lesseq");
            break;
        }
        case op_pre_inc: {
            int r0 = (++it)->u.operand;
            printf("[%4d] pre_inc\t\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_pre_dec: {
            int r0 = (++it)->u.operand;
            printf("[%4d] pre_dec\t\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_post_inc: {
            printUnaryOp(location, it, "post_inc");
            break;
        }
        case op_post_dec: {
            printUnaryOp(location, it, "post_dec");
            break;
        }
        case op_to_jsnumber: {
            printUnaryOp(location, it, "to_jsnumber");
            break;
        }
        case op_negate: {
            printUnaryOp(location, it, "negate");
            break;
        }
        case op_add: {
            printBinaryOp(location, it, "add");
            ++it;
            break;
        }
        case op_mul: {
            printBinaryOp(location, it, "mul");
            ++it;
            break;
        }
        case op_div: {
            printBinaryOp(location, it, "div");
            break;
        }
        case op_mod: {
            printBinaryOp(location, it, "mod");
            break;
        }
        case op_sub: {
            printBinaryOp(location, it, "sub");
            ++it;
            break;
        }
        case op_lshift: {
            printBinaryOp(location, it, "lshift");
            break;            
        }
        case op_rshift: {
            printBinaryOp(location, it, "rshift");
            break;
        }
        case op_urshift: {
            printBinaryOp(location, it, "urshift");
            break;
        }
        case op_bitand: {
            printBinaryOp(location, it, "bitand");
            ++it;
            break;
        }
        case op_bitxor: {
            printBinaryOp(location, it, "bitxor");
            ++it;
            break;
        }
        case op_bitor: {
            printBinaryOp(location, it, "bitor");
            ++it;
            break;
        }
        case op_bitnot: {
            printUnaryOp(location, it, "bitnot");
            break;
        }
        case op_instanceof: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int r2 = (++it)->u.operand;
            int r3 = (++it)->u.operand;
            printf("[%4d] instanceof\t\t %s, %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), registerName(r2).c_str(), registerName(r3).c_str());
            break;
        }
        case op_typeof: {
            printUnaryOp(location, it, "typeof");
            break;
        }
        case op_is_undefined: {
            printUnaryOp(location, it, "is_undefined");
            break;
        }
        case op_is_boolean: {
            printUnaryOp(location, it, "is_boolean");
            break;
        }
        case op_is_number: {
            printUnaryOp(location, it, "is_number");
            break;
        }
        case op_is_string: {
            printUnaryOp(location, it, "is_string");
            break;
        }
        case op_is_object: {
            printUnaryOp(location, it, "is_object");
            break;
        }
        case op_is_function: {
            printUnaryOp(location, it, "is_function");
            break;
        }
        case op_in: {
            printBinaryOp(location, it, "in");
            break;
        }
        case op_resolve: {
            int r0 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            printf("[%4d] resolve\t\t %s, %s\n", location, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str());
            break;
        }
        case op_resolve_skip: {
            int r0 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            int skipLevels = (++it)->u.operand;
            printf("[%4d] resolve_skip\t %s, %s, %d\n", location, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str(), skipLevels);
            break;
        }
        case op_resolve_global: {
            int r0 = (++it)->u.operand;
            JSValue* scope = static_cast<JSValue*>((++it)->u.jsCell);
            int id0 = (++it)->u.operand;
            printf("[%4d] resolve_global\t %s, %s, %s\n", location, registerName(r0).c_str(), valueToSourceString(exec, scope).ascii(), idName(id0, m_identifiers[id0]).c_str());
            it += 2;
            break;
        }
        case op_get_scoped_var: {
            int r0 = (++it)->u.operand;
            int index = (++it)->u.operand;
            int skipLevels = (++it)->u.operand;
            printf("[%4d] get_scoped_var\t %s, %d, %d\n", location, registerName(r0).c_str(), index, skipLevels);
            break;
        }
        case op_put_scoped_var: {
            int index = (++it)->u.operand;
            int skipLevels = (++it)->u.operand;
            int r0 = (++it)->u.operand;
            printf("[%4d] put_scoped_var\t %d, %d, %s\n", location, index, skipLevels, registerName(r0).c_str());
            break;
        }
        case op_get_global_var: {
            int r0 = (++it)->u.operand;
            JSValue* scope = static_cast<JSValue*>((++it)->u.jsCell);
            int index = (++it)->u.operand;
            printf("[%4d] get_global_var\t %s, %s, %d\n", location, registerName(r0).c_str(), valueToSourceString(exec, scope).ascii(), index);
            break;
        }
        case op_put_global_var: {
            JSValue* scope = static_cast<JSValue*>((++it)->u.jsCell);
            int index = (++it)->u.operand;
            int r0 = (++it)->u.operand;
            printf("[%4d] put_global_var\t %s, %d, %s\n", location, valueToSourceString(exec, scope).ascii(), index, registerName(r0).c_str());
            break;
        }
        case op_resolve_base: {
            int r0 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            printf("[%4d] resolve_base\t %s, %s\n", location, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str());
            break;
        }
        case op_resolve_with_base: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            printf("[%4d] resolve_with_base %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), idName(id0, m_identifiers[id0]).c_str());
            break;
        }
        case op_resolve_func: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            printf("[%4d] resolve_func\t %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), idName(id0, m_identifiers[id0]).c_str());
            break;
        }
        case op_get_by_id: {
            printGetByIdOp(location, it, m_identifiers, "get_by_id");
            break;
        }
        case op_get_by_id_self: {
            printGetByIdOp(location, it, m_identifiers, "get_by_id_self");
            break;
        }
        case op_get_by_id_self_list: {
            printGetByIdOp(location, it, m_identifiers, "get_by_id_self_list");
            break;
        }
        case op_get_by_id_proto: {
            printGetByIdOp(location, it, m_identifiers, "get_by_id_proto");
            break;
        }
        case op_get_by_id_proto_list: {
            printGetByIdOp(location, it, m_identifiers, "op_get_by_id_proto_list");
            break;
        }
        case op_get_by_id_chain: {
            printGetByIdOp(location, it, m_identifiers, "get_by_id_chain");
            break;
        }
        case op_get_by_id_generic: {
            printGetByIdOp(location, it, m_identifiers, "get_by_id_generic");
            break;
        }
        case op_get_array_length: {
            printGetByIdOp(location, it, m_identifiers, "get_array_length");
            break;
        }
        case op_get_string_length: {
            printGetByIdOp(location, it, m_identifiers, "get_string_length");
            break;
        }
        case op_put_by_id: {
            printPutByIdOp(location, it, m_identifiers, "put_by_id");
            break;
        }
        case op_put_by_id_replace: {
            printPutByIdOp(location, it, m_identifiers, "put_by_id_replace");
            break;
        }
        case op_put_by_id_transition: {
            printPutByIdOp(location, it, m_identifiers, "put_by_id_transition");
            break;
        }
        case op_put_by_id_generic: {
            printPutByIdOp(location, it, m_identifiers, "put_by_id_generic");
            break;
        }
        case op_put_getter: {
            int r0 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] put_getter\t %s, %s, %s\n", location, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str(), registerName(r1).c_str());
            break;
        }
        case op_put_setter: {
            int r0 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] put_setter\t %s, %s, %s\n", location, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str(), registerName(r1).c_str());
            break;
        }
        case op_del_by_id: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            printf("[%4d] del_by_id\t %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), idName(id0, m_identifiers[id0]).c_str());
            break;
        }
        case op_get_by_val: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int r2 = (++it)->u.operand;
            printf("[%4d] get_by_val\t %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), registerName(r2).c_str());
            break;
        }
        case op_put_by_val: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int r2 = (++it)->u.operand;
            printf("[%4d] put_by_val\t %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), registerName(r2).c_str());
            break;
        }
        case op_del_by_val: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int r2 = (++it)->u.operand;
            printf("[%4d] del_by_val\t %s, %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str(), registerName(r2).c_str());
            break;
        }
        case op_put_by_index: {
            int r0 = (++it)->u.operand;
            unsigned n0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] put_by_index\t %s, %u, %s\n", location, registerName(r0).c_str(), n0, registerName(r1).c_str());
            break;
        }
        case op_jmp: {
            int offset = (++it)->u.operand;
            printf("[%4d] jmp\t\t %d(->%d)\n", location, offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_loop: {
            int offset = (++it)->u.operand;
            printf("[%4d] loop\t\t %d(->%d)\n", location, offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_jtrue: {
            printConditionalJump(begin, it, location, "jtrue");
            break;
        }
        case op_loop_if_true: {
            printConditionalJump(begin, it, location, "loop_if_true");
            break;
        }
        case op_jfalse: {
            printConditionalJump(begin, it, location, "jfalse");
            break;
        }
        case op_jeq_null: {
            printConditionalJump(begin, it, location, "jeq_null");
            break;
        }
        case op_jneq_null: {
            printConditionalJump(begin, it, location, "jneq_null");
            break;
        }
        case op_jnless: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int offset = (++it)->u.operand;
            printf("[%4d] jnless\t\t %s, %s, %d(->%d)\n", location, registerName(r0).c_str(), registerName(r1).c_str(), offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_loop_if_less: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int offset = (++it)->u.operand;
            printf("[%4d] loop_if_less\t %s, %s, %d(->%d)\n", location, registerName(r0).c_str(), registerName(r1).c_str(), offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_loop_if_lesseq: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            int offset = (++it)->u.operand;
            printf("[%4d] loop_if_lesseq\t %s, %s, %d(->%d)\n", location, registerName(r0).c_str(), registerName(r1).c_str(), offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_switch_imm: {
            int tableIndex = (++it)->u.operand;
            int defaultTarget = (++it)->u.operand;
            int scrutineeRegister = (++it)->u.operand;
            printf("[%4d] switch_imm\t %d, %d(->%d), %s\n", location, tableIndex, defaultTarget, locationForOffset(begin, it, defaultTarget), registerName(scrutineeRegister).c_str());
            break;
        }
        case op_switch_char: {
            int tableIndex = (++it)->u.operand;
            int defaultTarget = (++it)->u.operand;
            int scrutineeRegister = (++it)->u.operand;
            printf("[%4d] switch_char\t %d, %d(->%d), %s\n", location, tableIndex, defaultTarget, locationForOffset(begin, it, defaultTarget), registerName(scrutineeRegister).c_str());
            break;
        }
        case op_switch_string: {
            int tableIndex = (++it)->u.operand;
            int defaultTarget = (++it)->u.operand;
            int scrutineeRegister = (++it)->u.operand;
            printf("[%4d] switch_string\t %d, %d(->%d), %s\n", location, tableIndex, defaultTarget, locationForOffset(begin, it, defaultTarget), registerName(scrutineeRegister).c_str());
            break;
        }
        case op_new_func: {
            int r0 = (++it)->u.operand;
            int f0 = (++it)->u.operand;
            printf("[%4d] new_func\t\t %s, f%d\n", location, registerName(r0).c_str(), f0);
            break;
        }
        case op_new_func_exp: {
            int r0 = (++it)->u.operand;
            int f0 = (++it)->u.operand;
            printf("[%4d] new_func_exp\t %s, f%d\n", location, registerName(r0).c_str(), f0);
            break;
        }
        case op_call: {
            int dst = (++it)->u.operand;
            int func = (++it)->u.operand;
            int argCount = (++it)->u.operand;
            int registerOffset = (++it)->u.operand;
            printf("[%4d] call\t\t %s, %s, %d, %d\n", location, registerName(dst).c_str(), registerName(func).c_str(), argCount, registerOffset);
            break;
        }
        case op_call_eval: {
            int dst = (++it)->u.operand;
            int func = (++it)->u.operand;
            int argCount = (++it)->u.operand;
            int registerOffset = (++it)->u.operand;
            printf("[%4d] call_eval\t %s, %s, %d, %d\n", location, registerName(dst).c_str(), registerName(func).c_str(), argCount, registerOffset);
            break;
        }
        case op_tear_off_activation: {
            int r0 = (++it)->u.operand;
            printf("[%4d] tear_off_activation\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_tear_off_arguments: {
            printf("[%4d] tear_off_arguments\n", location);
            break;
        }
        case op_ret: {
            int r0 = (++it)->u.operand;
            printf("[%4d] ret\t\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_construct: {
            int dst = (++it)->u.operand;
            int func = (++it)->u.operand;
            int argCount = (++it)->u.operand;
            int registerOffset = (++it)->u.operand;
            int proto = (++it)->u.operand;
            int thisRegister = (++it)->u.operand;
            printf("[%4d] construct\t %s, %s, %d, %d, %s, %s\n", location, registerName(dst).c_str(), registerName(func).c_str(), argCount, registerOffset, registerName(proto).c_str(), registerName(thisRegister).c_str());
            break;
        }
        case op_construct_verify: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] construct_verify\t %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str());
            break;
        }
        case op_get_pnames: {
            int r0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] get_pnames\t %s, %s\n", location, registerName(r0).c_str(), registerName(r1).c_str());
            break;
        }
        case op_next_pname: {
            int dest = (++it)->u.operand;
            int iter = (++it)->u.operand;
            int offset = (++it)->u.operand;
            printf("[%4d] next_pname\t %s, %s, %d(->%d)\n", location, registerName(dest).c_str(), registerName(iter).c_str(), offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_push_scope: {
            int r0 = (++it)->u.operand;
            printf("[%4d] push_scope\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_pop_scope: {
            printf("[%4d] pop_scope\n", location);
            break;
        }
        case op_push_new_scope: {
            int r0 = (++it)->u.operand;
            int id0 = (++it)->u.operand;
            int r1 = (++it)->u.operand;
            printf("[%4d] push_new_scope \t%s, %s, %s\n", location, registerName(r0).c_str(), idName(id0, m_identifiers[id0]).c_str(), registerName(r1).c_str());
            break;
        }
        case op_jmp_scopes: {
            int scopeDelta = (++it)->u.operand;
            int offset = (++it)->u.operand;
            printf("[%4d] jmp_scopes\t^%d, %d(->%d)\n", location, scopeDelta, offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_catch: {
            int r0 = (++it)->u.operand;
            printf("[%4d] catch\t\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_throw: {
            int r0 = (++it)->u.operand;
            printf("[%4d] throw\t\t %s\n", location, registerName(r0).c_str());
            break;
        }
        case op_new_error: {
            int r0 = (++it)->u.operand;
            int errorType = (++it)->u.operand;
            int k0 = (++it)->u.operand;
            printf("[%4d] new_error\t %s, %d, %s\n", location, registerName(r0).c_str(), errorType, constantName(exec, k0, m_unexpectedConstants[k0]).c_str());
            break;
        }
        case op_jsr: {
            int retAddrDst = (++it)->u.operand;
            int offset = (++it)->u.operand;
            printf("[%4d] jsr\t\t %s, %d(->%d)\n", location, registerName(retAddrDst).c_str(), offset, locationForOffset(begin, it, offset));
            break;
        }
        case op_sret: {
            int retAddrSrc = (++it)->u.operand;
            printf("[%4d] sret\t\t %s\n", location, registerName(retAddrSrc).c_str());
            break;
        }
        case op_debug: {
            int debugHookID = (++it)->u.operand;
            int firstLine = (++it)->u.operand;
            int lastLine = (++it)->u.operand;
            printf("[%4d] debug\t\t %s, %d, %d\n", location, debugHookName(debugHookID), firstLine, lastLine);
            break;
        }
        case op_profile_will_call: {
            int function = (++it)->u.operand;
            printf("[%4d] profile_will_call %s\n", location, registerName(function).c_str());
            break;
        }
        case op_profile_did_call: {
            int function = (++it)->u.operand;
            printf("[%4d] profile_did_call\t %s\n", location, registerName(function).c_str());
            break;
        }
        case op_end: {
            int r0 = (++it)->u.operand;
            printf("[%4d] end\t\t %s\n", location, registerName(r0).c_str());
            break;
        }
    }
}

#endif // !defined(NDEBUG) || ENABLE(OPCODE_SAMPLING)

CodeBlock::CodeBlock(ScopeNode* ownerNode, CodeType codeType, PassRefPtr<SourceProvider> sourceProvider, unsigned sourceOffset)
    : m_numCalleeRegisters(0)
    , m_numConstants(0)
    , m_numVars(0)
    , m_numParameters(0)
    , m_ownerNode(ownerNode)
    , m_globalData(0)
#if ENABLE(JIT)
    , m_jitCode(0)
#endif
    , m_needsFullScopeChain(ownerNode->needsActivation())
    , m_usesEval(ownerNode->usesEval())
    , m_codeType(codeType)
    , m_source(sourceProvider)
    , m_sourceOffset(sourceOffset)
{
    ASSERT(m_source);
}

CodeBlock::~CodeBlock()
{
    for (size_t size = m_globalResolveInstructions.size(), i = 0; i < size; ++i)
        derefStructures(&m_instructions[m_globalResolveInstructions[i]]);

    for (size_t size = m_propertyAccessInstructions.size(), i = 0; i < size; ++i) {
        derefStructures(&m_instructions[m_propertyAccessInstructions[i].bytecodeIndex]);
    }

    for (size_t size = m_callLinkInfos.size(), i = 0; i < size; ++i) {
        CallLinkInfo* callLinkInfo = &m_callLinkInfos[i];
        if (callLinkInfo->isLinked())
            callLinkInfo->callee->removeCaller(callLinkInfo);
    }

#if ENABLE(JIT) 
    unlinkCallers();
#endif
}

#if ENABLE(JIT) 
void CodeBlock::unlinkCallers()
{
    size_t size = m_linkedCallerList.size();
    for (size_t i = 0; i < size; ++i) {
        CallLinkInfo* currentCaller = m_linkedCallerList[i];
        JIT::unlinkCall(currentCaller);
        currentCaller->setUnlinked();
    }
    m_linkedCallerList.clear();
}
#endif

void CodeBlock::derefStructures(Instruction* vPC) const
{
    Interpreter* interpreter = m_globalData->interpreter;

    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_self)) {
        vPC[4].u.structure->deref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_proto)) {
        vPC[4].u.structure->deref();
        vPC[5].u.structure->deref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_chain)) {
        vPC[4].u.structure->deref();
        vPC[5].u.structureChain->deref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_transition)) {
        vPC[4].u.structure->deref();
        vPC[5].u.structure->deref();
        vPC[6].u.structureChain->deref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_replace)) {
        vPC[4].u.structure->deref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_resolve_global)) {
        if(vPC[4].u.structure)
            vPC[4].u.structure->deref();
        return;
    }
    if ((vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_proto_list))
        || (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_self_list))) {
        PolymorphicAccessStructureList* polymorphicStructures = vPC[4].u.polymorphicStructures;
        polymorphicStructures->derefStructures(vPC[5].u.operand);
        delete polymorphicStructures;
        return;
    }

    // These instructions don't ref their Structures.
    ASSERT(vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id) || vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id) || vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_generic) || vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_generic) || vPC[0].u.opcode == interpreter->getOpcode(op_get_array_length) || vPC[0].u.opcode == interpreter->getOpcode(op_get_string_length));
}

void CodeBlock::refStructures(Instruction* vPC) const
{
    Interpreter* interpreter = m_globalData->interpreter;

    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_self)) {
        vPC[4].u.structure->ref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_proto)) {
        vPC[4].u.structure->ref();
        vPC[5].u.structure->ref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_chain)) {
        vPC[4].u.structure->ref();
        vPC[5].u.structureChain->ref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_transition)) {
        vPC[4].u.structure->ref();
        vPC[5].u.structure->ref();
        vPC[6].u.structureChain->ref();
        return;
    }
    if (vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_replace)) {
        vPC[4].u.structure->ref();
        return;
    }
    
    // These instructions don't ref their Structures.
    ASSERT(vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id) || vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id) || vPC[0].u.opcode == interpreter->getOpcode(op_get_by_id_generic) || vPC[0].u.opcode == interpreter->getOpcode(op_put_by_id_generic));
}

void CodeBlock::mark()
{
    for (size_t i = 0; i < m_constantRegisters.size(); ++i)
        if (!m_constantRegisters[i].marked())
            m_constantRegisters[i].mark();

    for (size_t i = 0; i < m_unexpectedConstants.size(); ++i)
        if (!m_unexpectedConstants[i]->marked())
            m_unexpectedConstants[i]->mark();

    for (size_t i = 0; i < m_functions.size(); ++i)
        m_functions[i]->body()->mark();

    for (size_t i = 0; i < m_functionExpressions.size(); ++i)
        m_functionExpressions[i]->body()->mark();
}

bool CodeBlock::getHandlerForVPC(const Instruction* vPC, Instruction*& target, int& scopeDepth)
{
    Vector<HandlerInfo>::iterator ptr = m_exceptionHandlers.begin(); 
    Vector<HandlerInfo>::iterator end = m_exceptionHandlers.end();
    unsigned addressOffset = vPC - m_instructions.begin();
    ASSERT(addressOffset < m_instructions.size());
    
    for (; ptr != end; ++ptr) {
        // Handlers are ordered innermost first, so the first handler we encounter
        // that contains the source address is the correct handler to use.
        if (ptr->start <= addressOffset && ptr->end >= addressOffset) {
            scopeDepth = ptr->scopeDepth;
            target = m_instructions.begin() + ptr->target;
            return true;
        }
    }
    return false;
}

void* CodeBlock::nativeExceptionCodeForHandlerVPC(const Instruction* handlerVPC)
{
    Vector<HandlerInfo>::iterator ptr = m_exceptionHandlers.begin(); 
    Vector<HandlerInfo>::iterator end = m_exceptionHandlers.end();
    
    for (; ptr != end; ++ptr) {
        Instruction*target = m_instructions.begin() + ptr->target;
        if (handlerVPC == target)
            return ptr->nativeCode;
    }

    return 0;
}

int CodeBlock::lineNumberForVPC(const Instruction* vPC)
{
    unsigned instructionOffset = vPC - m_instructions.begin();
    ASSERT(instructionOffset < m_instructions.size());

    if (!m_lineInfo.size())
        return m_ownerNode->source().firstLine(); // Empty function

    int low = 0;
    int high = m_lineInfo.size();
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (m_lineInfo[mid].instructionOffset <= instructionOffset)
            low = mid + 1;
        else
            high = mid;
    }
    
    if (!low)
        return m_ownerNode->source().firstLine();
    return m_lineInfo[low - 1].lineNumber;
}

int CodeBlock::expressionRangeForVPC(const Instruction* vPC, int& divot, int& startOffset, int& endOffset)
{
    unsigned instructionOffset = vPC - m_instructions.begin();
    ASSERT(instructionOffset < m_instructions.size());

    if (!m_expressionInfo.size()) {
        // We didn't think anything could throw.  Apparently we were wrong.
        startOffset = 0;
        endOffset = 0;
        divot = 0;
        return lineNumberForVPC(vPC);
    }

    int low = 0;
    int high = m_expressionInfo.size();
    while (low < high) {
        int mid = low + (high - low) / 2;
        if (m_expressionInfo[mid].instructionOffset <= instructionOffset)
            low = mid + 1;
        else
            high = mid;
    }
    
    ASSERT(low);
    if (!low) {
        startOffset = 0;
        endOffset = 0;
        divot = 0;
        return lineNumberForVPC(vPC);
    }

    startOffset = m_expressionInfo[low - 1].startOffset;
    endOffset = m_expressionInfo[low - 1].endOffset;
    divot = m_expressionInfo[low - 1].divotPoint + m_sourceOffset;
    return lineNumberForVPC(vPC);
}

void CodeBlock::shrinkToFit()
{
    m_instructions.shrinkToFit();

    m_globalResolveInstructions.shrinkToFit();
    m_propertyAccessInstructions.shrinkToFit();
    m_callLinkInfos.shrinkToFit();
    m_linkedCallerList.shrinkToFit();

    m_exceptionHandlers.shrinkToFit();
    m_expressionInfo.shrinkToFit();
    m_lineInfo.shrinkToFit();

    m_identifiers.shrinkToFit();
    m_functions.shrinkToFit();
    m_functionExpressions.shrinkToFit();
    m_constantRegisters.shrinkToFit();
    m_unexpectedConstants.shrinkToFit();
    m_regexps.shrinkToFit();

    m_immediateSwitchJumpTables.shrinkToFit();
    m_characterSwitchJumpTables.shrinkToFit();
    m_stringSwitchJumpTables.shrinkToFit();
}

} // namespace JSC
