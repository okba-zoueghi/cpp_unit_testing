# Google Test/Mock

This repository is for my future self in case I forget how to use the google test framework. It implements a very basic unit test example using google test and google mock.

## Build and Install Google Test

```shell
git clone https://github.com/google/googletest.git
cd googletest && mkdir build
cmake ..
make
make install
```

## Build The Example

```shell
g++ test.cpp -lgtest -lgtest_main -lgmock -lpthread

```
