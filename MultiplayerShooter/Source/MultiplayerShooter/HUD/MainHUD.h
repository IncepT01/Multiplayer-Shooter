#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"
 
USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
};
 
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMainHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;
 
	class UCharacterOverlay* CharacterOverlay;
	
protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();
	
private:
	FHUDPackage HUDPackage;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
