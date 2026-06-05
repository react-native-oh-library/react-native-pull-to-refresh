package com.reactnative.pulltorefresh;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.view.ViewCompat;

import com.facebook.react.uimanager.ReactOverflowView;
import com.facebook.react.uimanager.events.NativeGestureUtil;
import com.scwang.smart.refresh.layout.SmartRefreshLayout;
import com.scwang.smart.refresh.layout.api.RefreshFooter;
import com.scwang.smart.refresh.layout.api.RefreshHeader;
import com.scwang.smart.refresh.layout.api.RefreshKernel;

import java.util.ArrayList;
import java.util.List;

public class PullToRefresh extends SmartRefreshLayout implements ReactOverflowView {

	private final static String TAG = "PullToRefresh";

	private final Rect mRect;
	private String mOverflow = "hidden";
	private boolean mShouldRequestDisallowInterceptTouchEvent = true;
	private final List<View> mReactChildren = new ArrayList<>();
	private View mNestedScrollTarget;

	private final Runnable mSettleNestedScrollRunnable = () -> {
		View target = mNestedScrollTarget;
		if (target == null && mRefreshContent != null) {
			target = mRefreshContent.getScrollableView();
		}
		settleNestedScroll(target);
	};

	public PullToRefresh(Context context) {
		super(context);
		mRect = new Rect();
	}

	public void setOverflow(String overflow) {
		mOverflow = overflow;
		invalidate();
	}

	public void setShouldRequestDisallowInterceptTouchEvent(boolean disallow) {
		mShouldRequestDisallowInterceptTouchEvent = disallow;
	}

	void addReactChild(View child, int index) {
		int existingIndex = mReactChildren.indexOf(child);
		if (existingIndex >= 0) {
			mReactChildren.remove(existingIndex);
			if (existingIndex < index) {
				index -= 1;
			}
		}

		int safeIndex = Math.max(0, Math.min(index, mReactChildren.size()));
		mReactChildren.add(safeIndex, child);
	}

	@Nullable
	View getReactChildAt(int index) {
		if (index < 0 || index >= mReactChildren.size()) {
			return null;
		}
		return mReactChildren.get(index);
	}

	int getReactChildCount() {
		return mReactChildren.size();
	}

	@Nullable
	View removeReactChildAt(int index) {
		if (index < 0 || index >= mReactChildren.size()) {
			return null;
		}
		return mReactChildren.remove(index);
	}

	void removeReactChild(View child) {
		mReactChildren.remove(child);
	}

	@Nullable
	@Override
	public String getOverflow() {
		return mOverflow;
	}

	private final Runnable measureAndLayout = () -> {
		measure(
			View.MeasureSpec.makeMeasureSpec(getWidth(), View.MeasureSpec.EXACTLY),
			View.MeasureSpec.makeMeasureSpec(getHeight(), View.MeasureSpec.EXACTLY));
		layout(getLeft(), getTop(), getRight(), getBottom());
	};

	@Override
	public void requestLayout() {
		super.requestLayout();
		post(measureAndLayout);
	}

