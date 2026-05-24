import React, { ReactNode } from 'react';
import {
	FlatList,
	Image,
	ListRenderItem,
	Platform,
	ScrollView,
	StyleProp,
	StyleSheet,
	Text,
	TouchableOpacity,
	View,
	ViewStyle,
} from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';

export const demoTheme = {
	colors: {
		background: '#F3F6FA',
		surface: '#FFFFFF',
		surfaceMuted: '#F8FAFC',
		text: '#101828',
		muted: '#5F6C80',
		subtle: '#9AA6B8',
		line: '#DDE3EC',
		lineSoft: '#E9EEF5',
		blue: '#2563EB',
		cyan: '#0891B2',
		green: '#168A5A',
		amber: '#B7791F',
		rose: '#C02668',
		indigo: '#4F46E5',
		violet: '#7C3AED',
		orange: '#C2410C',
	},
};

export interface DemoItem {
	title: string;
	subtitle?: string;
	badge?: string;
	accentColor?: string;
}

interface DemoListProps<T extends DemoItem> {
	title: string;
	subtitle?: string;
	eyebrow?: string;
	data: Array<T>;
	accentColor?: string;
	headerMedia?: ReactNode;
	contentContainerStyle?: StyleProp<ViewStyle>;
	onItemPress: (item: T) => void;
	keyExtractor?: (item: T, index: number) => string;
}

interface DemoListHeaderProps {
	title: string;
	subtitle?: string;
	eyebrow?: string;
	accentColor?: string;
	children?: ReactNode;
}

interface DemoScreenProps extends DemoListHeaderProps {
	children: ReactNode;
	contentContainerStyle?: StyleProp<ViewStyle>;
}

interface DemoPanelProps {
	children: ReactNode;
	style?: StyleProp<ViewStyle>;
}

interface DemoSafeAreaViewProps {
	children: ReactNode;
	style?: StyleProp<ViewStyle>;
	edges?: Array<'top' | 'bottom'>;
}

interface DemoButtonProps {
	title: string;
	accentColor?: string;
	disabled?: boolean;
	style?: StyleProp<ViewStyle>;
	onPress?: () => void;
}

export function DemoList<T extends DemoItem>({
	title,
	subtitle,
	eyebrow,
	data,
	accentColor = demoTheme.colors.blue,
	headerMedia,
	contentContainerStyle,
	onItemPress,
	keyExtractor,
}: DemoListProps<T>) {
	const insets = useSafeAreaInsets();
	const renderItem: ListRenderItem<T> = ({ item }) => (
		<DemoListCard
			item={item}
			accentColor={item.accentColor ?? accentColor}
			onPress={() => onItemPress(item)}
		/>
	);

	return (
		<FlatList
			style={styles.screen}
			contentContainerStyle={[
				styles.listContent,
				contentContainerStyle,
				{
					paddingTop: insets.top + pagePaddingTop,
					paddingBottom: insets.bottom + pagePaddingBottom,
				},
			]}
			data={data}
			keyExtractor={keyExtractor ?? ((item, index) => `${item.title}-${index}`)}
			ListHeaderComponent={
				<DemoListHeader
					title={title}
					subtitle={subtitle}
					eyebrow={eyebrow}
					accentColor={accentColor}
				>
					{headerMedia}
				</DemoListHeader>
			}
			renderItem={renderItem}
			showsVerticalScrollIndicator={false}
		/>
	);
}

export function DemoScreen({
	title,
	subtitle,
	eyebrow,
	accentColor = demoTheme.colors.blue,
	children,
	contentContainerStyle,
}: DemoScreenProps) {
	const insets = useSafeAreaInsets();

	return (
		<ScrollView
			style={styles.screen}
			contentContainerStyle={[
				styles.screenContent,
				contentContainerStyle,
				{
					paddingTop: insets.top + pagePaddingTop,
					paddingBottom: insets.bottom + pagePaddingBottom,
				},
			]}
			showsVerticalScrollIndicator={false}
			keyboardShouldPersistTaps="handled"
		>
			<DemoListHeader
				title={title}
				subtitle={subtitle}
				eyebrow={eyebrow}
				accentColor={accentColor}
			/>
			{children}
		</ScrollView>
	);
}

