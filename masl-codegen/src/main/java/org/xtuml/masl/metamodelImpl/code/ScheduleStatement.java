/*
 ----------------------------------------------------------------------------
 (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 The copyright of this Software is vested in the Crown
 and the Software is the property of the Crown.
 ----------------------------------------------------------------------------
 SPDX-License-Identifier: Apache-2.0
 ----------------------------------------------------------------------------
 */
package org.xtuml.masl.metamodelImpl.code;

import org.xtuml.masl.metamodel.ASTNode;
import org.xtuml.masl.metamodel.ASTNodeVisitor;
import org.xtuml.masl.metamodelImpl.common.Position;
import org.xtuml.masl.metamodelImpl.error.SemanticError;
import org.xtuml.masl.metamodelImpl.expression.Expression;
import org.xtuml.masl.metamodelImpl.type.DurationType;
import org.xtuml.masl.metamodelImpl.type.TimerType;
import org.xtuml.masl.metamodelImpl.type.TimestampType;

import java.util.List;

public class ScheduleStatement extends Statement implements org.xtuml.masl.metamodel.code.ScheduleStatement {

    public static ScheduleStatement create(final Position position,
                                           final Expression timerId,
                                           final GenerateStatement generate,
                                           final boolean isAbsolute,
                                           final Expression time,
                                           final Expression period) {
        if (timerId == null || generate == null || time == null) {
            return null;
        }
        try {
            return new ScheduleStatement(position, timerId, generate, isAbsolute, time, period);
        } catch (final SemanticError e) {
            e.report();
            return null;
        }
    }

    private ScheduleStatement(final Position position,
                              final Expression timerId,
                              final GenerateStatement generate,
                              final boolean isAbsolute,
                              final Expression time,
                              final Expression period) throws SemanticError {
        super(position);
        this.timerId = timerId;
        this.generate = generate;
        this.isAbsolute = isAbsolute;
        this.time = time;
        this.period = period;

        TimerType.createAnonymous().checkAssignable(timerId);

        if (isAbsolute) {
            TimestampType.createAnonymous().checkAssignable(time);
        } else {
            DurationType.createAnonymous().checkAssignable(time);
        }

        if (period != null) {
            DurationType.createAnonymous().checkAssignable(period);
        }

        this.generate.getEvent().setScheduled();

    }

    private final Expression timerId;
    private final GenerateStatement generate;
    private final boolean isAbsolute;
    private final Expression time;
    private final Expression period;

    @Override
    public String toString() {
        return "schedule " +
               timerId +
               " " +
               generate.toString().substring(0, generate.toString().length() - 1) +
               " " +
               (isAbsolute ? "at" : "delay") +
               " " +
               time +
               (period == null ? "" : " delta " + period) +
               ";";

    }

    @Override
    public GenerateStatement getGenerate() {
        return generate;
    }

    @Override
    public Expression getTime() {
        return time;
    }

    @Override
    public Expression getPeriod() {
        return period;
    }

    @Override
    public Expression getTimerId() {
        return timerId;
    }

    @Override
    public boolean isAbsoluteTime() {
        return isAbsolute;
    }

    @Override
    public void accept(final ASTNodeVisitor v) {
        v.visitScheduleStatement(this);
    }

    @Override
    public List<ASTNode> children() {
        return ASTNode.makeChildren(timerId, generate, time, period);
    }

}
