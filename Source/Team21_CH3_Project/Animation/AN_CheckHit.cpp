
#include "Animation/AN_CheckHit.h"
#include "Character/CharacterBase.h"


void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		ACharacterBase* AttackingCharacter = Cast<ACharacterBase>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->HandleOnCheckHit();
		}
	}
}
