#pragma once

#include <torch/arg.h>
#include <torch/enum.h>
#include <torch/csrc/WindowsTorchApiMacro.h>
#include <torch/types.h>

namespace torch {
namespace nn {

namespace detail {

/// Common options for RNN, LSTM and GRU modules.
struct TORCH_API RNNOptionsBase {
  typedef c10::variant<
    enumtype::kLSTM,
    enumtype::kGRU,
    enumtype::kRNN_TANH,
    enumtype::kRNN_RELU> rnn_options_base_mode_t;

  RNNOptionsBase(rnn_options_base_mode_t mode, int64_t input_size, int64_t hidden_size);
  virtual ~RNNOptionsBase() = default;
  // yf225 TODO: comment
  TORCH_ARG(rnn_options_base_mode_t, mode);
  /// The number of features of a single sample in the input sequence `x`.
  TORCH_ARG(int64_t, input_size);
  /// The number of features in the hidden state `h`.
  TORCH_ARG(int64_t, hidden_size);
  /// The number of recurrent layers (cells) to use.
  TORCH_ARG(int64_t, num_layers) = 1;
  /// Whether a bias term should be added to all linear operations.
  TORCH_ARG(bool, bias) = true;
  /// If true, the input sequence should be provided as `(batch, sequence,
  /// features)`. If false (default), the expected layout is `(sequence, batch,
  /// features)`.
  TORCH_ARG(bool, batch_first) = false;
  /// If non-zero, adds dropout with the given probability to the output of each
  /// RNN layer, except the final layer.
  TORCH_ARG(double, dropout) = 0.0;
  /// Whether to make the RNN bidirectional.
  TORCH_ARG(bool, bidirectional) = false;
};

} // namespace detail

/// Options for RNN modules.
struct TORCH_API RNNOptions {
  typedef c10::variant<enumtype::kTanh, enumtype::kReLU> nonlinearity_t;

  RNNOptions(int64_t input_size, int64_t hidden_size);

  /// The number of expected features in the input `x`
  TORCH_ARG(int64_t, input_size);
  /// The number of features in the hidden state `h`
  TORCH_ARG(int64_t, hidden_size);
  /// Number of recurrent layers. E.g., setting ``num_layers=2``
  /// would mean stacking two RNNs together to form a `stacked RNN`,
  /// with the second RNN taking in outputs of the first RNN and
  /// computing the final results. Default: 1
  TORCH_ARG(int64_t, num_layers) = 1;
  /// The non-linearity to use. Can be either ``torch::kTanh`` or ``torch::kReLU'``. Default: ``torch::kTanh``
  TORCH_ARG(nonlinearity_t, nonlinearity) = torch::kTanh;
  /// If ``false``, then the layer does not use bias weights `b_ih` and `b_hh`.
  /// Default: ``true``
  TORCH_ARG(bool, bias) = true;
  /// If ``true``, then the input and output tensors are provided
  /// as `(batch, seq, feature)`. Default: ``false``
  TORCH_ARG(bool, batch_first) = false;
  /// If non-zero, introduces a `Dropout` layer on the outputs of each
  /// RNN layer except the last layer, with dropout probability equal to
  /// `dropout`. Default: 0
  TORCH_ARG(double, dropout) = 0.0;
  /// If ``true``, becomes a bidirectional RNN. Default: ``false``
  TORCH_ARG(bool, bidirectional) = false;
};

struct TORCH_API LSTMOptions {
  LSTMOptions(int64_t input_size, int64_t hidden_size);

  /// The number of expected features in the input `x`
  TORCH_ARG(int64_t, input_size);
  /// The number of features in the hidden state `h`
  TORCH_ARG(int64_t, hidden_size);
  /// Number of recurrent layers. E.g., setting ``num_layers=2``
  /// would mean stacking two LSTMs together to form a `stacked LSTM`,
  /// with the second LSTM taking in outputs of the first LSTM and
  /// computing the final results. Default: 1
  TORCH_ARG(int64_t, num_layers) = 1;
  /// If ``false``, then the layer does not use bias weights `b_ih` and `b_hh`.
  /// Default: ``true``
  TORCH_ARG(bool, bias) = true;
  /// If ``true``, then the input and output tensors are provided
  /// as (batch, seq, feature). Default: ``false``
  TORCH_ARG(bool, batch_first) = false;
  /// If non-zero, introduces a `Dropout` layer on the outputs of each
  /// LSTM layer except the last layer, with dropout probability equal to
  /// `dropout`. Default: 0
  TORCH_ARG(double, dropout) = 0.0;
  /// If ``true``, becomes a bidirectional LSTM. Default: ``false``
  TORCH_ARG(bool, bidirectional) = false;
};

struct TORCH_API GRUOptions {
  GRUOptions(int64_t input_size, int64_t hidden_size);

  /// The number of expected features in the input `x`
  TORCH_ARG(int64_t, input_size);
  /// The number of features in the hidden state `h`
  TORCH_ARG(int64_t, hidden_size);
  /// Number of recurrent layers. E.g., setting ``num_layers=2``
  /// would mean stacking two GRUs together to form a `stacked GRU`,
  /// with the second GRU taking in outputs of the first GRU and
  /// computing the final results. Default: 1
  TORCH_ARG(int64_t, num_layers) = 1;
  /// If ``false``, then the layer does not use bias weights `b_ih` and `b_hh`.
  /// Default: ``true``
  TORCH_ARG(bool, bias) = true;
  /// If ``true``, then the input and output tensors are provided
  /// as (batch, seq, feature). Default: ``false``
  TORCH_ARG(bool, batch_first) = false;
  /// If non-zero, introduces a `Dropout` layer on the outputs of each
  /// GRU layer except the last layer, with dropout probability equal to
  /// `dropout`. Default: 0
  TORCH_ARG(double, dropout) = 0.0;
  /// If ``true``, becomes a bidirectional GRU. Default: ``false``
  TORCH_ARG(bool, bidirectional) = false;
};

} // namespace nn
} // namespace torch
