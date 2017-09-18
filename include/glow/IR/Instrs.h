#ifndef GLOW_IR_INSTRS_H
#define GLOW_IR_INSTRS_H

#include "glow/IR/IR.h"
#include "glow/IR/Type.h"

namespace glow {

class CopyInst : public Instruction {
public:
  CopyInst(Value *dest, Value *src)
      : Instruction(Kinded::Kind::CopyInstKind,
                    {{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::CopyInstKind;
  }
  void verify();
};

class ConvolutionInst : public Instruction {
  size_t kernel_;
  size_t stride_;
  size_t pad_;
  size_t depth_;

public:
  ConvolutionInst(Value *dest, Value *src, Value *filter, Value *bias,
                  size_t kernel, size_t stride, size_t pad, size_t depth)
      : Instruction(Kinded::Kind::ConvolutionInstKind,
                    {{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {filter, OperandKind::kIn},
                     {bias, OperandKind::kIn}}),

        kernel_(kernel), stride_(stride), pad_(pad), depth_(depth) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::ConvolutionInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class PoolInst : public Instruction {
public:
  /// Specifies the kind of pooling done by the operator.
  enum class OpKind {
    kMax,
    kAvg,
  };

private:
  size_t kernel_;
  size_t stride_;
  size_t pad_;
  OpKind kind_;

  const char *getKindStr();

public:
  PoolInst(Value *dest, Value *src, Value *srcXY, OpKind kind, size_t kernel,
           size_t stride, size_t pad)
      : Instruction(Kinded::Kind::PoolInstKind, {{dest, OperandKind::kOut},
                                                 {src, OperandKind::kIn},
                                                 {srcXY, OperandKind::kInOut}}),
        kernel_(kernel), stride_(stride), pad_(pad), kind_(kind) {}
  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::PoolInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class FullyConnectedInst : public Instruction {
  size_t depth_;

public:
  FullyConnectedInst(Value *dest, Value *src, Value *filter, Value *bias,
                     size_t depth)
      : Instruction(Kinded::Kind::FullyConnectedInstKind,
                    {{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {filter, OperandKind::kIn},
                     {bias, OperandKind::kIn}}),
        depth_(depth) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::FullyConnectedInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class ReluInst : public Instruction {
public:
  ReluInst(Value *dest, Value *src)
      : Instruction(Kinded::Kind::ReluInstKind,
                    {{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::ReluInstKind;
  }
  void verify();
};

class SigmoidInst : public Instruction {
public:
  SigmoidInst(Value *dest, Value *src)
      : Instruction(Kinded::Kind::SigmoidInstKind,
                    {{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::SigmoidInstKind;
  }
  void verify();
};

class TanhInst : public Instruction {
public:
  TanhInst(Value *dest, Value *src)
      : Instruction(Kinded::Kind::TanhInstKind,
                    {{dest, OperandKind::kOut}, {src, OperandKind::kIn}}) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::TanhInstKind;
  }
  void verify();
};

class SoftMaxInst : public Instruction {
public:
  SoftMaxInst(Value *dest, Value *src, Value *expected)
      : Instruction(Kinded::Kind::SoftMaxInstKind,
                    {{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {expected, OperandKind::kIn}}) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::SoftMaxInstKind;
  }
  void verify();
};

class RegressionInst : public Instruction {
public:
  RegressionInst(Value *dest, Value *src, Value *expected)
      : Instruction(Kinded::Kind::RegressionInstKind,
                    {{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {expected, OperandKind::kIn}}) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::RegressionInstKind;
  }
  void verify();
};

class TransposeInst : public Instruction {
  std::vector<unsigned> shuffle_;

public:
  TransposeInst(Value *dest, Value *src, ArrayRef<unsigned> shuffle)
      : Instruction(Kinded::Kind::TransposeInstKind,
                    {{dest, OperandKind::kOut}, {src, OperandKind::kIn}}),
        shuffle_(shuffle.begin(), shuffle.end()) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::TransposeInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class ReshapeInst : public Instruction {
  std::vector<size_t> dims_;

public:
  ReshapeInst(Value *dest, Value *src, ArrayRef<size_t> dims)
      : Instruction(Kinded::Kind::ReshapeInstKind,
                    {{dest, OperandKind::kOut}, {src, OperandKind::kIn}}),
        dims_(dims.begin(), dims.end()) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::ReshapeInstKind;
  }

  std::string getExtraDesc();
  void verify();
};

class ConcatInst : public Instruction {
  /// We concat the tensors along this dimension.
  size_t dim_;

public:
  ConcatInst(Value *dest, ArrayRef<Value *> src, size_t dim)
      : Instruction(Kinded::Kind::ConcatInstKind, {{dest, OperandKind::kOut}}),
        dim_(dim) {
    for (auto s : src) {
      pushOperand({s, OperandKind::kIn});
    }
  }

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::ConcatInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class BatchNormalizationInst : public Instruction {
  const size_t channelIdx_;
  const float epsilon_;
  const float momentum_;

public:
  BatchNormalizationInst(Value *dest, Value *src, Value *scale, Value *bias,
                         Value *mean, Value *var, size_t channelIdx,
                         float epsilon, float momentum)
      : Instruction(Kinded::Kind::BatchNormalizationInstKind,
                    {{dest, OperandKind::kOut},
                     {src, OperandKind::kIn},
                     {scale, OperandKind::kIn},
                     {bias, OperandKind::kIn},
                     {mean, OperandKind::kInOut},
                     {var, OperandKind::kInOut}}),
        channelIdx_(channelIdx), epsilon_(epsilon), momentum_(momentum) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::BatchNormalizationInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class ArithmeticInst : public Instruction {
public:
  /// Specifies the kind of pooling done by the operator.
  enum class OpKind {
    kAdd,
    kMul,
  };

private:
  OpKind kind_;
  const char *getKindStr();

public:
  ArithmeticInst(Value *dest, Value *LHS, Value *RHS, OpKind kind)
      : Instruction(Kinded::Kind::ArithmeticInstKind,
                    {{dest, OperandKind::kOut},
                     {LHS, OperandKind::kIn},
                     {RHS, OperandKind::kIn}}),
        kind_(kind) {}
  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::ArithmeticInstKind;
  }
  std::string getExtraDesc();
  void verify();
};

class StaticVariable : public Value {
public:
  enum class InitKind {
    kExtern,    // No initialization.
    kBroadcast, // Broadcast a single value to all elements.
    kXavier,    // Init the tensor with random values using the Xavier method.
  };

private:
  /// The value to use during initialization. This can be the value to splat or
  /// a parameter to specify the range of the random values.
  float val_;

  /// The initialization mode.
  InitKind mode_;

  const char *getKindStr();

public:
  StaticVariable(TypeRef Ty, InitKind mode, float val)
      : Value(Ty, Kinded::Kind::StaticVariableKind), val_(val), mode_(mode) {}

  static bool classof(const Kinded *k) {
    return k->getKind() == Kinded::Kind::StaticVariableKind;
  }
  InitKind getMode() { return mode_; }
  float getVal() { return val_; }
  std::string getExtraDesc();
  void verify() {}
};

} // namespace glow

#endif // GLOW_IR_INSTRS_H
