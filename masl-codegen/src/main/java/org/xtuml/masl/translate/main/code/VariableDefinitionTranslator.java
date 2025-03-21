/*
 ----------------------------------------------------------------------------
 (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 The copyright of this Software is vested in the Crown
 and the Software is the property of the Crown.
 ----------------------------------------------------------------------------
 SPDX-License-Identifier: Apache-2.0
 ----------------------------------------------------------------------------
 */
package org.xtuml.masl.translate.main.code;

import org.xtuml.masl.cppgen.*;
import org.xtuml.masl.metamodel.code.VariableDefinition;
import org.xtuml.masl.metamodel.type.TypeDefinition.ActualType;
import org.xtuml.masl.translate.main.Mangler;
import org.xtuml.masl.translate.main.Scope;
import org.xtuml.masl.translate.main.Types;
import org.xtuml.masl.translate.main.expression.ExpressionTranslator;

public class VariableDefinitionTranslator {

    VariableDefinitionTranslator(final VariableDefinition definition, final Scope parentScope) {
        this.definition = definition;

        fullCode = new StatementGroup(Comment.createComment(definition.toString(), false));

        fullCode.appendStatement(preamble);
        fullCode.appendStatement(code);
        fullCode.appendStatement(postamble);

        final TypeUsage type = Types.getInstance().getType(definition.getType());

        Expression initialValue = null;

        if (definition.getInitialValue() != null) {
            initialValue =
                    ExpressionTranslator.createTranslator(definition.getInitialValue(),
                                                          parentScope,
                                                          definition.getType()).getReadExpression();
        } else if (definition.getType().getBasicType().isNumeric() ||
                   definition.getType().getBasicType().isCharacter()) {
            initialValue = Literal.ZERO;
        } else if (definition.getType().getBasicType().getActualType() == ActualType.BOOLEAN) {
            initialValue = Literal.FALSE;
        } else if (definition.getType().getBasicType().getActualType() == ActualType.TIMER) {
            initialValue = type.getType().callConstructor();
        }

        variable = new Variable(type, Mangler.mangleName(definition), initialValue);
        code.appendStatement(new VariableDefinitionStatement(variable));

    }

    public StatementGroup getCode() {
        return code;
    }

    public VariableDefinition getDefinition() {
        return definition;
    }

    public StatementGroup getFullCode() {
        return fullCode;
    }

    public StatementGroup getPostamble() {
        return postamble;
    }

    public StatementGroup getPreamble() {
        return preamble;
    }

    public Variable getVariable() {
        return variable;
    }

    private final Variable variable;

    private final StatementGroup code = new StatementGroup();
    private final StatementGroup fullCode;

    private final StatementGroup postamble = new StatementGroup();
    private final StatementGroup preamble = new StatementGroup();

    private final VariableDefinition definition;
}