export function DemoListHeader({
	title,
	subtitle,
	eyebrow,
	accentColor = demoTheme.colors.blue,
	children,
}: DemoListHeaderProps) {
	return (
		<View style={styles.header}>
			{eyebrow ? (
				<View style={styles.eyebrowRow}>
					<View style={[styles.eyebrowDot, { backgroundColor: accentColor }]} />
					<Text style={[styles.eyebrow, { color: accentColor }]}>{eyebrow}</Text>
				</View>
			) : null}
			<Text
				style={styles.headerTitle}
				numberOfLines={2}
				adjustsFontSizeToFit
				minimumFontScale={0.82}
			>
				{title}
			</Text>
			{subtitle ? <Text style={styles.headerSubtitle}>{subtitle}</Text> : null}
			<View style={styles.headerLineTrack}>
				<View style={[styles.headerLine, { backgroundColor: accentColor }]} />
			</View>
			{children ? <View style={styles.headerMedia}>{children}</View> : null}
		</View>
	);
}

export function DemoPanel({ children, style }: DemoPanelProps) {
	return <View style={[styles.panel, style]}>{children}</View>;
}

export function DemoSafeAreaView({
	children,
	style,
	edges = ['top', 'bottom'],
}: DemoSafeAreaViewProps) {
	const insets = useSafeAreaInsets();

	return (
		<View
			style={[
				styles.safeArea,
				style,
				{
					paddingTop: edges.includes('top') ? insets.top : 0,
					paddingBottom: edges.includes('bottom') ? insets.bottom : 0,
				},
			]}
		>
			{children}
		</View>
	);
}

export function DemoButton({
	title,
	accentColor = demoTheme.colors.blue,
	disabled = false,
	style,
	onPress,
}: DemoButtonProps) {
	return (
		<TouchableOpacity
			activeOpacity={0.82}
			style={[
				styles.button,
				{ backgroundColor: disabled ? demoTheme.colors.subtle : accentColor },
				style,
			]}
			disabled={disabled}
			onPress={onPress}
		>
			<Text
				style={styles.buttonText}
				numberOfLines={1}
				adjustsFontSizeToFit
				minimumFontScale={0.82}
			>
				{title}
			</Text>
		</TouchableOpacity>
	);
}

function DemoListCard({
	item,
	accentColor,
	onPress,
}: {
	item: DemoItem;
	accentColor: string;
	onPress: () => void;
}) {
	return (
		<TouchableOpacity activeOpacity={0.82} style={styles.itemCard} onPress={onPress}>
			<View style={[styles.itemAccent, { backgroundColor: accentColor }]} />
			<View style={styles.itemBody}>
				<View style={styles.itemTopRow}>
					<Text style={styles.itemTitle} numberOfLines={2}>
						{item.title}
					</Text>
					{item.badge ? (
						<View
							style={[
								styles.badge,
								{
									backgroundColor: tint(accentColor, '10'),
									borderColor: tint(accentColor, '22'),
								},
							]}
						>
							<Text
								style={[styles.badgeText, { color: accentColor }]}
								numberOfLines={1}
							>
								{item.badge}
							</Text>
						</View>
					) : null}
				</View>
				{item.subtitle ? (
					<Text style={styles.itemSubtitle} numberOfLines={2}>
						{item.subtitle}
					</Text>
				) : null}
			</View>
			<Image
				source={require('assets/indicator.png')}
				style={styles.chevron}
				resizeMode="contain"
			/>
		</TouchableOpacity>
	);
}

function tint(color: string, alpha: string) {
	if (color.startsWith('#') && color.length === 7) {
		return `${color}${alpha}`;
	}
	return '#EEF2FF';
}

