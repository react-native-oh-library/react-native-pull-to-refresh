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

#include "RNCPullToRefreshInstance.h"
#include "PullState.h"
#include "PullUtils.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/RNInstanceInternal.h"
#include "RNOHCorePackage/ComponentInstances/ScrollViewComponentInstance.h"
#include <glog/logging.h>

namespace rnoh {

static void receiveEvent(ArkUI_NodeEvent *event) {
    try {
        auto eventType = OH_ArkUI_NodeEvent_GetEventType(event);
        auto target = static_cast<RNCPullToRefreshInstance *>(OH_ArkUI_NodeEvent_GetUserData(event));

        if (eventType == ArkUI_NodeEventType::NODE_SCROLL_EVENT_ON_SCROLL) {
            if (target) {
                target->handleScrollStop();
            }
            return;
        }
    } catch (std::exception &e) {
        LOG(ERROR) << e.what();
    }
}

RNCPullToRefreshInstance::RNCPullToRefreshInstance(Context context)
    : BaseRNCPullToRefreshNativeComponentInstance(std::move(context)) {

    m_PullRefreshNode.insertChild(m_headerStackNode, 0);
    m_PullRefreshNode.insertChild(m_listStackNode, 1);
    m_PullRefreshNode.insertChild(m_footerStackNode, 2);
    m_PullRefreshNode.setPullToRefreshComponentNodeDelegate(this);

    ArkUI_NumberValue clipValue[] = {{.u32 = 1}};
    ArkUI_AttributeItem clipItem = {clipValue, sizeof(clipValue) / sizeof(ArkUI_NumberValue)};
    NativeNodeApi::getInstance()->setAttribute(m_headerStackNode.getArkUINodeHandle(), NODE_CLIP, &clipItem);
    m_headerStackNode.setAlign(ARKUI_ALIGNMENT_BOTTOM);
    m_headerStackNode.setAlignment(ARKUI_ALIGNMENT_BOTTOM);

    m_headerStackNode.setHeight(0);

    NativeNodeApi::getInstance()->registerNodeEvent(m_PullRefreshNode.getArkUINodeHandle(), NODE_EVENT_ON_APPEAR,
                                                    NODE_EVENT_ON_APPEAR, this);

    panGesture(m_PullRefreshNode.getArkUINodeHandle());
}

RNCPullToRefreshInstance::~RNCPullToRefreshInstance() {
    if (m_panGesture) {
        auto anyGestureApi = OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_GESTURE, "ArkUI_NativeGestureAPI_1");
        auto gestureApi = reinterpret_cast<ArkUI_NativeGestureAPI_1 *>(anyGestureApi);
        if (gestureApi) {
            gestureApi->removeGestureFromNode(m_PullRefreshNode.getArkUINodeHandle(), m_panGesture);
        }
        m_panGesture = nullptr;
    }
    unRegisterScroll();
    if (m_footerInstance) {
        m_footerInstance = nullptr;
    }
    if (m_headerInstance) {
        m_headerInstance = nullptr;
    }
    if (animation) {
        animation = nullptr;
    }
}

void RNCPullToRefreshInstance::unRegisterScroll() {
    NativeNodeApi::getInstance()->unregisterNodeEvent(m_PullRefreshNode.getArkUINodeHandle(), NODE_EVENT_ON_APPEAR);
    if (m_footerInstance && m_footerInstance->isAutoLoadMore()) {
        std::vector<ComponentInstance::Shared> child = getChildren();
        for (ComponentInstance::Shared c : child) {
            if (c->getComponentName() == "ScrollView") {
                auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
                if (scrollView != nullptr) {
                    auto &m_node = scrollView->getLocalRootArkUINode();
                    NativeNodeApi::getInstance()->unregisterNodeEvent(m_node.getArkUINodeHandle(),
                                                                      NODE_SCROLL_EVENT_ON_SCROLL);
                    NativeNodeApi::getInstance()->removeNodeEventReceiver(m_node.getArkUINodeHandle(), receiveEvent);
                }
            }
        }
    }
}

