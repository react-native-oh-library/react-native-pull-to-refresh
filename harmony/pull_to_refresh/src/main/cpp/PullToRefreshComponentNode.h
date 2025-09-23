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

#pragma once
#include "RNOH/arkui/NativeNodeApi.h"
#include "RNOH/arkui/ArkUINode.h"
#include "react/renderer/graphics/Color.h"

namespace rnoh {

class PullToRefreshComponentNodeDelegate {
public:
    virtual ~PullToRefreshComponentNodeDelegate() = default;
    virtual void onAppArea(){};
};

class PullToRefreshComponentNode : public ArkUINode {
protected:
    ArkUI_NodeHandle m_headerArkUINodeHandle;
    ArkUI_NodeHandle m_listArkUINodeHandle;
    PullToRefreshComponentNodeDelegate *m_pullToRefreshNodeDelegate;
    
public:
    PullToRefreshComponentNode();
    ~PullToRefreshComponentNode() override;

    void insertChild(ArkUINode &child, std::size_t index);

    void removeChild(ArkUINode &child);

    void setPullToRefreshComponentNodeDelegate(PullToRefreshComponentNodeDelegate *pullToRefreshNodeDelegate);

    void setHeaderHeight(float h);
    void setEnableRefresh(bool enable);
    void setMaxTranslate(float maxHeight);
    void setHeaderBackgroundColor(facebook::react::SharedColor const &color);
    
    void onNodeEvent(ArkUI_NodeEventType eventType, EventArgs &eventArgs) override;
    void setSensitivity(float setSensitivity);
};

} // namespace rnoh