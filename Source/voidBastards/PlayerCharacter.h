#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "Templates/SubclassOf.h"
#include "UObject/CoreNet.h"
#include "Engine/NetSerialization.h"
#include "Engine/EngineTypes.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "Animation/AnimationAsset.h"
#include "GameFramework/RootMotionSource.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AController;
class FDebugDisplayInfo;
class UAnimMontage;
class UArrowComponent;
class UCapsuleComponent;
class UCharacterMovementComponent;
class UPawnMovementComponent;
class UPrimitiveComponent;
class USkeletalMeshComponent;
struct FAnimMontageInstance;

class UInputComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class USoundBase;
class UDevice;
class UFireArm;
class UIndirect;
class UMyUserWidget;
class UWeapon;
class Udamagable;
class UPart;


//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMovementModeChangedSignature, class ACharacter*, Character, EMovementMode, PrevMovementMode, uint8, PreviousCustomMode);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCharacterMovementUpdatedSignature, float, DeltaSeconds, FVector, OldLocation, FVector, OldVelocity);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterReachedApexSignature);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLandedSignature, const FHitResult&, Hit);

/**
 * Characters are Pawns that have a mesh, collision, and built-in movement logic.
 * They are responsible for all physical interaction between the player or AI and the world, and also implement basic networking and input models.
 * They are designed for a vertically-oriented player representation that can walk, jump, fly, and swim through the world using CharacterMovementComponent.
 *
 * @see APawn, UCharacterMovementComponent
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Framework/Pawn/Character/
 */ 
UCLASS()
class VOIDBASTARDS_API APlayerCharacter : public APawn
{
	GENERATED_BODY()
public:
	/** Default UObject constructor. */
	APlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(Category=Character, VisibleAnywhere,BlueprintReadWrite , meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	/** The CapsuleComponent being used for movement collision (by CharacterMovement). Always treated as being vertically aligned in simple collision check functions. */
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

#if WITH_EDITORONLY_DATA
	/** Component shown in the editor only to indicate character facing */
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;
#endif

public:

	//////////////////////////////////////////////////////////////////////////
	// Server RPC that passes through to CharacterMovement (avoids RPC overhead for components).
	// The base RPC function (eg 'ServerMove') is auto-generated for clients to trigger the call to the server function,
	// eventually going to the _Implementation function (which we just pass to the CharacterMovementComponent).
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(unreliable, server, WithValidation)
	void ServerMovePacked(const FCharacterServerMovePackedBits& PackedBits);
	void ServerMovePacked_Implementation(const FCharacterServerMovePackedBits& PackedBits);
	bool ServerMovePacked_Validate(const FCharacterServerMovePackedBits& PackedBits);

	//////////////////////////////////////////////////////////////////////////
	// Client RPC that passes through to CharacterMovement (avoids RPC overhead for components).
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(unreliable, client, WithValidation)
	void ClientMoveResponsePacked(const FCharacterMoveResponsePackedBits& PackedBits);
	void ClientMoveResponsePacked_Implementation(const FCharacterMoveResponsePackedBits& PackedBits);
	bool ClientMoveResponsePacked_Validate(const FCharacterMoveResponsePackedBits& PackedBits);


	//////////////////////////////////////////////////////////////////////////
	// BEGIN DEPRECATED RPCs that don't use variable sized payloads. Use ServerMovePacked and ClientMoveResponsePacked instead.
	//////////////////////////////////////////////////////////////////////////

	/** Replicated function sent by client to server - contains client movement and view info. */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMove, ServerMovePacked)
	UFUNCTION(unreliable, server, WithValidation)
	void ServerMove(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMove_Implementation, ServerMovePacked_Implementation)
	void ServerMove_Implementation(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	bool ServerMove_Validate(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);

	/**
	 * Replicated function sent by client to server. Saves bandwidth over ServerMove() by implying that ClientMovementBase and ClientBaseBoneName are null.
	 * Passes through to CharacterMovement->ServerMove_Implementation() with null base params.
	 */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveNoBase, ServerMovePacked)
	UFUNCTION(unreliable, server, WithValidation)
	void ServerMoveNoBase(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveNoBase_Implementation, ServerMovePacked_Implementation)
	void ServerMoveNoBase_Implementation(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);
	bool ServerMoveNoBase_Validate(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);

