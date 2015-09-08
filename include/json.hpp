#include <iostream>
#include <type_traits>
#include <tuple>
#include <string>
#include <vector>

#include <meta/meta.hpp>

#include <apply.hpp>

namespace json {

  /* Forward declaration of schema objects. */
  struct Boolean;
  struct Number;
  struct String;
  template <typename> struct Array;
  template <typename, typename...> struct Object;

  /* `typeof` takes a schema S and returns the corresponding C++ type. */

  template <typename> struct TypeOf;

  template <typename S>
  using typeof = meta::_t<TypeOf<S>>;

  template <>
  struct TypeOf<Boolean> { using type = bool; };

  template <>
  struct TypeOf<Number> { using type = double; };

  template <>
  struct TypeOf<String> { using type = std::string; };

  template <typename S>
  struct TypeOf<Array<S>> { using type = std::vector<typeof<S>>; };

  template <typename T, typename... Fs>
  struct TypeOf<Object<T, Fs...>> { using type = T; };

  /* `is_convertible` returns true iff type T can be converted to schema S. */

  template <typename T, typename S> struct IsConvertible;

  template <typename T, typename S>
  using is_convertible = meta::_t<IsConvertible<T, S>>;

  template <typename T>
  struct IsConvertible<T, Boolean> : std::is_convertible<T, typeof<Boolean>> {};

  template <typename T>
  struct IsConvertible<T, Number> : std::is_convertible<T, typeof<Number>> {};

  template <typename T>
  struct IsConvertible<T, String> : std::is_convertible<T, typeof<String>> {};

  template <typename T, typename S>
  struct IsConvertible<std::vector<T>, Array<S>> : IsConvertible<T, S> {};

  template <typename T, typename U, typename... Fs>
  struct IsConvertible<T, Object<U, Fs...>> : std::is_convertible<T *, U *> {};

  /* `convertible_check` is used to provide a helpful error message. */

  template <typename T, typename U>
  meta::_t<std::enable_if<is_convertible<T, U>{}>> convertible_check() {}

  template <typename T, typename U>
  meta::_t<std::enable_if<!is_convertible<T, U>{}>> convertible_check() = delete;

  /* `Format` is a proxy object that can be passed to an output stream. */

  template <typename S, typename... Ts>
  struct Format;

  // `Format` for Boolean, Number and String.
  template <typename S>
  struct Format<S> {

    const typeof<S> &value;

    private:

    Format(const Format &) = default;
    Format(Format &&) = default;

    friend struct Boolean;
    friend struct Number;
    friend struct String;
  };

  template <typename S, typename V>
  struct Format<Array<S>, V> {

    // The value of type `V` is stored here rather than `typeof<Array<S>>`
    // because `std::vector<T>` is not implicitly convertible to
    // `std::vector<U>` even if `T` is implicitly convertible to `U`.
    const V &value;
    S schema;

    private:

    Format(const Format &) = default;
    Format(Format &&) = default;

    template <typename T>
    friend struct Array;
  };

  template <typename T, typename... Fs>
  struct Format<Object<T, Fs...>> {
    public:

    const typeof<Object<T, Fs...>> &value;
    std::tuple<Fs...> fields;

    private:

    Format(const Format &) = default;
    Format(Format &&) = default;

    template <typename U, typename... Gs>
    friend struct Object;
  };

  /* Schema objects and constructors. */

  struct Boolean {
    Format<Boolean> operator()(const typeof<Boolean> &value) const {
      return {value};
    }
  };

  static constexpr Boolean boolean{};

  struct Number {
    Format<Number> operator()(const typeof<Number> &value) const {
      return {value};
    }
  };

  static constexpr Number number{};

  struct String {
    Format<String> operator()(const typeof<String> &value) const {
      return {value};
    }
  };

  static constexpr String string{};

  template <typename S>
  struct Array {
    template <typename V>
    Format<Array, V> operator()(const V &value) const {
      return {value, schema};
    }

    S schema;
  };

  template <typename S>
  Array<S> array(const S &s) { return {s}; }

  template <typename R, typename T, typename S>
  struct Field {
    void write(std::ostream &strm, const T &value) const {
      strm << json::string(name) << ":" << schema(value.*member);
    }

    const char *name;
    R T::*member;
    S schema;
  };

  template <typename R, typename T, typename S>
  Field<R, T, S> field(const char *name, R T::*member, S schema) {
    convertible_check<R, S>();
    return {name, member, schema};
  }

  template <typename T, typename... Fs>
  struct Object {
    Format<Object> operator()(const typeof<Object> &value) const {
      return {value, fields};
    }

    std::tuple<Fs...> fields;
  };

  template <typename T, typename... Fs>
  Object<T, Fs...> object(Fs... fields) { return {{fields...}}; }

  /* Streamers */

  inline std::ostream &operator<<(std::ostream &strm,
                                  const Format<Boolean> &that) {
    std::ios_base::fmtflags flags = strm.setf(std::ios_base::boolalpha);
    strm << that.value;
    strm.flags(flags);
    return strm;
  }

  inline std::ostream &operator<<(std::ostream &strm,
                                  const Format<Number> &that) {
    return strm << that.value;
  }

  inline std::ostream &operator<<(std::ostream &strm,
                                  const Format<String> &that) {
    return strm << '"' << that.value << '"';
  }

  template <typename S, typename V>
  std::ostream &operator<<(std::ostream &strm,
                           const Format<Array<S>, V> &that) {
    const auto &elems = that.value;
    strm << '[';
    for (const auto &elem : elems) {
      if (&elem != &elems.front()) {
        strm << ',';
      }  // if
      strm << that.schema(elem);
    }  // for
    strm << ']';
    return strm;
  }

  template <typename T>
  struct FieldWriter {
    template <typename F, typename... Fs>
    void operator()(const F &field, const Fs &... fields) const {
      field.write(strm, value);
      int dummy[] = {([&] { strm << ','; fields.write(strm, value); }(), 0)...};
      (void)dummy;
    }

    std::ostream &strm;
    const T &value;
  };

  template <typename T, typename... Fs>
  std::ostream &operator<<(std::ostream &strm,
                           const Format<Object<T, Fs...>> &that) {
    strm << '{';
    apply(FieldWriter<T>{strm, that.value}, that.fields);
    strm << '}';
    return strm;
  }

};  // namespace json