void RNCPullToRefreshInstance::onAppArea() {
    autoRefresh();
    // 注冊滚动监听 如开启自动加载更多开启注册滚动监听
    if (m_footerInstance && m_footerInstance->isAutoLoadMore()) {
        std::vector<ComponentInstance::Shared> child = getChildren();
        for (ComponentInstance::Shared c : child) {
            if (c->getComponentName() == "ScrollView") {
                auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
                if (scrollView != nullptr) {
                    auto &node = scrollView->getLocalRootArkUINode();
                    NativeNodeApi::getInstance()->addNodeEventReceiver(node.getArkUINodeHandle(), receiveEvent);
                    NativeNodeApi::getInstance()->registerNodeEvent(node.getArkUINodeHandle(),
                                                                    NODE_SCROLL_EVENT_ON_SCROLL,
                                                                    NODE_SCROLL_EVENT_ON_SCROLL, this);
                }
            }
        }
    }
}
void RNCPullToRefreshInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                               std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    auto componentName = childComponentInstance->getComponentName();
    if (componentName == "RNCPullToRefreshHeaderNative") {
        m_headerInstance = std::dynamic_pointer_cast<rnoh::RNCPullToRefreshHeaderInstance>(childComponentInstance);
        m_headerInstance->setPullToRefreshInterface(this);
        packageHeaderNode.insertChild(childComponentInstance->getLocalRootArkUINode(), 0);
        m_headerStackNode.insertChild(packageHeaderNode, index);
    } else if (componentName == "RNCPullToRefreshFooterNative") {
        m_footerInstance = std::dynamic_pointer_cast<rnoh::RNCPullToRefreshFooterInstance>(childComponentInstance);
        m_footerInstance->setPullToRefreshInterface(this);
        packageFooterNode.insertChild(childComponentInstance->getLocalRootArkUINode(), 0);
        packageFooterNode.setPosition({0, 0});
        m_footerStackNode.insertChild(packageFooterNode, 0);
        m_footerStackNode.setHeight(0);

    } else {

        ArkUI_NumberValue listWeightParam[] = {{.u32 = 1}};
        ArkUI_AttributeItem listWeight = {listWeightParam, sizeof(listWeightParam) / sizeof(ArkUI_NumberValue)};
        NativeNodeApi::getInstance()->setAttribute(m_listStackNode.getArkUINodeHandle(), NODE_LAYOUT_WEIGHT,
                                                   &listWeight);

        ArkUI_NumberValue clipValue[] = {{.u32 = 1}};
        ArkUI_AttributeItem clipItem = {clipValue, sizeof(clipValue) / sizeof(ArkUI_NumberValue)};
        NativeNodeApi::getInstance()->setAttribute(m_listStackNode.getArkUINodeHandle(), NODE_CLIP, &clipItem);

        m_listStackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    }
}

void RNCPullToRefreshInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
    auto componentName = childComponentInstance->getComponentName();

    if (componentName == "RNCPullToRefreshHeaderNative") {
        packageHeaderNode.removeChild(childComponentInstance->getLocalRootArkUINode());
        m_headerStackNode.removeChild(packageHeaderNode);
        m_headerInstance = nullptr;
    } else if (componentName == "RNCPullToRefreshFooterNative") {
        packageFooterNode.removeChild(childComponentInstance->getLocalRootArkUINode());
        m_footerStackNode.removeChild(packageFooterNode);
        m_footerInstance = nullptr;
    } else {
        m_listStackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
    }
};

ArkUINode &RNCPullToRefreshInstance::getLocalRootArkUINode() { return m_PullRefreshNode; };