	/** Replicated function sent by client to server - contains client movement and view info for two moves. */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveDual, ServerMovePacked)
	UFUNCTION(unreliable, server, WithValidation)
	void ServerMoveDual(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveDual_Implementation, ServerMovePacked_Implementation)
	void ServerMoveDual_Implementation(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	bool ServerMoveDual_Validate(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);

	/** Replicated function sent by client to server - contains client movement and view info for two moves. */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveDualNoBase, ServerMovePacked)
	UFUNCTION(unreliable, server, WithValidation)
	void ServerMoveDualNoBase(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveDualNoBase_Implementation, ServerMovePacked_Implementation)
	void ServerMoveDualNoBase_Implementation(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);
	bool ServerMoveDualNoBase_Validate(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode);

	/** Replicated function sent by client to server - contains client movement and view info for two moves. First move is non root motion, second is root motion. */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveDualHybridRootMotion, ServerMovePacked)
	UFUNCTION(unreliable, server, WithValidation)
	void ServerMoveDualHybridRootMotion(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveDualHybridRootMotion_Implementation, ServerMovePacked_Implementation)
	void ServerMoveDualHybridRootMotion_Implementation(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);
	bool ServerMoveDualHybridRootMotion_Validate(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode);

	/* Resending an (important) old move. Process it if not already processed. */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveOld, ServerMovePacked)
	UFUNCTION(unreliable, server, WithValidation)
	void ServerMoveOld(float OldTimeStamp, FVector_NetQuantize10 OldAccel, uint8 OldMoveFlags);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ServerMoveOld_Implementation, ServerMovePacked_Implementation)
	void ServerMoveOld_Implementation(float OldTimeStamp, FVector_NetQuantize10 OldAccel, uint8 OldMoveFlags);
	bool ServerMoveOld_Validate(float OldTimeStamp, FVector_NetQuantize10 OldAccel, uint8 OldMoveFlags);

	/** If no client adjustment is needed after processing received ServerMove(), ack the good move so client can remove it from SavedMoves */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAckGoodMove, ClientMoveResponsePacked)
	UFUNCTION(unreliable, client)
	void ClientAckGoodMove(float TimeStamp);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAckGoodMove_Implementation, ClientMoveResponsePacked_Implementation)
	void ClientAckGoodMove_Implementation(float TimeStamp);

	/** Replicate position correction to client, associated with a timestamped servermove.  Client will replay subsequent moves after applying adjustment.  */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAckGoodMove, ClientMoveResponsePacked)
	UFUNCTION(unreliable, client)
	void ClientAdjustPosition(float TimeStamp, FVector NewLoc, FVector NewVel, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAdjustPosition_Implementation, ClientMoveResponsePacked_Implementation)
	void ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);

	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientVeryShortAdjustPosition, ClientMoveResponsePacked)
	/* Bandwidth saving version, when velocity is zeroed */
	UFUNCTION(unreliable, client)
	void ClientVeryShortAdjustPosition(float TimeStamp, FVector NewLoc, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientVeryShortAdjustPosition_Implementation, ClientMoveResponsePacked_Implementation)
	void ClientVeryShortAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);

	/** Replicate position correction to client when using root motion for movement. (animation root motion specific) */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAdjustRootMotionPosition, ClientMoveResponsePacked)
	UFUNCTION(unreliable, client)
	void ClientAdjustRootMotionPosition(float TimeStamp, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAdjustRootMotionPosition_Implementation, ClientMoveResponsePacked_Implementation)
	void ClientAdjustRootMotionPosition_Implementation(float TimeStamp, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);

	/** Replicate root motion source correction to client when using root motion for movement. */
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAdjustRootMotionSourcePosition, ClientMoveResponsePacked)
	UFUNCTION(unreliable, client)
	void ClientAdjustRootMotionSourcePosition(float TimeStamp, FRootMotionSourceGroup ServerRootMotion, bool bHasAnimRootMotion, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);
	DEPRECATED_CHARACTER_MOVEMENT_RPC(ClientAdjustRootMotionSourcePosition_Implementation, ClientMoveResponsePacked_Implementation)
	void ClientAdjustRootMotionSourcePosition_Implementation(float TimeStamp, FRootMotionSourceGroup ServerRootMotion, bool bHasAnimRootMotion, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode);

	//////////////////////////////////////////////////////////////////////////
	// END DEPRECATED RPCs
	//////////////////////////////////////////////////////////////////////////

public:
	/** Returns Mesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }

	/** Name of the MeshComponent. Use this name if you want to prevent creation of the component (with ObjectInitializer.DoNotCreateDefaultSubobject). */
	static FName MeshComponentName;

	/** Returns CharacterMovement subobject **/
	FORCEINLINE class UCharacterMovementComponent* GetCharacterMovement() const { return CharacterMovement; }

	/** Name of the CharacterMovement component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName CharacterMovementComponentName;

	/** Returns CapsuleComponent subobject **/
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

	/** Name of the CapsuleComponent. */
	static FName CapsuleComponentName;

#if WITH_EDITORONLY_DATA
	/** Returns ArrowComponent subobject **/
	class UArrowComponent* GetArrowComponent() const { return ArrowComponent; }
#endif

	/** Sets the component the Character is walking on, used by CharacterMovement walking movement to be able to follow dynamic objects. */
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName = NAME_None, bool bNotifyActor=true);
	
	/**
	 * Cache mesh offset from capsule. This is used as the target for network smoothing interpolation, when the mesh is offset with lagged smoothing.
	 * This is automatically called during initialization; call this at runtime if you intend to change the default mesh offset from the capsule.
	 * @see GetBaseTranslationOffset(), GetBaseRotationOffset()
	 */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void CacheInitialMeshOffset(FVector MeshRelativeLocation, FRotator MeshRelativeRotation);

