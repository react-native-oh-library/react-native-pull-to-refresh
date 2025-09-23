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

#include "PullToRefreshComponentNode.h"
#include "RNOH/arkui/NativeNodeApi.h"
#include <arkui/native_node.h>

namespace rnoh {
PullToRefreshComponentNode::PullToRefreshComponentNode()
    : ArkUINode(NativeNodeApi::getInstance()->createNode(ArkUI_NodeType::ARKUI_NODE_COLUMN)),
      m_headerArkUINodeHandle(nullptr), m_listArkUINodeHandle(nullptr) {}

PullToRefreshComponentNode::~PullToRefreshComponentNode() {
    if (m_pullToRefreshNodeDelegate) {
        m_pullToRefreshNodeDelegate = nullptr;
    }
}

void PullToRefreshComponentNode::onNodeEvent(ArkUI_NodeEventType eventType, EventArgs &eventArgs) {
    if (eventType == ArkUI_NodeEventType::NODE_EVENT_ON_APPEAR) {
        m_pullToRefreshNodeDelegate->onAppArea();
    }
}
void PullToRefreshComponentNode::setPullToRefreshComponentNodeDelegate(
    PullToRefreshComponentNodeDelegate *pullToRefreshNodeDelegate) {
    m_pullToRefreshNodeDelegate = pullToRefreshNodeDelegate;
}
void PullToRefreshComponentNode::insertChild(ArkUINode &child, std::size_t index) {
    maybeThrow(NativeNodeApi::getInstance()->insertChildAt(m_nodeHandle, child.getArkUINodeHandle(), index));
}

void PullToRefreshComponentNode::removeChild(ArkUINode &child) {
    maybeThrow(NativeNodeApi::getInstance()->removeChild(m_nodeHandle, child.getArkUINodeHandle()));
    if (m_headerArkUINodeHandle == child.getArkUINodeHandle()) {
        m_headerArkUINodeHandle = nullptr;
    } else {
        m_listArkUINodeHandle = nullptr;
    }
}

void PullToRefreshComponentNode::setHeaderHeight(float h) {
    ArkUI_NumberValue heightNumberValue[] = {h};
    ArkUI_AttributeItem heightItem = {heightNumberValue, 1};
    NativeNodeApi::getInstance()->setAttribute(m_headerArkUINodeHandle, NODE_HEIGHT, &heightItem);
}

} // namespace rnoh