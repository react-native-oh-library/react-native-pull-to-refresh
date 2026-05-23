package com.reactnative.nestedscroll;

import android.view.View;

import androidx.annotation.Nullable;

import com.facebook.react.uimanager.PointerEvents;
import com.facebook.react.uimanager.ViewProps;
import com.facebook.react.views.view.ReactViewGroup;

final class NestedScrollPointerEvents {
	private NestedScrollPointerEvents() {
	}

	static boolean setPointerEvents(View view, String propName, @Nullable Object value) {
		if (!ViewProps.POINTER_EVENTS.equals(propName)) {
			return false;
		}

		PointerEvents pointerEvents = PointerEvents.parsePointerEvents((String) value);
		if (view instanceof NestedScrollView) {
			((NestedScrollView) view).setPointerEvents(pointerEvents);
		} else if (view instanceof ReactViewGroup) {
			((ReactViewGroup) view).setPointerEvents(pointerEvents);
		}
		return true;
	}
}
