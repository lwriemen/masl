/*
 ----------------------------------------------------------------------------
 (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 The copyright of this Software is vested in the Crown
 and the Software is the property of the Crown.
 ----------------------------------------------------------------------------
 SPDX-License-Identifier: Apache-2.0
 ----------------------------------------------------------------------------
 */
package org.xtuml.masl.metamodelImpl.expression;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collA;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collB;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collC;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collD;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collE;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collF;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collG;
import static org.xtuml.masl.metamodelImpl.SampleDomain.collH;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r1;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r2;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r3;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r4;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r5;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r6;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r7;
import static org.xtuml.masl.metamodelImpl.SampleDomain.r8;
import static org.xtuml.masl.metamodelImpl.SampleDomain.refA;
import static org.xtuml.masl.metamodelImpl.SampleDomain.refB;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleA;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleB;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleC;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleD;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleE;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleF;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleG;
import static org.xtuml.masl.metamodelImpl.SampleDomain.singleH;

import org.xtuml.masl.error.ErrorCode;
import org.xtuml.masl.metamodelImpl.error.SemanticErrorCode;
import org.xtuml.masl.metamodelImpl.relationship.RelationshipDeclaration;
import org.xtuml.masl.metamodelImpl.relationship.RelationshipSpecification;
import org.xtuml.masl.unittest.ErrorLog;

import junit.framework.TestCase;

public class TestUnlinkExpression extends TestCase {

    public void checkUnlink(final Expression lhs, final RelationshipDeclaration.Reference relRef,
            final String objOrRole, final ObjectNameExpression obj, final Expression rhs, final Expression assignTo) {
        ErrorLog.getInstance().reset();
        final LinkUnlinkExpression result = LinkUnlinkExpression.create(null, LinkUnlinkExpression.UNLINK, lhs,
                RelationshipSpecification.createReference(lhs, relRef, objOrRole, obj, false, false), rhs);
        ErrorLog.getInstance().checkErrors();
        assertTrue(assignTo.getType().isAssignableFrom(result, false));
    }

    public void checkUnlink(final Expression lhs, final RelationshipDeclaration.Reference relRef,
            final String objOrRole, final ObjectNameExpression obj, final Expression rhs, final ErrorCode... errors) {
        ErrorLog.getInstance().reset();
        LinkUnlinkExpression.create(null, LinkUnlinkExpression.UNLINK, lhs,
                RelationshipSpecification.createReference(lhs, relRef, objOrRole, obj, false, false), rhs);
        ErrorLog.getInstance().checkErrors(errors);
    }

    // R1

    public void test_unlink_a_R1_b() {
        checkUnlink(singleA, r1, "to left of", refB, singleB, SemanticErrorCode.AssociativeRelationshipRequired);
    }

    public void test_unlink_b_R1_a() {
        checkUnlink(singleB, r1, "to right of", refA, singleA, SemanticErrorCode.AssociativeRelationshipRequired);
    }

    // R2

    public void test_unlink_a_R2_b() {
        checkUnlink(singleA, r2, "to left of", refB, singleB, SemanticErrorCode.AssociativeRelationshipRequired);
    }

    public void test_unlink_b_R2_a() {
        checkUnlink(singleB, r2, "to right of", refA, singleA, SemanticErrorCode.AssociativeRelationshipRequired);
    }

    // R3 ->

    public void test_unlink_a_R3_b() {
        checkUnlink(singleA, r3, "to left of", refB, singleB, singleC);
    }

