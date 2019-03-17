// Fill out your copyright notice in the Description page of Project Settings.

#include "Haaraja.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"

#define VALJA


// Sets default values for this component's properties
UHaaraja::UHaaraja()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Kutsutakse v�lja, kui m�ng algab
void UHaaraja::BeginPlay()
{
	Super::BeginPlay();
	LeiaFyysikaKasitlejaKomponent();
	SeadistaSisendKomponent();
}

/// Otsi �hendatud f��sika k�sitlejat (Physics Handle)
void UHaaraja::LeiaFyysikaKasitlejaKomponent() 
{
	// Otsi �hendatud f��sika k�sitlejat
	FyysikaKasitleja = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (FyysikaKasitleja)
	{
		// f��sika k�sitleja leiti
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s puudu FyysikaKasitleja komponent"), *GetOwner()->GetName());
	}
}

/// Otsi yhendatud sisend komponenti (Input Component) (ilmub v�lja ainult m�ngu jooksmise ajal)
void UHaaraja::SeadistaSisendKomponent()
{
	SisendKomponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (SisendKomponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sisendkomponent leitud"));

		/// Seo sisendtelg
		SisendKomponent->BindAction("Haara", IE_Pressed, this, &UHaaraja::Haara);
		SisendKomponent->BindAction("Haara", IE_Released, this, &UHaaraja::Vabasta);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s puudu sisendkomponent"), *GetOwner()->GetName());
	}
}


void UHaaraja::Haara() {
	UE_LOG(LogTemp, Warning, TEXT("Haara vajutatud"));

	// J�rgi m�ngijast v�ljuvat kujutletavat joont ja vaata,
	// kas me ulatume m�ne actor'ini millel on seadistatud physics body collision channel
	auto TabamuseVastus = SaaLahimUlatusesOlevFyysikaKeha();
	auto ComponentToGrab = TabamuseVastus.GetComponent();
	auto ActorTabas = TabamuseVastus.GetActor();

	// Kui me tabame midagi, siis �henda f��sika k�sitleja (Physics Handle)
	if (ActorTabas != nullptr) {
		// �hendab f��sika k�sitleja
		FyysikaKasitleja->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // lubab p��relda
		);
	}
}

void UHaaraja::Vabasta() {
	UE_LOG(LogTemp, Warning, TEXT("Haara vabastatud"));
	FyysikaKasitleja->ReleaseComponent();
}

// Kutsutakse v�lja iga kaader
void UHaaraja::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Saa m�ngija vaatepunkt
	FVector MangijaVaatePunktiAsukoht;
	FRotator MangijaVaatePunktiRotatsioon;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		VALJA MangijaVaatePunktiAsukoht,
		VALJA MangijaVaatePunktiRotatsioon
	);
	FVector JooneJaljeLopp = MangijaVaatePunktiAsukoht + MangijaVaatePunktiRotatsioon.Vector() * 100;


	// kontroll, kas me oleme haaranud komponendi
	if (FyysikaKasitleja->GrabbedComponent) {
		// liigutab objekti mida me hoiame
		FyysikaKasitleja->SetTargetLocation(JooneJaljeLopp);
	}
}

const FHitResult UHaaraja::SaaLahimUlatusesOlevFyysikaKeha()
{
	// Saa m�ngija vaatepunkt
	FVector MangijaVaatePunktiAsukoht;
	FRotator MangijaVaatePunktiRotatsioon;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		VALJA MangijaVaatePunktiAsukoht,
		VALJA MangijaVaatePunktiRotatsioon
	);
	FVector JooneJaljeLopp = MangijaVaatePunktiAsukoht + MangijaVaatePunktiRotatsioon.Vector() * 100;
	
	/// t�mbab punase silumisjoone et visualiseerida haarajat
/*	DrawDebugLine(
		GetWorld(),
		MangijaVaatePunktiAsukoht,
		JooneJaljeLopp,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.f);*/

	/// Seadista p�ringu parameetrid
	FCollisionQueryParams TraceParameetrid(FName(TEXT("")), false, GetOwner());

	/// J�lgi v�lja ulatuvat joont (line-trace) (AKA ray-cast)
	FHitResult Tabamus;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Tabamus,
		MangijaVaatePunktiAsukoht,
		JooneJaljeLopp,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameetrid
	);

	// Vaatab, mida me tabasime
	AActor* ActorPihtas = Tabamus.GetActor();
	if (ActorPihtas) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace tabas: %s"), *(ActorPihtas->GetName()));
	} 
	return Tabamus;
}
