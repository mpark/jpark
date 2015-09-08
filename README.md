# JPark

## Quick Start

This project includes git submodules, use the `--recursive` flag when cloning.

```bash
git clone --recursive git@github.com:mpark/jpark.git
```

## Major Concepts

### Static JSON Schema

The following are the currently supported JSON schemas:

* `json::boolean`
* `json::number`
* `json::string`
* `json::array(<schema>)`  e.g. `json::array(json::number)`
* `json::object<Type>(json::field(<name>, <source>, <schema>), ...)`
  e.g. `json::object<Person>(json::field("Name", &Person::name, json::string))`

### Apply the schema to an instance

All of the schemas mentioned above has an `operator()` which projects the schema
onto an instance. For example, we can apply the `json::boolean` onto `true`:

`std::cout << json::boolean(true)  // prints: true`

## Example

```cpp
#include <json.hpp>

#include <string>
#include <vector>

struct Person {
  std::string first_name;
  std::string last_name;
  double height;
  std::vector<std::string> friends;
};

int main() {
  Person person{"J", "Park", 180, {"Moris"}};
  // Only extract the first/last name.
  auto name = json::object<Person>(
    json::field("first name", &Person::first_name, json::string),
    json::field("last name", &Person::last_name, json::string)
  );
  // {"first name":"J","last name":"Park"}
  std::cout << name(person) << std::endl;
  // Full info.
  auto full = json::object<Person>(
    json::field("given name", &Person::first_name, json::string),
    json::field("surname", &Person::last_name, json::string),
    json::field("height", &Person::height, json::number),
    json::field("friends", &Person::friends, json::array(json::string))
  );
  // {"given name":"J","surname":"Park","height":180,"friends":["Moris"]}
  std::cout << full(person) << std::endl;
}
```

## Requirements

This library requires a standard conformant __C++11__ compiler.

The following configurations were successfully tested:

* __Mac OS X Yosemite__: Clang 3.5, 3.6, _Apple_ (with libc++)
