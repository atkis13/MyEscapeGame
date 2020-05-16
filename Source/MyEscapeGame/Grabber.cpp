// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsComponent();
	SetupInputComponent();
	// ...
	
}

void UGrabber::FindPhysicsComponent()
{
	///find physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing physics handle"), *GetOwner()->GetName());
	}

}

/// look for attached input component
void UGrabber::SetupInputComponent()
{

	InputCommp = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputCommp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Input component found"));
		InputCommp->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputCommp->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing input component"), *GetOwner()->GetName());
	}
}



void UGrabber::Grab()
{
	//UE_LOG(LogTemp, Warning, TEXT("Grabbed presed"));
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComptoGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponent(ComptoGrab, NAME_None, ComptoGrab->GetOwner()->GetActorLocation(), true);
	}

}

void UGrabber::Release()
{
	if (!PhysicsHandle) { return; }
	//UE_LOG(LogTemp, Warning, TEXT("Grabbed released"));
	PhysicsHandle->ReleaseComponent();
}




// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	if (PhysicsHandle->GrabbedComponent)
	{

		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}

	// ...
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	GetReachLineEnd();

	///setup query parameters
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());	/// Line trace ...
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(OUT Hit, GetReachLineStart(), GetReachLineEnd(), FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), TraceParams);

	AActor* ActorHit = Hit.GetActor();
	/*if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(ActorHit->GetName()));
	}*/
	return Hit;
}

FVector UGrabber::GetReachLineStart() {
	FVector PlayerViewPointLoaction;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLoaction, OUT PlayerViewPointRotation);
	/*UE_LOG(LogTemp, Warning, TEXT("Location : %s, Rotation: %s"), *PlayerViewPointLoaction.ToString(), *PlayerViewPointRotation.ToString())*/


	return PlayerViewPointLoaction;
	//DrawDebugLine(GetWorld(), PlayerViewPointLoaction, LineTraceEnd, FColor(255, 0, 0), false, 0.f, 0.f, 10.f);
}

FVector UGrabber::GetReachLineEnd() {
	FVector PlayerViewPointLoaction;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLoaction, OUT PlayerViewPointRotation);
	/*UE_LOG(LogTemp, Warning, TEXT("Location : %s, Rotation: %s"), *PlayerViewPointLoaction.ToString(), *PlayerViewPointRotation.ToString())*/
	return PlayerViewPointLoaction + PlayerViewPointRotation.Vector() * Reach;
	//DrawDebugLine(GetWorld(), PlayerViewPointLoaction, LineTraceEnd, FColor(255, 0, 0), false, 0.f, 0.f, 10.f);
}



