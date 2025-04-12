#pragma once

#include <memory>

namespace BPT::Detail {
template <class Data>
class ConstValue {
 public:
  ConstValue() = default;
  ConstValue(Data&& data)
      : data_(std::make_shared<const Data>(std::move(data))) {}
  const Data* operator->() const { return data_.get(); }
  bool operator!() const { return data_ == nullptr; }

 private:
  std::shared_ptr<const Data> data_;
};
}  // namespace BPT::Detail