	@Override
	public boolean dispatchTouchEvent(MotionEvent ev) {
		if (mRefreshContent == null) {
			return super.dispatchTouchEvent(ev);
		}

		View scrollableView = mRefreshContent.getScrollableView();
		if (scrollableView == null) {
			return super.dispatchTouchEvent(ev);
		}

		boolean nestedScrollingEnabled = ViewCompat.isNestedScrollingEnabled(scrollableView);
		boolean canScrollVertical = scrollableView.canScrollVertically(-1)
			|| scrollableView.canScrollVertically(1);

		// When nested scrolling is enabled AND the inner content is actually scrollable,
		// SmartRefreshLayout handles refresh via NestedScrollingParent callbacks
		// (onNestedPreScroll, onNestedScroll, etc.). Skip custom touch handling to avoid
		// interfering with child gesture handlers (e.g., Pressable click events).
		//
		// A short list (content shorter than its viewport) has no scroll range, so the inner
		// scroll view never produces the nested-scroll deltas that drive the refresh. In that
		// case we must fall through to the manual handling below, otherwise pull-to-refresh and
		// load-more stop working inside a NestedScrollView. See issue #85.
		if (nestedScrollingEnabled && canScrollVertical) {
			boolean nestedScrollWasInProgress = mNestedInProgress;
			boolean handled = super.dispatchTouchEvent(ev);
			settleNestedScrollIfNeeded(ev, scrollableView, nestedScrollWasInProgress);
			return handled;
		}

		String viewName = scrollableView.getClass().getCanonicalName();

		if (viewName != null && viewName.contains("ViewPager2")) {
			if (mIsBeingDragged) {
				NativeGestureUtil.notifyNativeGestureStarted(this, ev);
			}
			return super.dispatchTouchEvent(ev);
		}

		if (scrollableView.canScrollHorizontally(-1) || scrollableView.canScrollHorizontally(1)) {
			if (mIsBeingDragged) {
				NativeGestureUtil.notifyNativeGestureStarted(this, ev);
			}
			return super.dispatchTouchEvent(ev);
		}

		// 数据不足以填满整个页面（短列表）。
		// 此时内部 ReactScrollView 没有滚动范围，自身不会开启嵌套滚动会话，也就不会通过
		// NestedScrollingParent 回调驱动刷新；同时它的 onInterceptTouchEvent 会因为无法滚动
		// 而提前返回 false，导致外层的 NestedScrollView 协调器抢走竖直方向的手势。
		//
		// 处理方式：手动驱动内部 ScrollView 的触摸逻辑，并从内部视图发起一次竖直方向的嵌套滚动。
		// SmartRefreshLayout 作为它的 NestedScrollingParent 会接受这次嵌套滚动并继续向上传递给
		// 外层协调器，于是协调器的 getNestedScrollAxes() 会包含竖直方向，其 onInterceptTouchEvent
		// 便不会再抢走手势；同时内部 ScrollView 把无法消费的滚动量通过 onNestedScroll 上报，由
		// SmartRefreshLayout 移动 header/footer 完成下拉刷新与上拉加载更多。一旦判定为竖直拖拽，
		// 就阻止外层协调器拦截并消费该事件。
		if (!canScrollVertical && scrollableView instanceof ViewGroup) {
			ViewGroup viewGroup = (ViewGroup) scrollableView;
			viewGroup.onInterceptTouchEvent(ev);
			viewGroup.onTouchEvent(ev);

			int action = ev.getActionMasked();
			if (action == MotionEvent.ACTION_DOWN) {
				scrollableView.startNestedScroll(ViewCompat.SCROLL_AXIS_VERTICAL);
			} else if (action == MotionEvent.ACTION_UP || action == MotionEvent.ACTION_CANCEL) {
				scrollableView.stopNestedScroll();
			}

			if (shouldInterceptTouchEvent(ev)) {
				NativeGestureUtil.notifyNativeGestureStarted(this, ev);
				ViewParent parent = getParent();
				if (parent != null) {
					parent.requestDisallowInterceptTouchEvent(true);
				}
				return true;
			}
		}
		return super.dispatchTouchEvent(ev);
	}

	public boolean onInterceptTouchEvent(MotionEvent ev) {
		View scrollableView = mRefreshContent != null ? mRefreshContent.getScrollableView() : null;
		if (scrollableView != null && ViewCompat.isNestedScrollingEnabled(scrollableView)) {
			// When nested scrolling is enabled, SmartRefreshLayout handles refresh via the
			// NestedScrollingParent protocol. Don't call requestDisallowInterceptTouchEvent here,
			// because SmartRefreshLayout propagates FLAG_DISALLOW_INTERCEPT to all ancestors.
			// When it reaches RNGestureHandlerRootView, that view's override calls
			// tryCancelAllHandlers(), which cancels all RNGH gesture handlers and breaks
			// Pressable click events.
			return false;
		}

		if (mShouldRequestDisallowInterceptTouchEvent) {
			requestDisallowInterceptTouchEvent(true);
		}

		if (super.onInterceptTouchEvent(ev)) {
			NativeGestureUtil.notifyNativeGestureStarted(this, ev);
			return true;
		}
		return false;
	}

	private int mLastMotionX;
	private int mLastMotionY;

