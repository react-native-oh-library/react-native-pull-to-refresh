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

#ifndef PULLTOREFRESHPACKAGE_H
#define PULLTOREFRESHPACKAGE_H

#include "RNCPullToRefreshFooterInstance.h"
#include "RNCPullToRefreshHeaderInstance.h"
#include "RNCPullToRefreshInstance.h"
#include "generated/RNOH/generated/BasePullToRefreshPackage.h"
#pragma once

namespace rnoh {

class PullToRefreshPackageComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
public:
    using ComponentInstanceFactoryDelegate::ComponentInstanceFactoryDelegate;

    ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
        if (ctx.componentName == "RNCPullToRefreshNative") {
            return std::make_shared<rnoh::RNCPullToRefreshInstance>(std::move(ctx));
        } else if (ctx.componentName == "RNCPullToRefreshHeaderNative") {
            return std::make_shared<RNCPullToRefreshHeaderInstance>(std::move(ctx));
        } else if (ctx.componentName == "RNCPullToRefreshFooterNative") {
            return std::make_shared<RNCPullToRefreshFooterInstance>(std::move(ctx));
        }
        return nullptr;
    }
};


class PullToRefreshPackage : public BasePullToRefreshPackage {
    using Super = BasePullToRefreshPackage;
    using Super::Super;

    ComponentInstanceFactoryDelegate::Shared createComponentInstanceFactoryDelegate() override {
        return std::make_shared<PullToRefreshPackageComponentInstanceFactoryDelegate>();
    }
};
} // namespace rnoh
#endif // SENSORPACKAGE_H