    public void test_unlink_a_R3_bb() {
        checkUnlink(singleA, r3, "to left of", refB, collB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_aa_R3_b() {
        checkUnlink(collA, r3, "to left of", refB, singleB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_aa_R3_bb() {
        checkUnlink(collA, r3, "to left of", refB, collB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_a_R3() {
        checkUnlink(singleA, r3, "to left of", refB, null, singleC);
    }

    public void test_unlink_aa_R3() {
        checkUnlink(collA, r3, "to left of", refB, null, collC);
    }

    // R3 <-

    public void test_unlink_b_R3_a() {
        checkUnlink(singleB, r3, "to right of", refA, singleA, singleC);
    }

    public void test_unlink_bb_R3_a() {
        checkUnlink(collB, r3, "to right of", refA, singleA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_b_R3_aa() {
        checkUnlink(singleB, r3, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_bb_R3_aa() {
        checkUnlink(collB, r3, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_b_R3() {
        checkUnlink(singleB, r3, "to right of", refA, null, singleC);
    }

    public void test_unlink_bb_R3() {
        checkUnlink(collB, r3, "to right of", refA, null, collC);
    }

    // R4 ->

    public void test_unlink_a_R4_b() {
        checkUnlink(singleA, r4, "to left of", refB, singleB, singleD);
    }

    public void test_unlink_a_R4_bb() {
        checkUnlink(singleA, r4, "to left of", refB, collB, collD);
    }

    public void test_unlink_aa_R4_b() {
        checkUnlink(collA, r4, "to left of", refB, singleB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_aa_R4_bb() {
        checkUnlink(collA, r4, "to left of", refB, collB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_a_R4() {
        checkUnlink(singleA, r4, "to left of", refB, null, collD);
    }

    public void test_unlink_aa_R4() {
        checkUnlink(collA, r4, "to left of", refB, null, collD);
    }

    // R4 <-

    public void test_unlink_b_R4_a() {
        checkUnlink(singleB, r4, "to right of", refA, singleA, singleD);
    }

    public void test_unlink_bb_R4_a() {
        checkUnlink(collB, r4, "to right of", refA, singleA, collD);
    }

    public void test_unlink_b_R4_aa() {
        checkUnlink(singleB, r4, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_bb_R4_aa() {
        checkUnlink(collB, r4, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_b_R4() {
        checkUnlink(singleB, r4, "to right of", refA, null, singleD);
    }

    public void test_unlink_bb_R4() {
        checkUnlink(collB, r4, "to right of", refA, null, collD);
    }

    // R5 ->

    public void test_unlink_a_R5_b() {
        checkUnlink(singleA, r5, "to left of", refB, singleB, singleE);
    }

    public void test_unlink_a_R5_bb() {
        checkUnlink(singleA, r5, "to left of", refB, collB, collE);
    }

    public void test_unlink_aa_R5_b() {
        checkUnlink(collA, r5, "to left of", refB, singleB, collE);
    }

    public void test_unlink_aa_R5_bb() {
        checkUnlink(collA, r5, "to left of", refB, collB, collE);
    }

    public void test_unlink_a_R5() {
        checkUnlink(singleA, r5, "to left of", refB, null, collE);
    }

    public void test_unlink_aa_R5() {
        checkUnlink(collA, r5, "to left of", refB, null, collE);
    }

    // R5 <-

    public void test_unlink_b_R5_a() {
        checkUnlink(singleB, r5, "to right of", refA, singleA, singleE);
    }

    public void test_unlink_bb_R5_a() {
        checkUnlink(collB, r5, "to right of", refA, singleA, collE);
    }

    public void test_unlink_b_R5_aa() {
        checkUnlink(singleB, r5, "to right of", refA, collA, collE);
    }

    public void test_unlink_bb_R5_aa() {
        checkUnlink(collB, r5, "to right of", refA, collA, collE);
    }

    public void test_unlink_b_R5() {
        checkUnlink(singleB, r5, "to right of", refA, null, collE);
    }

    public void test_unlink_bb_R5() {
        checkUnlink(collB, r5, "to right of", refA, null, collE);
    }

    // R6 <-

    public void test_unlink_b_R6_a() {
        checkUnlink(singleB, r6, "to right of", refA, singleA, singleF);
    }

    public void test_unlink_bb_R6_a() {
        checkUnlink(collB, r6, "to right of", refA, singleA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_b_R6_aa() {
        checkUnlink(singleB, r6, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_bb_R6_aa() {
        checkUnlink(collB, r6, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_b_R6() {
        checkUnlink(singleB, r6, "to right of", refA, null, singleF);
    }

    public void test_unlink_bb_R6() {
        checkUnlink(collB, r6, "to right of", refA, null, collF);
    }

    // R7 ->

    public void test_unlink_a_R7_b() {
        checkUnlink(singleA, r7, "to left of", refB, singleB, singleG);
    }

    public void test_unlink_a_R7_bb() {
        checkUnlink(singleA, r7, "to left of", refB, collB, collG);
    }

    public void test_unlink_aa_R7_b() {
        checkUnlink(collA, r7, "to left of", refB, singleB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_aa_R7_bb() {
        checkUnlink(collA, r7, "to left of", refB, collB, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_a_R7() {
        checkUnlink(singleA, r7, "to left of", refB, null, collG);
    }

    public void test_unlink_aa_R7() {
        checkUnlink(collA, r7, "to left of", refB, null, collG);
    }

    // R7 <-

    public void test_unlink_b_R7_a() {
        checkUnlink(singleB, r7, "to right of", refA, singleA, singleG);
    }

    public void test_unlink_bb_R7_a() {
        checkUnlink(collB, r7, "to right of", refA, singleA, collG);
    }

    public void test_unlink_b_R7_aa() {
        checkUnlink(singleB, r7, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_bb_R7_aa() {
        checkUnlink(collB, r7, "to right of", refA, collA, SemanticErrorCode.NotInstanceType);
    }

    public void test_unlink_b_R7() {
        checkUnlink(singleB, r7, "to right of", refA, null, singleG);
    }

    public void test_unlink_bb_R7() {
        checkUnlink(collB, r7, "to right of", refA, null, collG);
    }

    // R8 ->

    public void test_unlink_a_R8_b() {
        checkUnlink(singleA, r8, "to left of", refB, singleB, singleH);
    }

    public void test_unlink_a_R8_bb() {
        checkUnlink(singleA, r8, "to left of", refB, collB, collH);
    }

    public void test_unlink_aa_R8_b() {
        checkUnlink(collA, r8, "to left of", refB, singleB, collH);
    }

    public void test_unlink_aa_R8_bb() {
        checkUnlink(collA, r8, "to left of", refB, collB, collH);
    }

    public void test_unlink_a_R8() {
        checkUnlink(singleA, r8, "to left of", refB, null, collH);
    }

    public void test_unlink_aa_R8() {
        checkUnlink(collA, r8, "to left of", refB, null, collH);
    }

    // R8 <-

    public void test_unlink_b_R8_a() {
        checkUnlink(singleB, r8, "to right of", refA, singleA, singleH);
    }

    public void test_unlink_bb_R8_a() {
        checkUnlink(collB, r8, "to right of", refA, singleA, collH);
    }

    public void test_unlink_b_R8_aa() {
        checkUnlink(singleB, r8, "to right of", refA, collA, collH);
    }

    public void test_unlink_bb_R8_aa() {
        checkUnlink(collB, r8, "to right of", refA, collA, collH);
    }

    public void test_unlink_b_R8() {
        checkUnlink(singleB, r8, "to right of", refA, null, collH);
    }

    public void test_unlink_bb_R8() {
        checkUnlink(collB, r8, "to right of", refA, null, collH);
    }

}
