## NS_NearbyItemEntry 위젯 문제 해결 가이드

주변 아이템 위젯에 아이템 이름과 아이콘이 표시되지 않는 문제를 해결하기 위한 체크리스트입니다.

### 1. 블루프린트 위젯 설정 확인

1. **NS_NearbyItemEntry 블루프린트 위젯**:
   - 다음 위젯이 정확히 같은 이름으로 존재해야 합니다:
     - `ItemNameText`: TextBlock 위젯
     - `ItemQuantityText`: TextBlock 위젯
     - `ItemIcon`: Image 위젯
     - `PickupButton`: Button 위젯
   - 위젯 이름은 대소문자를 정확히 구분해야 합니다.

2. **NS_NearbyItemsWidget 블루프린트 위젯**:
   - `ItemsScrollBox`: ScrollBox 위젯이 있어야 합니다.
   - `ItemEntryClass` 변수에 `NS_NearbyItemEntry` 블루프린트가 설정되어 있어야 합니다.

### 2. 아이템 데이터 확인

1. **NS_InventoryBaseItem 클래스**:
   - `GetItemName()` 함수가 유효한 FText를 반환하는지 확인하세요.
   - `GetItemIcon()` 함수가 유효한 UTexture2D*를 반환하는지 확인하세요.

2. **Pickup 클래스**:
   - `GetItem()` 함수가 유효한 UNS_InventoryBaseItem*을 반환하는지 확인하세요.
   - `GetQuantity()` 함수가 올바른 수량을 반환하는지 확인하세요.

### 3. 디버그 로그 확인

추가된 디버그 로그를 통해 다음 사항을 확인할 수 있습니다:
- 위젯 컴포넌트가 제대로 생성되었는지 여부
- 아이템 정보가 올바르게 전달되는지 여부
- 아이템 이름과 아이콘이 설정되는지 여부

로그에서 다음과 같은 오류 메시지를 확인하세요:
- "ItemNameText가 유효하지 않습니다"
- "ItemIcon이 유효하지 않습니다"
- "아이콘 텍스처가 null입니다"

### 4. 수정된 내용

1. **위젯 유효성 검사 추가**:
   - `NativeConstruct`에서 모든 위젯 컴포넌트의 유효성을 검사합니다.
   - 유효하지 않은 컴포넌트가 있으면 오류 로그를 출력합니다.

2. **아이템 정보 설정 개선**:
   - 아이템 이름과 아이콘 설정 시 상세한 로그를 출력합니다.
   - 아이콘이 없는 경우 기본 브러시를 설정합니다.

3. **아이템 목록 업데이트 개선**:
   - 각 아이템 추가 시 로그를 출력합니다.
   - 총 추가된 아이템 수를 로그로 출력합니다.

### 5. 테스트 방법

1. 게임을 실행하고 인벤토리 메뉴를 엽니다.
2. 로그에서 다음 메시지를 확인합니다:
   - "NS_NearbyItemEntry::NativeConstruct 호출"
   - "SetItemInfo 호출 - 아이템: xxx"
   - "아이템 이름: xxx"
   - "아이콘 텍스처 설정: xxx"

3. 위젯이 여전히 표시되지 않는다면:
   - 블루프린트에서 위젯의 가시성 설정을 확인하세요.
   - 위젯의 크기와 위치가 적절한지 확인하세요.
   - 위젯의 Z-Order가 다른 위젯에 가려지지 않는지 확인하세요.