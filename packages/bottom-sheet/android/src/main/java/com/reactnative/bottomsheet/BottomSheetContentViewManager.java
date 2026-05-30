package com.reactnative.bottomsheet;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.bridge.Dynamic;
import com.facebook.react.uimanager.BackgroundStyleApplicator;
import com.facebook.react.uimanager.LengthPercentage;
import com.facebook.react.uimanager.LengthPercentageType;
import com.facebook.react.uimanager.ReactStylesDiffMap;
import com.facebook.react.uimanager.StateWrapper;
import com.facebook.react.uimanager.ThemedReactContext;
import com.facebook.react.uimanager.ViewProps;
import com.facebook.react.uimanager.ViewGroupManager;
import com.facebook.react.uimanager.ViewManagerDelegate;
import com.facebook.react.uimanager.annotations.ReactProp;
import com.facebook.react.uimanager.annotations.ReactPropGroup;
import com.facebook.react.uimanager.common.UIManagerType;
import com.facebook.react.uimanager.common.ViewUtil;
import com.facebook.react.uimanager.style.BorderRadiusProp;
import com.facebook.react.viewmanagers.BottomSheetContentViewManagerDelegate;
import com.facebook.react.viewmanagers.BottomSheetContentViewManagerInterface;
import com.facebook.yoga.YogaConstants;

public class BottomSheetContentViewManager extends ViewGroupManager<BottomSheetContentView>
        implements BottomSheetContentViewManagerInterface<BottomSheetContentView> {

    private final BottomSheetContentViewManagerDelegate<BottomSheetContentView, BottomSheetContentViewManager> mDelegate =
            new BottomSheetContentViewManagerDelegate<BottomSheetContentView, BottomSheetContentViewManager>(this) {
                @Override
                public void setProperty(
                        BottomSheetContentView view,
                        String propName,
                        @Nullable Object value) {
                    if (ViewProps.OVERFLOW.equals(propName)) {
                        setOverflow(view, (String) value);
                        return;
                    }

                    super.setProperty(view, propName, value);
                }
            };

    @Override
    protected ViewManagerDelegate<BottomSheetContentView> getDelegate() {
        return mDelegate;
    }

    @NonNull
    @Override
    public String getName() {
        return "BottomSheetContentView";
    }

    @NonNull
    @Override
    protected BottomSheetContentView createViewInstance(@NonNull ThemedReactContext context) {
        return new BottomSheetContentView(context);
    }

    @ReactPropGroup(names = {
            ViewProps.BORDER_RADIUS,
            ViewProps.BORDER_TOP_LEFT_RADIUS,
            ViewProps.BORDER_TOP_RIGHT_RADIUS,
            ViewProps.BORDER_BOTTOM_RIGHT_RADIUS,
            ViewProps.BORDER_BOTTOM_LEFT_RADIUS,
            ViewProps.BORDER_TOP_START_RADIUS,
            ViewProps.BORDER_TOP_END_RADIUS,
            ViewProps.BORDER_BOTTOM_START_RADIUS,
            ViewProps.BORDER_BOTTOM_END_RADIUS,
            ViewProps.BORDER_END_END_RADIUS,
            ViewProps.BORDER_END_START_RADIUS,
            ViewProps.BORDER_START_END_RADIUS,
            ViewProps.BORDER_START_START_RADIUS,
    })
    public void setBorderRadius(
            @NonNull BottomSheetContentView view,
            int index,
            @NonNull Dynamic rawBorderRadius) {
        LengthPercentage borderRadius = LengthPercentage.setFromDynamic(rawBorderRadius, false);

        if (ViewUtil.getUIManagerType(view) != UIManagerType.FABRIC
                && borderRadius != null
                && borderRadius.getType() == LengthPercentageType.PERCENT) {
            borderRadius = null;
        }

        BackgroundStyleApplicator.setBorderRadius(
                view,
                BorderRadiusProp.values()[index],
                borderRadius);
    }

    @Override
    public void setBorderRadius(BottomSheetContentView view, float borderRadius) {
        setBorderRadius(view, BorderRadiusProp.BORDER_RADIUS, borderRadius);
    }

    @Override
    public void setBorderTopLeftRadius(BottomSheetContentView view, float borderRadius) {
        setBorderRadius(view, BorderRadiusProp.BORDER_TOP_LEFT_RADIUS, borderRadius);
    }

    @Override
    public void setBorderTopRightRadius(BottomSheetContentView view, float borderRadius) {
        setBorderRadius(view, BorderRadiusProp.BORDER_TOP_RIGHT_RADIUS, borderRadius);
    }

    @Override
    public void setBorderBottomRightRadius(BottomSheetContentView view, float borderRadius) {
        setBorderRadius(view, BorderRadiusProp.BORDER_BOTTOM_RIGHT_RADIUS, borderRadius);
    }

    @Override
    public void setBorderBottomLeftRadius(BottomSheetContentView view, float borderRadius) {
        setBorderRadius(view, BorderRadiusProp.BORDER_BOTTOM_LEFT_RADIUS, borderRadius);
    }

    private void setBorderRadius(
            @NonNull BottomSheetContentView view,
            @NonNull BorderRadiusProp property,
            float rawBorderRadius) {
        LengthPercentage borderRadius = YogaConstants.isUndefined(rawBorderRadius)
                ? null
                : new LengthPercentage(rawBorderRadius, LengthPercentageType.POINT);

        BackgroundStyleApplicator.setBorderRadius(view, property, borderRadius);
    }

    @ReactProp(name = ViewProps.OVERFLOW)
    public void setOverflow(@NonNull BottomSheetContentView view, @Nullable String overflow) {
        view.setOverflow(overflow);
    }

    @Nullable
    @Override
    public Object updateState(
            @NonNull BottomSheetContentView view,
            ReactStylesDiffMap props,
            StateWrapper stateWrapper) {
        view.setStateWrapper(stateWrapper);
        return super.updateState(view, props, stateWrapper);
    }
}
