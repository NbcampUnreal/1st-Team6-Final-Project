## 주변 아이템 위젯 문제 해결 가이드

로그를 분석한 결과, 위젯이 생성되고 표시 시도까지는 정상적으로 진행되고 있지만 화면에 보이지 않는 문제가 있습니다. 다음 수정사항을 적용했습니다:

### 1. 위젯 크기 및 위치 설정
- NS_NearbyItemsWidget의 크기가 너무 작거나 0일 수 있어 기본 크기를 설정했습니다.
- NS_InventoryHUD에서 위젯을 표시할 때 명시적으로 위치와 크기를 설정했습니다.
- Z-Order를 100으로 높게 설정하여 다른 위젯에 가려지지 않도록 했습니다.

### 2. 디버그 로그 추가
- 위젯 생성 및 표시 과정에 상세한 로그를 추가했습니다.
- ItemsScrollBox와 ItemEntryClass의 유효성 검사 로그를 추가했습니다.

### 3. 블루프린트 설정 확인 사항
1. **NS_NearbyItemsWidget 블루프린트**:
   - 반드시 `ItemsScrollBox`라는 이름의 ScrollBox 위젯이 있어야 합니다.
   - 디자이너 뷰에서 위젯의 크기가 충분히 커야 합니다(최소 300x400).
   - `ItemEntryClass` 변수에 `NS_NearbyItemEntry` 블루프린트가 설정되어 있어야 합니다.

2. **NS_NearbyItemEntry 블루프린트**:
   - `ItemNameText`, `ItemQuantityText`, `ItemIcon`, `PickupButton` 위젯이 있어야 합니다.
   - 각 위젯의 이름이 정확히 일치해야 합니다.

3. **NS_InventoryHUD 블루프린트**:
   - `NearbyItemsWidgetClass` 변수에 `NS_NearbyItemsWidget` 블루프린트가 설정되어 있어야 합니다.

### 4. 테스트 방법
1. 게임을 실행하고 인벤토리 메뉴를 열어봅니다.
2. 로그에서 다음 메시지를 확인합니다:
   - "NearbyItemsWidget이 유효함, 표시 시도"
   - "NS_NearbyItemsWidget 크기: xxx x xxx"
   - "UpdateItemsList 호출됨 - 아이템 수: x"

3. 위젯이 여전히 보이지 않는다면:
   - 블루프린트에서 위젯의 Z-Order를 더 높게 설정해보세요.
   - 위젯의 배경색을 설정하여 가시성을 확인해보세요.
   - 위젯의 앵커를 화면 중앙으로 설정해보세요.

### 5. 주의사항
- 위젯이 생성되고 표시되더라도 주변에 아이템이 없으면 내용이 표시되지 않을 수 있습니다.
- 테스트를 위해 주변에 Pickup 액터를 배치하여 아이템이 감지되는지 확인하세요.