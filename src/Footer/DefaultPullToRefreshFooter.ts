import React, {useCallback, useState} from 'react';
import {StyleSheet, Text,Animated} from 'react-native';
import {
  PullToRefreshFooterProps,
  PullToRefreshOffsetChangedEvent,
  PullToRefreshStateChangedEvent,
  PullToRefreshStateIdle,
  PullToRefreshStateRefreshing,
} from '../types';
import {PullToRefreshFooter} from './native';

export function DefaultPullToRefreshFooter(props: PullToRefreshFooterProps) {
  const {onRefresh, refreshing, noMoreData} = props;

  const [text, setText] = useState('上拉加载更多');

  const onStateChanged = useCallback((event: PullToRefreshStateChangedEvent) => {
    const state = event.nativeEvent.state;
    if (state == PullToRefreshStateIdle) {
      setText('上拉加载更多');
    } else if (state == PullToRefreshStateRefreshing) {
      setText('正在加载更多...');
    } else {
      setText('松开加载更多');
    }
  }, []);

  const onOffsetChanged = useCallback((event: PullToRefreshOffsetChangedEvent) => {
    console.log('refresh footer offset', event.nativeEvent.offset);
  }, []);

  return React.createElement(
    PullToRefreshFooter,
    {
      style: styles.container,
      manual: true,
      onOffsetChanged: onOffsetChanged,
      onStateChanged: onStateChanged,
      onRefresh: onRefresh,
      refreshing: refreshing,
      noMoreData: noMoreData
    },
     React.createElement(
        Text,
        { style: styles.text },
        noMoreData ? '没有更多数据了' : text
      )
  );
}

const styles = StyleSheet.create({
  container: {
    alignItems: 'center',
    backgroundColor: 'red',
    width:'100%',
    height:0
  },
  text_v: {
    width: '100%',
    justifyContent: "center",
    alignItems: 'center',
  },
  text: {
    paddingVertical: 16,
    fontSize: 17,
    color: 'white'
  },
});
