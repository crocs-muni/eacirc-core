#pragma once

#include "variant.h"
#include <cstdint>
#include <pcg/pcg_random.hpp>
#include <random>
#include <utility>

#ifdef PORTABLE_RANDOMNESS
#include <boost/random/uniform_int_distribution.hpp>
#endif

namespace eacirc {
template <typename IntType = uint32_t>
using uniform_int_distribution =
#ifdef PORTABLE_RANDOMNESS
boost::random::uniform_int_distribution<IntType>;
#else
std::uniform_int_distribution<IntType>;
#endif
}

// Seeding multiple different generators
// Behaves differently than std:seed_seq (which returns same data for
// same length inputs in generate())
// https://en.cppreference.com/w/cpp/numeric/random/seed_seq
// https://en.cppreference.com/w/cpp/named_req/SeedSequence
template <typename Generator> struct seed_seq_from {
  using result_type = std::uint_least32_t;

  template <typename... Args>
  seed_seq_from(Args&&... args)
      : _rng(std::forward<Args>(args)...) {}

  seed_seq_from(seed_seq_from const&) = delete;
  seed_seq_from& operator=(seed_seq_from const&) = delete;

  template <typename I> void generate(I beg, I end) {
    for (; beg != end; ++beg)
      *beg = result_type(_rng());
  }

  constexpr std::size_t size() const {
    return (sizeof(typename Generator::result_type) > sizeof(result_type) &&
            Generator::max() > ~std::size_t(0UL))
               ? ~std::size_t(0UL)
               : size_t(Generator::max());
  }

private:
  Generator _rng;
};

// std::seed_seq behaviour, only for seeding one generator, created from Seeder randomness
template <typename Seeder, size_t seed_count=10> struct seed_seq_rng {
    using result_type = std::uint_least32_t;

    template <typename... Args>
    explicit seed_seq_rng(Seeder&& seeder){
        for(size_t i=0; i<seed_count; ++i){
            _seeds[i] = result_type(seeder());
        }
        _seeder = std::make_unique<std::seed_seq>(_seeds);
    }

    seed_seq_rng(seed_seq_rng const&) = delete;
    seed_seq_rng& operator=(seed_seq_rng const&) = delete;

    template <typename I> void generate(I beg, I end) {
        _seeder->generate(beg, end);
    }

    constexpr std::size_t size() const {
        return seed_count;
    }

private:
    std::unique_ptr<std::seed_seq> _seeder;
    result_type _seeds[seed_count]{};
};

// Backward-compatible PCG32 seeding (with older experiments, compiled with GNU GCC)
// Only for seeding one instance of PCG32 generator from another seeder
template <typename Seeder, size_t seed_count=8> struct seed_seq_pcg32 {
    using result_type = std::uint_least32_t;
    const size_t num_values = seed_count;

    template <typename... Args>
    explicit seed_seq_pcg32(Seeder&& seeder){
        seeder.generate(_seeds, _seeds + num_values);
    }

    seed_seq_pcg32(seed_seq_pcg32 const&) = delete;
    seed_seq_pcg32& operator=(seed_seq_pcg32 const&) = delete;

    template <typename I> void generate(I beg, I end) {
        // backward compatibility with old PCG with non-portable seeding from non-compliant
        // generator (compliant = reentrant) https://github.com/imneme/pcg-cpp/issues/67
        // Generates the same seeding sequence as older eacirc-core versions compiled with GNU GCC.
        // Originally, generator was seeded with [x0, x3] from [x0, x1, x2, x3]
        if ((end - beg) != 4) {
            throw std::runtime_error("Invalid use, intended only for seeding PCG32 generators");
        }
        if (use_count > 1) {
            throw std::runtime_error("Invalid use, intended only for one-time seeding");
        }

        *beg = _seeds[0];
        *(beg + 1) = _seeds[1];
        *(beg + 2) = _seeds[6];
        *(beg + 3) = _seeds[7];
        use_count += 1;
    }

    constexpr std::size_t size() const {
        return seed_count;
    }

private:
    result_type _seeds[seed_count]{};
    unsigned use_count = 0;
};


struct polymorphic_generator {
  using result_type = std::uint8_t;

  template <typename Seeder> polymorphic_generator(const std::string& type, Seeder&& seeder) {
    if (type == "mt19937")
      _rng.emplace<std::mt19937>(std::forward<Seeder>(seeder));
    if (type == "pcg32")
      _rng.emplace<pcg32>(std::forward<Seeder>(seeder));
    else
      throw std::runtime_error("requested random generator named \"" + type +
                               "\" is not valid polymorphic generator");
  }

  static result_type min() { return std::numeric_limits<result_type>::min(); }
  static result_type max() { return std::numeric_limits<result_type>::max(); }

  result_type operator()() {
    switch (_rng.index()) {
    case decltype(_rng)::index_of<std::mt19937>():
      return eacirc::uniform_int_distribution<result_type>()(_rng.as<std::mt19937>());
    case decltype(_rng)::index_of<pcg32>():
      return eacirc::uniform_int_distribution<result_type>()(_rng.as<pcg32>());
    }

    throw std::logic_error("cannot call polymorphic generator with undefined generator");
  }

private:
  core::variant<std::mt19937, pcg32> _rng;
};

using default_random_generator = pcg32;
using default_seed_source = seed_seq_from<default_random_generator>;
