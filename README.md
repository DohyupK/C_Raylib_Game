# Arena Breach

위에서 내려다보는 **3D 아레나 슈팅 게임**입니다.  
C + **raylib 6.2**로 만들었고, Windows에서 바로 돌려볼 수 있습니다.

초록 링이 **나**, 빨간 링이 **적**입니다. 괴물 30종을 처치하고, ESC에서 도감·업적·랭킹을 확인하세요.

더 자세한 플레이 설명은 [docs/GAME_GUIDE.md](docs/GAME_GUIDE.md)에 있습니다.

---

## 실행 방법

프로젝트 **루트 폴더**에서:

```bat
build.bat
ArenaBreach.exe
```

> `assets` 폴더를 못 찾으면 모델·한글·소리가 안 뜹니다. 반드시 루트에서 실행하세요.

- MinGW: `build.bat` 또는 `mingw32-make`
- Visual Studio: `C_Raylib_Game.slnx` → Win32

---

## 이 게임은 이런 느낌입니다

- **시점**: 약간 비스듬한 탑다운 카메라가 플레이어를 따라감
- **맵**: 원형 아레나 + 엄폐용 건물
- **전투**: 권총 / 샷건 / 라이플 + 수류탄 + Q·E·R 스킬
- **적**: 괴물 30종(각자 다른 모습·능력) + 경고 웨이브 + 보스 3종
- **성장**: 킬할수록 점수·콤보↑, 100킬마다 난이도↑, 50킬마다 배경 테마 변경
- **아이템**: 바닥에 3D로 떨어짐 → 60초 뒤 사라짐, 가방에 최대 10개

---

## 튜토리얼

처음 시작하면 화면 아래 팁이 뜹니다. **Enter**로 넘기면 됩니다.

1. WASD로 이동
2. 마우스로 조준, 좌클릭으로 사격
3. Z로 수류탄
4. 빛나는 아이템 옆에서 C로 줍기
5. B로 가방 열기 (드래그 / 숫자키 1–8로 사용)
6. ESC에서 도감·코스튬 확인

게임오버 후 **Enter** = 다시 시작 · **F4** = 종료

### 조작 한눈에 보기

| 키 | 하는 일 |
|----|---------|
| WASD | 이동 |
| 마우스 / 좌클릭 | 조준 / 사격 |
| Space / Shift | 점프 / 대시 |
| Z X C B | 수류탄 / 무기변경 / 줍기 / 가방 |
| 1–8 | 핫바 아이템 사용 |
| Q E R | 광폭화 / 방벽 / 과충전 |
| ESC | 메뉴 (일시정지) |
| 메뉴에서 1–5 | 일시정지 · 도감 · 코스튬 · 업적 · 랭킹 |

---

## 주요 기능

**전투·생존**
- 이동·점프·대시, 마우스 조준 사격
- 무기 3종 + 수류탄, Q/E/R 스킬
- 건물 뒤에 숨으면 총알을 막을 수 있음

**적**
- 괴물 30종 (흡혈, 분열, 독, 돌진 등 특수능력)
- 평소엔 사방에서 천천히 추격
- Warning이 뜨면 **한 방향**으로 중속 돌진 웨이브
- 보스 3종: 무리의 왕 / 역병의 어미 / 그림자 거신

**수집·성장**
- 가방 드래그, 핫바, 드롭 아이템
- 점수·콤보·하이 스코어
- 몬스터 북(낮은 확률 드롭)으로 도감 해금 → 외형 장착 시 그 능력 사용

**메뉴 (ESC)**
- 업적 확인
- 점수 랭킹 TOP 10 (게임오버 시 저장)
- 도감을 열거나 새로 해금하면 알림

---

## 에셋은 어디서 왔나요?

상업적으로 써도 되는 무료 에셋만 썼습니다.  
자세한 표기는 [assets/LICENSE](assets/LICENSE), [assets/sfx/LICENSE.txt](assets/sfx/LICENSE.txt)를 보세요.