void RNCPullToRefreshInstance::onPropsChanged(SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
    if (props == nullptr) {
        return;
    }
    if (props->requestDisallowInterceptTouchEvent.has_value()) {
        m_shouldRequestDisallowInterceptTouchEvent = props->requestDisallowInterceptTouchEvent.value();
    }

    if (m_shouldRequestDisallowInterceptTouchEvent) {
        // 当请求禁止拦截触摸事件时，调整手势优先级
        // 确保子组件（如 RNGH）的手势能够正常响应
        if (m_panGesture) {
            auto anyGestureApi = OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_GESTURE, "ArkUI_NativeGestureAPI_1");
            auto gestureApi = reinterpret_cast<ArkUI_NativeGestureAPI_1 *>(anyGestureApi);
            if (gestureApi) {
                gestureApi->removeGestureFromNode(m_PullRefreshNode.getArkUINodeHandle(), m_panGesture);
                gestureApi->addGestureToNode(m_PullRefreshNode.getArkUINodeHandle(), m_panGesture, PARALLEL, NORMAL_GESTURE_MASK);
            }
        }
    }
};
void RNCPullToRefreshInstance::onFinalizeUpdates(){

};


void RNCPullToRefreshInstance::panGesture(ArkUI_NodeHandle arkUI_NodeHandle) {
    auto anyGestureApi = OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_GESTURE, "ArkUI_NativeGestureAPI_1");
    auto gestureApi = reinterpret_cast<ArkUI_NativeGestureAPI_1 *>(anyGestureApi);
    if (!m_panGesture) {
        m_panGesture = gestureApi->createPanGesture(1, GESTURE_DIRECTION_VERTICAL, 0.1);
    }
    auto onPanActionCallBack = [](ArkUI_GestureEvent *event, void *extraParam) {
        if (!event || !extraParam) {
            return;
        }
        RNCPullToRefreshInstance *instance = (RNCPullToRefreshInstance *)(extraParam);
        if (!instance)
            return;

        ArkUI_GestureEventActionType actionType = OH_ArkUI_GestureEvent_GetActionType(event);
        if (actionType == GESTURE_EVENT_ACTION_ACCEPT) {
            instance->offsetY = 0;
            instance->downY = OH_ArkUI_PanGesture_GetOffsetY(event);
            instance->touchYOld = instance->offsetY;
        } else if (actionType == GESTURE_EVENT_ACTION_UPDATE) {
            instance->offsetY = OH_ArkUI_PanGesture_GetOffsetY(event) - instance->downY;
            if (instance->offsetY >= 0) { // 下拉
                instance->onActionPullUpdate();
            } else {
                instance->onActionUpUpdate();
            }

        } else if (actionType == GESTURE_EVENT_ACTION_END) {
            auto offsetY = OH_ArkUI_PanGesture_GetOffsetY(event) - instance->downY;
            if (offsetY >= 0) { // 下拉
                instance->onActionPullEnd();
            } else {
                instance->onActionUpEnd();
            }
        } else if (actionType == GESTURE_EVENT_ACTION_CANCEL) {
            // 手势被取消时（如被 ScrollView 抢占），重置状态，避免状态机卡住
            if (instance->state != REFRESHING) {
                instance->state = FREE;
            }
            if (instance->up_status != Up_REFRESHING) {
                instance->up_status = Up_FREE;
            }
            instance->trYTop = 0;
            instance->touchYOld = 0;
            instance->touchYNew = 0;
        }
    };
    // 监听 ACCEPT | UPDATE | END | CANCEL 四种事件，确保手势状态能正确重置
    gestureApi->setGestureEventTarget(
        m_panGesture, GESTURE_EVENT_ACTION_ACCEPT | GESTURE_EVENT_ACTION_UPDATE | GESTURE_EVENT_ACTION_END | GESTURE_EVENT_ACTION_CANCEL, this,
        onPanActionCallBack);
    // 使用 PARALLEL 优先级，让 PullToRefresh 手势与 ScrollView 手势并行响应
    // 这样可以实时获取手势位移，由状态机根据 ScrollView 是否到达边界来决定是否实际触发刷新
    // PARALLEL = 2 表示手势同时响应，避免 NORMAL 模式下子组件手势抢占导致的事件丢失
    gestureApi->addGestureToNode(arkUI_NodeHandle, m_panGesture, PARALLEL, NORMAL_GESTURE_MASK);
}

