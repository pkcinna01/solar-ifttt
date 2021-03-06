#ifndef AUTOMATION_TOGGLE_CONSTRAINT_H
#define AUTOMATION_TOGGLE_CONSTRAINT_H

#include "Constraint.h"
#include "../capability/Toggle.h"

namespace automation {

  class ToggleStateConstraint : public Constraint {
  public:
    RTTI_GET_TYPE_IMPL(automation,ToggleState);

    Toggle *pToggle;
    
    bool bAcceptState;

    explicit ToggleStateConstraint(Toggle *pToggle, bool bAcceptState = true) : pToggle(pToggle),
                                                                                bAcceptState(bAcceptState) {
      deferredTimeMs = automation::millisecs(); // make sure first run will apply passDelayMs (give time to prometheus to get readings)
    }

    bool checkValue() override {
      return pToggle->asBoolean() == bAcceptState;
    }

    string getTitle() const override {
      string title = pToggle->getTitle();
      title += "==";
      title += (bAcceptState ? "ON" : "OFF");
      return title;
    }
  };

}
#endif
