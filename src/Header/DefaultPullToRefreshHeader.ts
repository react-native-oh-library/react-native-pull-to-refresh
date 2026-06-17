import React, {useCallback, useState} from 'react';
import {StyleSheet, Text,Animated} from 'react-native';
import {
  PullToRefreshHeaderProps,
  PullToRefreshOffsetChangedEvent,
  PullToRefreshStateChangedEvent,
  PullToRefreshStateIdle,
  PullToRefreshStateRefreshing,
} from '../types';
import {PullToRefreshHeader} from './native';

export function DefaultPullToRefreshHeader(props: PullToRefreshHeaderProps) {
  const {onRefresh, refreshing, progressViewOffset} = props;

  const [text, setText] = useState('下拉刷新');

  const onStateChanged = useCallback((event: PullToRefreshStateChangedEvent) => {
    const state = event.nativeEvent.state;
    if (state === PullToRefreshStateIdle) {
      setText('下拉刷新');
    } else if (state === PullToRefreshStateRefreshing) {
      setText('正在刷新...');
    } else {
      setText('松开刷新');
    }
  }, []);

  const onOffsetChanged = useCallback((event: PullToRefreshOffsetChangedEvent) => {
    console.log('refresh header offset', event.nativeEvent.offset);
  }, []);

  return React.createElement(
    PullToRefreshHeader,
    {
      style: styles.container,
      onOffsetChanged: onOffsetChanged,
      onStateChanged: onStateChanged,
      onRefresh: onRefresh,
      refreshing: refreshing,
      progressViewOffset: progressViewOffset
    },
    React.createElement(
        Text,
        { style: styles.text },
        text
    )
  );
}

const styles = StyleSheet.create({
  container: {
    alignItems: 'center',
    backgroundColor: 'red',
    height:0
  },
  text_v: {
    height: 50,
    width: '100%',
    justifyContent: "center",
    alignItems: 'center'
  },
  text: {
    paddingVertical: 16,
    fontSize: 17,
    color: 'white',
  },
});