void RNCPullToRefreshInstance::onActionPullUpdate() {
    if (state == FREE || state == PULL_DOWN_1 || state == PULL_DOWN_2 || state == PULL_UP) {
        touchYNew = offsetY;
        if (!isComponentTop()) {
            // 当不在顶部时，重置 touchYOld 以便下次手势可以正确计算
            // 同时重置 trYTop，避免手势被抢占后界面卡住
            touchYOld = touchYNew;
            if (trYTop > 0) {
                closeHeaderRefresh(0, PULL_HEADER);
            }
            return;
        }
        auto isPullAction = (touchYNew - touchYOld) > 0;
        if ((state == FREE && isPullAction) || state == PULL_DOWN_1 || state == PULL_DOWN_2) {
            // 获取最新位移距离
            float trY = touchYNew - touchYOld;
            // 计算当前需要位移的距离
            trYTop = this->getTranslateYOfRefresh(trY, SENSITIVITY);
            setPullHeaderHeight(trYTop);
            if (trYTop / MAX_TRANSLATE < 0.5) {
                state = PULL_DOWN_1;
            } else {
                state = PULL_DOWN_2;
                onStateChanged(true, JS_PullToRefreshStateComing);
            }
        }
        touchYOld = touchYNew;
    }
}

void RNCPullToRefreshInstance::onActionPullEnd() {
    auto maxTranslate = MAX_TRANSLATE;
    if (trYTop > 0 && up_status == Up_FREE) {
        if (state == FREE || state == PULL_DOWN_1 || state == PULL_DOWN_2) {
            if (trYTop / maxTranslate < 0.5) {
                closeHeaderRefresh(0, PULL_HEADER);
            } else {
                state = REFRESHING;
                trYTop = maxTranslate * 0.5; // 这个位置要设置高度，收回去
                onStateChanged(true, JS_PullToRefreshStateRefreshing);
                this->onRefresh(true);
                setPullHeaderHeight(trYTop);
            }
        }
    }
}
void RNCPullToRefreshInstance::onActionUpEnd() {
    auto maxTranslate = MAX_TRANSLATE;
    if (trYTop > 0 && state == FREE) {
        if (up_status == Up_FREE || up_status == Up_PULL_DOWN_1 || up_status == Up_PULL_DOWN_2) {
            if ((trYTop / maxTranslate < 0.5) || (m_footerInstance && m_footerInstance->isNoMoreData())) {
                closeHeaderRefresh(0, PULL_FOOTER);
            } else {
                up_status = Up_REFRESHING;
                // 这个位置要设置高度(原footer高度)，收回去
                if (m_footerInstance) {
                    trYTop = m_footerInstance->getFooterHeight();
                } else {
                    trYTop = maxTranslate * 0.5;
                }

                onStateChanged(false, JS_PullToRefreshStateRefreshing);
                this->onRefresh(false);
                setUpFooterHeight(trYTop);
            }
        }
    }
}

void RNCPullToRefreshInstance::onActionUpUpdate() {
    if (up_status == Up_FREE || up_status == Up_PULL_DOWN_1 || up_status == Up_PULL_DOWN_2 || up_status == Up_PULL_UP) {
        touchYNew = offsetY;
        if (!isComponentBottom()) {
            // 当不在底部时，重置 touchYOld 以便下次手势可以正确计算
            // 同时重置 trYTop，避免手势被抢占后界面卡住
            touchYOld = touchYNew;
            if (trYTop > 0) {
                closeHeaderRefresh(0, PULL_FOOTER);
            }
            return;
        }
        float trY = touchYOld - touchYNew;
        if ((up_status == Up_FREE && trY > 0) || up_status == Up_PULL_DOWN_1 || up_status == Up_PULL_DOWN_2) {
            float des = this->getTranslateYOfRefresh(trY, 0.3);
            trYTop = des;
            setUpFooterHeight(trYTop);
            if (trYTop / MAX_TRANSLATE < 0.5) {
                up_status = Up_PULL_DOWN_1;
            } else {
                up_status = Up_PULL_DOWN_2;
                onStateChanged(false, JS_PullToRefreshStateComing);
            }
        }
        touchYOld = touchYNew;
    }
}


