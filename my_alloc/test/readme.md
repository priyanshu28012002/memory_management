# ✅ Google Test (GTest)

### 🔁 **1. Understanding Assertions (`ASSERT_` vs `EXPECT_`)**

Google Test provides two main types of assertions that determine the behavior of the test when a failure occurs.

| **Check Type**       | **Description**                                                                 | **Non-Fatal (`EXPECT_`)**             | **Fatal (`ASSERT_`)**               |
|----------------------|---------------------------------------------------------------------------------|---------------------------------------|-------------------------------------|
| **Equality**          | Are the values equal?                                                           | `EXPECT_EQ(val1, val2)`              | `ASSERT_EQ(val1, val2)`             |
| **Inequality**        | Are the values not equal?                                                       | `EXPECT_NE(val1, val2)`              | `ASSERT_NE(val1, val2)`             |
| **Less Than**         | Is `val1` < `val2`?                                                             | `EXPECT_LT(val1, val2)`              | `ASSERT_LT(val1, val2)`             |
| **Less Than or Equal**| Is `val1` ≤ `val2`?                                                             | `EXPECT_LE(val1, val2)`              | `ASSERT_LE(val1, val2)`             |
| **Greater Than**      | Is `val1` > `val2`?                                                             | `EXPECT_GT(val1, val2)`              | `ASSERT_GT(val1, val2)`             |
| **Greater Than or Equal** | Is `val1` ≥ `val2`?                                                          | `EXPECT_GE(val1, val2)`              | `ASSERT_GE(val1, val2)`             |
| **Boolean True**      | Is the condition `true`?                                                       | `EXPECT_TRUE(condition)`             | `ASSERT_TRUE(condition)`            |
| **Boolean False**     | Is the condition `false`?                                                      | `EXPECT_FALSE(condition)`            | `ASSERT_FALSE(condition)`           |
| **C-String Equality** | Are the C-strings equal (`strcmp(lhs, rhs) == 0`)?                              | `EXPECT_STREQ(str1, str2)`           | `ASSERT_STREQ(str1, str2)`          |
| **C-String Inequality**| Are the C-strings different?                                                   | `EXPECT_STRNE(str1, str2)`           | `ASSERT_STRNE(str1, str2)`          |
| **Pointer is NULL**   | Is the pointer `nullptr`?                                                      | `EXPECT_EQ(ptr, nullptr)`            | `ASSERT_EQ(ptr, nullptr)`           |
| **Pointer is NOT NULL** | Is the pointer not `nullptr`?                                                 | `EXPECT_NE(ptr, nullptr)`            | `ASSERT_NE(ptr, nullptr)`           |
| **Float Equality**    | Are two floats equal?                                                          | `EXPECT_FLOAT_EQ(f1, f2)`            |+
### ❗ **Fatal vs. Non-Fatal Assertions**

- **🔴 Fatal Assertion (`ASSERT_`)**  
  - If the assertion **fails**, the **current test function is aborted immediately**.  
  - Example: `ASSERT_EQ(a, b);`  
  - If `a != b`, the test **stops** and no further code in that test function runs.

- **🟡 Non-Fatal Assertion (`EXPECT_`)**  
  - If the assertion **fails**, the test logs the failure but **continues running**.  
  - Example: `EXPECT_EQ(a, b);`  
  - Even if `a != b`, the rest of the test function still runs.

**Usage Tip**:  
Use **`ASSERT_*`** when you want the test to **stop immediately** if a failure occurs. This is useful when continuing after a failure doesn't make sense (e.g., after checking an initial condition that renders the rest of the test invalid).

Use **`EXPECT_*`** when the test can continue and you want to check for multiple conditions in one test. This allows you to log more than one failure per test.

---

### 📌 **C-String Comparisons (`EXPECT_STREQ`, `ASSERT_STREQ`)**

- **`EXPECT_STREQ`** and **`ASSERT_STREQ`** are used to compare **C-style strings** (i.e., `const char*`).
- They compare **string contents** rather than pointers.

