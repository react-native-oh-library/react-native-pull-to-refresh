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

#include "RNCPullToRefreshFooterInstance.h"
#include "PullState.h"
#include "PullUtils.h"
namespace rnoh {

RNCPullToRefreshFooterInstance::RNCPullToRefreshFooterInstance(Context context)
    : BaseRNCPullToRefreshFooterNativeComponentInstance(std::move(context)) {}
RNCPullToRefreshFooterInstance::~RNCPullToRefreshFooterInstance() { m_pullToRefreshNodeDelegate = nullptr; }

void RNCPullToRefreshFooterInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                     std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_footerStackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
}

void RNCPullToRefreshFooterInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
    m_footerStackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
};

ArkUINode &RNCPullToRefreshFooterInstance::getLocalRootArkUINode() { return m_footerStackNode; };
void RNCPullToRefreshFooterInstance::finalizeUpdates() {
    std::vector<ComponentInstance::Shared> child = getChildren();
    for (ComponentInstance::Shared c : child) {
        if (c) {
            auto height = c->getLayoutMetrics().frame.size.height;
            if (height == 0) { 
                height = c->getBoundingBox().size.height;
            }
            if (height > mChildHeight) {
                mChildHeight = height;
            }
            auto x = c->getLayoutMetrics().frame.origin.x > 0 ? c->getLayoutMetrics().frame.origin.x: 0;
            auto y = c->getLayoutMetrics().frame.origin.y > 0 ? c->getLayoutMetrics().frame.origin.y: 0;
            c->getLocalRootArkUINode().setPosition({x, y});
        }
    }
    double pointScaleFactor = getLayoutMetrics().pointScaleFactor;
    m_footerStackNode.setLayoutRect({0, 0},
                                    {PullUtils::vp2px(pointScaleFactor, getLayoutMetrics().frame.size.width),
                                     PullUtils::vp2px(pointScaleFactor, mChildHeight)},
                                    1.0);
}

void RNCPullToRefreshFooterInstance::setPullToRefreshInterface(PullToRefreshInterface *pullToRefreshNodeDelegate) {
    m_pullToRefreshNodeDelegate = pullToRefreshNodeDelegate;
}

void RNCPullToRefreshFooterInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
    if (props == nullptr) {
        return;
    }
    manual = props->manual;
    noMoreData = props->noMoreData;
    if (m_pullToRefreshNodeDelegate) {
        if (props->refreshing ) {
            m_pullToRefreshNodeDelegate->beginManualPull();
        } else {
            m_pullToRefreshNodeDelegate->onClosePull(PULL_FOOTER);
        }
    }
};
void RNCPullToRefreshFooterInstance::onFinalizeUpdates(){

};

void RNCPullToRefreshFooterInstance::onRefresh() {
    if (m_eventEmitter) {
        m_eventEmitter->onRefresh({});
    }
}
void RNCPullToRefreshFooterInstance::onStateChanged(int status) {
    if (m_eventEmitter) {
        m_eventEmitter->onStateChanged({status});
    }
}
void RNCPullToRefreshFooterInstance::onOffsetChanged(float offset) {
    if (m_eventEmitter) {
        m_eventEmitter->onOffsetChanged({offset});
    }
}
}