float RNCPullToRefreshInstance::getTranslateYOfRefresh(float newTranslateY, float sensitivity) {
    // 考虑 progressViewOffset
    float effectiveMaxTranslate = MAX_TRANSLATE;
    if (m_headerInstance) {
        effectiveMaxTranslate += m_headerInstance->getProgressViewOffset();
    }
    facebook::react::Float maxTranslateY = effectiveMaxTranslate;
    // 阻尼值计算
    facebook::react::Float dampingFactor;
    if ((trYTop / maxTranslateY) < 0.2) {
        dampingFactor = 1.0;
    } else if ((trYTop / maxTranslateY) < 0.4) {
        dampingFactor = 0.8;
    } else if ((trYTop / maxTranslateY) < 0.6) {
        dampingFactor = 0.6;
    } else if ((trYTop / maxTranslateY) < 0.8) {
        dampingFactor = 0.4;
    } else {
        dampingFactor = 0.2;
    }
    newTranslateY = newTranslateY * dampingFactor * sensitivity;
    // 下拉值计算
    facebook::react::Float newTotalTranslateY = trYTop + newTranslateY;
    if (newTotalTranslateY > maxTranslateY) {
        return maxTranslateY;
    } else if (newTotalTranslateY < 0) {
        return 0;
    } else {
        return newTotalTranslateY;
    }
}


void RNCPullToRefreshInstance::setPullHeaderHeight(float height) {
    m_headerStackNode.setHeight(height);
    onOffsetChanged(true, height);
}
void RNCPullToRefreshInstance::setUpFooterHeight(float height) {
    m_footerStackNode.setHeight(height);
    onOffsetChanged(false, height);
}

void RNCPullToRefreshInstance::onNativeResponderBlockChange(bool blocked) {
    std::vector<ComponentInstance::Shared> child = getChildren();
    for (ComponentInstance::Shared c : child) {
        auto componentName = c->getComponentName();
        // 支持 RNGH FlatList 的嵌套滚动协调
        if (componentName == "ScrollView" || componentName == "RNGHScrollView" ||
            componentName.find("FlatList") != std::string::npos) {
            auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
            if (scrollView) {
                if (blocked) {
                    scrollView->setNativeResponderBlocked(!blocked, "REACT_NATIVE");
                }
                break;
            }
        }
    }
}

bool RNCPullToRefreshInstance::isComponentTop() {
    std::vector<ComponentInstance::Shared> child = getChildren();
    for (ComponentInstance::Shared c : child) {
        if (c->getComponentName() == "ScrollView") {
            auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
            if (scrollView != nullptr) {
                auto metrics = scrollView->getScrollViewMetrics();
                // 处理短列表嵌套场景
                // 当内容高度小于容器高度时，允许直接触发下拉刷新
                if (metrics.contentSize.height < metrics.containerSize.height) {
                    return up_status == Up_FREE;
                }
                return scrollView->getScrollViewMetrics().contentOffset.y <= 0 && up_status == Up_FREE;
            }
        }
    }
    return false;
};

bool RNCPullToRefreshInstance::isComponentBottom() {
    std::vector<ComponentInstance::Shared> child = getChildren();
    for (ComponentInstance::Shared c : child) {
        if (c->getComponentName() == "ScrollView") {
            auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
            if (scrollView != nullptr) {
                auto metrics = scrollView->getScrollViewMetrics();
                // 内容不足一屏，不算到底
                if (metrics.contentSize.height < metrics.containerSize.height) {
                    return false;
                }
                auto maxScrollHeight = scrollView->getScrollViewMetrics().contentSize.height -
                                       scrollView->getScrollViewMetrics().containerSize.height;
                return scrollView->getScrollViewMetrics().contentOffset.y >= maxScrollHeight - 10 && state == FREE;
            }
        }
    }
    return false;
};


