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

#ifndef HARMONY_RNCPULLTOREFRESHHEADERINSTANCE_H
#define HARMONY_RNCPULLTOREFRESHHEADERINSTANCE_H
#include "PullStackNode.h"
#include "PullToRefreshInterface.h"
#include "generated/RNOH/generated/components/BaseRNCPullToRefreshHeaderNativeComponentInstance.h"
namespace rnoh {
class RNCPullToRefreshHeaderInstance : public rnoh::BaseRNCPullToRefreshHeaderNativeComponentInstance {
public:
    RNCPullToRefreshHeaderInstance(Context context);
    ~RNCPullToRefreshHeaderInstance();

    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;

    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    ArkUINode &getLocalRootArkUINode() override;
    void finalizeUpdates() override;
    void onPropsChanged(SharedConcreteProps const &props) override;
    void onFinalizeUpdates() override;
    void onRefresh();
    void setPullToRefreshInterface(PullToRefreshInterface *pullToRefreshNodeDelegate);
    void onStateChanged(int status);
    void onOffsetChanged(float offset);
    bool getRefreshing() { return refreshing; }
    float getProgressViewOffset() { return m_progressViewOffset; }

private:
    PullStackNode m_headerStackNode;
    PullToRefreshInterface *m_pullToRefreshNodeDelegate{nullptr};
    bool refreshing{false};
    float m_progressViewOffset{0.0f};
};
} // namespace rnoh


#endif // HARMONY_RNCPULLTOREFRESHHEADERINSTANCE_H