const elevated = Platform.select({
	ios: {
		shadowColor: '#182235',
		shadowOpacity: 0.045,
		shadowRadius: 10,
		shadowOffset: { width: 0, height: 4 },
	},
	default: {},
});

const pagePaddingTop = 18;
const pagePaddingBottom = 28;

const styles = StyleSheet.create({
	screen: {
		flex: 1,
		backgroundColor: demoTheme.colors.background,
	},
	listContent: {
		paddingHorizontal: 18,
	},
	screenContent: {
		paddingHorizontal: 18,
	},
	safeArea: {
		flex: 1,
	},
	header: {
		paddingTop: 2,
		paddingBottom: 20,
	},
	eyebrowRow: {
		flexDirection: 'row',
		alignItems: 'center',
		marginBottom: 9,
	},
	eyebrowDot: {
		width: 7,
		height: 7,
		borderRadius: 3.5,
		marginRight: 8,
	},
	eyebrow: {
		fontSize: 12,
		lineHeight: 16,
		fontWeight: '800',
		textTransform: 'uppercase',
		letterSpacing: 0,
	},
	headerTitle: {
		color: demoTheme.colors.text,
		fontSize: 32,
		lineHeight: 38,
		fontWeight: '800',
		letterSpacing: 0,
	},
	headerSubtitle: {
		color: demoTheme.colors.muted,
		fontSize: 15,
		lineHeight: 22,
		marginTop: 8,
	},
	headerLineTrack: {
		width: 96,
		height: 4,
		backgroundColor: demoTheme.colors.lineSoft,
		borderRadius: 2,
		marginTop: 16,
		overflow: 'hidden',
	},
	headerLine: {
		width: 42,
		height: 4,
		borderRadius: 2,
	},
	headerMedia: {
		marginTop: 20,
	},
	itemCard: {
		minHeight: 82,
		flexDirection: 'row',
		alignItems: 'center',
		backgroundColor: demoTheme.colors.surface,
		borderRadius: 8,
		borderWidth: StyleSheet.hairlineWidth,
		borderColor: demoTheme.colors.lineSoft,
		marginBottom: 12,
		paddingVertical: 15,
		paddingLeft: 16,
		paddingRight: 14,
		...elevated,
	},
	itemAccent: {
		width: 3,
		height: 44,
		borderRadius: 1.5,
		marginRight: 14,
	},
	itemBody: {
		flex: 1,
		minWidth: 0,
		marginRight: 12,
	},
	itemTopRow: {
		flexDirection: 'row',
		alignItems: 'center',
	},
	itemTitle: {
		flex: 1,
		minWidth: 0,
		color: demoTheme.colors.text,
		fontSize: 16,
		lineHeight: 21,
		fontWeight: '800',
		letterSpacing: 0,
	},
	itemSubtitle: {
		color: demoTheme.colors.muted,
		fontSize: 13,
		lineHeight: 19,
		marginTop: 5,
	},
	badge: {
		maxWidth: 86,
		borderRadius: 6,
		borderWidth: StyleSheet.hairlineWidth,
		paddingHorizontal: 8,
		paddingVertical: 4,
		marginLeft: 10,
	},
	badgeText: {
		fontSize: 11,
		lineHeight: 14,
		fontWeight: '800',
		letterSpacing: 0,
	},
	chevron: {
		width: 12,
		height: 12,
		tintColor: demoTheme.colors.subtle,
	},
	panel: {
		backgroundColor: demoTheme.colors.surface,
		borderRadius: 8,
		borderWidth: StyleSheet.hairlineWidth,
		borderColor: demoTheme.colors.lineSoft,
		padding: 20,
		marginBottom: 16,
		...elevated,
	},
	button: {
		height: 48,
		borderRadius: 8,
		alignItems: 'center',
		justifyContent: 'center',
		paddingHorizontal: 18,
	},
	buttonText: {
		color: '#FFFFFF',
		fontSize: 16,
		lineHeight: 21,
		fontWeight: '700',
		letterSpacing: 0,
	},
});