protected:
	/** Info about our current movement base (object we are standing on). */
	UPROPERTY()
	struct FBasedMovementInfo BasedMovement;

	/** Replicated version of relative movement. Read-only on simulated proxies! */
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedBasedMovement)
	struct FBasedMovementInfo ReplicatedBasedMovement;

	/** Scale to apply to root motion translation on this Character */
	UPROPERTY(Replicated)
	float AnimRootMotionTranslationScale;

public:
	/** Rep notify for ReplicatedBasedMovement */
	UFUNCTION()
	virtual void OnRep_ReplicatedBasedMovement();

	/** Set whether this actor's movement replicates to network clients. */
	virtual void SetReplicateMovement(bool bInReplicateMovement) override;

protected:
	/** Saved translation offset of mesh. */
	UPROPERTY()
	FVector BaseTranslationOffset;

	/** Saved rotation offset of mesh. */
	UPROPERTY()
	FQuat BaseRotationOffset;

	/** Event called after actor's base changes (if SetBase was requested to notify us with bNotifyPawn). */
	virtual void BaseChange();

	/** CharacterMovement ServerLastTransformUpdateTimeStamp value, replicated to simulated proxies. */
	UPROPERTY(Replicated)
	float ReplicatedServerLastTransformUpdateTimeStamp;

	UPROPERTY(ReplicatedUsing=OnRep_ReplayLastTransformUpdateTimeStamp)
	float ReplayLastTransformUpdateTimeStamp;

	/** CharacterMovement MovementMode (and custom mode) replicated for simulated proxies. Use CharacterMovementComponent::UnpackNetworkMovementMode() to translate it. */
	UPROPERTY(Replicated)
	uint8 ReplicatedMovementMode;

	/** Flag that we are receiving replication of the based movement. */
	UPROPERTY()
	bool bInBaseReplication;

