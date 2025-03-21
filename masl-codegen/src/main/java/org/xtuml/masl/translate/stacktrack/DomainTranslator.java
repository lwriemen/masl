/*
 ----------------------------------------------------------------------------
 (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 The copyright of this Software is vested in the Crown
 and the Software is the property of the Crown.
 ----------------------------------------------------------------------------
 SPDX-License-Identifier: Apache-2.0
 ----------------------------------------------------------------------------
 */
package org.xtuml.masl.translate.stacktrack;

import org.xtuml.masl.metamodel.domain.Domain;
import org.xtuml.masl.metamodel.domain.DomainService;
import org.xtuml.masl.metamodel.domain.DomainTerminator;
import org.xtuml.masl.metamodel.object.ObjectDeclaration;
import org.xtuml.masl.translate.Alias;
import org.xtuml.masl.translate.Default;

import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

@Alias("StackTrack")
@Default
public class DomainTranslator extends org.xtuml.masl.translate.DomainTranslator {

    public static DomainTranslator getInstance(final Domain domain) {
        return getInstance(DomainTranslator.class, domain);
    }

    private DomainTranslator(final Domain domain) {
        super(domain);
        mainDomainTranslator = org.xtuml.masl.translate.main.DomainTranslator.getInstance(domain);
    }

    /**
     * @return
     * @see org.xtuml.masl.translate.Translator#getPrerequisites()
     */
    @Override
    public Collection<org.xtuml.masl.translate.DomainTranslator> getPrerequisites() {
        return Collections.singletonList(mainDomainTranslator);
    }

    @Override
    public void translate() {
        for (final ObjectDeclaration object : domain.getObjects()) {
            objectTranslators.put(object, new ObjectTranslator(object));
        }

        for (final DomainTerminator object : domain.getTerminators()) {
            termTranslators.put(object, new TerminatorTranslator(object));
        }

        for (final ObjectTranslator objectTranslator : objectTranslators.values()) {
            objectTranslator.translate();
        }

        for (final TerminatorTranslator termTranslator : termTranslators.values()) {
            termTranslator.translate();
        }

        for (final DomainService service : domain.getServices()) {
            final ActionTranslator
                    serviceTranslator =
                    new ActionTranslator(mainDomainTranslator.getServiceTranslator(service));
            serviceTranslator.translate();
        }

    }

    ObjectTranslator getObjectTranslator(final ObjectDeclaration object) {
        return objectTranslators.get(object);
    }

    Map<ObjectDeclaration, ObjectTranslator> objectTranslators = new HashMap<>();
    Map<DomainTerminator, TerminatorTranslator> termTranslators = new HashMap<>();

    private final org.xtuml.masl.translate.main.DomainTranslator mainDomainTranslator;

}
