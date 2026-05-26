package com.reactnative.nestedscroll;

import android.content.Context;
import android.graphics.Rect;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewConfiguration;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.view.NestedScrollingChild3;
import androidx.core.view.NestedScrollingChildHelper;
import androidx.core.view.ViewCompat;

import com.facebook.react.views.view.ReactViewGroup;

public class NestedScrollViewChild extends ReactViewGroup implements NestedScrollingChild3 {
	private static final int INVALID_POINTER = -1;

	private final NestedScrollingChildHelper mChildHelper;
	private final int[] mScrollConsumed = new int[2];
	private final int[] mScrollOffset = new int[2];
	private final Rect mTempRect = new Rect();

	private int mActivePointerId = INVALID_POINTER;
	private int mLastMotionY;
	private int mNestedYOffset;
	private final int mTouchSlop;
	private boolean mIsBeingDragged;

	public NestedScrollViewChild(@Nullable Context context) {
		super(context);
		mChildHelper = new NestedScrollingChildHelper(this);
		mTouchSlop = ViewConfiguration.get(context).getScaledTouchSlop();
		setNestedScrollingEnabled(true);
	}

	@Override
	public boolean dispatchTouchEvent(MotionEvent ev) {
		final int actionMasked = ev.getActionMasked();
		if (actionMasked == MotionEvent.ACTION_DOWN) {
			mNestedYOffset = 0;
		}

		MotionEvent vtev = MotionEvent.obtain(ev);
		vtev.offsetLocation(0, mNestedYOffset);

		switch (actionMasked) {
			case MotionEvent.ACTION_DOWN:
				mIsBeingDragged = false;
				mLastMotionY = (int) ev.getY();
				mActivePointerId = ev.getPointerId(0);
				startNestedScroll(ViewCompat.SCROLL_AXIS_VERTICAL, ViewCompat.TYPE_TOUCH);
				break;
			case MotionEvent.ACTION_MOVE:
				handleMoveEvent(ev, vtev);
				break;
			case MotionEvent.ACTION_POINTER_DOWN: {
				final int index = ev.getActionIndex();
				mLastMotionY = (int) ev.getY(index);
				mActivePointerId = ev.getPointerId(index);
				break;
			}
			case MotionEvent.ACTION_POINTER_UP:
				onSecondaryPointerUp(ev);
				break;
			case MotionEvent.ACTION_UP:
			case MotionEvent.ACTION_CANCEL:
				mActivePointerId = INVALID_POINTER;
				mIsBeingDragged = false;
				stopNestedScroll(ViewCompat.TYPE_TOUCH);
				break;
		}

		boolean handled = super.dispatchTouchEvent(vtev);
		vtev.recycle();
		return handled;
	}

	private void handleMoveEvent(MotionEvent ev, MotionEvent vtev) {
		final int activePointerIndex = ev.findPointerIndex(mActivePointerId);
		if (activePointerIndex == -1) {
			return;
		}

		final int y = (int) ev.getY(activePointerIndex);
		int deltaY = mLastMotionY - y;
		if (!mIsBeingDragged && Math.abs(deltaY) > mTouchSlop) {
			mIsBeingDragged = true;
			deltaY = deltaY > 0 ? deltaY - mTouchSlop : deltaY + mTouchSlop;
		}

		// Short Android lists may keep the gesture without emitting nested pre-scroll.
		if (mIsBeingDragged && deltaY > 0 && !canDescendantScroll(ev, activePointerIndex, 1)) {
			mScrollConsumed[1] = 0;
			mScrollOffset[1] = 0;
			if (dispatchNestedPreScroll(0, deltaY, mScrollConsumed, mScrollOffset, ViewCompat.TYPE_TOUCH)) {
				mNestedYOffset += mScrollOffset[1];
				vtev.offsetLocation(0, mScrollOffset[1]);
			}
		}

		mLastMotionY = y;
	}