void RNCPullToRefreshInstance::onRefresh(bool isHeader) {
    if (m_headerInstance && isHeader) {
        m_headerInstance->onRefresh();
    }
    if (m_footerInstance && !isHeader) {
        setListScrollPosition(m_footerInstance->getChildHeight());
        m_footerInstance->onRefresh();
    }
}

/**
 * TODO 因js端定位的内容高度已经定死，导致刷新底部的时候，内容与其底部有重叠，先将内容在y轴上偏移下
 * @param y
 */
void RNCPullToRefreshInstance::setListScrollPosition(float y) {
    std::vector<ComponentInstance::Shared> child = getChildren();
    for (ComponentInstance::Shared c : child) {
        if (c->getComponentName() == "ScrollView") {
            auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
            if (scrollView != nullptr) {
                scrollView->getLocalRootArkUINode().setPosition({0, -y});
            }
        }
    }
}

void RNCPullToRefreshInstance::onStateChanged(bool isHeader, int status) {
    if (m_headerInstance && isHeader) {
        m_headerInstance->onStateChanged(status);
    }
    if (m_footerInstance && !isHeader) {
        m_footerInstance->onStateChanged(status);
    }
}
void RNCPullToRefreshInstance::onOffsetChanged(bool isHeader, float offset) {
    if (m_headerInstance && isHeader) {
        m_headerInstance->onOffsetChanged(offset);
    }
    if (m_footerInstance && !isHeader) {
        m_footerInstance->onOffsetChanged(offset);
    }
}

void RNCPullToRefreshInstance::closeHeaderRefresh(float target, int closeTag) {
    if (trYTop == target) {
        DLOG(INFO)<<"pull-to-refresh closeHeaderRefresh trYTop:"<<trYTop << ";target:"<<target;
        return;
    }
    // 如果动画正在运行，先取消当前动画再启动新动画
    if (animation != nullptr && (animation->GetAnimationStatus() == CLOSE_ANIMATION_START ||
                                 animation->GetAnimationStatus() == CLOSE_ANIMATION_RUN)) {
        animation->Rest();
        animation = nullptr;
    }
    if (animation == nullptr) {
        animation = std::make_shared<PullAnimated>();
    }
    animation->SetAnimationParams(
        static_cast<std::chrono::milliseconds>(ANIMA_DURATION), trYTop, target, [this, closeTag](double value) {
            trYTop = value < 0 ? 0 : value;
            auto instance = std::static_pointer_cast<RNInstanceInternal>(m_deps->rnInstance.lock());
            if (!instance) {
                return;
            }
            instance->getTaskExecutor()->runTask(
                TaskThread::MAIN, [wptr = this->weak_from_this(), closeTag, wInstance = instance->weak_from_this()] {
                    auto ptr = std::static_pointer_cast<RNCPullToRefreshInstance>(wptr.lock());
                    if (ptr) {
                        if (closeTag == PULL_HEADER) {
                            ptr->setPullHeaderHeight(ptr->trYTop);
                        }
                        if (closeTag == PULL_FOOTER) {
                            ptr->setUpFooterHeight(ptr->trYTop);
                        }

                        if (ptr->trYTop == 0) {
                            ptr->state = FREE;
                            ptr->up_status = Up_FREE;
                            if (closeTag == PULL_HEADER) {
                                ptr->onStateChanged(true, JS_PullToRefreshStateIdle);
                            }
                            if (closeTag == PULL_FOOTER) {
                                ptr->onStateChanged(false, JS_PullToRefreshStateIdle);
                            }
                            ptr->m_PullRefreshNode.markDirty();
                            if (ptr->animation && ptr->animation->GetAnimationStatus() != CLOSE_ANIMATION_FREE) {
                                ptr->animation = nullptr;
                            }
                        }
                    }
                });
        });
    if (animation->GetAnimationStatus() == CLOSE_ANIMATION_FREE ||
        animation->GetAnimationStatus() == CLOSE_ANIMATION_FINISH) {
        animation->Start();
    }
}

/**
 * 自动下拉刷新
 */
