# Engine

## Build

Clone the repository with:
```
git clone --recursive https://github.com/Alexelnet/Engine.git
```

Inside `Engine` directory do:

```
cmake -S . -B build
```

or

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

then where 30 represent the number of threads:

```
cmake --build build -j 30
```

before starting the `OpenGL` executable make sure that you place the `shader` and `assets` folders next it.

like this:

```
.
├── assets
├── OpenGL
└── shader
```