#include <json.hpp>

#include <sstream>
#include <string>

#include <gtest/gtest.h>

template <typename T>
std::string stringify(const T &value) {
  std::ostringstream strm;
  strm << value;
  return strm.str();
}

TEST(Json, Boolean) {
  EXPECT_EQ("true", stringify(json::boolean(true)));
  EXPECT_EQ("false", stringify(json::boolean(false)));
}

TEST(Json, Number) {
  EXPECT_EQ("42", stringify(json::number(42)));
  EXPECT_EQ("4.2", stringify(json::number(4.2)));
}

TEST(Json, String) {
  EXPECT_EQ("\"hello\"", stringify(json::string("hello")));
  EXPECT_EQ("\"world\"", stringify(json::string(std::string("world"))));
}

TEST(Json, ArrayOfBoolean) {
  auto booleans = json::array(json::boolean);
  EXPECT_EQ("[true,true,false]",
            stringify(booleans(std::vector<bool>{true, true, false})));
}

TEST(Json, ArrayOfNumbers) {
  auto numbers = json::array(json::number);
  EXPECT_EQ("[1.1,2.2]", stringify(numbers(std::vector<double>{1.1, 2.2})));
  EXPECT_EQ("[101,202]", stringify(numbers(std::vector<int>{101, 202})));
}

TEST(Json, ArrayOfString) {
  auto strings = json::array(json::string);
  auto expected = "[\"hello\",\"world\"]";
  EXPECT_EQ(expected,
            stringify(strings(std::vector<std::string>{"hello", "world"})));
  EXPECT_EQ(expected,
            stringify(strings(std::vector<const char *>{"hello", "world"})));
}

TEST(Json, ArrayOfArrayOfBooleans) {
  auto booleans = json::array(json::array(json::boolean));
  EXPECT_EQ("[[true,true],[false,true,false],[false]]",
            stringify(booleans(std::vector<std::vector<bool>>{
                {true, true}, {false, true, false}, {false}})));
}

TEST(Json, ArrayOfArrayOfNumbers) {
  auto numbers = json::array(json::array(json::number));
  EXPECT_EQ("[[1.1,2.2],[3.3,4.4]]",
            stringify(numbers(
                std::vector<std::vector<double>>{{1.1, 2.2}, {3.3, 4.4}})));
  EXPECT_EQ("[[101,202],[303,404,505]]",
            stringify(numbers(std::vector<std::vector<std::size_t>>{
                {101, 202}, {303, 404, 505}})));
}

TEST(Json, ArrayOfArrayOfStrings) {
  auto strings = json::array(json::array(json::string));
  auto expected = R"~~([["x","y"],["a","b","c"]])~~";
  EXPECT_EQ(expected,
            stringify(strings(std::vector<std::vector<std::string>>{
                {"x", "y"}, {"a", "b", "c"}})));
  EXPECT_EQ(expected,
            stringify(strings(std::vector<std::vector<const char *>>{
                {"x", "y"}, {"a", "b", "c"}})));
}

struct Resource {
  Resource(const std::string &name, const std::string &role)
      : name_(name), role_(role) {}
  std::string name_;
  const std::string &role() const { return role_; }
  private:
  std::string role_;
};

struct Framework {
  std::string id;
  std::string name;
  bool checkpoint;
  std::vector<Resource> resources;
};

TEST(Json, SummaryObject) {
  auto framework = json::object<Framework>(
    json::field("ID", &Framework::id, json::string),
    json::field("Name", &Framework::name, json::string)
  );
  EXPECT_EQ(
      R"~~({"ID":"framework","Name":"summary"})~~",
      stringify(framework(Framework{
          "framework", "summary", true, {{"cpus", "ads"}, {"mem", "ads"}}})));
}

TEST(Json, ElaborateObject) {
  auto resource = json::object<Resource>(
    json::field("name", &Resource::name_, json::string),
    json::field("role", &Resource::role, json::string)
  );
  auto framework = json::object<Framework>(
    json::field("id", &Framework::id, json::string),
    json::field("name", &Framework::name, json::string),
    json::field("checkpoint", &Framework::checkpoint, json::boolean),
    json::field("id-name",
                [](const Framework &f) { return f.id + "-" + f.name; },
                json::string),
    json::field("resources", &Framework::resources, json::array(resource))
  );
  EXPECT_EQ(
      R"~~({"id":"framework","name":"summary",)~~"
       R"~~("checkpoint":true,"id-name":"framework-summary",)~~"
       R"~~("resources":[{"name":"cpus","role":"ads"},)~~"
                    R"~~({"name":"mem","role":"ads"}]})~~",
      stringify(framework(Framework{
          "framework", "summary", true, {{"cpus", "ads"}, {"mem", "ads"}}})));
}
