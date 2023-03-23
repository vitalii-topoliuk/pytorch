#include <c10/util/ssize.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

namespace c10 {
namespace {

template <typename size_type_>
class Container {
 public:
  using size_type = size_type_;

  constexpr explicit Container(size_type size) : size_(size) {}

  constexpr auto size() const noexcept -> size_type {
    return size_;
  }

 private:
  size_type size_;
};

TEST(ssize_test, size_t) {
  ASSERT_THAT(ssize(Container(std::size_t{3})), testing::Eq(std::ptrdiff_t{3}));
}

TEST(ssize_test, size_t_overflow) {
#if defined(NDEBUG)
  GTEST_SKIP() << "Only valid if assert is enabled" << std::endl;
#endif

  constexpr auto ptrdiff_t_max =
      std::size_t{std::numeric_limits<std::ptrdiff_t>::max()};
  static_assert(ptrdiff_t_max < std::numeric_limits<std::size_t>::max());
  EXPECT_DEATH(
      ssize(Container(ptrdiff_t_max + 1)),
      testing::HasSubstr("greater_than_max"));
}

TEST(ssize_test, small_container_promotes_to_ptrdiff_t) {
  auto signed_size = ssize(Container(std::uint16_t{3}));
  static_assert(std::is_same_v<decltype(signed_size), std::ptrdiff_t>);
  ASSERT_THAT(signed_size, testing::Eq(3));
}

TEST(ssize_test, promotes_to_64_bit_on_32_bit_platform) {
  if (sizeof(std::intptr_t) != 4) {
    GTEST_SKIP() << "Only valid in 64-bits." << std::endl;
  }

  auto signed_size = ssize(Container(std::uint64_t{3}));
  static_assert(std::is_same_v<decltype(signed_size), std::int64_t>);
  ASSERT_THAT(signed_size, testing::Eq(3));
}

} // namespace
} // namespace c10
