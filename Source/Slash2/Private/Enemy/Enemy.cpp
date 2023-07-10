// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash2/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

//�U����������������AM���N��
void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//�@�q�b�g���ɂ����邱��
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);

	DirectionalHitReact(ImpactPoint);

	if (HitSound )
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
			
	}
	//�����Ԃ����o������
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint //�Փ˒n�_���猌���Ԃ��̃p�[�e�B�N�����Đ�
		);
	}

}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();

	// �x�N�g����n�ʂƕ��s�ɂ���
	const FVector ImapactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	// �Փ˓_��Actor�̃x�N�g�������߂Đ��K��
	const FVector ToHit = (ImapactLowered - GetActorLocation()).GetSafeNormal();

	/*
	* Forwaed * ToHit = |Forward||ToHit| * cos(theta)
	* |Forward| = 1, |ToHit| =1�@�Ȃ̂� Forwaed * ToHit = cos(theta)
	*/
	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	//�t�O�p�֐���p���ďՓ˓_��Actor�̐��ʂƂ̊p�x���擾
	double Theta = FMath::Acos(CosTheta);

	//�@radians����degrees�ɕϊ�
	Theta = FMath::RadiansToDegrees(Theta);

	//�O�ς�p����-180������180���ɂ���
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	/*
	* AnimationMontage��Section�����p�x�ɂ���ĕ��ނ���.
	* Theta�̒l�ɂ���ăq�b�g���ɍĐ������A�j���[�V�����𕪗�
	*/
	FName Section;

	// -45������45���̂Ƃ��C�O��������q�b�g
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	// -135������-45���̂Ƃ��C����������q�b�g
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	// 45������135���̂Ƃ��C�E��������q�b�g
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	// -135���ȉ��C135���ȏ�̎��C����������q�b�g
	else
	{
		Section = FName("FromBack");
	}
	
	PlayHitReactMontage(Section);

	/*
	* Debug�p�̏���
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Green, 5.f);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	}
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Blue, 5.f);
	*/
}

