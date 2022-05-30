// Copyright 2022 Pierre Talbot

#ifndef HELPER_HPP
#define HELPER_HPP

#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#include "z.hpp"
#include "cartesian_product.hpp"
#include "interval.hpp"
#include "arithmetic.hpp"
#include "vstore.hpp"
#include "value_order.hpp"
#include "variable_order.hpp"
#include "split.hpp"
#include "vector.hpp"
#include "shared_ptr.hpp"
#include "ipc.hpp"
#include "terms.hpp"
#include "fixpoint.hpp"

using namespace lala;
using namespace battery;

using F = TFormula<StandardAllocator>;

static LVar<StandardAllocator> var_x = "x";
static LVar<StandardAllocator> var_x0 = "x0";
static LVar<StandardAllocator> var_x1 = "x1";
static LVar<StandardAllocator> var_x2 = "x2";
static LVar<StandardAllocator> var_y = "y";
static LVar<StandardAllocator> var_z = "z";
static LVar<StandardAllocator> var_b = "b";

#define EXPECT_EQ2(a,b) EXPECT_EQ(unwrap(a), unwrap(b))
#define EXPECT_TRUE2(a) EXPECT_TRUE(unwrap(a))
#define EXPECT_FALSE2(a) EXPECT_FALSE(unwrap(a))

using zi = ZInc<int>;
using zd = ZDec<int>;
using Itv = Interval<zi>;
using IStore = VStore<Itv, StandardAllocator>;
using IIPC = IPC<IStore>;

const AType sty = 0;
const AType pty = 1;
const AType tty = 2;
const AType split_ty = 3;
const AType bab_ty = 4;

template <typename U, typename F>
void tell_store(VStore<U, StandardAllocator>& store, F f, const LVar<StandardAllocator>& x, U v) {
  auto cons = store.interpret(f);
  EXPECT_TRUE(cons.has_value());
  BInc has_changed = BInc::bot();
  store.tell(*cons, has_changed);
  EXPECT_TRUE2(has_changed);
}

/** At most 10 variables. (Names range from x0 to x9). */
void populate_istore_n_vars(IStore& store, int n, int l, int u) {
  assert(n <= 10);
  for(int i = 0; i < n; ++i) {
    LVar<StandardAllocator> x = "x ";
    x[1] = '0' + i;
    EXPECT_TRUE(store.interpret(F::make_exists(sty, x, Int)).has_value());
    tell_store(store, make_v_op_z(x, GEQ, l), x, Itv(l, zd::bot()));
    tell_store(store, make_v_op_z(x, LEQ, u), x, Itv(l, u));
  }
}
void populate_istore_10_vars(IStore& store, int l, int u) {
  populate_istore_n_vars(store, 10, l, u);
}

void x0_plus_x1_eq_x2(IIPC& ipc) {
  auto f = F::make_binary(F::make_binary(F::make_lvar(sty, var_x0), ADD, F::make_lvar(sty, var_x1), pty), EQ, F::make_lvar(sty, var_x2), pty);
  BInc has_changed = BInc::bot();
  ipc.tell(*(ipc.interpret(f)), has_changed);
}

template <class A>
void seq_refine_check(A& a, BInc expect_changed = BInc::top()) {
  BInc has_changed = BInc::bot();
  seq_refine(a, has_changed);
  EXPECT_EQ2(has_changed, expect_changed);
}

#endif