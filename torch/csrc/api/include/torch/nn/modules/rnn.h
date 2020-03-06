#pragma once

#include <torch/nn/cloneable.h>
#include <torch/nn/options/rnn.h>
#include <torch/nn/modules/common.h>
#include <torch/nn/modules/dropout.h>
#include <torch/nn/pimpl.h>
#include <torch/nn/utils/rnn.h>
#include <torch/types.h>

#include <ATen/ATen.h>
#include <c10/util/Exception.h>

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

using namespace torch::nn::utils::rnn;

namespace torch {
namespace nn {

namespace detail {
/// Base class for all RNN implementations (intended for code sharing).
template <typename Derived>
class TORCH_API RNNImplBase : public torch::nn::Cloneable<Derived> {
 public:
  explicit RNNImplBase(const RNNOptionsBase& options_);

  /// Initializes the parameters of the RNN module.
  void reset() override;

  void reset_parameters();

  /// Overrides `nn::Module::to()` to call `flatten_parameters()` after the
  /// original operation.
  void to(torch::Device device, torch::Dtype dtype, bool non_blocking = false)
      override;
  void to(torch::Dtype dtype, bool non_blocking = false) override;
  void to(torch::Device device, bool non_blocking = false) override;

  /// Pretty prints the RNN module into the given `stream`.
  void pretty_print(std::ostream& stream) const override;

  /// Modifies the internal storage of weights for optimization purposes.
  ///
  /// On CPU, this method should be called if any of the weight or bias vectors
  /// are changed (i.e. weights are added or removed). On GPU, it should be
  /// called __any time the storage of any parameter is modified__, e.g. any
  /// time a parameter is assigned a new value. This allows using the fast path
  /// in cuDNN implementations of respective RNN `forward()` methods. It is
  /// called once upon construction, inside `reset()`.
  void flatten_parameters();

  std::tuple<Tensor, Tensor> forward(const Tensor& input, Tensor hx = {});
 protected:
  // yf225 TODO: make sure we have test for this one
  FORWARD_HAS_DEFAULT_ARGS({1, AnyValue(Tensor())})

 public:
  // yf225 TODO: make sure we have test for PackedSequence input, and have test for the optional argument in this forward method
  std::tuple<PackedSequence, Tensor> forward_with_packed_input(const PackedSequence& packed_input, Tensor hx = {});

  std::vector<Tensor> all_weights() const;

  /// The RNN's options.
  RNNOptionsBase options;

 protected:
  // Resets _flat_weights
  // Note: be v. careful before removing this, as 3rd party device types
  // likely rely on this behavior to properly .to() modules like LSTM.
  void reset_flat_weights();

  std::vector<std::string> _flat_weights_names;
  std::vector<std::vector<std::string>> _all_weights;
  std::vector<Tensor> _flat_weights;

  void check_input(const Tensor& input, const Tensor& batch_sizes);

  std::tuple<int64_t, int64_t, int64_t> get_expected_hidden_size(const Tensor& input, const Tensor& batch_sizes);

  void check_hidden_size(
    const Tensor& hx,
    std::tuple<int64_t, int64_t, int64_t> expected_hidden_size,
    std::string msg = "Expected hidden size {1}, got {2}");

  void check_forward_args(Tensor input, Tensor hidden, Tensor batch_sizes);

  Tensor permute_hidden(Tensor hx, const Tensor& permutation);