### 3D 모델 — Quaternius (CC0)

출처: [quaternius.com](https://quaternius.com)  
(미러: [quaternius-showcase](https://github.com/trebeljahr/quaternius-showcase))

- 플레이어·괴물 → `assets/models/`
- 건물 → `assets/models/buildings/`
- 드롭 아이템 → `assets/models/items/`

### 효과음 — Kenney (CC0) + 직접 만든 소리

출처: [kenney.nl](https://kenney.nl)

- 사격·피격·줍기 → Kenney Starter Kit (`.ogg`)
- 수류탄·보스·일시정지 → `tools/gen_sfx.py`로 만든 오리지널 WAV

### 폰트 — Nanum Gothic (OFL)

`assets/fonts/NanumGothic.ttf`  
([Google Fonts / Nanum Gothic](https://github.com/google/fonts/tree/main/ofl/nanumgothic))

### 그 외 (직접 만듦)

- 가방 아이콘 → 실행 중 이미지로 그림
- 한글 UI 문구 → `assets/locale/ko_ui.txt`
- 몬스터 이름·능력 설명·점수/업적 설계 → 이 프로젝트 오리지널 (특정 게임·영화 IP 아님)

### 엔진

[raylib 6.2](https://github.com/raysan5/raylib) (zlib/libpng)

---

## raylib로는 뭘 썼나요?

한 줄로 말하면: **창·입력·3D 그리기·소리·한글 폰트·파일 저장**을 전부 raylib로 처리합니다.

**화면·조작**  
창 열기, 60FPS, 키보드·마우스 입력, 프레임 시간

**3D**  
카메라, GLB 모델 그리기, 바닥·큐브·구·실린더,  
마우스 위치 → 월드 좌표로 바꿔 조준 (`GetScreenToWorldRay`)

**모델·그림**  
`LoadModel`로 GLB 로드,  
필요하면 이미지를 그려서 텍스처로 만들고 (`GenImage*` → `LoadTextureFromImage`) 가방 아이콘에 사용

**소리**  
짧은 효과음만 사용 (`LoadSound` / `PlaySound`).  
사격·피격 때는 피치를 살짝 바꿔서 같은 소리가 덜 단조롭게 들리게 함  
(배경음악용 `Music` 스트림은 쓰지 않음)

**한글 UI**  
`LoadFontEx`로 필요한 글자만 폰트에 넣고, 메뉴·HUD를 2D로 그림

**수학·충돌**  
벡터 계산, 총알↔건물 충돌, UI 버튼·가방 칸 클릭 판정

**파일**  
랭킹·한국어 텍스트 읽고 쓰기 (`LoadFileText` / `SaveFileText`)

### 에셋이 코드에서 쓰이는 흐름

1. **모델** — `LoadModel` → `DrawModelEx`로 배치 (크기 맞출 때 `GetModelBoundingBox`)
2. **소리** — `LoadSound` → 필요할 때 `PlaySound`
3. **폰트** — 로케일 파일에서 글자 모음 → `LoadFontEx` → 한글 출력
4. **랭킹** — 게임오버 시 `save/ranking.txt`에 저장

모델은 애니메이션 없이 **정지 메쉬**이고, 초록/빨간 링으로 피아를 구분합니다.

---

## 폴더 구조

```
C_Raylib_Game/     게임 소스 (main, player, monsters, menu …)
assets/
  models/          3D 모델
  sfx/             효과음
  fonts/           한글 폰트
  locale/          한국어 UI 문구
docs/              플레이 가이드
build.bat          빌드
```

---

## 라이선스 한눈에

| 무엇 | 라이선스 |
|------|----------|
| raylib | zlib/libpng |
| Quaternius 모델 | CC0 |
| Kenney 효과음 | CC0 |
| Nanum Gothic | SIL OFL |
| 직접 만든 WAV·아이콘·게임 로직 | 오리지널 |
