#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>

/*
The paragraph below is taken from this stackoverflow thread:
https://stackoverflow.com/questions/30186405/specifying-output-string-argument-with-googlemock

In general, in order to better understand these actions it helps to think of them as operations over the argument arg:

SetArgPointee(value) is essentially *arg = value (arg must be a pointer)
SetArgReferee(value) is arg = value (arg must be a reference)
SetArrayArgument(first, last) is memcpy(arg, first, last - first) (arg must be a pointer)
SaveArg(ptr) is *ptr = arg
SaveArgPointee(ptr) is *ptr = *arg (arg must be a pointer)
*/

class X
{
public:
  virtual void increment_using_addr(int * x) = 0;
  virtual void increment_using_reference(int& x) = 0;
  virtual void set_buffer(char * buf) = 0;
  virtual int set_return_value(void) = 0;
  virtual int do_all(int * a, int& b, char * buf) = 0;
};

class MockX : public X
{
public:
  /* MOCK_METHODN with N is equal to the number of the arguments*/
  MOCK_METHOD1(increment_using_addr, void(int * x));
  MOCK_METHOD1(increment_using_reference, void(int& x));
  MOCK_METHOD1(set_buffer, void(char * buf));
  MOCK_METHOD0(set_return_value, int(void));
  MOCK_METHOD3(do_all, int(int * a, int& b, char * buf));
};

//modify a parameter using a pointer
TEST(ControlBehaviour, increment_using_addr)
{
  //arrange
  MockX x;
  int value = 9, incrementedValue = 10;
  EXPECT_CALL(x, increment_using_addr(testing::_)).Times(1).WillOnce(testing::SetArgPointee<0>(incrementedValue));

  //act
  std::cout << "value before increment : " << value << '\n';
  x.increment_using_addr(&value);
  std::cout << "value after output : " << value <<'\n';

  //assert
  ASSERT_EQ(value, incrementedValue);
}

//modify a parameter using a reference
TEST(ControlBehaviour, increment_using_reference)
{
  //arrange
  MockX x;
  int value = 9, incrementedValue = 10;
  EXPECT_CALL(x, increment_using_reference(testing::_)).Times(1).WillOnce(testing::SetArgReferee<0>(incrementedValue));

  //act
  std::cout << "value before increment : " << value << '\n';
  x.increment_using_reference(value);
  std::cout << "value after output : " << value <<'\n';

  //assert
  ASSERT_EQ(value, incrementedValue);
}

//modify the content of a buffer
TEST(ControlBehaviour, set_buffer)
{
  //arrange
  MockX x;
  char buffer[10] = {0};
  char resultBuffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_CALL(x, set_buffer(testing::_)).Times(1).WillOnce(testing::SetArrayArgument<0>(resultBuffer, resultBuffer + sizeof(resultBuffer)));

  //act
  x.set_buffer(buffer);

  //assert
  ASSERT_EQ(memcmp(buffer, resultBuffer, sizeof(buffer)), 0);
}

//set return value
TEST(ControlBehaviour, set_return_value)
{
  //arrange
  MockX x;
  int returnValue = 0, valueThatShouldBeReturned = 10;
  EXPECT_CALL(x, set_return_value()).Times(1).WillOnce(testing::Return(valueThatShouldBeReturned));

  //act
  returnValue = x.set_return_value();

  //assert
  ASSERT_EQ(returnValue, valueThatShouldBeReturned);
}

//do all the previous tests in one test
TEST(ControlBehaviour, do_all)
{
  //arrange
  MockX x;
  int value = 9, incrementedValue1 = 10, incrementedValue2 = 11;
  char buffer[10] = {0};
  char resultBuffer[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int returnValue = 0, valueThatShouldBeReturned = 10;
  EXPECT_CALL(x, do_all(testing::_, testing::_, testing::_)).Times(1).
  WillOnce(DoAll(
  testing::SetArgPointee<0>(incrementedValue1), // 0 is the index of the pointer parameter
  testing::SetArgReferee<1>(incrementedValue2), // 1 is the index of the reference
  testing::SetArrayArgument<2>(resultBuffer, resultBuffer + sizeof(resultBuffer)), // 2 is the index of the buffer
  testing::Return(valueThatShouldBeReturned)
  ));

  //act
  returnValue = x.do_all(&value, value, buffer);

  //assert
  ASSERT_EQ(value, incrementedValue2);
  ASSERT_EQ(memcmp(buffer, resultBuffer, sizeof(buffer)), 0);

}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
