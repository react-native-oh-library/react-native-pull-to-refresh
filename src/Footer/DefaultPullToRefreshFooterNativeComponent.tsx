import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent"
import {
    NativeSyntheticEvent,
    StyleProp,
    ViewStyle,
    NativeScrollPoint,
    HostComponent,
    ViewProps,
  } from 'react-native'
  import { Int32, DirectEventHandler, BubblingEventHandler, WithDefault, Float } from "react-native/Libraries/Types/CodegenTypes";  

export const PullToRefreshStateIdle = 0;
export const PullToRefreshStateComing = 1;
export const PullToRefreshStateRefreshing = 2;
export type PullToRefreshStateIdle = typeof PullToRefreshStateIdle;
export type PullToRefreshStateComing = typeof PullToRefreshStateComing;
export type PullToRefreshStateRefreshing = typeof PullToRefreshStateRefreshing;

export type PullToRefreshState =
  | PullToRefreshStateIdle
  | PullToRefreshStateComing
  | PullToRefreshStateRefreshing;

export type StateChangedEventData = Readonly<{
  state: Int32;
}>;
  
export type OffsetChangedEventData = Readonly<{
  offset: Float;
}>;

  
export type PullToRefreshStateChangedEvent = StateChangedEventData; 
export type PullToRefreshOffsetChangedEvent = OffsetChangedEventData;

export interface NativePullToRefreshFooterProps extends ViewProps {
  onRefresh?: DirectEventHandler<undefined>; 
  onStateChanged?: DirectEventHandler<PullToRefreshStateChangedEvent>;
  onOffsetChanged?: DirectEventHandler<PullToRefreshOffsetChangedEvent>;
  refreshing: boolean;
  noMoreData?: boolean;
  manual?: boolean;
}
  
export default codegenNativeComponent<NativePullToRefreshFooterProps>("RNCPullToRefreshFooterNative") as HostComponent<NativePullToRefreshFooterProps>; 