#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

using testing::Return;

enum class PacketSenderErrc {SEND_SUCCEEDED, SEND_FAILED, RECEIVE_SUCCEEDED, RECEIVE_FAILED};

class PacketSender
{
public:
  using Sptr = std::shared_ptr<PacketSender>;

  virtual PacketSenderErrc send() = 0;
};

enum class MessengerErrc { SEND_MESSAGE_FAILED, SEND_MESSAGE_SUCCEEDED, RECEIVE_MESSAGE_FAILED, RECEIVE_MESSAGE_SUCCEEDED, UNEXPECTED_ERROR };

class Messenger
{
private:
  PacketSender::Sptr packetSender;

public:
  Messenger(PacketSender::Sptr ps) : packetSender(ps)
  {

  };

  MessengerErrc sendMessage()
  {
    MessengerErrc mErrc = MessengerErrc::UNEXPECTED_ERROR;

    PacketSenderErrc psErrc = packetSender->send();

    if (psErrc == PacketSenderErrc::SEND_SUCCEEDED)
    {
      mErrc = MessengerErrc::SEND_MESSAGE_SUCCEEDED;
    }
    else if (psErrc == PacketSenderErrc::SEND_FAILED)
    {
      mErrc = MessengerErrc::SEND_MESSAGE_FAILED;
    }

    return mErrc;
  };
};

class MockedPacketSender : public PacketSender
{
public:
  using Sptr = std::shared_ptr<MockedPacketSender>;

  MOCK_METHOD0(send, PacketSenderErrc());
};

TEST(Messenger, send_succeeds)
{
  //arrange
  MockedPacketSender::Sptr mps(new MockedPacketSender());
  Messenger ms(mps);
  EXPECT_CALL(*mps, send()).Times(1).WillOnce(Return(PacketSenderErrc::SEND_SUCCEEDED));

  //act
  MessengerErrc errc = ms.sendMessage();

  //assert
  ASSERT_EQ(errc, MessengerErrc::SEND_MESSAGE_SUCCEEDED);
}

TEST(Messenger, send_unexpected_error)
{
  //arrange
  MockedPacketSender::Sptr mps(new MockedPacketSender());
  Messenger ms(mps);
  EXPECT_CALL(*mps, send()).Times(1).WillOnce(Return(PacketSenderErrc::RECEIVE_FAILED));

  //act
  MessengerErrc errc = ms.sendMessage();

  //assert
  ASSERT_EQ(errc, MessengerErrc::UNEXPECTED_ERROR);
}

class MessengerTest : public testing::Test
{
public:
  MockedPacketSender::Sptr mps;

  void SetUp()
  {
    mps = std::make_shared<MockedPacketSender>();
  }

  void TearDown()
  {

  }
};

TEST_F(MessengerTest, send_succeeds)
{
  //arrange
  Messenger ms(mps);
  EXPECT_CALL(*mps, send()).Times(1).WillOnce(Return(PacketSenderErrc::SEND_SUCCEEDED));

  //act
  MessengerErrc errc = ms.sendMessage();

  //assert
  ASSERT_EQ(errc, MessengerErrc::SEND_MESSAGE_SUCCEEDED);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
