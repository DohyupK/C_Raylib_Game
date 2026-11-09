https://github.com/user-attachments/assets/7cf154ca-1a04-479f-9bdd-96bc94a1ae4a

Arena Breach
C / raylib 6.2 · 3D 탑다운 슈팅 게임
초록색 링은 플레이어, 빨간색 링은 적을 나타냅니다.

자세한 조작 방법과 플레이 팁은 docs/GAME_GUIDE.md에서 확인할 수 있습니다.

# 슬라이드 1 · 프로젝트 소개
Arena Breach는 아레나에서 몰려오는 몬스터를 처치하며 생존하는 3D 탑다운 슈팅 게임입니다.

위에서 비스듬히 내려다보는 3D 시점으로 진행됩니다.
총 30종의 몬스터와 경고 웨이브, 보스 전투를 제공합니다.
ESC 메뉴에서 몬스터 도감, 업적, 랭킹 등을 확인할 수 있습니다.
mermaid
flowchart LR
  A[플레이어] -->|사격 / 수류탄| B[몬스터]
  B -->|추격 / 공격| A
  B -->|아이템 드롭| C[아이템]
  C -->|아이템 획득| A
  A -->|몬스터 처치| D[점수 / 업적]

# 슬라이드 2 · 빌드 및 실행 방법
프로그램의 진입점은 src/main.c입니다.
이 파일에서 나머지 C 소스 파일을 통합하여 관리하므로, 별도의 소스 파일을 하나씩 빌드할 필요 없이 main.c를 기준으로 프로젝트를 빌드할 수 있습니다.

권장 실행 방법
프로젝트 루트 디렉터리에서 다음 명령을 실행합니다.
```
bat
build.bat
ArenaBreach.exe
```
mingw32-make를 사용해도 동일하게 빌드할 수 있습니다.

GCC를 이용한 직접 빌드
bat
gcc -std=c11 -O2 -Isrc -Ithird_party/raylib-6.2_win64_mingw-w64/include ^
  src/main.c -o ArenaBreach.exe ^
  -Lthird_party/raylib-6.2_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm

Visual Studio에서 실행
ArenaBreach.sln 파일을 열어 프로젝트를 빌드하면 됩니다.
x86과 x64 플랫폼을 모두 지원하며, 두 환경 모두 raylib가 포함되어 있습니다.

실행 시 참고 사항

에셋을 정상적으로 불러오려면 ArenaBreach.exe를 assets 폴더와 같은 프로젝트 루트 디렉터리에서 실행해야 합니다. 실행 위치가 다르면 3D 모델, 한글 폰트, 사운드 등이 정상적으로 표시되지 않을 수 있습니다.

mermaid
flowchart TD
  M[main.c] --> P[플레이어 / 무기 / 인벤토리]
  M --> E[몬스터 / 웨이브 / 보스]
  M --> U[HUD / 메뉴 / 점수]
  M --> X[에셋 / 오디오 / 폰트]
  M --> EXE[ArenaBreach.exe]

# 슬라이드 3 · 기본 플레이 흐름
게임을 시작하면 화면 하단에 기본 조작과 플레이 팁이 표시됩니다.
Enter 키를 누르면 다음 안내로 넘어갈 수 있습니다.

순서	안내 내용
1	WASD 키로 캐릭터 이동
2	마우스로 조준하고 좌클릭으로 사격
3	Z 키로 수류탄 투척
4	아이템 근처에서 C 키로 획득
5	B 키로 인벤토리 열기 (드래그 및 1~8번 슬롯 사용)
6	ESC 키로 도감 및 코스튬 메뉴 열기

캐릭터가 사망하면 Enter 키를 눌러 게임을 다시 시작할 수 있습니다.
게임을 종료하려면 F4 키를 사용합니다.

# 슬라이드 4 · 조작 키 안내
키	기능
WASD	캐릭터 이동
마우스	조준 및 좌클릭 사격
Space / Shift	점프 / 대시
Z	수류탄 투척
X	무기 변경
C	아이템 획득
B	인벤토리 열기
1~8	핫바 슬롯 선택
Q	광폭화
E	방벽
R	과충전
ESC	메뉴 열기

메뉴 조작

숫자 키	기능
1	일시정지
2	몬스터 도감
3	코스튬
4	업적
5	랭킹

# 슬라이드 5 · 주요 게임 시스템
## 1. 전투 시스템
총 3종의 무기를 사용할 수 있습니다.
수류탄과 Q/E/R 특수 능력을 활용할 수 있습니다.
건물과 지형지물을 엄폐물로 활용할 수 있으며, 건물은 적의 총알을 막아줍니다.

