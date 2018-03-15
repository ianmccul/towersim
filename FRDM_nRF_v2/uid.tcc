// -*- C++ -*-

namespace detail
{

constexpr uint32_t Prime = 16777619;
constexpr uint32_t Offset = 2166136261;

constexpr
uint32_t hash_fnv32(uint32_t Value, uint32_t const* Beg, uint32_t const* End)
{
   return (Beg == End) ? Value : hash_fnv32((Value ^ (*Beg)) * Prime, Beg+1, End);
}

} // namespace detail

constexpr
uint32_t hash_fnv32(uint32_t const* Beg, uint32_t const* End)
{
   return detail::hash_fnv32(detail::Offset, Beg, End);
}

