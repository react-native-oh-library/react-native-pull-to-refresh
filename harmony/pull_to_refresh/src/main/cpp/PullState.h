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

#ifndef HARMONY_PULLSTATE_H
#define HARMONY_PULLSTATE_H
typedef enum {
    FREE = 0x01,        // 空闲
    PULL_DOWN_1 = 0x02, // 下拉不超过一半
    PULL_DOWN_2 = 0x03, // 下拉超过一半
    REFRESHING = 0x04,  // 正在刷新
    REFRESHED = 0x05,   // 刷新结束
    PULL_UP = 0x06,     // 回弹
} Pull_Status;

typedef enum {
    Up_FREE = 0x001,        // 空闲
    Up_PULL_DOWN_1 = 0x002, // 上拉拉不超过一半
    Up_PULL_DOWN_2 = 0x003, // 上拉超过一半
    Up_REFRESHING = 0x004,  // 正在刷新
    Up_REFRESHED = 0x005,   // 刷新结束
    Up_PULL_UP = 0x006,     // 回弹
} UpPull_Status;

const float MAX_TRANSLATE = 200.0; // 最大拉距
const float SENSITIVITY = 0.7;     // 灵敏度
const int ANIMA_DURATION = 150;    // 默认关闭刷新头动画的时间

/**
 * 传给js端状态的定义
 */
typedef enum {
    JS_PullToRefreshStateIdle = 0,       // 空闲
    JS_PullToRefreshStateComing = 1,     // 上拉下拉
    JS_PullToRefreshStateRefreshing = 2, // 正在刷新

} JS_REFRESH_STATUS;


/**
 * 刷新头
 */
const int PULL_HEADER = 1;
/**
 * 刷新底部
 */
const int PULL_FOOTER = 2;

#endif // HARMONY_PULLSTATE_H
