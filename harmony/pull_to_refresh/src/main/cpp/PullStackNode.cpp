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

#include "PullStackNode.h"
#include "RNOH/arkui/NativeNodeApi.h"

namespace rnoh {

PullStackNode::PullStackNode()
    : ArkUINode(NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_STACK)) {}

void PullStackNode::insertChild(ArkUINode &child, std::size_t index) {
    maybeThrow(NativeNodeApi::getInstance()->insertChildAt(m_nodeHandle, child.getArkUINodeHandle(),
                                                           static_cast<int32_t>(index)));
}

void PullStackNode::removeChild(ArkUINode &child) {
    maybeThrow(NativeNodeApi::getInstance()->removeChild(m_nodeHandle, child.getArkUINodeHandle()));
}

PullStackNode &PullStackNode::setMargin(float left, float top, float right, float bottom) {
    ArkUI_NumberValue value[] = {{.f32 = top}, {.f32 = right}, {.f32 = bottom}, {.f32 = left}};
    ArkUI_AttributeItem item = {value, sizeof(value) / sizeof(ArkUI_NumberValue)};
    maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_MARGIN, &item));
    return *this;
}

PullStackNode::~PullStackNode() {}

PullStackNode &PullStackNode::setAlign(int32_t align) {
    ArkUI_NumberValue value[] = {{.i32 = align}};
    ArkUI_AttributeItem item = {.value = value, .size = 1};
    maybeThrow(NativeNodeApi::getInstance()->setAttribute(m_nodeHandle, NODE_STACK_ALIGN_CONTENT, &item));
    return *this;
}
} // namespace rnoh