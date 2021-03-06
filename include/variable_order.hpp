// Copyright 2022 Pierre Talbot

#ifndef VARIABLE_ORDER_HPP
#define VARIABLE_ORDER_HPP

#include "ast.hpp"
#include "z.hpp"
#include "vector.hpp"
#include "shared_ptr.hpp"

namespace lala {

template <class A>
class VariableOrder
{
public:
  using Allocator = typename A::Allocator;
  using TellType = typename A::TellType;
  using LVarArray = battery::vector<LVar<Allocator>, Allocator>;

protected:
  battery::vector<AVar, Allocator> vars;
  battery::shared_ptr<A, Allocator> a;

public:
  VariableOrder(VariableOrder&&) = default;
  CUDA VariableOrder(battery::shared_ptr<A, Allocator> a) : a(a) {}

  template<class A2>
  CUDA VariableOrder(const VariableOrder<A2>& other, AbstractDeps<Allocator>& deps)
   : vars(other.vars), a(deps.clone(other.a)) {}

  CUDA void interpret() {
    const auto& env = a->environment();
    if(vars.size() != env.size()) {
      vars.clear();
      vars.reserve(env.size());
      for(int i = 0; i < env.size(); ++i) {
        vars.push_back(*(env.to_avar(env[i])));
      }
    }
  }

  CUDA BInc is_top() const {
    return a->is_top();
  }
};

template <class A>
class InputOrder : public VariableOrder<A> {
public:
  using Allocator = typename A::Allocator;
  using LVarArray = typename VariableOrder<A>::LVarArray;

private:
  ZDec<int> smallest;

public:
  InputOrder(InputOrder&&) = default;
  CUDA InputOrder(battery::shared_ptr<A, Allocator> a)
   : VariableOrder<A>(std::move(a)), smallest(ZDec<int>::bot()) {}
  CUDA InputOrder(battery::shared_ptr<A, Allocator> a, const LVarArray& lvars)
   : VariableOrder<A>(std::move(a)), smallest(ZDec<int>::bot()) {}

  template<class A2>
  CUDA InputOrder(const InputOrder<A2>& other, AbstractDeps<Allocator>& deps)
   : VariableOrder<A>(other, deps), smallest(other.smallest) {}

  CUDA int num_refinements() const {
    return this->vars.size();
  }

  CUDA void reset() {
    smallest.dtell(ZDec<int>::bot());
  }

  CUDA void refine(int i, BInc& has_changed) {
    if(i < this->vars.size() && !this->is_top().guard()) {
      using D = typename A::Universe;
      const D& x = this->a->project(this->vars[i]);
      // This condition is actually monotone under the assumption that x is not updated anymore between two invocations of this refine function.
      if(lt<typename D::LB>(x.lb(), x.ub()).value()) {
        smallest.tell(ZDec<int>(i), has_changed);
      }
    }
  }

  CUDA thrust::optional<AVar> project() const {
    if(smallest.is_bot().value()) {
      return {};
    }
    return this->vars[smallest.value()];
  }
};

}

#endif
