/*
 ----------------------------------------------------------------------------
 (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 The copyright of this Software is vested in the Crown
 and the Software is the property of the Crown.
 ----------------------------------------------------------------------------
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ----------------------------------------------------------------------------
 Classification: UK OFFICIAL
 ----------------------------------------------------------------------------
 */
package org.xtuml.masl.metamodelImpl.expression;

import org.xtuml.masl.metamodel.ASTNodeVisitor;
import org.xtuml.masl.metamodelImpl.common.Position;
import org.xtuml.masl.metamodelImpl.type.BasicType;
import org.xtuml.masl.utils.HashCode;

import java.util.Collections;
import java.util.List;

public class FindParameterExpression extends Expression
        implements org.xtuml.masl.metamodel.expression.FindParameterExpression {

    private BasicType type;
    private String name;

    public FindParameterExpression(final Position position, final BasicType type) {
        super(position);
        this.type = type;
        this.name = null;
    }

    @Override
    public String toString() {
        return name;
    }

    @Override
    public BasicType getType() {
        return type;
    }

    public void overrideType(final BasicType type) {
        this.type = type;
    }

    @Override
    public boolean equals(final Object obj) {
        if (this == obj) {
            return true;
        }
        if (!(obj instanceof FindParameterExpression fp)) {
            return false;
        } else {
            return type.equals(fp.type) && name.equals(fp.name);
        }
    }

    @Override
    public int hashCode() {
        return HashCode.combineHashes(name.hashCode(), type.hashCode());
    }

    @Override
    public String getName() {
        return name;
    }

    public void setName(final String name) {
        this.name = name;
    }

    @Override
    protected List<FindParameterExpression> getFindParametersInner() {
        return Collections.singletonList(this);
    }

    @Override
    public <R, P> R accept(final ASTNodeVisitor<R, P> v, final P p) throws Exception {
        return v.visitFindParameterExpression(this, p);
    }

}
