// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>

#include "src/heap/gc-idle-time-handler.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace v8 {
namespace internal {

namespace {

class GCIdleTimeHandlerTest : public ::testing::Test {
 public:
  GCIdleTimeHandlerTest() = default;
  ~GCIdleTimeHandlerTest() override = default;

  GCIdleTimeHandler* handler() { return &handler_; }

  GCIdleTimeHeapState DefaultHeapState() {
    GCIdleTimeHeapState result;
    result.incremental_marking_stopped = false;
    result.size_of_objects = kSizeOfObjects;
    return result;
  }

  static const size_t kSizeOfObjects = 100 * MB;
  static const size_t kMarkCompactSpeed = 200 * KB;

 private:
  GCIdleTimeHandler handler_;
};

}  // namespace


TEST(GCIdleTimeHandler, EstimateMarkingStepSizeInitial) {
  size_t step_size = GCIdleTimeHandler::EstimateMarkingStepSize(1, 0);
  EXPECT_EQ(
      static_cast<size_t>(GCIdleTimeHandler::kInitialConservativeMarkingSpeed *
                          GCIdleTimeHandler::kConservativeTimeRatio),
      step_size);
}


TEST(GCIdleTimeHandler, EstimateMarkingStepSizeNonZero) {
  size_t marking_speed_in_bytes_per_millisecond = 100;
  size_t step_size = GCIdleTimeHandler::EstimateMarkingStepSize(
      1, marking_speed_in_bytes_per_millisecond);
  EXPECT_EQ(static_cast<size_t>(marking_speed_in_bytes_per_millisecond *
                                GCIdleTimeHandler::kConservativeTimeRatio),
            step_size);
}


TEST(GCIdleTimeHandler, EstimateMarkingStepSizeOverflow1) {
  size_t step_size = GCIdleTimeHandler::EstimateMarkingStepSize(
      10, static_cast<double>(std::numeric_limits<size_t>::max()));
  EXPECT_EQ(static_cast<size_t>(GCIdleTimeHandler::kMaximumMarkingStepSize),
            step_size);
}


TEST(GCIdleTimeHandler, EstimateMarkingStepSizeOverflow2) {
  size_t step_size = GCIdleTimeHandler::EstimateMarkingStepSize(
      static_cast<double>(std::numeric_limits<size_t>::max()), 10);
  EXPECT_EQ(static_cast<size_t>(GCIdleTimeHandler::kMaximumMarkingStepSize),
            step_size);
}

TEST_F(GCIdleTimeHandlerTest, IncrementalMarking1) {
  if (!handler()->Enabled()) return;
  GCIdleTimeHeapState heap_state = DefaultHeapState();
  double idle_time_ms = 10;
  EXPECT_EQ(GCIdleTimeAction::kIncrementalStep,
            handler()->Compute(idle_time_ms, heap_state));
}


TEST_F(GCIdleTimeHandlerTest, NotEnoughTime) {
  if (!handler()->Enabled()) return;
  GCIdleTimeHeapState heap_state = DefaultHeapState();
  heap_state.incremental_marking_stopped = true;
  size_t speed = kMarkCompactSpeed;
  double idle_time_ms = static_cast<double>(kSizeOfObjects / speed - 1);
  EXPECT_EQ(GCIdleTimeAction::kDone,
            handler()->Compute(idle_time_ms, heap_state));
}


TEST_F(GCIdleTimeHandlerTest, DoNotStartIncrementalMarking) {
  if (!handler()->Enabled()) return;
  GCIdleTimeHeapState heap_state = DefaultHeapState();
  heap_state.incremental_marking_stopped = true;
  double idle_time_ms = 10.0;
  EXPECT_EQ(GCIdleTimeAction::kDone,
            handler()->Compute(idle_time_ms, heap_state));
}


TEST_F(GCIdleTimeHandlerTest, ContinueAfterStop) {
  if (!handler()->Enabled()) return;
  GCIdleTimeHeapState heap_state = DefaultHeapState();
  heap_state.incremental_marking_stopped = true;
  double idle_time_ms = 10.0;
  EXPECT_EQ(GCIdleTimeAction::kDone,
            handler()->Compute(idle_time_ms, heap_state));
  heap_state.incremental_marking_stopped = false;
  EXPECT_EQ(GCIdleTimeAction::kIncrementalStep,
            handler()->Compute(idle_time_ms, heap_state));
}


TEST_F(GCIdleTimeHandlerTest, DoneIfNotMakingProgressOnIncrementalMarking) {
  if (!handler()->Enabled()) return;

  // Regression test for crbug.com/489323.
  GCIdleTimeHeapState heap_state = DefaultHeapState();

  // Simulate incremental marking stopped and not eligible to start.
  heap_state.incremental_marking_stopped = true;
  double idle_time_ms = 10.0;
  // We should return kDone if we cannot start incremental marking.
  EXPECT_EQ(GCIdleTimeAction::kDone,
            handler()->Compute(idle_time_ms, heap_state));
}

}  // namespace internal
}  // namespace v8
