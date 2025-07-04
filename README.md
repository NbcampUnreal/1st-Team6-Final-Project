# 🧟‍♂️ No Signal

![NoSignal (2)](https://github.com/user-attachments/assets/6269c4e1-d883-4010-9dd9-fe22db51541a)


프로젝트명: No Signal

장르: 생존, 호러, 공포, 좀비

개발 엔진: Unreal Engine 5.5

개발 기간: 2025.04.30 ~ 2025.07.01

'No Signal'은 좀비로 폐허가 된 도시에서 살아남아야 하는 1인칭 생존 호러 게임입니다. 모든 것이 무너진 세상에서 생존에 필수적인 탈출 아이템들을 찾아내고, 끊임없이 몰려오는 좀비 무리를 피해 탈출로를 확보해야 합니다. 과연 당신은 이 절망적인 도시에서 벗어날 수 있을까요?


# 🎮 주요 게임 특징
+ 1인칭 생존 게임: 긴장감 넘치는 1인칭 시점으로 극한의 공포를 경험하세요.

+ 세션 매칭 기반 멀티플레이: 친구들과 함께 또는 홀로 생존에 도전할 수 있습니다.

+ 다양한 상호작용 아이템: 생존에 필요한 핵심 아이템들을 찾아 활용하세요.

+ 포스트 아포칼립스 세계: 디테일한 레벨 디자인으로 재현된 절망적인 도시를 탐험하세요.

+ 지능적인 좀비 AI: 플레이어를 끈질기게 추적하고 위협하는 좀비 AI를 경험하세요.



# 💻 핵심 기술

## 멀티플레이

+ 데디케이티드 서버 빌드 및 Flask 웹 프레임워크를 활용한 매치메이킹 시스템을 구현
+ 세션 탐지, 하트비트, 삭제, 포트 할당 등 세션 관리 기능을 완벽하게 지원하며 안정적인 멀티플레이 환경을 제공.
+ 타이머 핸들러 사용을 최소화 한 멀티플레이 오버플로우 방지

## 캐릭터 

+ 1인칭 캐릭터를 구현하여 더 현실감이 느껴지는 플레이에 초점을 두었고 실제 사람인것같은 느낌을 주기위해 AimOffset2D제작을 모두 해줌.
+ 무기를 잡고있는 손에 뼈들에 Key값도 세밀하게 수정.
+ 캐릭터가 상/하/좌/우를 바라보는게 멀티에서 정확하게 표현되도록 함.
+ Turn In Place를 구현하여 캐릭터가 회전할때도 더 현실감 있게 회전하는것을 구현.
+ 캐릭터가 던지는 병을 구현할때 실제로 깨지는 것을 구현하기 위해 언리얼에서 제공하는 "지오메트리컬렉션 액터"를 만들어 깨지는것을 실제처럼 구현.

## AI

### Animation
  - AnimNotify를 통해 타이밍 기반 공격 판정 및 피격 이펙트 트리거
  - Random Sequence 노드를 활용해 반복적이지 않은 공격/리액션 구현
  - 차지 공격시 RootMotion 적용으로 자연스러운 이동 연출
  
### 최적화
- 거리 기반 좀비 활성화
- 플레이어와의 거리를 계산하여 일정 반경 내일 때만 활성화 (Active)

- 활성화 상태에서는:
  AI 로직 실행
  컴포넌트 Tick, 애니메이션, 충돌 활성화
  일정 거리 이상 떨어지면 자동으로 UnActive 처리
  → AI Tick, 컴포넌트 Tick, 애니메이션 비활성화

- 근거리 중심 스폰
  - 게임 시작 시 맵 전체에 좀비를 스폰하지 않음
  - 플레이어 주변 일정 거리 내에서만 좀비를 동적으로 스폰 또는 활성화
  - 불필요한 Actor 생성과 리소스 점유 최소화
   
- AI/컴포넌트 효율화
  - UNavigationInvokerComponent로 NavMesh를 좀비 주변에만 생성
  - 스켈레탈 메쉬/AI 컴포넌트는 상태에 따라 Tick 자동 제어
  - 멀티플레이에서도 상태 동기화를 최소 비용으로 유지 (RepNotify)

### 피격 효과

#### 1. VFX.
- 나이아가라 이펙트 (Niagara)
  - 피격 위치(Bone Socket 기준)에서 나이아가라 시스템을 이용한 피 분사 효과 생성
  - 총알 방향에 따라 이펙트의 방향성과 강도도 반영
  - Multicast로 전파되어 멀티플레이어에서도 동일한 피격 효과 연출
  - BP로 Export하여 이미터의 정보를 전달.

- 데칼(Decal) 생성
  - Export된 정보를 참조한 지점에 혈흔(데칼) 액터를 동적으로 생성
  - 바닥이나 벽면에 혈흔이 남는 잔상 효과 제공
  - BoneName 을 가져와 AttachToActor()로연출

#### 2. PhysicsSimulate.

- ApplyPointDamage 이벤트를 통해 피격 부위의 HitInfo을 획득
  - 해당 본(Bone)과 Direction을 계산.
- SetAllBodiesBelowSimulatePhysics() + AddImpulse() 조합으로 피격 부위에만 집중된 물리 반응 구현

    
## 상호작용 & 아이템
+ 아이템 데이터 테이블에서 아이템을 추가하고, 아이템을 월드에 스폰하여 캐릭터가 아이템을 파밍할 수 있도록 함.
+ 특정 아이템은 월드에서 단서를 얻어서 해당 위치에서 파밍이 가능.
+ 플레이어 근처에 아이템이 있는 경우에 아웃라인으로 아이템 위치가 어디인지 표기.

## 최적화

+ fab에서 받은 에셋들의 폴리곤 수를 줄이고 LOD의 화면 비율을 더 크게 설정해 렌더링 부하를 줄임.
+ 텍스처는 Base 2048, Normal 1024, ORM 512를 기준으로 텍스처 최대 사이즈를 설정했고 액터를 병합하여 아틀라스 텍스처를 제작해 드로우콜을 줄임.
+ 중요 환경요소인 포그(안개)로 인해 가시거리가 멀지 않다는 점을 활용해 디스턴스 컬링을 이용해 SM, BP 등 에셋들을 모두 컬링해 FPS를 확보할 수 있었고 크기가 큰 빌딩의 경우 분할하여 빌딩의 보이지 않는 부분은 오클루전 컬링 되게 함.


# 👥 팀 구성 및 역할

## 한상혁 (팀장)

+ 데디케이티드 서버 빌드 및 매치메이킹 시스템 구현 (서버 빌드, Flask 웹 프레임워크)
+ EC2 세팅
+ 세션 관리 기능 작성 (탐지, 하트비트, 삭제, 포트 할당 등)
+ 게임 플레이 프레임워크 구축 (Game Mode, State, Player State, Instance)
+ 멀티플레이 로비 제작
+ 추적자 좀비 AI 설계 및 구현
+ 레벨 디자인 및 HUD 보조
+ 패키징 및 에러 수정(Buffer Over Flow 해결, 나침반 에러 해결, 멀티플레이 에러 해결)
+ 멀티플레이 오버플로우 방지를 위한 타이머핸들러 조절

## 최명일 (부팀장)
+ 인벤토리 구현
+ Pickup 아이템 구현
+ 아이템 줍기, 문 열기, 아이템 버리기 등.. 상호작용 구현
+ 캐릭터와 협업으로 노티파이 작업(상호작용 애니메이션 출력)
+ 퀵슬롯 구현
+ 인벤토리 + 퀵슬롯 UI 제작
+ 엔딩 트리거 존 구현


## 박찬영

+ 캐릭터 제작 및 애니메이션 제작
+ 캐릭터 RPC로직 구현
+ 무기 공격 로직 구현
+ Turn In Place구현
+ 지오메트리 컬렉션 액터 제작 및 적용하여 던지는 병이 벽이나 바닥에 닿으면 깨지도록 구현
+ 총기류 무기 단발/연발 구현
+ 총 이펙트 작업 구현
+ 사운드 작업
+ UI구현 및 로딩스크린  구현
+ 기존 UI 리펙토링
+ UI에 설정에서 실제로 위도우 크기, 창 모드, 사운드 크기 조절 설정 구현
+ 좀비 스폰로직 싱글/멀티 구현
+ 케릭터 낙하 데미지 구현
+ 캐릭터 죽음 처리 애니메이션 구현
+ UI 로고 제작
+ 총알 자국 Decal스폰 구현

## 장진우
+ 캐릭터 기본 이동 제작
+ 캐릭터 이동 관련 제한 로직 구현
+ 무기 동작 제한 로직 구현
+ Turn In Place 보완
+ 메타휴먼 적용
+ 멀티에서 캐릭터가 다르게 되도록 구현

## 정광식
+ 랜드스케이프 및 머터리얼 제작
+ 레벨 목업
+ 불필요한 머터리얼 노드 수정(Emessive, Reflection)
+ 빌딩 및 정화시설 제작
+ 레벨 인스턴스, 패킹된 레벨 블루프린트 작업
+ 폴리지 작업 및 최적화
+ 레벨 디테일 작업
+ 에셋 및 텍스처 최적화 작업(메시, 텍스처 사이즈 관리, 액터 병합, 아틀라스 텍스처)
+ 렌더링 최적화 작업(SM, BP 디스턴스 컬링 작업)


## 지승현
- 좀비 캐릭터 및 애니메이션 구현.
- Perception을 이용한 감지 구현.
- Enum을 활용한 State구현.
- BehaviorTree와 Blackboard로 AI구현
- 거리별 공격 로직 구현
- 좀비 피격 시 물리 효과 구현
- 좀비 사운드 구현.
- 나이아가라 및 데칼로 VFX구현.

## 김혜정
+ 아이템 데이터 테이블 작성
+ 아이템 월드 스폰 로직 작성
+ 아이템 상호작용 로직 작업
+ 아이템 콜리전 작업
+ 아이템 레벨 배치
+ UI 보조

## 김세원
+ 전체적인 HUD 작업
+ 포스트아포칼립스 레벨디자인
+ 나침반 작업
+ 로딩스크린 작업
  


# 🛠️ 협업 도구
Slack: 실시간 소통

Notion: 프로젝트 관리 및 문서화

Figma: UI/UX 디자인

GitHub: 버전 관리 및 코드 공유

# 🌐 레벨 디자인

# 📸 시연 영상
