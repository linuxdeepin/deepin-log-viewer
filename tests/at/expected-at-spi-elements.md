# deepin-log-viewer Expected AT-SPI Elements

## Expected AT-SPI Element List

| # | AccessibleName | Role | Derivation Chain | Location |
|---|---------------|------|------------------|----------|
| 1 | left_side_bar | Form | setAccessibleName() | `logcollectormain.cpp:161` (non-splitter path), `logcollectormain.cpp:120` (splitter path) |
| 2 | filterWidget | Form | setAccessibleName() | `logcollectormain.cpp:167` (non-splitter path), `logcollectormain.cpp:129` (splitter path) |
| 3 | logTreeView | Form | accessibleName | `displaycontent.cpp:110` |
| 4 | spinnerWidget | Form | setAccessibleName() | `displaycontent.cpp:144` |
| 5 | spinnerWidget_K | Form | setAccessibleName() | `displaycontent.cpp:147` |
| 6 | detailInfoWidget | Form | setAccessibleName() | `displaycontent.cpp:152` |
| 7 | ExportProgressDlg | Form | setAccessibleName() | `displaycontent.cpp:178` |
| 8 | Export All | Button | setAccessibleName() | `logcollectormain.cpp:233` |
| 9 | Refresh Now | Button | setAccessibleName() | `logcollectormain.cpp:239` |
| 10 | refresh_interval_menu | Menu | setAccessibleName() | `logcollectormain.cpp:200` |
| 11 | centralWidget | Form | setAccessibleName() | `logcollectormain.cpp:180` (non-splitter path) |
| 12 | searchEdt | Form | objectName | `logcollectormain.cpp:182` (non-splitter path) |
| 13 | searchChildEdt | Text | objectName | `logcollectormain.cpp:183` (non-splitter path) |
| 14 | LogListView | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:277` |
| 15 | LogTreeView | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:278` |
| 16 | FilterContent | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:279` |
| 17 | DisplayContent | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:280` |
| 18 | LogPeriodButton | Button | SET_BUTTON_ACCESSIBLE fallback | `accessible.h:281` |
| 19 | LogNormalButton | Button | SET_BUTTON_ACCESSIBLE fallback | `accessible.h:282` |
| 20 | LogIconButton | Button | SET_BUTTON_ACCESSIBLE fallback | `accessible.h:283` |
| 21 | LogCombox | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:284` |
| 22 | LogSpinnerWidget | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:285` |
| 23 | logDetailInfoWidget | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:286` |
| 24 | logDetailEdit | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:287` |
| 25 | ExportProgressDlg | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:288` |
| 26 | LogCollectorMain | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:289` |
| 27 | LogViewHeaderView | Form | SET_FORM_ACCESSIBLE fallback | `accessible.h:290` |

## Notes
- All custom widget types now have class-level AT-SPI registration in `accessible.h`
- Widgets like LogListView and FilterContent also have explicit setAccessibleName() in addition to the class-level registration
- The `SET_FORM_ACCESSIBLE` macro provides a fallback name when `setAccessibleName()` is not set
- All widget names use English and PascalCase convention