void RNCPullToRefreshInstance::autoRefresh() {
    if (m_headerInstance && m_headerInstance->getRefreshing() && state == FREE) { // 设置自动刷新
        // 模拟下拉操作
        autoAnimation(true);
    }
}

/**
 * 自动上拉刷新
 */
void RNCPullToRefreshInstance::handleScrollStop() {
    if (!m_footerInstance || !m_footerInstance->isAutoLoadMore() ||
        up_status != Up_FREE || m_footerInstance->isNoMoreData()) {
        return;
    }
    std::vector<ComponentInstance::Shared> child = getChildren();
    for (ComponentInstance::Shared c : child) {
        if (c->getComponentName() == "ScrollView") {
            auto scrollView = std::dynamic_pointer_cast<rnoh::ScrollViewComponentInstance>(c);
            if (scrollView != nullptr) {
                auto metrics = scrollView->getScrollViewMetrics();
                if (metrics.contentSize.height < metrics.containerSize.height) {
                    return;
                }
                float threshold = metrics.contentSize.height 
                                - metrics.containerSize.height 
                                + m_footerInstance->getFooterHeight() * 0.3;
                if (metrics.contentOffset.y >= threshold && state == FREE) {
                    up_status = Up_PULL_DOWN_2; // 防止多次触发（自动加载）
                    autoAnimation(false);
                }
            }
        }
    }
}



void RNCPullToRefreshInstance::autoAnimation(bool isHeader) {
    if (animation != nullptr && (animation->GetAnimationStatus() == CLOSE_ANIMATION_START ||
                                 animation->GetAnimationStatus() == CLOSE_ANIMATION_RUN)) {
        return;
    }
    if (animation == nullptr) {
        animation = std::make_shared<PullAnimated>();
    }
    animation->SetAnimationParams(
        static_cast<std::chrono::milliseconds>(ANIMA_DURATION), 0, MAX_TRANSLATE / 2, [this, isHeader](double value) {
            trYTop = value < 0 ? 0 : value;
            auto instance = std::static_pointer_cast<RNInstanceInternal>(m_deps->rnInstance.lock());
            if (!instance) {
                return;
            }
            instance->getTaskExecutor()->runTask(
                TaskThread::MAIN, [wptr = this->weak_from_this(), isHeader, wInstance = instance->weak_from_this()] {
                    auto ptr = std::static_pointer_cast<RNCPullToRefreshInstance>(wptr.lock());
                    if (ptr) {
                        if (isHeader) {
                            ptr->setPullHeaderHeight(ptr->trYTop);
                        } else {
                            ptr->setUpFooterHeight(ptr->trYTop);
                        }

                        if (ptr->trYTop == MAX_TRANSLATE / 2) {
                            if (isHeader) {
                                ptr->onActionPullEnd();
                            } else {
                                ptr->onActionUpEnd();
                            }
                            ptr->m_PullRefreshNode.markDirty();
                            if (ptr->animation && ptr->animation->GetAnimationStatus() != CLOSE_ANIMATION_FREE) {
                                ptr->animation = nullptr;
                            }
                        }
                    }
                });
        });
    if (animation->GetAnimationStatus() == CLOSE_ANIMATION_FREE ||
        animation->GetAnimationStatus() == CLOSE_ANIMATION_FINISH) {
        animation->Start();
    }
}

void RNCPullToRefreshInstance::onClosePull(int tag) {
    closeHeaderRefresh(0, tag);
    if (tag == PULL_FOOTER) { // 关闭底部刷新的时候，将内容区域Y轴上的偏移量回归正常
        setListScrollPosition(0);
    }
}

/**
 * 手动触发refresh
 */
void RNCPullToRefreshInstance::beginManualPull(){
     up_status = Up_REFRESHING;
     float footerHeight = m_footerInstance ? m_footerInstance->getFooterHeight() 
                                              : MAX_TRANSLATE / 2;
     trYTop = footerHeight; 
     setUpFooterHeight(footerHeight);
     onStateChanged(false, JS_PullToRefreshStateRefreshing);
}
} // namespace rnoh
