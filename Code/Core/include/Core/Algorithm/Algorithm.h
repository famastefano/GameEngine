#pragma once

#include <Core/Definitions.h>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <type_traits>

namespace Core::Algorithm
{
namespace Private
{
template <std::input_iterator InputIterator, typename OutputIterator>
__declspec(noalias) void FastCopy(InputIterator fromStart, InputIterator fromEnd, OutputIterator to)
{
  u64 const fromStartAddr = (u64)fromStart;
  u64 const fromEndAddr   = (u64)fromEnd;
  u64 const toAddr        = (u64)to;
  if (fromStartAddr <= toAddr && toAddr <= fromEndAddr)
    std::memmove(to, fromStart, (fromEnd - fromStart) * sizeof(*fromStart));
  else
    std::memcpy(to, fromStart, (fromEnd - fromStart) * sizeof(*fromStart));
}

template <std::input_iterator InputIterator, typename OutputIterator>
constexpr bool CanFastCopy()
{
  if constexpr (std::is_pointer_v<InputIterator> && std::is_pointer_v<OutputIterator>)
  {
    using InputType  = std::remove_pointer_t<InputIterator>;
    using OutputType = std::remove_pointer_t<OutputIterator>;
    return std::is_same_v<InputType, OutputType>
        && std::is_trivially_copyable_v<InputType>
        && std::is_trivially_copyable_v<OutputType>;
  }
  return false;
}
} // namespace Private

template <std::input_iterator InputIterator, typename OutputIterator>
void Move(InputIterator fromStart, InputIterator fromEnd, OutputIterator to)
{
  static_assert(std::output_iterator<OutputIterator, decltype(std::move(*fromStart))>);
  if constexpr (Private::CanFastCopy<InputIterator, OutputIterator>())
  {
    Private::FastCopy(fromStart, fromEnd, to);
  }
  else
  {
    while (fromStart != fromEnd)
      *to++ = std::move(*fromStart++);
  }
}

template <std::input_iterator InputIterator, typename OutputIterator>
void Copy(InputIterator fromStart, InputIterator fromEnd, OutputIterator to)
{
  if constexpr (Private::CanFastCopy<InputIterator, OutputIterator>())
  {
    Private::FastCopy(fromStart, fromEnd, to);
  }
  else
  {
    while (fromStart != fromEnd)
      *to++ = *fromStart++;
  }
}

template <typename Container, typename Value>
constexpr decltype(auto) BinarySearchEmplace(Container& c, Value&& v)
{
  return c.Emplace(std::lower_bound(std::begin(c), std::end(c), std::forward<Value>(v)), std::forward<Value>(v));
}
} // namespace Core::Algorithm