#### Example:

```cpp
TEST(StringTests, CStringComparison) {
    const char* expected = "hello";
    const char* actual = strdup("hello");

    EXPECT_STREQ(expected, actual);     // Passes if content is the same
    ASSERT_STRNE(expected, "world");    // Fails and stops if equal
}
```

- `EXPECT_STREQ` compares two **null-terminated strings** to check if they are equal by their content.
- `ASSERT_STRNE` is used to check that two strings are **not equal**.

---

### 🧩 **2. Steps to Write a Good Unit Test**

Use the **AAA Pattern**: Arrange, Act, Assert.

| **Step**            | **What to do**                                                                 |
|---------------------|---------------------------------------------------------------------------------|
| **Arrange**         | Set up inputs, variables, and dependencies                                      |
| **Act (Run Test)**  | Call the function or system under test                                         |
| **Assert**          | Check expected vs actual results using assertions                              |
| **Independent**     | Keep tests **independent**—no shared state or dependencies between tests       |

---

### 🧪 **3. Example Test Structure**

#### A simple unit test:

```cpp
TEST(MyFunctionTest, ReturnsExpectedValue) {
    // Arrange
    int input = 5;
    int expected_output = 25;

    // Act
    int actual_output = Square(input);

    // Assert
    EXPECT_EQ(actual_output, expected_output);
}
```

This test follows the **AAA Pattern**:
1. **Arrange**: Setup the inputs and expected output.
2. **Act**: Call the function being tested (`Square` in this case).
3. **Assert**: Use assertions to check if the actual result matches the expected result.

---

### 🧱 **4. Test Fixtures with `TEST_F`**

Use **test fixtures** when:
- You need to reuse setup/teardown code across multiple tests.
- You want to test the same setup under different conditions.

#### ✅ Example:

```cpp
class CalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        calc = new Calculator();
    }

    void TearDown() override {
        delete calc;
    }

    Calculator* calc;
};

TEST_F(CalculatorTest, AddsCorrectly) {
    EXPECT_EQ(calc->Add(2, 3), 5);
}

TEST_F(CalculatorTest, SubtractsCorrectly) {
    EXPECT_EQ(calc->Subtract(5, 3), 2);
}
```

**`TEST_F`** allows you to reuse the test fixture (`CalculatorTest`) for multiple test cases. Each test case (`AddsCorrectly`, `SubtractsCorrectly`) shares the same setup and teardown code.

---

### 🧪 **5. Mocking (Using `Mock` Objects)**

Mocking is used to:
- Test interactions with **APIs, interfaces, or dependencies**.
- Verify **calls, arguments, and behavior**, not just outputs.

Used with frameworks like **Google Mock**:

```cpp
class MockDatabase : public DatabaseInterface {
public:
    MOCK_METHOD(bool, Save, (const std::string& data), (override));
};

TEST(SaveTest, CallsDatabaseSave) {
    MockDatabase mockDb;
    EXPECT_CALL(mockDb, Save("data123")).Times(1);

    MyService service(&mockDb);
    service.SaveData("data123");
}
```

**Google Mock** allows you to mock classes and check if specific methods were called during the test.

---

### 🧠 **Tips for Writing Solid Tests**:
- **Descriptive Test Names**: Give your tests names like `HandlesEmptyInput`, `ReturnsErrorOnNull` to make their purpose clear.
- **Focused Tests**: Keep each test **focused** on one behavior.
- **Use Test Fixtures**: For shared setup/teardown across tests.
- **Mock Dependencies**: Use mocks for external dependencies to isolate the functionality being tested.
- **Test Both Success and Failure Cases**: Ensure your tests check both expected and unexpected scenarios.

---

### 💡 **TL;DR**:
- **Use `ASSERT_*`** when the test **cannot continue** if the check fails.
- **Use `EXPECT_*`** when the test **can still continue** after the failure and you want to check multiple things in one test.

---