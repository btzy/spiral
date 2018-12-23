#pragma once

#include <memory>
#include <type_traits>
#include <tuple>

#include <spiral/detail/typedefs.hpp>

namespace spiral {

    /**
     * Represents an untagged union, i.e. a variant that does not know the type that is currently stored.
     * It is a simple wrapper over union that allows get<T>(x) to work.
     * Construction and destruction of this type is trivial, no matter what T... is.  Users of this type should manage their own construction/destruction.
     * This class cannot be copied or moved.  Users have to move/copy the data manually (using get())
     */

    template <typename... T>
    class untagged_union {
    public:
        untagged_union() noexcept {}
        untagged_union(const untagged_union& other) = delete;
        untagged_union(untagged_union&& other) = delete;
        untagged_union& operator=(const untagged_union& other) = delete;
        untagged_union& operator=(untagged_union&& other) = delete;
        ~untagged_union() {}
        template <typename U, typename = std::enable_if_t<std::disjunction_v<std::is_constructible<T, U>...>>>
        untagged_union(U&& u) {
            initialize_impl<0>(std::forward<U>(u));
        }
        template <typename U, typename... Args, typename = std::enable_if_t<std::conjunction<std::is_constructible<U, Args...>, std::disjunction_v<std::is_same<T, U>...>>>>
        U& emplace(Args&&... args) {
            return *(new (&data) U(std::forward<Args>(args)...));
        }
        template <typename U, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
        void destroy() {
            std::destroy_at(reinterpret_cast<U*>(&data));
        }
        template <typename U, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
        U& get() & noexcept {
            return reinterpret_cast<U&>(data);
        }
        template <typename U, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
        const U& get() const & noexcept {
            return reinterpret_cast<const U&>(data);
        }
        template <typename U, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
        U&& get() && noexcept {
            return reinterpret_cast<U&&>(data);
        }

    private:
        template <size_t Index, typename... Args>
        inline auto& initialize_impl(Args&&... args) {
            static_assert(Index != sizeof...(T), "Bug in untagged_union; overload should not be chosen if unconstructible from U.");
            using current_element_type = std::tuple_element_t<Index, std::tuple<T...>>;
            if constexpr (std::is_constructible_v<current_element_type, Args...>) {
                return *(new (&data) current_element_type(std::forward<Args>(args)...));
            }
            else {
                return initialize_impl<Index + 1>(std::forward<Args>(args)...);
            }
        }

        std::aligned_union_t<0, T...> data;
    };
    template <typename U, typename... T, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
    inline U& get(untagged_union<T...>& u) noexcept {
        return u.get();
    }
    template <typename U, typename... T, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
    inline const U& get(const untagged_union<T...>& u) noexcept {
        return u.get();
    }
    template <typename U, typename... T, typename = std::enable_if_t<std::disjunction_v<std::is_same<T, U>...>>>
    inline U&& get(untagged_union<T...>&& u) noexcept {
        return std::move(u).get();
    }
}
