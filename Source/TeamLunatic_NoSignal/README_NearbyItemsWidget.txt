## 주변 아이템 위젯 설정 가이드

NS_NearbyItemsWidget이 표시되지 않는 문제를 해결하기 위한 체크리스트:

1. **게임 모드 설정 확인**
   - 게임 모드 블루프린트에서 HUD 클래스가 `ANS_InventoryHUD`로 설정되어 있는지 확인하세요.
   - 경로: 프로젝트 설정 > 맵 & 모드 > 기본 클래스 > HUD 클래스

2. **NS_InventoryHUD 블루프린트 설정**
   - `NS_InventoryHUD` 블루프린트를 열고 `NearbyItemsWidgetClass` 변수에 `NS_NearbyItemsWidget` 블루프린트를 할당했는지 확인하세요.

3. **NS_NearbyItemsWidget 블루프린트 생성**
   - `NS_NearbyItemsWidget` 블루프린트를 생성하고 다음 요소를 포함해야 합니다:
     - `ItemsScrollBox`: 스크롤 박스 위젯 (이름이 정확히 일치해야 함)
   - 디자인 탭에서 위젯의 가시성이 적절하게 설정되어 있는지 확인하세요.

4. **NS_NearbyItemEntry 블루프린트 생성**
   - `NS_NearbyItemEntry` 블루프린트를 생성하고 다음 요소를 포함해야 합니다:
     - `ItemNameText`: 텍스트 블록 위젯
     - `ItemQuantityText`: 텍스트 블록 위젯
     - `ItemIcon`: 이미지 위젯
     - `PickupButton`: 버튼 위젯

5. **NS_NearbyItemsWidget 블루프린트 설정**
   - `ItemEntryClass` 변수에 `NS_NearbyItemEntry` 블루프린트를 할당했는지 확인하세요.

6. **디버그 로그 확인**
   - 게임 실행 시 출력 로그에서 다음 메시지를 확인하세요:
     - "NearbyItemsWidgetClass가 유효합니다."
     - "NearbyItemsWidget이 성공적으로 생성되었습니다."
     - "ShowNearbyItemsWidget 호출됨"
   - 오류 메시지가 있다면 해당 문제를 해결하세요.

7. **Pickup 클래스 확인**
   - `Pickup` 클래스에 `GetItem`과 `GetQuantity` 함수가 제대로 구현되어 있는지 확인하세요.

8. **NS_InventoryBaseItem 클래스 확인**
   - `NS_InventoryBaseItem` 클래스에 `GetItemIcon` 함수가 제대로 구현되어 있는지 확인하세요.

문제가 지속되면 다음 코드를 확인하세요:
1. NS_InventoryHUD.cpp의 OpenInventoryWidget 함수
2. NS_InteractionComponent.cpp의 ToggleInventoryMenu 함수
3. NS_PlayerCharacterBase.cpp의 SetupPlayerInputComponent 함수에서 ToggleMenuAction 바인딩 부분