#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <gmock/gmock.h>
#include "interfaces/message_handler_interface.hpp"
#include "readers/fifo_reader.hpp"

using ::testing::_;
using ::testing::Return;

class FifoReaderTest : public ::testing::Test {
public:
protected:
  FifoReaderTest(){}
  ~FifoReaderTest(){}
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

class MessageHandlerInterface : public IMessageHandlerInterface {
public:
  MOCK_METHOD(void, process_message, (const std::string&, const std::string&));
};

TEST_F(FifoReaderTest, will_create_and_destroy_fifo) {
  char name[]="will_create_and_destroy_fifo";
  struct stat name_stat;
  {
    FifoReader r;
    ASSERT_EQ(0, r.fifo_create(name, true));
    ASSERT_EQ(0, stat(name, &name_stat));
    EXPECT_EQ(int(S_IFIFO), int(name_stat.st_mode & S_IFIFO));
    FILE *f = fopen(name, "w");
    EXPECT_NE(nullptr, f);
    fclose(f);
  }
  ASSERT_EQ(-1, stat(name, &name_stat));
} // end

TEST_F(FifoReaderTest, does_not_process_non_lines) {
  char name[]="does_not_process_non_lines";
  FifoReader r;
  MessageHandlerInterface h;
  r.init(&h);
  ASSERT_EQ(0, r.fifo_create(name, true));
  FILE *f = fopen(name, "w");
  EXPECT_NE(nullptr, f);
  fprintf(f, "not a line");
  fflush(f);

  EXPECT_CALL(h, process_message(_, _))
    .Times(0)
  ;

  ASSERT_EQ(0, r.read_data());
  fclose(f);

} // end

TEST_F(FifoReaderTest, processes_single_line) {
  char name[]="processes_single_line";
  FifoReader r;
  MessageHandlerInterface h;
  r.init(&h);
  ASSERT_EQ(0, r.fifo_create(name, true));
  FILE *f = fopen(name, "w");
  EXPECT_NE(nullptr, f);
  fprintf(f, "just one line\n");
  fflush(f);

  EXPECT_CALL(h, process_message(_, "just one line"))
    .Times(1)
  ;

  ASSERT_EQ(0, r.read_data());
  fclose(f);

} // end

TEST_F(FifoReaderTest, processes_spliced_lines) {
  char name[]="processes_spliced_lines";
  FifoReader r;
  MessageHandlerInterface h;
  r.init(&h);
  ASSERT_EQ(0, r.fifo_create(name, true));
  FILE *f = fopen(name, "w");
  EXPECT_NE(nullptr, f);
  fprintf(f, "just one ");
  fprintf(f, "line\n");
  fflush(f);

  EXPECT_CALL(h, process_message(_, "just one line"))
    .Times(1)
  ;

  ASSERT_EQ(0, r.read_data());
  fclose(f);

} // end

TEST_F(FifoReaderTest, processes_multiple_lines) {
  char name[]="processes_multiple_lines";
  FifoReader r;
  MessageHandlerInterface h;
  r.init(&h);
  ASSERT_EQ(0, r.fifo_create(name, true));
  FILE *f = fopen(name, "w");
  EXPECT_NE(nullptr, f);
  fprintf(f, "line one\nline two\nline 3\n");
  fflush(f);

  EXPECT_CALL(h, process_message(_, "line one"))
    .Times(1)
  ;
  EXPECT_CALL(h, process_message(_, "line two"))
    .Times(1)
  ;
  EXPECT_CALL(h, process_message(_, "line 3"))
    .Times(1)
  ;

  ASSERT_EQ(0, r.read_data());
  fclose(f);

} // end

TEST_F(FifoReaderTest, processes_multiple_spliced_lines) {
  char name[]="processes_multiple_spliced_lines";
  FifoReader r;
  MessageHandlerInterface h;
  r.init(&h);
  ASSERT_EQ(0, r.fifo_create(name, true));
  FILE *f = fopen(name, "w");
  ASSERT_NE(nullptr, f);

  EXPECT_CALL(h, process_message(_, "line one"))
    .Times(1)
  ;
  EXPECT_CALL(h, process_message(_, "line two"))
    .Times(1)
  ;
  EXPECT_CALL(h, process_message(_, "line 3"))
    .Times(1)
  ;

  fprintf(f, "line one\nline ");
  fflush(f);
  ASSERT_EQ(0, r.read_data());
  fprintf(f, "two\nline 3\n");
  fflush(f);
  ASSERT_EQ(0, r.read_data());
  fclose(f);

} // end
