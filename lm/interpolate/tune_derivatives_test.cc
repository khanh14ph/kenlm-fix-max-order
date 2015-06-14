#include "lm/interpolate/tune_derivatives.hh"

#include "lm/interpolate/tune_instance.hh"

#define BOOST_TEST_MODULE DerivativeTest
#include <boost/test/unit_test.hpp>

namespace lm { namespace interpolate { namespace {

BOOST_AUTO_TEST_CASE(Small) {
  // Three vocabulary words, two models.
  Matrix unigrams(3, 2);
  unigrams <<
    0.1, 0.6,
    0.4, 0.3,
    0.5, 0.1;
  unigrams = unigrams.array().log();

  // One instance
  util::FixedArray<Instance> instances(1);
  instances.push_back(2);
  Instance &instance = instances.back();

  instance.ln_backoff << 0.2, 0.4;
  instance.ln_backoff = instance.ln_backoff.array().log();


  // Sparse cases: model 0 word 2 and model 1 word 1.

  // Assuming that model 1 only matches word 1, this is p_1(1 | context)
  Accum model_1_word_1 = 1.0 - .6 * .4 - .1 * .4;

  // We'll suppose correct has WordIndex 1, which backs off in model 0, and matches in model 1
  instance.ln_correct << (0.4 * 0.2), model_1_word_1;
  instance.ln_correct = instance.ln_correct.array().log();

  Accum model_0_word_2 = 1.0 - .1 * .2 - .4 * .2;

  instance.extension_words.push_back(1);
  instance.extension_words.push_back(2);
  instance.ln_extensions.resize(2, 2);
  instance.ln_extensions <<
    (0.4 * 0.2), model_1_word_1,
    model_0_word_2, 0.1 * 0.4;
  instance.ln_extensions = instance.ln_extensions.array().log();

  ComputeDerivative compute(instances, unigrams);
  Vector weights(2);
  weights << 0.9, 1.2;

  Vector gradient(2);
  Matrix hessian(2,2);
  compute.Iteration(weights, gradient, hessian);
}

}}} // namespaces