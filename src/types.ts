import React from 'react';
import {NativeSyntheticEvent, ViewProps} from 'react-native';
import { Int32, DirectEventHandler,Float } from "react-native/Libraries/Types/CodegenTypes";
export const PullToRefreshStateIdle = 0;
export const PullToRefreshStateComing = 1;
export const PullToRefreshStateRefreshing = 2;

export type StateChangedEventData = Readonly<{
  state: Int32;
}>;
  
export type OffsetChangedEventData = Readonly<{
  offset: Float;
}>;

  
export type PullToRefreshStateChangedEvent = StateChangedEventData; 
export type PullToRefreshOffsetChangedEvent = OffsetChangedEventData;

export interface PullToRefreshHeaderProps {
  onRefresh?: () => void;
  refreshing: boolean;
}

export interface PullToRefreshFooterProps {
  onRefresh?: () => void;
  refreshing: boolean;
  noMoreData?: boolean;
  manual?: boolean;
}

export interface PullToRefreshProps extends ViewProps {
  onRefresh?: () => void; 
  refreshing?: boolean;
  onLoadMore?: () => void; 
  loadingMore?: boolean;
  noMoreData?: boolean;
  header?: React.ReactElement<PullToRefreshHeaderProps>;
  footer?: React.ReactElement<PullToRefreshFooterProps>;
}
