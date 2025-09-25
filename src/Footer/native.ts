import React, {Component, ReactNode} from 'react';
import {findNodeHandle, requireNativeComponent, UIManager, ViewProps} from 'react-native';
import { NativePullToRefreshFooterProps } from './DefaultPullToRefreshFooterNativeComponent';
import  RNCPullToRefreshFooter from './DefaultPullToRefreshFooterNativeComponent';

const NativePullToRefreshFooter = RNCPullToRefreshFooter;

type NativePullToRefreshFooterInstance = InstanceType<typeof NativePullToRefreshFooter>;

const PullToRefreshFooterCommands = {
  setNativeRefreshing(componentOrHandle: NativePullToRefreshFooterInstance, refreshing: boolean) {
    UIManager.dispatchViewManagerCommand(findNodeHandle(componentOrHandle), 'setNativeRefreshing', [
      refreshing,
    ]);
  },
};
class PullToRefreshFooter extends Component<NativePullToRefreshFooterProps> {
  _nativeRef: NativePullToRefreshFooterInstance | null = null;
  _lastNativeRefreshing = false;

  _onRefresh = () => {
    this._lastNativeRefreshing = true;
    this.props?.onRefresh?.();
    this.forceUpdate();
  };

  _setNativeRef = (ref: NativePullToRefreshFooterInstance) => {
    this._nativeRef = ref;
  };

  componentDidMount() {
    this._lastNativeRefreshing = this.props.refreshing;
  }

  componentDidUpdate(prevProps: NativePullToRefreshFooterProps) {
    if (this.props.refreshing !== prevProps.refreshing) {
      this._lastNativeRefreshing = this.props.refreshing;
    } else if (this.props.refreshing !== this._lastNativeRefreshing && this._nativeRef) {
      PullToRefreshFooterCommands.setNativeRefreshing(this._nativeRef, this.props.refreshing);
      this._lastNativeRefreshing = this.props.refreshing;
    }
  }

  render(): ReactNode {
    return React.createElement(
      NativePullToRefreshFooter,
      {
        ...this.props,
        ref: this._setNativeRef,
        onRefresh: this._onRefresh
      }
    );
  }
}

export {PullToRefreshFooter};
