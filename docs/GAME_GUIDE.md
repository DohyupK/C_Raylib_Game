# Arena Breach — 기능 가이드 & 튜토리얼

**엔진:** raylib 6.2 (C)  
**시점:** 3D 탑다운(비스듬한 추적 카메라)  
**장르:** 아레나 슈팅 / 서바이벌

이 문서는 과제 제출용 기능 설명서이자 플레이 튜토리얼입니다.

---

## 1. 빌드 & 실행

### 권장 (MinGW)

```bat
build.bat
ArenaBreach.exe
```

또는 프로젝트 루트에서:

```bat
mingw32-make
```

`third_party/raylib-6.2_win64_mingw-w64` 와 `raylib.dll` 이 필요합니다(저장소에 포함).

### Visual Studio

`ArenaBreach.sln` 을 열고 **Win32** 구성으로 빌드합니다.  
NuGet 패키지 `raylib 6.2.0` 이 `packages/` 에 있습니다. (현재 NuGet 바이너리는 win32)

---

## 2. 조작법 (튜토리얼)

| 입력 | 동작 |
|------|------|
| W A S D | 이동 (화면 기준, W=위) |
| 마우스 | 조준(지면 교차점) |
| 마우스 왼쪽 | 사격 |
| Space | 점프 |
| Shift | 대시 (쿨다운) |
| Z | 수류탄 |
| X | 무기 변경 (Pistol / Shotgun / Rifle) |
| C | 근처 아이템 줍기 |
| B | 가방 열기/닫기 |
| 1 ~ 8 | 핫바 아이템 사용 |
| 가방 안 드래그 | 슬롯 위치 교환 |
| 가방 안 RMB | 해당 슬롯 아이템 사용 |
| Q | 능력 Berserk (공격·이속 증가) |
| E | 능력 Barrier (실드) |
| R | 능력 Overcharge (연사) |
| Tab | 코스튬 변경 |
| Enter | 튜토리얼 다음 / 게임오버 재시작 |
| ESC | 일시정지 / 재개 |
| F4 | 종료 |

시작 시 Enter 로 튜토리얼 팁을 넘길 수 있습니다.

---

## 3. 단계별 구현 기능 요약

### Step 1 — 플레이어 코어
- 탑다운 카메라, 이동/점프/대시
- 마우스 조준 + 투사체 사격
- 수류탄 포물선·폭발
- 무기 전환, 줍기, 가방 토글 골격

### Step 2 — 인벤토리
- 16슬롯 가방, 하단 1~8 핫바
- 마우스 드래그로 슬롯 스왑
- 아이템: Medkit, Ammo, Speed Tonic, Shield Cell, Grenades, Score Gem

### Step 3 — 괴물 30종
- `monster_data.c` 에 30종 정의 (이름·색·HP·속도·특수능력)
- 특수능력 예: 흡혈, 광폭 가속, 감속, 진흙 장판, 페이즈, 분열, 방어, 소환, 독, 돌진, 하울, 저주, 가시, 힐 오라
- 비주얼: Quaternius CC0 GLB 3D 모델 30종 (`assets/models/`) — 종류마다 다른 메시

### Step 4 — 사방 추격
- 맵 외곽에서 느린 속도로 플레이어를 향해 이동
- 근접 공격, 총알/수류탄으로 처치, 드롭 아이템

### Step 5 — Warning 웨이브
- 주기적 WARNING 표시 + 위협 방향 화살표
- **한 방향**, **2종류만**, **중속**으로 돌진

### Step 6 — 장애물
- 엄폐용 상자/잔해, 원형 충돌 해석
- 탄환은 장애물에 막힘
- 테두리 폐허로 전장 경계 형성

### Step 7 — 캐릭터 능력
- Q Berserk / E Barrier / R Overcharge (쿨다운)
- 코스튬별 패시브: Scout 이속·대시, Heavy 이속↓·화력 안정, Gold 轻微 데미지↑ 등

### Step 8 — 스코어
- 처치 점수, 콤보 배율, 하이 스코어, HUD 표시
- Score Gem 아이템으로 추가 점수

### Step 9 — 보스
- 점수 구간마다 보스 등장
  - **Horde King** — 졸개 소환
  - **Plague Mother** — 역병 쥐 소환 + 근접 독성
  - **Shadow Colossus** — 일시 페이즈(무적에 가까운 투명)
- 보스 처치 시 큰 점수·업적

### Step 10 — 업적 & 코스튬
- 업적 8종 (첫 처치, 콤보, 보스, 수집, 생존, 수류탄, 코스튬, 고득점)
- Tab 으로 Default / Scout / Heavy / Night Ops / Gold Hero 순환

---

## 4. 플레이 팁

1. 엄폐물 뒤로 대시해 각을 재설정하세요.
2. Shotgun은 근거리, Rifle은 중장거리에 유리합니다.
3. WARNING이 뜨면 화살표 **반대편**으로 공간을 확보하세요.
4. 보스는 소환 패턴을 끊기 위해 광역(수류탄)을 아끼세요.
5. 가방에서 Medkit·Shield를 핫바 앞쪽에 두세요.

---

## 5. 소스 구조

```
src/               C 소스 (main.c가 나머지 include)
assets/            모델·SFX·폰트·로케일
docs/GAME_GUIDE.md 이 문서
assets/LICENSE     라이선스
ArenaBreach.sln    Visual Studio
build.bat          MinGW 빌드
```

---

## 6. 저작권·벤치마킹 범위

- **사용:** raylib (zlib), 자체 제작 프리미티브 아트, 오리지널 시스템 디자인
- **벤치마킹 아이디어만 참고(저작물 미복제):** 탑다운 서바이벌 웨이브, 엄폐 슈팅, 핫바·드래그 인벤토리, 보스 페이즈형 패턴
- **미사용:** 특정 영화/게임의 캐릭터명·디자인·음원·공식 에셋
- CC0 후보(선택): Quaternius Ultimate Monsters, Kenney packs — 상세는 `assets/LICENSE`

---

## 7. 과제 체크리스트

- [x] 3D raylib 슈팅 코어
- [x] 지정 키맵 (WASD 이동, Z/X/C/B, Space, Shift + 사격 LMB)
- [x] 가방 드래그 + 1~8 사용
- [x] 괴물 다수 + 특수능력
- [x] 사방 느린 추격
- [x] Warning 단방향 2종 중속 웨이브
- [x] 장애물 / 능력 / 스코어 / 보스 / 업적·코스튬
- [x] 튜토리얼·기능 가이드 문서