## 2. 몬스터 및 웨이브 시스템
총 30종의 일반 몬스터가 등장하며, 몬스터마다 고유한 특수 능력을 보유하고 있습니다.

구분	특징
일반 몬스터 30종	흡혈, 독, 분열 등 다양한 특수 능력
일반 스폰	아레나 여러 방향에서 지속적으로 등장
Warning 웨이브	특정 방향에서 대규모 몬스터가 집중적으로 등장
보스 3종	무리의 왕, 역병의 어미, 그림자 거신

## 3. 성장 및 보상 시스템
몬스터를 처치하면 점수와 콤보가 증가합니다.

100킬마다 게임 난이도가 상승합니다.
50킬마다 게임 배경이 변경됩니다.
몬스터 북 아이템을 획득하면 해당 몬스터의 도감이 해금됩니다.
코스튬을 장착하면 해당 코스튬의 능력이 적용됩니다.

## 4. 메뉴 및 기록 시스템
ESC 메뉴에서 다양한 부가 기능을 이용할 수 있습니다.

업적 확인
랭킹 TOP 10 확인
몬스터 도감 및 해금 알림
코스튬 확인 및 변경

랭킹 데이터는 save/ranking.txt 파일에 저장됩니다.

# 슬라이드 6 · 에셋 및 라이선스
프로젝트에는 상업적 이용이 가능한 라이선스의 에셋을 사용했습니다.
세부 라이선스 정보는 assets/LICENSE 파일에서 확인할 수 있습니다.

구분	출처	라이선스
3D 모델 / 건물 / 아이템	Quaternius	CC0
사격 / 피격 / 아이템 획득 효과음	Kenney	CC0
폭발 / 보스 / 일시정지 효과음	tools/gen_sfx.py로 자체 제작	오리지널
한글 폰트	Nanum Gothic	SIL OFL
게임 엔진	raylib 6.2	zlib

몬스터의 이름과 설정, 특수 능력 및 게임 로직은 프로젝트에서 자체적으로 제작했습니다. 특정 기존 IP를 모방하지 않은 오리지널 콘텐츠입니다.

mermaid
flowchart LR
  Q[Quaternius GLB 모델] --> R[raylib LoadModel]
  K[Kenney OGG 사운드] --> S[LoadSound]
  W[자체 제작 WAV] --> S
  F[Nanum Gothic 폰트] --> T[LoadFontEx]
  R --> G[게임 화면 출력]
  S --> G
  T --> G

# 슬라이드 7 · raylib 활용 기술
raylib를 활용하여 별도의 복잡한 엔진 구성 없이 창 관리, 입력 처리, 3D 렌더링, 사운드, 폰트 및 파일 입출력을 구현했습니다.

영역	주요 함수
창 및 입력 처리	InitWindow, IsKey*, GetMousePosition
3D 렌더링	BeginMode3D, DrawModelEx, Camera3D
마우스 조준	GetScreenToWorldRay
사운드	LoadSound, PlaySound, SetSoundPitch
한글 폰트	LoadFontEx
충돌 판정	CheckCollisionBoxSphere, CheckCollisionPointRec
파일 저장 및 불러오기	SaveFileText, LoadFileText

에셋 처리 과정
파일 로드 → 필요에 따른 스케일 및 피치 조정 → 매 프레임 렌더링 및 재생
3D 모델에는 별도의 애니메이션을 적용하지 않았으며, 플레이어와 적의 구분은 링의 색상을 통해 직관적으로 표현했습니다.

# 슬라이드 8 · 프로젝트 폴더 구조
프로젝트는 소스 코드, 에셋, 문서, 라이브러리를 분리하여 관리하도록 구성했습니다.

text
(프로젝트 루트)
  src/              ← C 소스 코드 (main.c 및 모듈)
  assets/           ← 3D 모델, 사운드, 폰트 및 기타 에셋
  docs/             ← 게임 가이드 및 문서
  third_party/      ← raylib (MinGW)
  packages/         ← raylib (Visual Studio)
  build.bat         ← 빌드 스크립트
  ArenaBreach.sln   ← Visual Studio 솔루션
  ArenaBreach.exe   ← 실행 파일

# 라이선스 정리

항목	라이선스
raylib	zlib/libpng
Quaternius	CC0
Kenney	CC0
Nanum Gothic	SIL OFL
자체 제작 사운드 / 아이콘 / 게임 로직	오리지널
