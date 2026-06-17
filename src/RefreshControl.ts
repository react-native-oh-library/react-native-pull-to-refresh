import React from 'react';
import {Platform, RefreshControlProps} from 'react-native';
import {PullToRefresh} from './PullToRefresh';

export default function PullToRefreshControl(props: RefreshControlProps) {
  const pullToRefreshProps = {
    onRefresh: props.onRefresh,
    refreshing: props.refreshing,
    tintColor: props.tintColor,
    title: props.title,
    titleColor: props.titleColor,
    colors: props.colors,
    progressBackgroundColor: props.progressBackgroundColor,
    progressViewOffset: props.progressViewOffset,
  };

  if (Platform.OS === 'android') {
      return React.createElement(PullToRefresh, pullToRefreshProps);
  }
  // @ts-ignore
  return React.createElement(PullToRefresh.DefaultHeader, pullToRefreshProps);
}
