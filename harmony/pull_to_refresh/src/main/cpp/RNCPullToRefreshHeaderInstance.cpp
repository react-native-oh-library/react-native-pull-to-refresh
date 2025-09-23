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

#include "RNCPullToRefreshHeaderInstance.h"
#include "PullState.h"
#include "PullUtils.h"
namespace rnoh {
RNCPullToRefreshHeaderInstance::RNCPullToRefreshHeaderInstance(Context context)
    : BaseRNCPullToRefreshHeaderNativeComponentInstance(std::move(context)) {

    ArkUI_NumberValue clipValue[] = {{.u32 = 1}};
    ArkUI_AttributeItem clipItem = {clipValue, sizeof(clipValue) / sizeof(ArkUI_NumberValue)};
    NativeNodeApi::getInstance()->setAttribute(m_headerStackNode.getArkUINodeHandle(), NODE_CLIP, &clipItem);
}
RNCPullToRefreshHeaderInstance::~RNCPullToRefreshHeaderInstance() { m_pullToRefreshNodeDelegate = nullptr; }
void RNCPullToRefreshHeaderInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                     std::size_t index) {
    childComponentInstance->getLocalRootArkUINode().setPosition({0, 0});
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_headerStackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
}

void RNCPullToRefreshHeaderInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
    m_headerStackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
};

ArkUINode &RNCPullToRefreshHeaderInstance::getLocalRootArkUINode() { return m_headerStackNode; };
void RNCPullToRefreshHeaderInstance::finalizeUpdates() {
    std::vector<ComponentInstance::Shared> child = getChildren();
    float childHeight = 0.0;
    for (ComponentInstance::Shared c : child) {
        if (c) {
            auto height = c->getLayoutMetrics().frame.size.height;
            if (height > childHeight) {
                childHeight = height;
            }
        }
    }
    double pointScaleFactor = getLayoutMetrics().pointScaleFactor;
    m_headerStackNode.setLayoutRect({0, 0},
                                    {PullUtils::vp2px(pointScaleFactor, getLayoutMetrics().frame.size.width),
                                     PullUtils::vp2px(pointScaleFactor, childHeight)},
                                    1.0);
}

void RNCPullToRefreshHeaderInstance::onPropsChanged(SharedConcreteProps const &props) {

    CppComponentInstance::onPropsChanged(props);
    if (props == nullptr) {
        return;
    }
    refreshing = props->refreshing;
    if (m_pullToRefreshNodeDelegate) {
        if (!props->refreshing) {
            m_pullToRefreshNodeDelegate->onClosePull(PULL_HEADER);
        } else {
            m_pullToRefreshNodeDelegate->autoRefresh();
        }
    }
};
void RNCPullToRefreshHeaderInstance::onFinalizeUpdates(){

};
void RNCPullToRefreshHeaderInstance::setPullToRefreshInterface(PullToRefreshInterface *pullToRefreshNodeDelegate) {
    m_pullToRefreshNodeDelegate = pullToRefreshNodeDelegate;
}
void RNCPullToRefreshHeaderInstance::onRefresh() {
    if (m_eventEmitter) {
        m_eventEmitter->onRefresh({});
    }
}
void RNCPullToRefreshHeaderInstance::onStateChanged(int status) {
    if (m_eventEmitter) {
        m_eventEmitter->onStateChanged({status});
    }
}
void RNCPullToRefreshHeaderInstance::onOffsetChanged(float offset) {
    if (m_eventEmitter) {
        m_eventEmitter->onOffsetChanged({offset});
    }
}
}