/*
 * ----------------------------------------------------------------------------
 * (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 * The copyright of this Software is vested in the Crown
 * and the Software is the property of the Crown.
 * ----------------------------------------------------------------------------
 * SPDX-License-Identifier: Apache-2.0
 * ----------------------------------------------------------------------------
 */

#ifndef Inspector_ActionHandler_HH
#define Inspector_ActionHandler_HH

#include "CommunicationChannel.hh"
#include <iostream>
#include <vector>
#include "swa/Set.hh"
#include "swa/Stack.hh"
#include "types.hh"

namespace Inspector
{
  class ActionHandler
  {
    public:
      virtual Callable getInvoker ( CommunicationChannel& channel ) const { return Callable(); }
      virtual void writeLocalVars ( CommunicationChannel& channel, const SWA::StackFrame& frame  ) const = 0;

      virtual ~ActionHandler() = default;
  };
}

#endif
