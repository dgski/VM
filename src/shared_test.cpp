#include "shared.hpp"
#include <gtest/gtest.h>

TEST(shared_test, isNegative)
{
  const int8_t value = -1;
  const uint8_t unsignedValue = value;
  EXPECT_TRUE(isNegative(unsignedValue, 8));
  EXPECT_TRUE(isNegative(unsignedValue, 4));
  EXPECT_TRUE(isNegative(unsignedValue, 2));
}

TEST(shared_test, setBit)
{
  const auto value = setBit(setBit(0, 0), 7);
  EXPECT_EQ(value, 0b10000001);
}

TEST(shared_test, getSignedValue)
{
  EXPECT_EQ(getSignedValue(0b00001110, 1, 3), -1);
  EXPECT_EQ(getSignedValue(0b00000110, 1, 3), 3);
}

TEST(shared_test, getValue)
{
  EXPECT_EQ(getValue(0b11100000, 5, 3), 7);
  EXPECT_EQ(getValue(0b00000000, 0, 8), 0);
}

TEST(shared_test, setValue)
{
  const uint8_t value = 2;
  EXPECT_EQ(setSignedValue(value, 0, 2), 0b00000010);
}

TEST(shared_test, setSignedValue)
{
  const uint8_t value = int8_t(-1);
  EXPECT_EQ(setSignedValue(value, 1, 4), 0b00011110);
}


TEST(shared_test, setInstruction)
{
  const auto instruction = setInstruction(Instruction::JG);
  EXPECT_EQ(instruction, 0b10100000);
}

TEST(shared_test, setRegisters)
{
  const auto destinationRegister = Register::R0;
  const auto sourceRegister = Register::PC;
  EXPECT_EQ(setRegisters(destinationRegister, sourceRegister), 0b00000110);
}

