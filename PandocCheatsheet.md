---

## Common Documentation Keywords for C++ Block Comments

When documenting C++ code, especially within block comments (`/** ... */`), using standardized keywords helps in generating clear and structured documentation. Here's a list of widely used documentation keywords:

### General Documentation Tags

- **`@brief`**  
  Provides a short, concise description of a function, class, or variable.

  ```cpp
  /**
   * @brief Calculates the sum of two integers.
   */
  int add(int a, int b);
  ```

- **`@details`**  
  Offers a more detailed explanation following the brief description.

  ```cpp
  /**
   * @brief Calculates the sum of two integers.
   * @details This function takes two integers as input and returns their sum. It handles both positive and negative values.
   */
  int add(int a, int b);
  ```

- **`@param`**  
  Describes a function parameter. Each parameter should have its own `@param` tag.

  ```cpp
  /**
   * @brief Calculates the sum of two integers.
   * @param a The first integer.
   * @param b The second integer.
   * @return The sum of a and b.
   */
  int add(int a, int b);
  ```

- **`@return`**  
  Describes the return value of a function.

  ```cpp
  /**
   * @brief Retrieves the user's name.
   * @return A string containing the user's name.
   */
  std::string getUserName();
  ```

- **`@throws`** / **`@exception`**  
  Documents exceptions that a function may throw.

  ```cpp
  /**
   * @brief Opens a file.
   * @param filename The name of the file to open.
   * @throws std::ifstream::failure If the file cannot be opened.
   */
  void openFile(const std::string& filename);
  ```

- **`@see`**  
  References related functions, classes, or documentation sections.

  ```cpp
  /**
   * @brief Initializes the system.
   * @see shutdownSystem()
   */
  void initializeSystem();
  ```

### Class and Member Documentation

- **`@class`**  
  Documents a class.

  ```cpp
  /**
   * @class Calculator
   * @brief Performs basic arithmetic operations.
   */
  class Calculator {
      // ...
  };
  ```

- **`@struct`**  
  Documents a struct.

  ```cpp
  /**
   * @struct Point
   * @brief Represents a point in 2D space.
   */
  struct Point {
      double x;
      double y;
  };
  ```

- **`@typedef`**  
  Documents a `typedef` or `using` alias.

  ```cpp
  /**
   * @typedef StringList
   * @brief A list of strings.
   */
  typedef std::vector<std::string> StringList;
  ```

- **`@enum`**  
  Documents an enumeration.

  ```cpp
  /**
   * @enum Color
   * @brief Represents basic colors.
   */
  enum Color { RED, GREEN, BLUE };
  ```

- **`@var`**  
  Documents a variable.

  ```cpp
  /**
   * @var const double PI
   * @brief The value of Pi.
   */
  const double PI = 3.141592653589793;
  ```

### Additional Documentation Tags

- **`@note`**  
  Adds a note or important information.

  ```cpp
  /**
   * @brief Connects to the server.
   * @note Ensure that the server address is correct before calling this function.
   */
  void connectToServer(const std::string& address);
  ```

- **`@warning`**  
  Highlights a warning or potential issue.

  ```cpp
  /**
   * @brief Deletes a user account.
   * @warning This action is irreversible.
   */
  void deleteUserAccount(int userId);
  ```

- **`@deprecated`**  
  Marks a function or feature as deprecated.

  ```cpp
  /**
   * @brief Sets the user's age.
   * @deprecated Use setUserAge(int age) instead.
   */
  void setAge(int age);
  ```

- **`@since`**  
  Indicates since which version a feature exists.

  ```cpp
  /**
   * @brief Exports data to JSON.
   * @since 2.0
   */
  void exportToJson(const Data& data);
  ```

- **`@ingroup`**  
  Groups related functions or classes together.

  ```cpp
  /**
   * @brief Initializes the graphics subsystem.
   * @ingroup Graphics
   */
  void initGraphics();
  ```

### Formatting and Code Blocks

- **`@code`** ... **`@endcode`**  
  Denotes a block of code within the documentation.

  ```cpp
  /**
   * @brief Calculates the factorial of a number.
   * @param n The number to calculate the factorial for.
   * @return The factorial of n.
   * @code
   * int result = factorial(5); // result = 120
   * @endcode
   */
  int factorial(int n);
  ```

- **`@image`**  
  Includes an image in the documentation.

  ```cpp
  /**
   * @brief Displays the system architecture.
   * @image html system_architecture.png "System Architecture Diagram"
   */
  void displayArchitecture();
  ```

- **`@table`**  
  Creates a table in the documentation.

  ```cpp
  /**
   * @brief Supported file formats.
   *
   * @table
   * | Format | Description          |
   * |--------|----------------------|
   * | .txt   | Plain text files     |
   * | .json  | JSON formatted files |
   * @endtable
   */
  void listSupportedFormats();
  ```