	private boolean shouldInterceptTouchEvent(MotionEvent ev) {
		final int action = ev.getActionMasked();
		if ((action == MotionEvent.ACTION_MOVE) && (mIsBeingDragged)) {
			return true;
		}

		switch (action) {
			case MotionEvent.ACTION_MOVE: {
				final int x = (int) ev.getRawX();
				final int y = (int) ev.getRawY();
				final int xDiff = Math.abs(x - mLastMotionX);
				final int yDiff = Math.abs(y - mLastMotionY);
				if (yDiff >= mTouchSlop && yDiff > xDiff) {
					mIsBeingDragged = true;
				}
				break;
			}
			case MotionEvent.ACTION_DOWN: {
				mLastMotionX = (int) ev.getRawX();
				mLastMotionY = (int) ev.getRawY();
				mIsBeingDragged = false;
				break;
			}
			case MotionEvent.ACTION_CANCEL:
			case MotionEvent.ACTION_UP:
				mIsBeingDragged = false;
		}

		return mIsBeingDragged;
	}

	private void settleNestedScrollIfNeeded(MotionEvent ev, View target, boolean nestedScrollWasInProgress) {
		if (!nestedScrollWasInProgress || !mNestedInProgress) {
			return;
		}

		int action = ev.getActionMasked();
		if (action != MotionEvent.ACTION_UP && action != MotionEvent.ACTION_CANCEL) {
			return;
		}

		settleNestedScroll(target);
	}

	private void settleNestedScroll(@Nullable View target) {
		if (!shouldSettleNestedScroll()) {
			return;
		}

		removeCallbacks(mSettleNestedScrollRunnable);
		if (target != null) {
			mNestedParent.onStopNestedScroll(target);
		}
		mNestedInProgress = false;
		mTotalUnconsumed = 0;
		overSpinner();
		mNestedChild.stopNestedScroll();
		mNestedScrollTarget = null;
	}

	private void scheduleNestedScrollSettleIfNeeded() {
		removeCallbacks(mSettleNestedScrollRunnable);
		if (shouldSettleNestedScroll()) {
			// RNGH FlatList can keep SmartRefreshLayout in nested-scroll mode without
			// delivering the touch end. Let a pending release settle once nested deltas go idle.
			postDelayed(mSettleNestedScrollRunnable, 200);
		}
	}

	private boolean shouldSettleNestedScroll() {
		// Settle whenever a nested-scroll session left the spinner displaced. This covers the
		// pull-to-refresh / load-more drag states as well as pure over-scroll drag
		// (mEnableOverScrollDrag, state == None), which an RNGH FlatList can otherwise leave stuck
		// because it swallows the touch end and keeps SmartRefreshLayout in nested-scroll mode,
		// so the spinner never bounces back. The settle is only posted after nested-scroll deltas
		// go idle (see scheduleNestedScrollSettleIfNeeded), i.e. once the finger has lifted.
		return mNestedInProgress && mSpinner != 0;
	}

	@Override
	public void onNestedScrollAccepted(@NonNull View child, @NonNull View target, int axes) {
		mNestedScrollTarget = target;
		removeCallbacks(mSettleNestedScrollRunnable);
		super.onNestedScrollAccepted(child, target, axes);
	}

	@Override
	public void onNestedPreScroll(@NonNull View target, int dx, int dy, @NonNull int[] consumed) {
		super.onNestedPreScroll(target, dx, dy, consumed);
		scheduleNestedScrollSettleIfNeeded();
	}

	@Override
	public void onNestedScroll(@NonNull View target, int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed) {
		super.onNestedScroll(target, dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed);
		scheduleNestedScrollSettleIfNeeded();
	}

	@Override
	public void onStopNestedScroll(@NonNull View target) {
		removeCallbacks(mSettleNestedScrollRunnable);
		mNestedScrollTarget = null;
		super.onStopNestedScroll(target);
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		super.onSizeChanged(w, h, oldw, oldh);
		float height = (float) (getMeasuredHeight() * 0.3);

		RefreshHeader header = getRefreshHeader();
		if (header != null) {
			int headerHeight = header.getView().getMeasuredHeight();
			setHeaderMaxDragRate(height / headerHeight);
		}

		RefreshFooter footer = getRefreshFooter();
		if (footer != null) {
			int footerHeight = footer.getView().getMeasuredHeight();
			setFooterMaxDragRate(height / footerHeight);
		}
	}

	@Override
	protected void dispatchDraw(@NonNull Canvas canvas) {
		getDrawingRect(mRect);
		if (!"visible".equals(mOverflow)) {
			canvas.clipRect(mRect);
		}
		super.dispatchDraw(canvas);
	}

	public RefreshKernel getRefreshKernel() {
		return mKernel;
	}
}
