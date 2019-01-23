// Copyright (C) 2017, Fakulta Informatiky Masarykovy univerzity
//
// This file is a part of Seminator, a tool for semi-determinization of omega automata.
//
// Seminator is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Seminator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <types.hpp>

/**
* Returns a string in the form `{s1, s2, s3}` where si is a reference to the input_aut
*/
std::string powerset_name(power_state);

// Class that computes successors for powerset construction.
//
// The main function is get_succs(state_set ss, mark) which returns a vector
// `succ` of size `nc` of state_sets, where `nc` is the number of possible
// combinations of atomic propositions. `succ[ci]` are successors of ss under
// the `ci`-th combination of AP. The mapping to the condition can be done
// using num2bdd_ vector. In powerset construction, there should be edge:
// `ss --num2bdd_[ci]--> get_succs(ss)->at(ci)` for each `ci`.
//
// The mark, if supplied, limits the successors using only transitions marked by
// mark. If mark is higher than the highest mark (or not supplied), no
// restriction is applied.
//
// Uses bitvector arrays to store already computed successors of the states
// from the input automaton.
class powerset_builder {
public:

  typedef spot::bitvect_array bitvect_array;
  typedef std::map<state_t, bitvect_array *> state_to_pwsucc_m;
  typedef std::vector<state_to_pwsucc_m *> level2pwsucc_map;

  powerset_builder(const_aut_ptr src) :
  src_(src),
  ns_(src_->num_states()),
  nap_(src_->ap().size())
  {
    if ((-1UL / ns_) >> nap_ == 0)
      throw std::runtime_error("too many atomic propositions (or states)");

    // Build a correspondence between conjunctions of APs and unsigned
    // indexes. Fills num2bdd_ and bdd2num_
    num2bdd_.reserve(1UL << nap_);
    bdd all = bddtrue;
    bdd allap = src_->ap_vars();
    while (all != bddfalse)
      {
        bdd one = bdd_satoneset(all, allap, bddfalse);
        all -= one;
        bdd2num_.emplace(one, num2bdd_.size());
        num2bdd_.emplace_back(one);
      }

    nc_ = num2bdd_.size();        // number of conditions
    assert(nc_ == (1UL << nap_));

    // Initialize the maps for each level
    for (unsigned l = 0; l <= src_->num_sets(); ++l)
    {
      pw_storage->emplace_back(new state_to_pwsucc_m());
    }

  };

  size_t nc_; // Number of conditions
  std::vector<bdd> num2bdd_;
  std::map<bdd, unsigned, spot::bdd_less_than> bdd2num_;

  // Returns successors of the input state_set under given mark. If the mark
  // is >= src_->num_sets(), no restriction happens.
  //
  // Returns:
  // 0: successors of state_set under num2bdd_[0]-transtions marked by mark
  // 1: successors of state_set under num2bdd_[1]-transtions marked by mark
  //   ...  ...  ... ... ... ... ... ... ... ...
  // nc_-1: successors of state_set under num2bdd_[nc-1]-transtions marked by mark
  std::vector<state_set> * get_succs(state_set, unsigned mark);
  // By default do not restrict to marks == use h+1
  std::vector<state_set> * get_succs(state_set ss) {
    return get_succs(ss, src_->num_sets());
  }

private:
  const_aut_ptr src_; // input automaton
  unsigned ns_; // number of states of input automaton
  unsigned nap_; // number of atomic propositions

  // The storage for precomputed powerset successors of states of `src_`.
  // We have a vector of size src_->num_of_acc_sets()+1 = `h+2`.
  // For each mark we constrain the successors on those under the given mark.
  // The level `h` is not contrained by any mark.
  //
  // The keys are:
  //   level - state - condition -> bitvector
  //
  // For level `l` a l-successor `t` of `s` is a state such that we have a
  // transition marked by `l` from `s` to `t`. For each level, we have a map
  // from states to bitvetor_array. Each bitvect_array looks like this (for 2 AP):
  //   0 ( a &  b) | <bitvector representing l-successors from `s` under  a &  b>
  //   1 (!a &  b) | <bitvector representing l-successors from `s` under !a &  b>
  //   2 ( a & !b) | <bitvector representing l-successors from `s` under  a & !b>
  //   3 (!a & !b) | <bitvector representing l-successors from `s` under !a & !b>
  level2pwsucc_map* pw_storage = new level2pwsucc_map;

  /**
  * Compute bitvector_array for `s` and `mark`
  */
  bitvect_array * compute_bva(state_t, unsigned mark);
};
