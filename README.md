# Survive the Wave

Unreal Engine 5.5 기반 3D 슈터 팀 프로젝트입니다.  
웨이브 전투, 증강 카드, 보스 기믹, 특성 성장, OutGame UI 흐름을 C++와 UMG 중심으로 구현했습니다.

## Demo

- 플레이 영상: https://drive.google.com/file/d/1AziBvFRK9sNuXAfgzrE1o4i080ZW3PxB/view?usp=drive_link
- 발표 자료: 

## Project Info

| 항목 | 내용 |
| --- | --- |
| 개발 기간 | YYYY.MM.DD - YYYY.MM.DD |
| 개발 인원 | 5명 |
| 엔진 | Unreal Engine 5.5 |
| 언어 | C++ |
| 주요 모듈 | Enhanced Input, UMG, AIModule, NavigationSystem, Niagara, LevelSequence |
| 플랫폼 | Windows |

## Overview

플레이어가 웨이브 단위로 적을 처치하며 골드와 증강 효과를 얻고,  
OutGame에서 무기/특성/설정을 관리한 뒤 다음 전투로 진입하는 구조의 슈터 프로젝트입니다.

핵심 목표는 단순 콘텐츠 나열보다, 전투 루프와 UI 흐름, 데이터 기반 성장 시스템, 보스 기믹 구조를 Unreal C++ 기반으로 구현하는 것이었습니다.

## Key Features

- 웨이브 기반 전투 진행
- 적 처치 기반 골드 획득
- 5킬마다 증강 카드 선택 UI 출력
- 증강 효과 중첩 및 GameInstance 저장
- 보스 체력 UI 및 보스 웨이브 기믹
- OutGame 메인 메뉴, 미션 선택, 무기 선택, 특성, 상점, 설정 UI
- DataTable 기반 Trait 시스템
- SaveGame 기반 플레이어 진행 정보 저장
- SoundClass/SoundMix 기반 Master/BGM/SFX/UI 볼륨 설정

## Project Structure
### OutGame UI - 조범근
OutGameUI는 전투 진입 전 플레이어가 사용하는 메뉴 흐름을 담당합니다.
OutGameRootWidget을 중심으로 메인 메뉴, 미션 선택, 무기 선택, 특성, 상점, 설정, 결과 화면을 전환하며, 공통 헤더와 확인 팝업, Fade 전환 위젯을 통해 화면 이동 흐름을 관리합니다.


### InGame UI - 박태민
InGameUI는 전투 중 HUD와 웨이브 진행, 증강 카드 선택, 결과 UI를 담당합니다.
ShooterInGameMode에서 웨이브 상태, 처치 수, 골드, 증강 카드 출력 타이밍, 라운드 클리어 흐름을 관리하고, InGameHUD를 통해 플레이 중 필요한 UI를 표시합니다.


### Character - 김동연
Character 모듈은 플레이어, 일반 적, 보스 캐릭터의 공통 전투 기반을 담당합니다.
CharacterBase에서 무기, 데미지, StatusComponent 연동을 처리하고, PlayerCharacter는 입력, 이동, 사격, 재장전, 상호작용을 담당합니다. NonPlayerCharacter와 BossNPC는 AI 캐릭터 전투와 사망 처리 흐름을 담당합니다.


### AI - 이후일
AI 모듈은 Behavior Tree 기반의 적 행동 판단을 담당합니다.
Task, Service, Decorator를 C++로 구현하여 플레이어 위치 갱신, 공격 범위 판단, 보스 돌진 가능 여부, 점프/낙하/추격 행동 등을 Behavior Tree에서 사용할 수 있도록 구성했습니다.


### Gimmick - 김태민
Gimmick 모듈은 전투 맵에서 사용되는 상호작용 오브젝트와 웨이브 스폰을 담당합니다.
SpawnManager가 웨이브 데이터에 따라 적을 스폰하고, Door, ItemBox, MovingCover 같은 맵 오브젝트는 Interaction 인터페이스 또는 Tick 기반 이동 로직으로 동작합니다. 


## How To Run

Unreal Engine 5.5 설치
Team21_CH3_Project.uproject 실행
필요한 플러그인 활성화 확인
Enhanced Input
Modeling Tools Editor Mode
Development Editor / Win64로 빌드
에디터에서 플레이


## Troubleshooting

팀 노션 : https://www.notion.so/teamsparta/2-1-3532dc3ef5148110818ec3a9ba3e9c1e

팀 노션 데일리 스크럼 일지에 트러블 슈팅 정리되어 있습니다.

