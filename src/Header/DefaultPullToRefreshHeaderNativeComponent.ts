import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent"
import {
    HostComponent,
    ViewProps,
  } from 'react-native'
  import { Int32, DirectEventHandler, Float } from "react-native/Libraries/Types/CodegenTypes";

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

export interface NativePullToRefreshHeaderProps extends ViewProps {
  onRefresh?: DirectEventHandler<undefined>;
  onStateChanged?: DirectEventHandler<PullToRefreshStateChangedEvent>;
  onOffsetChanged?: DirectEventHandler<PullToRefreshOffsetChangedEvent>;
  refreshing: boolean;
  progressViewOffset?: Float;
}
  
export default codegenNativeComponent<NativePullToRefreshHeaderProps>("RNCPullToRefreshHeaderNative") as HostComponent<NativePullToRefreshHeaderProps>; 