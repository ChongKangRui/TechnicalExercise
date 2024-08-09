#include "PickupBase.h"
#include "Components/SphereComponent.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Initialize the SphereCollisionComponent
    SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
    SphereCollisionComponent->InitSphereRadius(100.0f);
    SphereCollisionComponent->SetCollisionProfileName(TEXT("Trigger"));

    // Enable overlap events
    SphereCollisionComponent->SetGenerateOverlapEvents(true);

    // Set the SphereComponent as the RootComponent (optional, depending on your setup)
    RootComponent = SphereCollisionComponent;

    // Bind the overlap event to the custom function
    SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnOverlapBegin);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

