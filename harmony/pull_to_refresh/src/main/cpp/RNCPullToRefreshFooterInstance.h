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

#ifndef HARMONY_RNCPULLTOREFRESHFOOTERINSTANCE_H
#define HARMONY_RNCPULLTOREFRESHFOOTERINSTANCE_H
#include "PullStackNode.h"
#include "PullToRefreshInterface.h"
#include "generated/RNOH/generated/components/BaseRNCPullToRefreshFooterNativeComponentInstance.h"
namespace rnoh {
class RNCPullToRefreshFooterInstance : public rnoh::BaseRNCPullToRefreshFooterNativeComponentInstance {
public:
    RNCPullToRefreshFooterInstance(Context context);
    ~RNCPullToRefreshFooterInstance();

    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;

    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    ArkUINode &getLocalRootArkUINode() override;
    void finalizeUpdates() override;
    void onPropsChanged(SharedConcreteProps const &props) override;
    void onFinalizeUpdates() override;
    float getFooterHeight() { return mChildHeight; }
    void onRefresh();
    void onStateChanged(int status);
    void setPullToRefreshInterface(PullToRefreshInterface *pullToRefreshNodeDelegate);
    bool isNoMoreData() { return noMoreData; }
    void onOffsetChanged(float offset);
    float getChildHeight() { return mChildHeight; }

private:
    PullStackNode m_footerStackNode;
    float mChildHeight{0.0};
    PullToRefreshInterface *m_pullToRefreshNodeDelegate{nullptr};
    bool noMoreData{false};
};
} // namespace rnoh


#endif // HARMONY_RNCPULLTOREFRESHFOOTERINSTANCE_H
