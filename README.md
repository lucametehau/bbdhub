# BBD

This is a chess engine. Don't ask what the name means.

# Compilation

Since we use CMake with clang++, do as follows (do the 1st command only if it's the first time you compile):

```
cmake -DCMAKE_CXX_COMPILER=clang++ -S . -B build/
cmake --build build/
./build/bbd
```

Team:

- luca-mihnea.metehau
- emeric.payer
- georgy.salakhutdinov
- georgii.kuznetsov