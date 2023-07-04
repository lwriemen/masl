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
package org.xtuml.masl.translate.cmake.functions;

import org.xtuml.masl.translate.cmake.language.arguments.SimpleArgument;

public class SimpleAddInterfaceLibrary extends SimpleAddLibrary {
    public SimpleAddInterfaceLibrary(final SimpleArgument name,
                                     final Iterable<? extends SimpleArgument> sources,
                                     final Iterable<? extends SimpleArgument> links,
                                     final SimpleArgument export,
                                     final boolean install,
                                     final Iterable<? extends SimpleArgument> includes) {
        super("simple_add_interface_library", name, sources, links, export, install, includes);
    }

}
