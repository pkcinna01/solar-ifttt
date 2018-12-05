#ifndef SOLAR_IFTTT_VALUECONSTRAINT_H
#define SOLAR_IFTTT_VALUECONSTRAINT_H

#include "Constraint.h"

#include <string>

using namespace std;

namespace automation {

  template<typename ValueT, typename ValueSourceT>
  class ValueConstraint : public Constraint {

  public:
    ValueConstraint(ValueSourceT& valueSource) : valueSource(valueSource) {
    }

    bool checkValue() override {
      const ValueT &value = getValue();
      bool bCheckPassed = checkValue(value);
      return bCheckPassed;
    }

    virtual ValueT getValue() {
      return valueSource.getValue();
    }

    virtual bool checkValue(const ValueT &val) = 0;

  protected:
    ValueSourceT& valueSource;

  };

  template<typename ValueT, typename ValueSourceT>
  class RangeConstraint : public ValueConstraint<ValueT,ValueSourceT> {
  public:

    ValueT minVal, maxVal;

    RangeConstraint(ValueT minVal, ValueT maxVal, ValueSourceT &valueSource)
        : ValueConstraint<ValueT,ValueSourceT>(valueSource), minVal(minVal), maxVal(maxVal) {
    }

    bool checkValue(const ValueT &value) override {
      ValueT minVal = this->minVal;
      ValueT maxVal = this->maxVal;

      if (this->deferredTimeMs) {
        if (this->bTestResult) {
          minVal -= this->failMargin;
          maxVal += this->failMargin;
        } else {
          minVal += this->passMargin;
          maxVal -= this->passMargin;
        }
      }

      return value >= minVal && value <= maxVal;
    }

    string getTitle() override {
      string rtn(this->valueSource.name);
      rtn += " Range(";
      rtn += asString(minVal);
      rtn += ",";
      rtn += asString(maxVal);
      rtn += ")";
      return rtn;
    }
  };


  template<typename ValueT>
  class ConstantValueHolder : public ValueHolder<ValueT>  {
  public:
    ValueT val;
    
    ConstantValueHolder(ValueT val)
      : val(val) {
    }
    
    ValueT getValue() const override {
      return val;
    }
  };

  template<typename ValueT, typename ValueSourceT>
  class ThresholdValueConstraint : public ValueConstraint<ValueT,ValueSourceT> {
  public:

    ValueHolder<ValueT>* pThreshold;

    bool bDeleteThreshold;

    ThresholdValueConstraint(ValueHolder<ValueT>& threshold, ValueSourceT &valueSource)
        : ValueConstraint<ValueT,ValueSourceT>(valueSource)
        , pThreshold(&threshold)
        , bDeleteThreshold(false) {
    }


    virtual std::string& getType() = 0;

    string getTitle() override {
      string rtn(this->valueSource.name);
      rtn += " ";
      rtn += getType();
      rtn += "(";
      rtn += asString(pThreshold->getValue());
      rtn += ")";
      return rtn;
    }

    virtual ~ThresholdValueConstraint() {
      if ( bDeleteThreshold ) {        
        delete pThreshold;
      }
    }

    protected:
    ThresholdValueConstraint(ValueHolder<ValueT>* pThreshold, ValueSourceT &valueSource, bool bDeleteThreshold = true )
        : ValueConstraint<ValueT,ValueSourceT>(valueSource)
        , pThreshold(pThreshold)
        , bDeleteThreshold(bDeleteThreshold) {
    }

  };
  
  template<typename ValueT, typename ValueSourceT>
  class AtMost : public ThresholdValueConstraint<ValueT,ValueSourceT> {
  public:

    AtMost(ValueHolder<ValueT>& threshold, ValueSourceT &valueSource)
        : ThresholdValueConstraint<ValueT,ValueSourceT>(threshold,valueSource) {
    }

    AtMost(ValueT threshold, ValueSourceT &valueSource)
        : AtMost<ValueT,ValueSourceT>(new ConstantValueHolder<ValueT>(threshold),valueSource) {
    }

    std::string& getType() override {
      static std::string strType("AtMost");
      return strType;
    }

    bool checkValue(const ValueT &value) override {
      ValueT maxVal = this->pThreshold->getValue();

      if (this->deferredTimeMs) {
        if (this->bTestResult) {
          maxVal += this->failMargin;
        } else {
          maxVal -= this->passMargin;
        }
      }

      return value <= maxVal;
    }
  };

  template<typename ValueT, typename ValueSourceT>
  class AtLeast : public ThresholdValueConstraint<ValueT,ValueSourceT> {
  public:

    AtLeast(ValueHolder<ValueT>& threshold, ValueSourceT &valueSource)
        : ThresholdValueConstraint<ValueT,ValueSourceT>(threshold,valueSource) {
    }

    AtLeast(ValueT threshold, ValueSourceT &valueSource)
        : ThresholdValueConstraint<ValueT,ValueSourceT>(new ConstantValueHolder<ValueT>(threshold),valueSource,true) {
    }

    std::string& getType() override {
      static std::string strType("AtLeast");
      return strType;
    }

    bool checkValue(const ValueT &value) override {
      ValueT minVal = this->pThreshold->getValue();

      if (this->deferredTimeMs) {
        if (this->bTestResult) {
          minVal -= this->failMargin;
        } else {
          minVal += this->passMargin;
        }
      }
      return value >= minVal;
    }
  };

}
#endif //SOLAR_IFTTT_VALUECONSTRAINT_H