public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRep_ReplayLastTransformUpdateTimeStamp();

	/** Accessor for ReplicatedServerLastTransformUpdateTimeStamp. */
	FORCEINLINE float GetReplicatedServerLastTransformUpdateTimeStamp() const { return ReplicatedServerLastTransformUpdateTimeStamp; }

	/** Accessor for BasedMovement */
	FORCEINLINE const FBasedMovementInfo& GetBasedMovement() const { return BasedMovement; }
	
	/** Accessor for ReplicatedBasedMovement */
	FORCEINLINE const FBasedMovementInfo& GetReplicatedBasedMovement() const { return ReplicatedBasedMovement; }

	/** Save a new relative location in BasedMovement and a new rotation with is either relative or absolute. */
	void SaveRelativeBasedMovement(const FVector& NewRelativeLocation, const FRotator& NewRotation, bool bRelativeRotation);

	/** Returns ReplicatedMovementMode */
	uint8 GetReplicatedMovementMode() const { return ReplicatedMovementMode; }

	/** Get the saved translation offset of mesh. This is how much extra offset is applied from the center of the capsule. */
	UFUNCTION(BlueprintCallable, Category=Character)
	FVector GetBaseTranslationOffset() const { return BaseTranslationOffset; }

	/** Get the saved rotation offset of mesh. This is how much extra rotation is applied from the capsule rotation. */
	virtual FQuat GetBaseRotationOffset() const { return BaseRotationOffset; }

	/** Get the saved rotation offset of mesh. This is how much extra rotation is applied from the capsule rotation. */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(DisplayName="GetBaseRotationOffset", ScriptName="GetBaseRotationOffset"))
	FRotator GetBaseRotationOffsetRotator() const { return GetBaseRotationOffset().Rotator(); }

	/** Default crouched eye height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float CrouchedEyeHeight;

	/** Set by character movement to specify that this Character is currently crouched. */
	UPROPERTY(BlueprintReadOnly, replicatedUsing=OnRep_IsCrouched, Category=Character)
	uint32 bIsCrouched:1;

	/** Set to indicate that this Character is currently under the force of a jump (if JumpMaxHoldTime is non-zero). IsJumpProvidingForce() handles this as well. */
	UPROPERTY(Transient, Replicated)
	uint32 bProxyIsJumpForceApplied : 1;

	/** Handle Crouching replicated from server */
	UFUNCTION()
	virtual void OnRep_IsCrouched();

	/** When true, player wants to jump */
	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bPressedJump:1;

	/** When true, applying updates to network client (replaying saved moves for a locally controlled character) */
	UPROPERTY(Transient)
	uint32 bClientUpdating:1;

	/** True if Pawn was initially falling when started to replay network moves. */
	UPROPERTY(Transient)
	uint32 bClientWasFalling:1; 

	/** If server disagrees with root motion track position, client has to resimulate root motion from last AckedMove. */
	UPROPERTY(Transient)
	uint32 bClientResimulateRootMotion:1;

	/** If server disagrees with root motion state, client has to resimulate root motion from last AckedMove. */
	UPROPERTY(Transient)
	uint32 bClientResimulateRootMotionSources:1;

	/** Disable simulated gravity (set when character encroaches geometry on client, to keep him from falling through floors) */
	UPROPERTY()
	uint32 bSimGravityDisabled:1;

	UPROPERTY(Transient)
	uint32 bClientCheckEncroachmentOnNetUpdate:1;

	/** Disable root motion on the server. When receiving a DualServerMove, where the first move is not root motion and the second is. */
	UPROPERTY(Transient)
	uint32 bServerMoveIgnoreRootMotion:1;

	/** Tracks whether or not the character was already jumping last frame. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category=Character)
	uint32 bWasJumping : 1;

	/** 
	 * Jump key Held Time.
	 * This is the time that the player has held the jump key, in seconds.
	 */
	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly, Category=Character)
	float JumpKeyHoldTime;

	/** Amount of jump force time remaining, if JumpMaxHoldTime > 0. */
	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly, Category=Character)
	float JumpForceTimeRemaining;

	/** Track last time a jump force started for a proxy. */
	UPROPERTY(Transient, BlueprintReadOnly, VisibleInstanceOnly, Category=Character)
	float ProxyJumpForceStartedTime;

	/** 
	 * The max time the jump key can be held.
	 * Note that if StopJumping() is not called before the max jump hold time is reached,
	 * then the character will carry on receiving vertical velocity. Therefore it is usually 
	 * best to call StopJumping() when jump input has ceased (such as a button up event).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category=Character, Meta=(ClampMin=0.0, UIMin=0.0))
	float JumpMaxHoldTime;

    /**
     * The max number of jumps the character can perform.
     * Note that if JumpMaxHoldTime is non zero and StopJumping is not called, the player
     * may be able to perform and unlimited number of jumps. Therefore it is usually
     * best to call StopJumping() when jump input has ceased (such as a button up event).
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category=Character)
    int32 JumpMaxCount;

    /**
     * Tracks the current number of jumps performed.
     * This is incremented in CheckJumpInput, used in CanJump_Implementation, and reset in OnMovementModeChanged.
     * When providing overrides for these methods, it's recommended to either manually
     * increment / reset this value, or call the Super:: method.
     */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Character)
    int32 JumpCurrentCount;

	/**
	 * Represents the current number of jumps performed before CheckJumpInput modifies JumpCurrentCount.
	 * This is set in CheckJumpInput and is used in SetMoveFor and PrepMoveFor instead of JumpCurrentCount
	 * since CheckJumpInput can modify JumpCurrentCount.
	 * When providing overrides for these methods, it's recommended to either manually
	 * set this value, or call the Super:: method.
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Character)
	int32 JumpCurrentCountPreJump;

	/** Incremented every time there is an Actor overlap event (start or stop) on this actor. */
	uint32 NumActorOverlapEventsCounter;

	//~ Begin AActor Interface.
	virtual void BeginPlay() override;
	virtual void ClearCrossLevelReferences() override;
	virtual void PreNetReceive() override;
	virtual void PostNetReceive() override;
	virtual void OnRep_ReplicatedMovement() override;
	virtual void PostNetReceiveLocationAndRotation() override;
	virtual void GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const override;
	virtual UActorComponent* FindComponentByClass(const TSubclassOf<UActorComponent> ComponentClass) const override;
	virtual void TornOff() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);
	//~ End AActor Interface

	template<class T>
	T* FindComponentByClass() const
	{
		return AActor::FindComponentByClass<T>();
	}

	//~ Begin INavAgentInterface Interface
	virtual FVector GetNavAgentLocation() const override;
	//~ End INavAgentInterface Interface

	//~ Begin APawn Interface.
	virtual void PostInitializeComponents() override;
	virtual UPawnMovementComponent* GetMovementComponent() const override;
	virtual UPrimitiveComponent* GetMovementBase() const override final { return BasedMovement.MovementBase; }
	virtual float GetDefaultHalfHeight() const override;
	virtual void TurnOff() override;
	virtual void Restart() override;
	virtual void PawnClientRestart() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	virtual void RecalculateBaseEyeHeight() override;
	virtual void UpdateNavigationRelevance() override;
	//~ End APawn Interface

	/** Apply momentum caused by damage. */
	virtual void ApplyDamageMomentum(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	/** 
	 * Make the character jump on the next update.	 
	 * If you want your character to jump according to the time that the jump key is held,
	 * then you can set JumpMaxHoldTime to some non-zero value. Make sure in this case to
	 * call StopJumping() when you want the jump's z-velocity to stop being applied (such 
	 * as on a button up event), otherwise the character will carry on receiving the 
	 * velocity until JumpKeyHoldTime reaches JumpMaxHoldTime.
	 */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void Jump();

	/** 
	 * Stop the character from jumping on the next update. 
	 * Call this from an input event (such as a button 'up' event) to cease applying
	 * jump Z-velocity. If this is not called, then jump z-velocity will be applied
	 * until JumpMaxHoldTime is reached.
	 */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void StopJumping();

	/**
	 * Check if the character can jump in the current state.
	 *
	 * The default implementation may be overridden or extended by implementing the custom CanJump event in Blueprints. 
	 * 
	 * @Return Whether the character can jump in the current state. 
	 */
	UFUNCTION(BlueprintCallable, Category=Character)
	bool CanJump() const;

protected:
	/**
	 * Customizable event to check if the character can jump in the current state.
	 * Default implementation returns true if the character is on the ground and not crouching,
	 * has a valid CharacterMovementComponent and CanEverJump() returns true.
	 * Default implementation also allows for 'hold to jump higher' functionality: 
	 * As well as returning true when on the ground, it also returns true when GetMaxJumpTime is more
	 * than zero and IsJumping returns true.
	 * 
	 *
	 * @Return Whether the character can jump in the current state. 
	 */
	UFUNCTION(BlueprintNativeEvent, Category=Character, meta=(DisplayName="CanJump"))
	bool CanJumpInternal() const;
	virtual bool CanJumpInternal_Implementation() const;

public:

	/** Marks character as not trying to jump */
	virtual void ResetJumpState();

	/**
	 * True if jump is actively providing a force, such as when the jump key is held and the time it has been held is less than JumpMaxHoldTime.
	 * @see CharacterMovement->IsFalling
	 */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool IsJumpProvidingForce() const;

	/** Play Animation Montage on the character mesh. Returns the length of the animation montage in seconds, or 0.f if failed to play. **/
	UFUNCTION(BlueprintCallable, Category=Animation)
	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	/** Stop Animation Montage. If nullptr, it will stop what's currently active. The Blend Out Time is taken from the montage asset that is being stopped. **/
	UFUNCTION(BlueprintCallable, Category=Animation)
	virtual void StopAnimMontage(class UAnimMontage* AnimMontage = nullptr);

	/** Return current playing Montage **/
	UFUNCTION(BlueprintCallable, Category=Animation)
	class UAnimMontage* GetCurrentMontage() const;

	/**
	 * Set a pending launch velocity on the Character. This velocity will be processed on the next CharacterMovementComponent tick,
	 * and will set it to the "falling" state. Triggers the OnLaunched event.
	 * @PARAM LaunchVelocity is the velocity to impart to the Character
	 * @PARAM bXYOverride if true replace the XY part of the Character's velocity instead of adding to it.
	 * @PARAM bZOverride if true replace the Z component of the Character's velocity instead of adding to it.
	 */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);

	/** Let blueprint know that we were launched */
	UFUNCTION(BlueprintImplementableEvent)
	void OnLaunched(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);

	/** Event fired when the character has just started jumping */
	UFUNCTION(BlueprintNativeEvent, Category=Character)
	void OnJumped();
	virtual void OnJumped_Implementation();

	/** Called when the character's movement enters falling */
	virtual void Falling() {}

	/** Called when character's jump reaches Apex. Needs CharacterMovement->bNotifyApex = true */
	virtual void NotifyJumpApex();

	/** Broadcast when Character's jump reaches its apex. Needs CharacterMovement->bNotifyApex = true */
	UPROPERTY(BlueprintAssignable, Category=Character)
	FCharacterReachedApexSignature OnReachedJumpApex;

	/**
	 * Called upon landing when falling, to perform actions based on the Hit result. Triggers the OnLanded event.
	 * Note that movement mode is still "Falling" during this event. Current Velocity value is the velocity at the time of landing.
	 * Consider OnMovementModeChanged() as well, as that can be used once the movement mode changes to the new mode (most likely Walking).
	 *
	 * @param Hit Result describing the landing that resulted in a valid landing spot.
	 * @see OnMovementModeChanged()
	 */
	virtual void Landed(const FHitResult& Hit);

	/**
	 * Called upon landing when falling, to perform actions based on the Hit result.
	 * Note that movement mode is still "Falling" during this event. Current Velocity value is the velocity at the time of landing.
	 * Consider OnMovementModeChanged() as well, as that can be used once the movement mode changes to the new mode (most likely Walking).
	 *
	 * @param Hit Result describing the landing that resulted in a valid landing spot.
	 * @see OnMovementModeChanged()
	 */
	FLandedSignature LandedDelegate;

	/**
	 * Called upon landing when falling, to perform actions based on the Hit result.
	 * Note that movement mode is still "Falling" during this event. Current Velocity value is the velocity at the time of landing.
	 * Consider OnMovementModeChanged() as well, as that can be used once the movement mode changes to the new mode (most likely Walking).
	 *
	 * @param Hit Result describing the landing that resulted in a valid landing spot.
	 * @see OnMovementModeChanged()
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnLanded(const FHitResult& Hit);

	/**
	 * Event fired when the Character is walking off a surface and is about to fall because CharacterMovement->CurrentFloor became unwalkable.
	 * If CharacterMovement->MovementMode does not change during this event then the character will automatically start falling afterwards.
	 * @note Z velocity is zero during walking movement, and will be here as well. Another velocity can be computed here if desired and will be used when starting to fall.
	 *
	 * @param  PreviousFloorImpactNormal Normal of the previous walkable floor.
	 * @param  PreviousFloorContactNormal Normal of the contact with the previous walkable floor.
	 * @param  PreviousLocation	Previous character location before movement off the ledge.
	 * @param  TimeTick	Time delta of movement update resulting in moving off the ledge.
	 */
	UFUNCTION(BlueprintNativeEvent, Category=Character)
	void OnWalkingOffLedge(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta);
	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta);

	/**
	 * Called when pawn's movement is blocked
	 * @param Impact describes the blocking hit.
	 */
	virtual void MoveBlockedBy(const FHitResult& Impact) {};

	/**
	 * Request the character to start crouching. The request is processed on the next update of the CharacterMovementComponent.
	 * @see OnStartCrouch
	 * @see IsCrouched
	 * @see CharacterMovement->WantsToCrouch
	 */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void Crouch(bool bClientSimulation = false);

	/**
	 * Request the character to stop crouching. The request is processed on the next update of the CharacterMovementComponent.
	 * @see OnEndCrouch
	 * @see IsCrouched
	 * @see CharacterMovement->WantsToCrouch
	 */
	UFUNCTION(BlueprintCallable, Category=Character, meta=(HidePin="bClientSimulation"))
	virtual void UnCrouch(bool bClientSimulation = false);

	/** @return true if this character is currently able to crouch (and is not currently crouched) */
	UFUNCTION(BlueprintCallable, Category=Character)
	virtual bool CanCrouch() const;

	/** 
	 * Called when Character stops crouching. Called on non-owned Characters through bIsCrouched replication.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/** 
	 * Event when Character stops crouching.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndCrouch", ScriptName="OnEndCrouch"))
	void K2_OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/**
	 * Called when Character crouches. Called on non-owned Characters through bIsCrouched replication.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/**
	 * Event when Character crouches.
	 * @param	HalfHeightAdjust		difference between default collision half-height, and actual crouched capsule half-height.
	 * @param	ScaledHalfHeightAdjust	difference after component scale is taken in to account.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartCrouch", ScriptName="OnStartCrouch"))
	void K2_OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	/**
	 * Called from CharacterMovementComponent to notify the character that the movement mode has changed.
	 * @param	PrevMovementMode	Movement mode before the change
	 * @param	PrevCustomMode		Custom mode before the change (applicable if PrevMovementMode is Custom)
	 */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0);

	/** Multicast delegate for MovementMode changing. */
	UPROPERTY(BlueprintAssignable, Category=Character)
	FMovementModeChangedSignature MovementModeChangedDelegate;

	/**
	 * Called from CharacterMovementComponent to notify the character that the movement mode has changed.
	 * @param	PrevMovementMode	Movement mode before the change
	 * @param	NewMovementMode		New movement mode
	 * @param	PrevCustomMode		Custom mode before the change (applicable if PrevMovementMode is Custom)
	 * @param	NewCustomMode		New custom mode (applicable if NewMovementMode is Custom)
	 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnMovementModeChanged", ScriptName="OnMovementModeChanged"))
	void K2_OnMovementModeChanged(EMovementMode PrevMovementMode, EMovementMode NewMovementMode, uint8 PrevCustomMode, uint8 NewCustomMode);

	/**
	 * Event for implementing custom character movement mode. Called by CharacterMovement if MovementMode is set to Custom.
	 * @note C++ code should override UCharacterMovementComponent::PhysCustom() instead.
	 * @see UCharacterMovementComponent::PhysCustom()
	 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="UpdateCustomMovement", ScriptName="UpdateCustomMovement"))
	void K2_UpdateCustomMovement(float DeltaTime);

	/**
	 * Event triggered at the end of a CharacterMovementComponent movement update.
	 * This is the preferred event to use rather than the Tick event when performing custom updates to CharacterMovement properties based on the current state.
	 * This is mainly due to the nature of network updates, where client corrections in position from the server can cause multiple iterations of a movement update,
	 * which allows this event to update as well, while a Tick event would not.
	 *
	 * @param	DeltaSeconds		Delta time in seconds for this update
	 * @param	InitialLocation		Location at the start of the update. May be different than the current location if movement occurred.
	 * @param	InitialVelocity		Velocity at the start of the update. May be different than the current velocity.
	 */
	UPROPERTY(BlueprintAssignable, Category=Character)
	FCharacterMovementUpdatedSignature OnCharacterMovementUpdated;

	/** Returns true if the Landed() event should be called. Used by CharacterMovement to prevent notifications while playing back network moves. */
	virtual bool ShouldNotifyLanded(const struct FHitResult& Hit);

	/** Trigger jump if jump button has been pressed. */
	virtual void CheckJumpInput(float DeltaTime);

	/** Update jump input state after having checked input. */
	virtual void ClearJumpInput(float DeltaTime);

	/**
	 * Get the maximum jump time for the character.
	 * Note that if StopJumping() is not called before the max jump hold time is reached,
	 * then the character will carry on receiving vertical velocity. Therefore it is usually 
	 * best to call StopJumping() when jump input has ceased (such as a button up event).
	 * 
	 * @return Maximum jump time for the character
	 */
	virtual float GetJumpMaxHoldTime() const;

	UFUNCTION(Reliable, Client)
	void ClientCheatWalk();
	virtual void ClientCheatWalk_Implementation();

	UFUNCTION(Reliable, Client)
	void ClientCheatFly();
	virtual void ClientCheatFly_Implementation();

	UFUNCTION(Reliable, Client)
	void ClientCheatGhost();
	virtual void ClientCheatGhost_Implementation();

	UFUNCTION(Reliable, Client)
	void RootMotionDebugClientPrintOnScreen(const FString& InString);
	virtual void RootMotionDebugClientPrintOnScreen_Implementation(const FString& InString);

	// Root Motion

	/** 
	 * For LocallyControlled Autonomous clients. 
	 * During a PerformMovement() after root motion is prepared, we save it off into this and
	 * then record it into our SavedMoves.
	 * During SavedMove playback we use it as our "Previous Move" SavedRootMotion which includes
	 * last received root motion from the Server
	 */
	UPROPERTY(Transient)
	FRootMotionSourceGroup SavedRootMotion;

	/** For LocallyControlled Autonomous clients. Saved root motion data to be used by SavedMoves. */
	UPROPERTY(Transient)
	FRootMotionMovementParams ClientRootMotionParams;

	/** Array of previously received root motion moves from the server. */
	UPROPERTY(Transient)
	TArray<FSimulatedRootMotionReplicatedMove> RootMotionRepMoves;
	
	/** Find usable root motion replicated move from our buffer.
	 * Goes through the buffer back in time, to find the first move that clears 'CanUseRootMotionRepMove' below.
	 * Returns index of that move or INDEX_NONE otherwise.
	 */
	int32 FindRootMotionRepMove(const FAnimMontageInstance& ClientMontageInstance) const;

	/** True if buffered move is usable to teleport client back to. */
	bool CanUseRootMotionRepMove(const FSimulatedRootMotionReplicatedMove& RootMotionRepMove, const FAnimMontageInstance& ClientMontageInstance) const;

	/** Restore actor to an old buffered move. */
	bool RestoreReplicatedMove(const FSimulatedRootMotionReplicatedMove& RootMotionRepMove);
	
	/**
	 * Called on client after position update is received to respond to the new location and rotation.
	 * Actual change in location is expected to occur in CharacterMovement->SmoothCorrection(), after which this occurs.
	 * Default behavior is to check for penetration in a blocking object if bClientCheckEncroachmentOnNetUpdate is enabled, and set bSimGravityDisabled=true if so.
	 */
	virtual void OnUpdateSimulatedPosition(const FVector& OldLocation, const FQuat& OldRotation);

	/** Replicated Root Motion montage */
	UPROPERTY(ReplicatedUsing=OnRep_RootMotion)
	struct FRepRootMotionMontage RepRootMotion;
	
	/** Handles replicated root motion properties on simulated proxies and position correction. */
	UFUNCTION()
	void OnRep_RootMotion();

	/** Position fix up for Simulated Proxies playing Root Motion */
	void SimulatedRootMotionPositionFixup(float DeltaSeconds);

	/** Get FAnimMontageInstance playing RootMotion */
	FAnimMontageInstance * GetRootMotionAnimMontageInstance() const;

	/** True if we are playing Anim root motion right now */
	UFUNCTION(BlueprintCallable, Category=Animation, meta=(DisplayName="IsPlayingAnimRootMotion"))
	bool IsPlayingRootMotion() const;

	/** True if we are playing root motion from any source right now (anim root motion, root motion source) */
	UFUNCTION(BlueprintCallable, Category=Animation)
	bool HasAnyRootMotion() const;

	/**
	 * True if we are playing Root Motion right now, through a Montage with RootMotionMode == ERootMotionMode::RootMotionFromMontagesOnly.
	 * This means code path for networked root motion is enabled.
	 */
	UFUNCTION(BlueprintCallable, Category=Animation)
	bool IsPlayingNetworkedRootMotionMontage() const;

	/** Sets scale to apply to root motion translation on this Character */
	void SetAnimRootMotionTranslationScale(float InAnimRootMotionTranslationScale = 1.f);

	/** Returns current value of AnimRootMotionScale */
	UFUNCTION(BlueprintCallable, Category=Animation)
	float GetAnimRootMotionTranslationScale() const;

	/**
	 * Called on the actor right before replication occurs.
	 * Only called on Server, and for autonomous proxies if recording a Client Replay.
	 */
	virtual void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	/**
	 * Called on the actor right before replication occurs.
	 * Called for everyone when recording a Client Replay, including Simulated Proxies.
	 */
	virtual void PreReplicationForReplay(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
	//UDevice* device; 
	//
	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
	//UFireArm* firearm; 
	//
	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
	//UIndirect* indirect; 
	//
	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
	//UWeapon* actualWeapon;
	//
	////UFUNCTION(BlueprintCallable)
	////void
	////attachWeapon(UWeapon* weapon);
	//
	////UPROPERTY(EditAnywhere,BlueprintReadWrite)
	////Udamagable* damagable;
	//
	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//float oxigen;
	//
	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//float maxOxigen;
	//
	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//float health = 1;
	//
	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
	//FVector feets;
	//
	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
	//float collicionDistance;
	//
	//bool shooting = false;
	//
	//float timer = 0;
	//
	//float deadTimer;
	//
	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//bool isDead;
	//
	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	//void 
	//die();
};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Pawn.h"
//#include "PlayerCharacter.generated.h"
//
//class UInputComponent;
//class USkeletalMeshComponent;
//class USceneComponent;
//class UCameraComponent;
//class UMotionControllerComponent;
//class UAnimMontage;
//class USoundBase;
//class UDevice;
//class UFireArm;
//class UIndirect;
//class UMyUserWidget;
//class UWeapon;
//class Udamagable;
//class UPart;
//class UMovement_C;
//class UShapeComponent;
//
//UCLASS()
//class VOIDBASTARDS_API APlayerCharacter : public APawn
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this pawn's properties
//	APlayerCharacter();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//
//	/** Pawn mesh: 1st person view (arms; seen only by self) */
//	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
//	USkeletalMeshComponent* Mesh1P;
//
//	/** Gun mesh: 1st person view (seen only by self) */
//	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
//	USkeletalMeshComponent* FP_Gun;
//
//	/** Location on gun mesh where projectiles should spawn. */
//	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
//	USceneComponent* FP_MuzzleLocation;
//
//	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
//	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
//	USkeletalMeshComponent* VR_Gun;
//
//	/** Location on VR gun mesh where projectiles should spawn. */
//	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
//	USceneComponent* VR_MuzzleLocation;
//
//	/** First person camera */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
//	UCameraComponent* FirstPersonCameraComponent;
//
//	/** Motion controller (right hand) */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
//	UMotionControllerComponent* R_MotionController;
//
//	/** Motion controller (left hand) */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
//	UMotionControllerComponent* L_MotionController;
//
//protected:
////	virtual void BeginPlay();
//
//	public:
//	//virtual void Tick(float DeltaTime) override;
//
//	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
//	void 
//	die();
//
//	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
//	void exitLevel();
//
//	void MoveForward(float value);
//
//	void MoveHorizontal(float value);
//
//public:
//
//	UFUNCTION( BlueprintCallable)
//	float
//	getOxigenPercentage();
//
//	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
//	float BaseTurnRate;
//
//	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
//	float BaseLookUpRate;
//
//	/** Gun muzzle's offset from the characters location */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
//	FVector GunOffset;
//
//	/** Projectile class to spawn */
//	UPROPERTY(EditDefaultsOnly, Category=Projectile)
//	TSubclassOf<class AvoidBastardsProjectile> ProjectileClass;
//
//	/** Sound to play each time we fire */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
//	USoundBase* FireSound;
//
//	/** AnimMontage to play each time we fire */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
//	UAnimMontage* FireAnimation;
//
//	/** Whether to use motion controller location for aiming. */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
//	uint8 bUsingMotionControllers : 1;
//
//protected:
//	
//	/** Fires a projectile. */
//	void OnFire();
//
//	/** Fires a projectile. */
//	void OnEndFire();
//
//	/** Resets HMD orientation and position in VR. */
//	void OnResetVR();
//
//	/** Handles moving forward/backward */
//	//void MoveForward(float Val);
//	//
//	///** Handles stafing movement, left and right */
//	//void MoveRight(float Val);
//
//	/**
//	 * Called via input to turn at a given rate.
//	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
//	 */
//	void TurnAtRate(float Rate);
//
//	/**
//	 * Called via input to turn look up/down at a given rate.
//	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
//	 */
//	void LookUpAtRate(float Rate);
//
//
//	
//
//	struct TouchData
//	{
//		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
//		bool bIsPressed;
//		ETouchIndex::Type FingerIndex;
//		FVector Location;
//		bool bMoved;
//	};
//	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
//	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
//	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
//
//	void use();
//	TouchData	TouchItem;
//	
//protected:
//	// APawn interface
//	//virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
//	// End of APawn interface
//
//	/* 
//	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
//	 *
//	 * @param	InputComponent	The input component pointer to bind controls to
//	 * @returns true if touch controls were enabled.
//	 */
//	bool EnableTouchscreenMovement(UInputComponent* InputComponent);
//
//public:
//	/** Returns Mesh1P subobject **/
//	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
//	/** Returns FirstPersonCameraComponent subobject **/
//	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
//
//	UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	UDevice* device; 
//
//	UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	UFireArm* firearm; 
//
//	UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	UIndirect* indirect; 
//
//	UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	UWeapon* actualWeapon;
//
//	UFUNCTION(BlueprintCallable)
//	void
//	attachWeapon(UWeapon* weapon);
//
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	Udamagable* damagable;
//
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	float oxigen;
//
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	float maxOxigen;
//
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	float health = 1;
//
//	//UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	//FVector feets;
//
//	UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	float collicionDistance;
//
//	bool shooting = false;
//
//	float timer = 0;
//
//	float deadTimer;
//
//	UPROPERTY(EditAnywhere,BlueprintReadWrite)
//	bool isDead;
//
//	UPROPERTY(BlueprintReadWrite,EditAnywhere)
//	UShapeComponent* feets;
//
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnityComponents)
//  UMovement_C* MovementComp;
//
//	FVector SumForces;
//
//};
