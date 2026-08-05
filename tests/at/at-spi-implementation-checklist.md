# deepin-log-viewer AT-SPI Implementation Checklist

## Pre-Implementation Coverage Baseline

**Scan Method**: Manual static analysis of source code (libclang AST scan unavailable due to build dependency issues; alternative local source analysis performed with annotations where limitations exist).

**Limitation Note**: Full libclang scan not possible due to missing build dependencies (Qt6Core5Compat, polkit-qt6 dev headers, gio-qt6). Analysis was performed via manual source examination of all header files and key cpp files.

### Pre-Status Summary
- Custom widget classes: 15
- Widget classes with AT-SPI registration: 1 (QWidget only)
- Widget classes with setAccessibleName() instances: ~10
- **Estimated coverage**: ~30% (only QWidget fallback was registered)

## Changes Applied

### 1. AT-SPI Factory Registration (accessible.h)
**File**: `application/accessible.h`

**What changed**: Added 14 new class-level AT-SPI registrations for all custom widget types used in the application.

**Details**:
- Added includes for all custom widget headers
- Registered the following widget types with SET_FORM_ACCESSIBLE:
  - `LogListView` - "LogListView" form
  - `LogTreeView` - "LogTreeView" form
  - `FilterContent` - "FilterContent" form
  - `DisplayContent` - "DisplayContent" form
  - `LogPeriodButton` - "LogPeriodButton" button (with press action)
  - `LogNormalButton` - "LogNormalButton" button (with press action)
  - `LogIconButton` - "LogIconButton" button (with press action)
  - `LogCombox` - "LogCombox" form
  - `LogSpinnerWidget` - "LogSpinnerWidget" form
  - `logDetailInfoWidget` - "logDetailInfoWidget" form
  - `logDetailEdit` - "logDetailEdit" form
  - `ExportProgressDlg` - "ExportProgressDlg" form
  - `LogCollectorMain` - "LogCollectorMain" form
  - `LogViewHeaderView` - "LogViewHeaderView" form
- Updated accessibleFactory to match all new types

**Reason**: These widget types had no AT-SPI registration at all. Without registration, accessibility tools cannot identify these custom widgets.

### 2. LogTreeView Accessible Name (displaycontent.cpp)
**File**: `application/displaycontent.cpp`

**What changed**: Added `setAccessibleName("logTreeView")` to the LogTreeView instance created in `initUI()`.

**Details**:
- Line: After `initTableView()`
- Reason: The main log data table needs a recognizable accessible name

### 3. Status Labels Accessible Names (displaycontent.cpp)
**File**: `application/displaycontent.cpp`

**What changed**: Added accessible names to 4 status labels used for displaying empty/no-data states.

**Details**:
- `noResultLabel` → "noResultLabel"
- `notAuditLabel` → "notAuditLabel"
- `noCoredumpctlLabel` → "noCoredumpctlLabel"
- `noPermissionLabel` → "noPermissionLabel"

### 4. Splitter Path Widgets (logcollectormain.cpp)
**File**: `application/logcollectormain.cpp`

**What changed**: Added setObjectName and setAccessibleName to widgets in the SPLITTER_TYPE code path.

**Details**:
- `m_logCatelogue` → objectName: "logTypeSelectList", accessibleName: "left_side_bar"
- `m_topRightWgt` → accessibleName: "filterWidget"

## Post-Implementation Coverage Estimate

### Post-Status Summary
- Custom widget classes: 15
- Widget classes with AT-SPI registration: 15 (100%)
- Widget classes with explicit setAccessibleName(): ~12
- **Estimated coverage**: 85-90%

## Verification Assertions (for AT Test Framework)
1. The main window's accessible name should be "LogCollectorMain" (SET_FORM_ACCESSIBLE fallback)
2. The sidebar list can be located by accessibleName "left_side_bar"
3. The filter area can be located by accessibleName "filterWidget"
4. The data table can be located by accessibleName "logTreeView"
5. The detail widget can be located by accessibleName "detailInfoWidget"
6. Export button is named "Export All" (DIconButton)
7. Refresh button is named "Refresh Now" (DIconButton)
8. All custom buttons (LogPeriodButton, LogNormalButton, LogIconButton) are identifiable by their class fallback names
