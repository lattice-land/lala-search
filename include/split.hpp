// Copyright 2022 Pierre Talbot

#ifndef SPLIT_HPP
#define SPLIT_HPP

#include "ast.hpp"
#include "vector.hpp"
#include "shared_ptr.hpp"
#include "branch.hpp"

namespace lala {

template <class A, class VariableOrder, class ValueOrder>
class Split {
public:
  using BranchType = typename ValueOrder::BranchType;

private:
  VariableOrder var_order;
  ValueOrder val_order;

public:
  CUDA Split(VariableOrder&& var_order, ValueOrder&& val_order)
  : var_order(std::move(var_order)), val_order(std::move(val_order)) {}

  CUDA int num_refinements() const {
    return var_order.num_refinements();
  }

  CUDA void refine(int i, BInc& has_changed) {
    var_order.refine(i, has_changed);
  }

  CUDA void reset() {
    var_order.reset();
  }

  CUDA BranchType split() {
    auto x = var_order.project();
    if(x.has_value()) {
      return val_order.split(*x);
    }
    return BranchType{};
  }
};

}

#endif