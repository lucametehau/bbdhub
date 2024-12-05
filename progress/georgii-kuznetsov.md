# My Progress

#### Board Class Basics
```cpp
// board.h 
class Board {
    // Core functionality implemented
    // make move method
    // undo move method
    // sotring board via bitboard classes and array of pieces
};
```

#### Color
```cpp
// color.h 
- color flip function
```

#### Bitboard
```cpp
// birboard.h 
- set_bit function
```

#### Piece
```cpp
// piece.h
- Copy Constructor
- Copy Assignment Operator
- Destructor
- NO_PIECE piece
```

#### Cmake
``` Cmake
# CmakeFile.txt
- added testing with enable_tesing()
    now can run tests with 'ctest'
    
- run format-check:
    cmake --build . --target format-check

- run formatting:
    cmake --build . --target format-check

```

#### Testing
``` cpp
to add your files to testing system, add the following lines:

// tests/CmakeFile.txt
add_executable(name_of_the_test1 testing_file1.cpp)
add_executable(name_of_the_test2 testing_file2.cpp)
add_executable(name_of_the_test3 testing_file3.cpp)
...

target_include_directories(name_of_the_tests1 name_of_the_tests2 name_of_the_tests3 ... PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME "test 1" COMMAND name_of_the_tests1)
add_test(NAME "test 2" COMMAND name_of_the_tests2)
add_test(NAME "test 3" COMMAND name_of_the_tests3)
... 

set_tests_properties("test 1" "test 2" "test 3" ...  PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

```

### 🔄 in Progress

- chilling

---

## Last Updated: December 5, 2024