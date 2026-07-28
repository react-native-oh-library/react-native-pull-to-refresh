> Template Version: v0.3.0

<p align="center">
  <h1 align="center"> <code>@react-native-ohos/pull_to_refresh</code> </h1>
</p>

This project is developed based on [@sdcx/pull-to-refresh@0.27.0](https://github.com/sdcxtech/react-native-troika/tree/master/packages/pull-to-refresh).

## 1. Installation and Usage

Please go to the Releases release address of the third-party library to view the supporting version information: [@react-native-ohos/pull_to_refresh Releases](https://github.com/react-native-oh-library/react-native-pull-to-refresh/releases). 

| Version | Releases info                   | Support RN version |
| ---------- | ------------------------------------------------------------ | ---------- |
| 0.27.1      | [@react-native-ohos/pull_to_refresh Releases](https://github.com/react-native-oh-library/react-native-pull-to-refresh/releases) | 0.72       |
| 0.28.1     | [@react-native-ohos/pull_to_refresh Releases](https://github.com/react-native-oh-library/react-native-pull-to-refresh/releases) | 0.77  

For older versions that are not published to npm, install the tgz package by referring to the [Installation Guide](/en-us/tgz-usage.md).

Navigate to your project directory and enter the following command:

<!-- tabs:start -->

#### **npm**

```bash
npm install @react-native-ohos/pull_to_refresh
```

#### **yarn**

```bash
yarn install @react-native-ohos/pull_to_refresh
```

<!-- tabs:end -->

The following code demonstrates the basic usage of this library:


```js

import React, {useRef, useState} from 'react';
import {PullToRefresh} from '@sdcx/pull-to-refresh';
import {FlatListPage, useDemoFlatlistData} from './FlatListPage';
import {SpinnerPullToRefreshHeader} from '../SpinnerPullToRefreshHeader';
import { Tester, Filter, TestCase, TestSuite } from '@rnoh/testerino';
function PullRefreshFlatList() {
    const [refreshing, setRefreshing] = useState(true);
    const [loadingMore, setLoadingMore] = useState(true);
    const [noMoreData, setNoMoreData] = useState(false);
    const {flatlistData, addFlatlistRefreshItem, addFlatlistLoadMoreItem} = useDemoFlatlistData();
    const pendingAction = useRef<ReturnType<typeof setTimeout> | null>(null);

    const clearPendingAction = () => {
        if (pendingAction.current) {
            clearTimeout(pendingAction.current);
        }
    };

    const beginRefresh = async () => {
        setRefreshing(true);
        pendingAction.current = setTimeout(() => {
            addFlatlistRefreshItem();
            endRefresh();
        }, 2000);
    };

    const endRefresh = () => {
        clearPendingAction();
        setRefreshing(false);
        setNoMoreData(false);
    };

    const loadMore = () => {
        setLoadingMore(true);
        pendingAction.current = setTimeout(() => {
            addFlatlistLoadMoreItem();
            endLoadMore();
        }, 3500);
    };

    const endLoadMore = () => {
        clearPendingAction();
        setLoadingMore(false);
        setNoMoreData(true);
    };

    return (
        <Tester style={{ flex: 1, marginTop: 30 }}>
<PullToRefresh
style={{height: '100%'}}
header={<SpinnerPullToRefreshHeader refreshing={refreshing} onRefresh={
    beginRefresh
} />}
    loadingMore={loadingMore}
    noMoreData={noMoreData}
    onLoadMore={loadMore}>
        <FlatListPage data={flatlistData} />
    </PullToRefresh>
    </Tester>
    );
}

export default PullRefreshFlatList;


```

## 2. Manual Link

This section provides guidance for manually linking native dependencies.

First, you need to open the HarmonyOS project `harmony` in your project with DevEco Studio.

### 2.1 Overrides RN SDK

To ensure the project depends on the same version of the RN SDK, you need to add an `overrides` field in the `harmony/oh-package.json5` file at the project's root directory. This field should point to the RN SDK version required by the project. The version can be a specific version number, a fuzzy version, a local HAR package, or a source code directory.

For more information about this field, please refer to the [official documentation](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/ide-oh-package-json5-V5#zh-cn_topic_0000001792256137_overrides).

```json
{
  "overrides": {
    "@rnoh/react-native-openharmony": "^0.72.58"
  }
}
```

### 2.2 Importing Native Code

There are currently two methods:

- By importing HAR packages;
- By directly linking the source code.

Method 1: Importing via HAR package (Recommended)

> [!TIP]
> The HAR package is located in the `harmony` folder of the third-party library's installation path.

Open `entry/oh-package.json5` and add the following dependency:

```json
"dependencies": {
  "@react-native-ohos/pull_to_refresh": "file:../../node_modules/@react-native-ohos/pull_to_refresh/harmony/pull_to_refresh.har"
}
```

Click the `sync` button in the top right corner.

Or execute in the terminal:

```bash
cd entry
ohpm install
```

Method 2: Directly linking the source code

> [!TIP]
> To link the source code directly, please refer to the [instructions for linking source code directly](/zh-cn/link-source-code.md).

### 2.3 Configure CMakeLists and Import NestedScrollViewPackage

Open `entry/src/main/cpp/CMakeLists.txt` and add the following:

```diff
+ set(OH_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules")

# RNOH_BEGIN: manual_package_linking_1
+ add_subdirectory("${OH_MODULES}/@react-native-ohos/pull_to_refresh/src/main/cpp" ./pull_to_refresh)
# RNOH_END: manual_package_linking_1

# RNOH_BEGIN: manual_package_linking_2
+ target_link_libraries(rnoh_app PUBLIC rnoh_pull_to_refresh)
# RNOH_END: manual_package_linking_2
```

Open `entry/src/main/cpp/PackageProvider.cpp` and add the following:

```diff
#include "RNOH/PackageProvider.h"
#include "generated/RNOHGeneratedPackage.h"
+ #include "PullToRefreshPackage.h"

using namespace rnoh;

std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
    return {
      std::make_shared<RNOHGeneratedPackage>(ctx),
+      std::make_shared<PullToRefreshPackage>(ctx)
    };
}
```

### 2.4 Run

Click the `sync` button in the top right corner.

Or execute in the terminal:

```bash
cd entry
ohpm install
```

Then, build and run the project.

## 3. Constraints and Limitations

### 3.1 Compatibility

Please go to the Releases page of the third-party library to check the version information for compatibility: [@react-native-ohos/pull_to_refresh Releases](https://github.com/react-native-oh-library/react-native-pull-to-refresh/releases)

## 4. Props

> [!TIP]
> The "Platform" column indicates the platforms on which these props are supported in the original third-party library.

> [!TIP]
> If the value in the "HarmonyOS Support" column is "yes," it means the prop is supported on the HarmonyOS platform; "no" means it is not supported; "partially" means that some of its features are supported. The usage of this prop is the same on different platforms, and its effect is consistent with that on iOS or Android.

| Name        | Description         | Type    | Required | Platform | HarmonyOS Support |
| ----------- |---------------------|---------|----------| -------- | ----------------- |
| refreshing| Control the display and hide of the refresh head or bottom     | boolean | YES      | All      | yes               |
| noMoreData | Control whether there is more data           | boolean  | NO       | All      | yes               |
| manual | Control the automatic pull-up loading function          | boolean  | NO       | All      | yes               |

## 5.方法
| Name              | Description           | Type     | Platform | HarmonyOS Support |
| ----------- |---------------------|---------| -------- | ----------------- |
| onRefresh         | Refresh callback               |   function       | All      | yes               |
| onStateChanged    | Refresh status callback           |   function       | All      | yes               |
| onOffsetChanged   | Refresh the offset callback          |    function      | All      | yes               |

## 6. Known Issues

None

## 7. Open Source License

This project is based on [The MIT License (MIT)](https://github.com/react-native-oh-library/react-native-pull-to-refresh/blob/sig/LICENSE), feel free to enjoy and participate in open source.