  std::tuple<Tensor, Tensor> forward_helper(
    const Tensor& input,
    const Tensor& batch_sizes,
    const Tensor& sorted_indices,
    int64_t max_batch_size,
    Tensor hx);
};
} // namespace detail

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ RNN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/// A multi-layer Elman RNN module with Tanh or ReLU activation.
/// See https://pytorch.org/docs/master/nn.html#torch.nn.RNN to learn about the
/// exact behavior of this module.
class TORCH_API RNNImpl : public detail::RNNImplBase<RNNImpl> {
 public:
  RNNImpl(int64_t input_size, int64_t hidden_size)
      : RNNImpl(RNNOptions(input_size, hidden_size)) {}
  explicit RNNImpl(RNNOptions options_);
 public:
  RNNOptions options;
};

/// A `ModuleHolder` subclass for `RNNImpl`.
/// See the documentation for `RNNImpl` class to learn what methods it provides,
/// or the documentation for `ModuleHolder` to learn about PyTorch's module
/// storage semantics.
TORCH_MODULE(RNN);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LSTM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/// A multi-layer long-short-term-memory (LSTM) module.
/// See https://pytorch.org/docs/master/nn.html#torch.nn.LSTM to learn about the
/// exact behavior of this module.
class TORCH_API LSTMImpl : public detail::RNNImplBase<LSTMImpl> {
 public:
  LSTMImpl(int64_t input_size, int64_t hidden_size)
      : LSTMImpl(LSTMOptions(input_size, hidden_size)) {}
  explicit LSTMImpl(LSTMOptions options);

  std::tuple<Tensor, std::tuple<Tensor, Tensor>> forward(
    const Tensor& input, torch::optional<std::tuple<Tensor, Tensor>> hx_opt = {});
 protected:
  // yf225 TODO: make sure we have test for this one
  FORWARD_HAS_DEFAULT_ARGS({1, AnyValue(torch::nullopt)})

 public:
  // yf225 TODO: make sure we have test for PackedSequence input, and have test for the optional argument in this forward method
  std::tuple<PackedSequence, std::tuple<Tensor, Tensor>> forward_with_packed_input(
    const PackedSequence& packed_input, torch::optional<std::tuple<Tensor, Tensor>> hx_opt = {});

  LSTMOptions options;

 protected:
  void check_forward_args(const Tensor& input, std::tuple<Tensor, Tensor> hidden, const Tensor& batch_sizes);

  std::tuple<Tensor, Tensor> permute_hidden(std::tuple<Tensor, Tensor> hx, const Tensor& permutation);

  std::tuple<Tensor, std::tuple<Tensor, Tensor>> forward_helper(
    const Tensor& input,
    const Tensor& batch_sizes,
    const Tensor& sorted_indices,
    int64_t max_batch_size,
    torch::optional<std::tuple<Tensor, Tensor>> hx_opt);
};

/// A `ModuleHolder` subclass for `LSTMImpl`.
/// See the documentation for `LSTMImpl` class to learn what methods it
/// provides, or the documentation for `ModuleHolder` to learn about PyTorch's
/// module storage semantics.
TORCH_MODULE(LSTM);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GRU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/// A multi-layer gated recurrent unit (GRU) module.
/// See https://pytorch.org/docs/master/nn.html#torch.nn.GRU to learn about the
/// exact behavior of this module.
class TORCH_API GRUImpl : public detail::RNNImplBase<GRUImpl> {
 public:
  GRUImpl(int64_t input_size, int64_t hidden_size)
      : GRUImpl(GRUOptions(input_size, hidden_size)) {}
  explicit GRUImpl(GRUOptions options_);

  std::tuple<Tensor, Tensor> forward(const Tensor& input, Tensor hx = {});
 protected:
  // yf225 TODO: make sure we have test for this one
  FORWARD_HAS_DEFAULT_ARGS({1, AnyValue(torch::Tensor())})

 public:
  // yf225 TODO: make sure we have test for PackedSequence input, and have test for the optional argument in this forward method
  std::tuple<PackedSequence, Tensor> forward_with_packed_input(const PackedSequence& packed_input, Tensor hx = {});

  GRUOptions options;

 protected:
  std::tuple<Tensor, Tensor> forward_helper(
    const Tensor& input,
    const Tensor& batch_sizes,
    const Tensor& sorted_indices,
    int64_t max_batch_size,
    Tensor hx);
};

/// A `ModuleHolder` subclass for `GRUImpl`.
/// See the documentation for `GRUImpl` class to learn what methods it provides,
/// or the documentation for `ModuleHolder` to learn about PyTorch's module
/// storage semantics.
TORCH_MODULE(GRU);

} // namespace nn
} // namespace torch
