#pragma once
#include <Core/Container/String.h>

#include <iostream>

inline std::ostream& operator<<(std::ostream& out, Core::String<char> const& s)
{
  if (s.IsEmpty())
    return out;
  return out << s.Data();
}

inline std::ostream& operator<<(std::ostream& out, Core::StringView<char> const& s)
{
  return out << Core::String<char>(s);
}