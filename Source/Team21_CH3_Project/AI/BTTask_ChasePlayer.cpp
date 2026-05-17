// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ChasePlayer.h"
#include "Controller/AI_Controller.h"
#include "Character/NonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer()
{
	NodeName = TEXT("ChasePlayer");
}
