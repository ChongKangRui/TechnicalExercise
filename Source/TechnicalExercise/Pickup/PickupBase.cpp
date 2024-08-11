#include "PickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/W_StartTimer.h"

APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

    // Initialize the SphereCollisionComponent
    SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
    SphereCollisionComponent->InitSphereRadius(100.0f);
    SphereCollisionComponent->SetCollisionProfileName(TEXT("Trigger"));

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EventTimerWidgetComponent"));
    WidgetComponent->SetupAttachment(RootComponent);

    static ConstructorHelpers::FClassFinder<UUserWidget> EventTimerWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_ItemCoolDown"));
    if (EventTimerWidgetClassFinder.Succeeded())
    {
        WidgetComponent->SetWidgetClass(EventTimerWidgetClassFinder.Class);
    }

    // Enable overlap events
    SphereCollisionComponent->SetGenerateOverlapEvents(true);
    RootComponent = SphereCollisionComponent;

    // Bind the overlap event to the custom function
    SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnOverlapBegin);
}

void APickupBase::BeginPlay()
{
    Super::BeginPlay();

    m_CoolDownTimer = Cast<UW_StartTimer>(WidgetComponent->GetUserWidgetObject());
    if (m_CoolDownTimer) {
        m_CoolDownTimer->ShouldRemoveFromViewportAfterCountdownEnd = false;
    }
    WidgetComponent->SetVisibility(false);
    WidgetComponent->SetWorldLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + WidgetHeight));
    CanPickup = true;

}

void APickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APickupBase::StartPickupCoolDown()
{
    WidgetComponent->SetVisibility(true);
    m_CoolDownTimer->StartTimer("", PickupCD);
    CanPickup = false;

    FTimerHandle tempHandle;
    FTimerDelegate tempDelegate;
    tempDelegate.BindWeakLambda(this, [&]()
        {
            CanPickup = true;
            WidgetComponent->SetVisibility(false);
        });
    GetWorld()->GetTimerManager().SetTimer(tempHandle, tempDelegate, 1.0f, false, PickupCD);

}

