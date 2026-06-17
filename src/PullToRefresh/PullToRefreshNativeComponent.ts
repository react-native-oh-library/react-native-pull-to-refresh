import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent"
import {DirectEventHandler } from "react-native/Libraries/Types/CodegenTypes";  
import {
    HostComponent,
    ViewProps,
  } from 'react-native'

export interface NativePullToRefreshProps extends ViewProps {
  requestDisallowInterceptTouchEvent?: boolean;
}
  
export default codegenNativeComponent<NativePullToRefreshProps>("RNCPullToRefreshNative") as HostComponent<NativePullToRefreshProps>; 