	private boolean canDescendantScroll(MotionEvent ev, int pointerIndex, int direction) {
		int x = (int) ev.getX(pointerIndex);
		int y = (int) ev.getY(pointerIndex);
		return canDescendantScroll(this, x, y, direction);
	}

	private boolean canDescendantScroll(View view, int x, int y, int direction) {
		if (view instanceof ViewGroup) {
			ViewGroup viewGroup = (ViewGroup) view;
			for (int i = viewGroup.getChildCount() - 1; i >= 0; i--) {
				View child = viewGroup.getChildAt(i);
				if (child.getVisibility() != View.VISIBLE) {
					continue;
				}

				child.getHitRect(mTempRect);
				if (mTempRect.contains(x, y)) {
					int childX = x - child.getLeft() + child.getScrollX();
					int childY = y - child.getTop() + child.getScrollY();
					if (child.canScrollVertically(direction) ||
						canDescendantScroll(child, childX, childY, direction)) {
						return true;
					}
				}
			}
		}
		return view != this && view.canScrollVertically(direction);
	}

	private void onSecondaryPointerUp(MotionEvent ev) {
		final int pointerIndex = ev.getActionIndex();
		final int pointerId = ev.getPointerId(pointerIndex);
		if (pointerId == mActivePointerId) {
			final int newPointerIndex = pointerIndex == 0 ? 1 : 0;
			mLastMotionY = (int) ev.getY(newPointerIndex);
			mActivePointerId = ev.getPointerId(newPointerIndex);
		}
	}

	@Override
	public boolean isNestedScrollingEnabled() {
		return mChildHelper.isNestedScrollingEnabled();
	}

	@Override
	public int getNestedScrollAxes() {
		return ViewCompat.SCROLL_AXIS_NONE;
	}

	@Override
	public void setNestedScrollingEnabled(boolean enabled) {
		mChildHelper.setNestedScrollingEnabled(enabled);
	}

	@Override
	public boolean startNestedScroll(int axes) {
		return mChildHelper.startNestedScroll(axes);
	}

	@Override
	public boolean startNestedScroll(int axes, int type) {
		return mChildHelper.startNestedScroll(axes, type);
	}

	@Override
	public void stopNestedScroll() {
		mChildHelper.stopNestedScroll();
	}

	@Override
	public void stopNestedScroll(int type) {
		mChildHelper.stopNestedScroll(type);
	}

	@Override
	public boolean hasNestedScrollingParent() {
		return mChildHelper.hasNestedScrollingParent();
	}

	@Override
	public boolean hasNestedScrollingParent(int type) {
		return mChildHelper.hasNestedScrollingParent(type);
	}

	@Override
	public boolean dispatchNestedPreScroll(int dx, int dy, int[] consumed, int[] offsetInWindow) {
		return mChildHelper.dispatchNestedPreScroll(dx, dy, consumed, offsetInWindow);
	}

	@Override
	public boolean dispatchNestedPreScroll(int dx, int dy, @Nullable int[] consumed, @Nullable int[] offsetInWindow, int type) {
		return mChildHelper.dispatchNestedPreScroll(dx, dy, consumed, offsetInWindow, type);
	}

	@Override
	public boolean dispatchNestedScroll(int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed, int[] offsetInWindow) {
		return mChildHelper.dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, offsetInWindow);
	}

	@Override
	public boolean dispatchNestedScroll(int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed, @Nullable int[] offsetInWindow, int type) {
		return mChildHelper.dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, offsetInWindow, type);
	}

	@Override
	public void dispatchNestedScroll(int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed, @Nullable int[] offsetInWindow, int type, @NonNull int[] consumed) {
		mChildHelper.dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, offsetInWindow, type, consumed);
	}

	@Override
	public boolean dispatchNestedPreFling(float velocityX, float velocityY) {
		return mChildHelper.dispatchNestedPreFling(velocityX, velocityY);
	}

	@Override
	public boolean dispatchNestedFling(float velocityX, float velocityY, boolean consumed) {
		return mChildHelper.dispatchNestedFling(velocityX, velocityY, consumed);
	}
}
