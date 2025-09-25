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

#ifndef HARMONY_RNCPULLTOREFRESHINSTANCE_H
#define HARMONY_RNCPULLTOREFRESHINSTANCE_H
#include "PullAnimated.h"
#include "PullStackNode.h"
#include "PullState.h"
#include "PullToRefreshComponentNode.h"
#include "RNCPullToRefreshFooterInstance.h"
#include "RNCPullToRefreshHeaderInstance.h"
#include "generated/RNOH/generated/components/BaseRNCPullToRefreshNativeComponentInstance.h"
#include <arkui/native_gesture.h>

namespace rnoh {
class RNCPullToRefreshInstance : public BaseRNCPullToRefreshNativeComponentInstance,
                                 public PullToRefreshInterface,
                                 public PullToRefreshComponentNodeDelegate {
//      using super = BaseCodegenLibCppSampleComponentComponentInstance;
public:
    RNCPullToRefreshInstance(Context context);
    ~RNCPullToRefreshInstance();
    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;

    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    ArkUINode &getLocalRootArkUINode() override;

    void onPropsChanged(SharedConcreteProps const &props) override;
    void onFinalizeUpdates() override;
    void onNativeResponderBlockChange(bool isBlocked) override;
    void onClosePull(int tag) override;
    void autoRefresh() override;
    void onAppArea() override;
    void handleScrollStop();

private:
    bool isComponentTop();
    void panGesture(ArkUI_NodeHandle arkUI_NodeHandle);
    bool isComponentBottom();
    void onActionPullUpdate();
    void onActionPullEnd();
    void onActionUpUpdate();
    void onActionUpEnd();
    void setPullHeaderHeight(float height);
    void setUpFooterHeight(float height);
    float getTranslateYOfRefresh(float newTranslateY, float sensitivity);
    void onRefresh(bool isHeader);
    void onStateChanged(bool isHeader, int status);
    void onOffsetChanged(bool isHeader, float offset);
    void closeHeaderRefresh(float target, int tag);
    void setListScrollPosition(float y);
    void autoAnimation(bool isHeader);
    void unRegisterScroll();
private:
    PullToRefreshComponentNode m_PullRefreshNode;
    PullStackNode m_footerStackNode;
    PullStackNode m_headerStackNode;
    PullStackNode m_listStackNode;
    PullStackNode packageHeaderNode;
    PullStackNode packageFooterNode;
    ArkUI_GestureRecognizer *m_panGesture{nullptr};
    std::shared_ptr<rnoh::RNCPullToRefreshHeaderInstance> m_headerInstance{nullptr};
    std::shared_ptr<rnoh::RNCPullToRefreshFooterInstance> m_footerInstance{nullptr};
    std::shared_ptr<PullAnimated> animation{nullptr};
    float m_headerWidth{0};
    float m_headerHeight{0};
    int32_t state{FREE};        // 下拉状态
    int32_t up_status{Up_FREE}; // 上拉状态
    float trYTop{0.0};
    int32_t touchYOld{0};
    int32_t touchYNew{0};
    int32_t downY{0};   // first down touch on Y
    int32_t offsetY{0}; // pan offset on Y
};

} // namespace rnoh

#endif // HARMONY_RNCPULLTOREFRESHINSTANCE_H