### Example of Comprehensive Documentation

```cpp
/**
 * @class Calculator
 * @brief Performs basic arithmetic operations.
 *
 * The Calculator class provides methods to perform addition, subtraction,
 * multiplication, and division of integers.
 *
 * @note This class does not handle floating-point numbers.
 *
 * @author
 * Jane Doe
 */
class Calculator {
public:
    /**
     * @brief Adds two integers.
     * @param a The first integer.
     * @param b The second integer.
     * @return The sum of a and b.
     */
    int add(int a, int b);

    /**
     * @brief Subtracts the second integer from the first.
     * @param a The integer to subtract from.
     * @param b The integer to subtract.
     * @return The result of a - b.
     */
    int subtract(int a, int b);

    /**
     * @brief Multiplies two integers.
     * @param a The first integer.
     * @param b The second integer.
     * @return The product of a and b.
     */
    int multiply(int a, int b);

    /**
     * @brief Divides the first integer by the second.
     * @param a The dividend.
     * @param b The divisor.
     * @return The quotient of a / b.
     * @throws std::invalid_argument If b is zero.
     */
    int divide(int a, int b);
};
```

---

## Integrating Documentation Comments with Pandoc

While **Doxygen** is a popular tool for generating documentation from C++ comments, **Pandoc** is a versatile document converter that can handle various formats. However, Pandoc doesn't natively parse Doxygen-style comments. To integrate your C++ documentation comments with Pandoc, consider the following approaches:

### 1. Extract Documentation Comments

Use a tool or script to extract documentation comments from your source code and convert them into a format that Pandoc can process, such as Markdown. Here's a general workflow:

1. **Extraction Script:** Write a script (e.g., in Python) that scans your C++ files for `/** ... */` comments and extracts the content.
2. **Conversion:** Convert the extracted comments into Markdown, translating Doxygen tags to Markdown syntax where possible.
3. **Pandoc Processing:** Use Pandoc to convert the Markdown files into your desired output format (HTML, PDF, etc.).

**Example Python Snippet for Extraction:**

```python
import re

def extract_comments(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    
    # Regex to match /** ... */ comments
    comments = re.findall(r'/\*\*(.*?)\*/', content, re.DOTALL)
    return comments

comments = extract_comments('example.cpp')
for comment in comments:
    print(comment.strip())
```

### 2. Use Pandoc Filters

Develop or utilize existing **Pandoc filters** that can interpret Doxygen-style annotations within your extracted documentation. Filters can transform the abstract syntax tree (AST) of your Markdown to include special formatting based on the annotations.

- **Pandoc Filters Documentation:** [Pandoc Filters](https://pandoc.org/filters.html)
- **Libraries for Writing Filters:**
  - **Python:** [pandocfilters](https://github.com/jgm/pandocfilters)
  - **Lua:** Built-in support in recent Pandoc versions.

### 3. Intermediate Documentation Tools

Leverage tools that convert Doxygen output into Markdown, which can then be processed by Pandoc.

- **doxypypy:** Although primarily for Python, similar tools might exist or can be developed for C++.
- **Custom Tools:** Depending on your project's complexity, creating a custom toolchain might be necessary.

### 4. Maintain Separate Documentation

Alternatively, maintain your documentation in separate Markdown files and use Pandoc alongside Doxygen:

1. **Doxygen for Code Documentation:** Use Doxygen to generate API documentation from your code comments.
2. **Pandoc for General Documentation:** Use Pandoc to handle other documentation aspects (guides, tutorials) in Markdown.
3. **Integrate Outputs:** Combine the outputs from both tools as needed for your project's documentation.

---

## Best Practices for Documentation Comments

To ensure your documentation is clear, maintainable, and effective, consider the following best practices:

1. **Consistency:** Use the same set of tags and formatting throughout your codebase.
2. **Clarity:** Write clear and concise descriptions. Avoid ambiguity.
3. **Completeness:** Document all public interfaces, including classes, functions, parameters, return values, and exceptions.
4. **Examples:** Provide code examples where applicable to illustrate usage.
5. **Update Regularly:** Keep documentation up-to-date with code changes to prevent discrepancies.
6. **Avoid Redundancy:** Don't repeat information that's obvious from the code. Focus on explaining the "why" and any complex logic.

---

## Additional Resources

- **Doxygen Documentation:** [Doxygen Manual](https://www.doxygen.nl/manual/index.html)
- **Pandoc Documentation:** [Pandoc User Guide](https://pandoc.org/MANUAL.html)
- **Markdown Guide:** [Markdown Syntax](https://www.markdownguide.org/basic-syntax/)
- **Pandoc Filters Examples:** [Pandoc Filters GitHub](https://github.com/jgm/pandocfilters)

---

If you have specific requirements or need assistance with a particular aspect of integrating C++ documentation comments with Pandoc, feel free to provide more details, and I’d be happy to help further!