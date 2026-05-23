package com.reactnative.nestedscroll;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.react.uimanager.ThemedReactContext;
import com.facebook.react.uimanager.ViewGroupManager;
import com.facebook.react.uimanager.ViewManagerDelegate;
import com.facebook.react.viewmanagers.NestedScrollViewContentManagerDelegate;
import com.facebook.react.viewmanagers.NestedScrollViewContentManagerInterface;

public class NestedScrollViewContentManager extends ViewGroupManager<NestedScrollViewContent>
	implements NestedScrollViewContentManagerInterface<NestedScrollViewContent> {

	public final static String REACT_CLASS = "NestedScrollViewContent";

	private final NestedScrollViewContentManagerDelegate<NestedScrollViewContent, NestedScrollViewContentManager> mDelegate
		= new NestedScrollViewContentManagerDelegate<>(this) {
			@Override
			public void setProperty(NestedScrollViewContent view, String propName, @Nullable Object value) {
				if (NestedScrollPointerEvents.setPointerEvents(view, propName, value)) {
					return;
				}
				super.setProperty(view, propName, value);
			}
		};

	@Override
	protected ViewManagerDelegate<NestedScrollViewContent> getDelegate() {
		return mDelegate;
	}

	@NonNull
	@Override
	public String getName() {
		return REACT_CLASS;
	}

	@NonNull
	@Override
	protected NestedScrollViewContent createViewInstance(@NonNull ThemedReactContext reactContext) {
		return new NestedScrollViewContent(reactContext);
	}
}
