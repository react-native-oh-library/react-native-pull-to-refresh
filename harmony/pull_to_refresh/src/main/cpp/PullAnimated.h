/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HARMONY_PULLANIMATED_H
#define HARMONY_PULLANIMATED_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <glog/logging.h>
#include <iostream>
#include <thread>

typedef enum {
    CLOSE_ANIMATION_FREE = 0,
    CLOSE_ANIMATION_START = 1,
    CLOSE_ANIMATION_RUN = 2,
    CLOSE_ANIMATION_FINISH = 3,
} Pull_Animation_State;

class PullAnimated {
public:
    PullAnimated() { state_ = Pull_Animation_State::CLOSE_ANIMATION_FREE; }


    ~PullAnimated() { cancelAnimation(); }

    PullAnimated(const PullAnimated &) = delete;
    PullAnimated &operator=(PullAnimated const &) = delete;

    void SetAnimationParams(std::chrono::milliseconds durationSeconds, double startValue, double endValue,
                            std::function<void(double)> callback) {
        duration_ = durationSeconds; // 150
        currentValue_ = startValue;  // 50
        targetValue_ = endValue;     // 0
        startValue_ = startValue;    // 50
        callback_ = callback;
        if (!callback_) {
            throw std::invalid_argument("Callback cannot be null");
        }
         DLOG(INFO) << "closeHeaderRefresh startValue:" << startValue << ";target:" << endValue;
    }

    void Rest() { cancelAnimation(); }
    double GetCurrentValue() const { return currentValue_; }
    Pull_Animation_State GetAnimationStatus() { return state_; }
    void Start() {
        state_ = Pull_Animation_State::CLOSE_ANIMATION_START;
        startTime_ = std::chrono::steady_clock::now();
        // 启动更新线程
        updateThread_ = new std::thread(&PullAnimated::UpdateLoop, this);
    }

    void UpdateLoop() {
        state_ = Pull_Animation_State::CLOSE_ANIMATION_RUN;
        const double p0 = 0.1, p1 = 0.0, p2 = 0.58, p3 = 1.0; // CubicBezier控制点
        auto startTime = std::chrono::high_resolution_clock::now();
        for (double t = 0.0; t < 1.01; t += 0.01) {
            // 逐步增加t的值来模拟动画进度
            double progress = cubicBezier(t, p0, p1, p2, p3); // 计算CubicBezier曲线上的点作为进度
            currentValue_ = startValue_ + (targetValue_ - startValue_) * progress; // 根据进度计算当前值
            if (callback_) {
                callback_(currentValue_);
            }
            // 调用回调函数处理当前值
            // 暂停以匹配动画的持续时间
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
            if (elapsed < duration_ * progress) {
                std::this_thread::sleep_for((duration_ * progress - elapsed) * 0.05);
            }
            startTime = currentTime; // 重置开始时间以计算下一帧的延迟
        }
        if (callback_) {
            callback_(targetValue_);
        }
        state_ = Pull_Animation_State::CLOSE_ANIMATION_FINISH;
    }


private:
    std::atomic<double> startValue_;
    std::atomic<double> currentValue_;
    double targetValue_;
    Pull_Animation_State state_ = Pull_Animation_State::CLOSE_ANIMATION_FINISH;
    std::chrono::milliseconds duration_;
    std::chrono::steady_clock::time_point startTime_;
    std::thread *updateThread_; // 使用指针是为了能够动态分配线程对象
    std::function<void(double)> callback_;

    double cubicBezier(double t, double p0, double p1, double p2, double p3) { return (1 - t) * p0 + p3 * t; }
    void cancelAnimation() {
        state_ = Pull_Animation_State::CLOSE_ANIMATION_FREE;
        // 等待线程结束
        if (updateThread_ != nullptr) {
            if (updateThread_->joinable()) {
                updateThread_->join();
            }
            delete updateThread_; // 如果使用new分配，则需要delete释放
            updateThread_ = nullptr;
        }
    }
};
#endif // HARMONY_PULLANIMATED_H
