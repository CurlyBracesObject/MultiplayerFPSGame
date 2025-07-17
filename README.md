# LegoGame - Multiplayer FPS Game Technical Implementation

A multiplayer online FPS game developed with Unreal Engine C++, featuring intelligent AI, EQS environmental queries, network synchronization, and faction warfare systems.

**Video Demo:** https://www.bilibili.com/video/BV1msugzJEdT/?vd_source=c096d37a6e7624ca39a2afef5c3f64d2

## Core Technical Architecture

### 1. AI Perception and Behavior System

**AI Perception Mechanism: Visual Perception**

```cpp
// AI Controller with integrated visual perception component
ALGAIController::ALGAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
}

// Automatically start behavior tree when AI possesses character
void ALGAIController::OnPossess(APawn* InPawn)
{
    if(ALGEnemyCharacter* EnemyCharacter = Cast<ALGEnemyCharacter>(InPawn))
    {
        RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
        // Initialize blackboard data
        GetBlackboardComponent()->SetValueAsVector(TEXT("FindPos"), InPawn->GetActorLocation() + InPawn->GetActorForwardVector() * 1000);
        GetBlackboardComponent()->SetValueAsVector(TEXT("OriginPos"), InPawn->GetActorLocation());
    }
}
```

### 2. EQS Environmental Query System

**EQS_HidePos (Cover Position Query)**

SimpleGrid Generator: Generate candidate points in 1500x1500 unit grid around target with 200 unit spacing

Scoring Mechanism and Weight Configuration:
- **Distance to EnvQueryC_Target (Distance to Player)** - Weight x1
  - Linear scoring: Close to player=0 points, Far from player=1 point
  - Goal: Select points far from player (return 1-point locations)
- **Distance to Querier (Distance to AI)** - Weight x2
  - Inverse Linear scoring: Far from AI=0 points, Close to AI=1 point
  - Goal: Select points close to AI (return 1-point locations)
- **Trace to EnvQueryC_Target on Visibility (Line of Sight Test)** - Weight x1
  - ItemHeightOffset: +80 (Sample point upward offset 80 units)
  - ContextHeightOffset: +40 (AI center point upward offset 40 units, simulating eye position)
  - Ray casting: From sample point+80 height → toward AI+40 height
  - Cannot see AI=1 point, Can see AI=0 points
  - Goal: Select points where player cannot see AI, suitable as cover (return 1-point locations)

Weight Analysis: Distance to Querier has highest weight (x2), indicating this EQS prioritizes points close to AI, ensuring AI can quickly reach cover positions.

Return Logic: System selects points with highest weighted total score, i.e., optimal cover positions that are: far from player, close to AI, and where AI cannot be seen.

**EQS_FindFirePos (Fire Position Query)**

SimpleGrid Generator: Generate candidate points in 1500x1500 unit grid around target with 200 unit spacing

Scoring Mechanism and Weight Configuration:
- **Distance to Querier (Distance to AI)** - Weight x3
  - Inverse Linear scoring: Far from AI=0 points, Close to AI=1 point
  - Goal: Select points close to AI (return 1-point locations)
- **Distance to EnvQueryC_Target (Distance to Player)** - Weight x2
  - Linear scoring: Close to player=0 points, Far from player=1 point
  - Goal: Select points far from player (return 1-point locations)
- **Trace to EnvQueryC_Target on Visibility (Line of Sight Test)** - Weight x10
  - ItemHeightOffset: +90 (Sample point upward offset 90 units)
  - ContextHeightOffset: +60 (Target center point upward offset 60 units)
  - Ray casting: From sample point+90 height → toward target+60 height
  - Boolean match unchecked: Can see target=1 point, Cannot see target=0 points
  - Goal: Select points that can see player target, suitable for firing (return 1-point locations)

Weight Analysis: Trace Visibility has highest weight (x10), indicating this EQS prioritizes clear line-of-sight firing positions, ensuring AI can effectively attack targets.

Return Logic: System selects points with highest weighted total score, i.e., optimal firing positions that are: close to AI, far from player, and with clear sight of target.

### 3. Weapon System and Reload Mechanisms

**AI Reload Logic**

```cpp
// PawnAction system implementation for AI reload
class UPawnAction_Reload : public UPawnAction
{
    virtual bool Start() override
    {
        if(ALGCharacterBase* CharacterBase = Cast<ALGCharacterBase>(BrainComp->GetAIOwner()->GetPawn()))
        {
            if(!CharacterBase->IsHoldWeapon()) return false;
            
            float WaitTime = CharacterBase->GetHoldWeapon()->ReloadWeapon();
            if(WaitTime > 0)
            {
                // Set timer to wait for reload completion
                GetWorld()->GetTimerManager().SetTimer(Handle, this, &UPawnAction_Reload::ReloadEndCallBack, WaitTime);
            }
        }
        return true;
    }
    
    void ReloadEndCallBack()
    {
        CharacterBase->GetHoldWeapon()->MakeFullClip();
        Finish(EPawnActionResult::Success);
    }
};
```

**Player Reload Logic**

```cpp
// Weapon state machine controlling reload process
enum class EWeaponState : uint8
{
    EWS_Normal,   // Normal state
    EWS_Fire,     // Firing state
    EWS_Reload,   // Reloading state
    EWS_Empty     // Empty ammunition state
};

float AWeaponActor::ReloadWeapon()
{
    if(CurrentState == EWeaponState::EWS_Fire)
    {
        StopFire(); // Stop firing before reloading
    }
    CurrentState = EWeaponState::EWS_Reload;
    
    if(ReloadMontage && MyMaster)
    {
        NetMulti_PlayReloadAnim(); // Network synchronized animation
        return ReloadMontage->GetPlayLength();
    }
    return 0;
}
```

### 4. Network RPC Synchronization

**Server RPCs - Authoritative Operations**

```cpp
// Fire control RPC
UFUNCTION(Server, Reliable, WithValidation)
void Server_StartFire();

void AWeaponActor::StartFire()
{
    if(!HasAuthority())
    {
        Server_StartFire(); // Client calls server RPC
        return;
    }
    // Server executes firing logic
    CurrentState = EWeaponState::EWS_Fire;
    SpawnBullet();
}

// Sprint state synchronization
UFUNCTION(Server, Reliable, WithValidation)
void Server_ChangeSprint(bool bOutSprint);

void ALGCharacterBase::StartSprint()
{
    bSprint = true;
    if(!HasAuthority())
    {
        Server_ChangeSprint(true);
    }
}
```

**Multicast RPCs - Visual Effect Synchronization**

```cpp
// Reload animation synchronized to all clients
UFUNCTION(NetMulticast, Reliable)
void NetMulti_PlayReloadAnim();

void AWeaponActor::NetMulti_PlayReloadAnim_Implementation()
{
    MyMaster->PlayAnimMontage(ReloadMontage);
}
```

**Network Variable Replication**

```cpp
// Critical data network synchronization
void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWeaponActor, MyMaster);
    DOREPLIFETIME(AWeaponActor, CurrentClipVolume);
    DOREPLIFETIME(AWeaponActor, CurrentState);
}

// Ammunition synchronization callback
void AWeaponActor::OnRep_CurrentClipVolume()
{
    if(CurrentClipVolume != 0 || CurrentClipVolume != MaxClipVolume)
    {
        SpawnEffect(); // Play firing effects
    }
}
```

### 5. Faction System Design

**Three-Faction Relationship Definition**

```cpp
// Global faction ID constants
const FGenericTeamId TeamID_Red(1);
const FGenericTeamId TeamID_Blue(2);
const FGenericTeamId TeamID_Yellow(3);

// Faction enumeration
UENUM()
enum class ETeamColor
{
    ETC_Red,     // Red faction
    ETC_Blue,    // Blue faction
    ETC_Yellow   // Yellow faction (Neutral)
};
```

**Faction Relationship Solver**

```cpp
// Set faction rules in GameMode
void ALGGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Lambda function defining faction relationships
    FGenericTeamId::SetAttitudeSolver([](FGenericTeamId A, FGenericTeamId B)
    {
        // Yellow faction is neutral to everyone
        if(A == TeamID_Yellow || B == TeamID_Yellow)
        {
            return ETeamAttitude::Neutral;
        }
        // Other factions: same = friendly, different = hostile
        return A != B ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
    });
}
```

**AI Faction Determination Logic**

```cpp
// AI Controller implements faction interface
ETeamAttitude::Type ALGAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    if(const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other))
    {
        if(IGenericTeamAgentInterface* MyTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn()))
        {
            // Use global faction rule solver
            return FGenericTeamId::GetAttitude(MyTeamAgent->GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId());
        }
    }
    return ETeamAttitude::Neutral;
}

// Get AI faction ID
FGenericTeamId ALGAIController::GetGenericTeamId() const
{
    switch(TeamColor)
    {
        case ETeamColor::ETC_Blue: return TeamID_Blue;
        case ETeamColor::ETC_Yellow: return TeamID_Yellow;
        default: return TeamID_Red;
    }
}
```

### 6. Precision Shooting System

**Player Precise Aiming**

```cpp
void AWeaponActor::GetFirePostAndDirection(FVector& Position, FVector& Direction)
{
    Position = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleSocket"));
    
    if(Cast<ALGPlayerCharacter>(MyMaster))
    {
        // Player uses camera center ray casting
        FVector WorldPos = MyMaster->GetCameraComponent()->GetComponentLocation();
        FVector WorldDir = MyMaster->GetCameraComponent()->GetComponentRotation().Vector();
        
        FHitResult Hit;
        if(GetWorld()->LineTraceSingleByChannel(Hit, WorldPos, WorldPos + WorldDir * 50000, ECC_WeaponTrace))
        {
            // Precise direction from muzzle to player's aim point
            Direction = (Hit.Location - Position).GetSafeNormal();
        }
    }
    else
    {
        // AI uses controller orientation
        Direction = MyMaster->GetControlRotation().Vector();
        
        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(MyMaster);
        
        if(GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() + Direction * 50000, ECC_WeaponTrace, Params))
        {
            Direction = (Hit.Location - Position).GetSafeNormal();
        }
    }
}
```

### 7. Death and Spectator System

**Death Handling and Spectator Mode**

```cpp
float ALGCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    CurrentHP--;
    if(CurrentHP <= 0)
    {
        // Player death spawns spectator pawn
        if(Cast<APlayerController>(GetController()))
        {
            TSubclassOf<ASpectatorPawn> SpectatorClass = LoadClass<ASpectatorPawn>(nullptr, 
                TEXT("Blueprint'/Game/Lego/BP/Player/BP_Spectator.BP_Spectator_C'"));
            ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, GetActorLocation(), GetActorRotation());
            
            if(SpectatorPawn)
            {
                GetController()->Possess(SpectatorPawn);
            }
        }
        else
        {
            GetController()->UnPossess(); // AI death releases control
        }
        
        // Ragdoll physics simulation
        const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        FVector ForceDir = (PointDamageEvent->HitInfo.Location - DamageCauser->GetActorLocation()).GetSafeNormal();
        Multi_Dead(ForceDir, PointDamageEvent->HitInfo.BoneName);
    }
    return 0;
}

// Network synchronized death effects
UFUNCTION(NetMulticast, Reliable)
void Multi_Dead(FVector Impulse, FName BoneName);

void ALGCharacterBase::Multi_Dead_Implementation(FVector Impulse, FName BoneName)
{
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->AddImpulse(Impulse * 2000, BoneName, true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
```

### 8. Event Broadcasting System

**Equipment System Event Notifications**

```cpp
// Trigger events when equipping weapons
bool UPackageComponent::EquipWeaponByPropsID(int32 ID)
{
    HoldWeapon = GetOwner()->GetWorld()->SpawnActor<AWeaponActor>(WeaponInfo->WeaponClass);
    if(HoldWeapon)
    {
        HoldWeapon->SetMaster(Cast<ALGCharacterBase>(GetOwner()));
        CurrentWeaponID = ID;
        OnRep_HoldWeapon(); // Trigger network sync callback
    }
    return true;
}

// Weapon equipment event broadcasting
void UPackageComponent::OnRep_HoldWeapon()
{
    if(HoldWeapon)
    {
        if(OnEquipWeapon.IsBound())
        {
            OnEquipWeapon.Broadcast(HoldWeapon); // Notify all listeners
        }
        
        if(ALGCharacterBase* CharacterBase = Cast<ALGCharacterBase>(GetOwner()))
        {
            CharacterBase->SetLockView(true);
        }
    }
}
```

## Technical Highlights Summary

1. **Modular AI Architecture**: Complete AI solution combining Behavior Trees + EQS + PawnAction systems
2. **Robust Network Synchronization**: Server RPC authority validation + Multicast visual sync + Critical variable replication
3. **Flexible Faction System**: Lambda solver + Interface polymorphism + Extensible design
4. **Precision Shooting Mechanics**: Player screen-center detection + AI intelligent aiming + Network latency compensation
5. **Event-Driven Architecture**: Broadcast delegates + Observer pattern + Loosely coupled component communication

This project demonstrates the core technology stack of modern multiplayer game development, featuring complete implementation from low-level network synchronization to high-level AI decision making.


---

# LegoGame - 多人FPS游戏完整技术实现

基于虚幻引擎C++开发的多人联机FPS游戏，集成智能AI、EQS环境查询、网络同步、阵营对战等核心系统。

**Video Demo:** https://www.bilibili.com/video/BV1msugzJEdT/?vd_source=c096d37a6e7624ca39a2afef5c3f64d2

## 核心技术架构

### 1. AI感知与行为系统

**AI感知机制：视觉感知**

```cpp
// AI控制器集成视觉感知组件
ALGAIController::ALGAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
}

// AI接管角色时自动启动行为树
void ALGAIController::OnPossess(APawn* InPawn)
{
    if(ALGEnemyCharacter* EnemyCharacter = Cast<ALGEnemyCharacter>(InPawn))
    {
        RunBehaviorTree(EnemyCharacter->GetBehaviorTree());
        // 初始化黑板数据
        GetBlackboardComponent()->SetValueAsVector(TEXT("FindPos"), InPawn->GetActorLocation() + InPawn->GetActorForwardVector() * 1000);
        GetBlackboardComponent()->SetValueAsVector(TEXT("OriginPos"), InPawn->GetActorLocation());
    }
}
```

### 2. EQS环境查询系统

**EQS_HidePos (掩体位置查询)**

SimpleGrid生成器：在目标周围1500x1500单位网格，200单位间距生成候选点

评分机制与权重配置：
- **Distance to EnvQueryC_Target (距离玩家)** - 权重x1
  - Linear评分：距离玩家近=0分，距离玩家远=1分
  - 目标：选择距离玩家远的点 (返回1分的点)
- **Distance to Querier (距离AI自己)** - 权重x2
  - Inverse Linear评分：距离AI远=0分，距离AI近=1分
  - 目标：选择距离AI自己近的点 (返回1分的点)
- **Trace to EnvQueryC_Target on Visibility (视线检测)** - 权重x1
  - ItemHeightOffset: +80 (采样点向上偏移80单位)
  - ContextHeightOffset: +40 (AI中心点向上偏移40单位，模拟眼部位置)
  - 射线检测：从采样点+80高度 → 向AI+40高度
  - 看不到AI=1分，能看到AI=0分
  - 目标：选择玩家看不到AI的点，可作为掩体 (返回1分的点)

权重分析：Distance to Querier权重最高(x2)，表明该EQS优先考虑距离AI近的点，确保AI能快速到达掩体位置。

返回逻辑：系统选择加权总分最高的点，即：距离玩家远、距离AI近、且AI看不到的最优掩体位置。

**EQS_FindFirePos (射击位置查询)**

SimpleGrid生成器：在目标周围1500x1500单位网格，200单位间距生成候选点

评分机制与权重配置：
- **Distance to Querier (距离AI自己)** - 权重x3
  - Inverse Linear评分：距离AI远=0分，距离AI近=1分
  - 目标：选择距离AI自己近的点 (返回1分的点)
- **Distance to EnvQueryC_Target (距离玩家)** - 权重x2
  - Linear评分：距离玩家近=0分，距离玩家远=1分
  - 目标：选择距离玩家远的点 (返回1分的点)
- **Trace to EnvQueryC_Target on Visibility (视线检测)** - 权重x10
  - ItemHeightOffset: +90 (采样点向上偏移90单位)
  - ContextHeightOffset: +60 (目标中心点向上偏移60单位)
  - 射线检测：从采样点+90高度 → 向目标+60高度
  - 布尔匹配未勾选：能看到目标=1分，看不到目标=0分
  - 目标：选择能看到玩家目标的点，适合开火射击 (返回1分的点)

权重分析：Trace Visibility权重最高(x10)，表明该EQS最优先考虑视线通畅的射击位置，确保AI能有效攻击目标。

返回逻辑：系统选择加权总分最高的点，即：距离AI近、距离玩家远、且能清晰看到目标的最优射击位置。

### 3. 武器系统与重新装弹机制

**AI重新装弹逻辑**

```cpp
// PawnAction系统实现AI重新装弹
class UPawnAction_Reload : public UPawnAction
{
    virtual bool Start() override
    {
        if(ALGCharacterBase* CharacterBase = Cast<ALGCharacterBase>(BrainComp->GetAIOwner()->GetPawn()))
        {
            if(!CharacterBase->IsHoldWeapon()) return false;
            
            float WaitTime = CharacterBase->GetHoldWeapon()->ReloadWeapon();
            if(WaitTime > 0)
            {
                // 设置定时器等待重新装弹完成
                GetWorld()->GetTimerManager().SetTimer(Handle, this, &UPawnAction_Reload::ReloadEndCallBack, WaitTime);
            }
        }
        return true;
    }
    
    void ReloadEndCallBack()
    {
        CharacterBase->GetHoldWeapon()->MakeFullClip();
        Finish(EPawnActionResult::Success);
    }
};
```

**玩家重新装弹逻辑**

```cpp
// 武器状态机控制重新装弹
enum class EWeaponState : uint8
{
    EWS_Normal,   // 正常状态
    EWS_Fire,     // 射击状态
    EWS_Reload,   // 重新装弹状态
    EWS_Empty     // 弹药耗尽状态
};

float AWeaponActor::ReloadWeapon()
{
    if(CurrentState == EWeaponState::EWS_Fire)
    {
        StopFire(); // 射击中需要先停止射击
    }
    CurrentState = EWeaponState::EWS_Reload;
    
    if(ReloadMontage && MyMaster)
    {
        NetMulti_PlayReloadAnim(); // 网络同步播放动画
        return ReloadMontage->GetPlayLength();
    }
    return 0;
}
```

### 4. 网络RPC同步机制

**服务器RPC - 权威性操作**

```cpp
// 射击控制RPC
UFUNCTION(Server, Reliable, WithValidation)
void Server_StartFire();

void AWeaponActor::StartFire()
{
    if(!HasAuthority())
    {
        Server_StartFire(); // 客户端调用服务器RPC
        return;
    }
    // 服务器执行射击逻辑
    CurrentState = EWeaponState::EWS_Fire;
    SpawnBullet();
}

// 冲刺状态同步
UFUNCTION(Server, Reliable, WithValidation)
void Server_ChangeSprint(bool bOutSprint);

void ALGCharacterBase::StartSprint()
{
    bSprint = true;
    if(!HasAuthority())
    {
        Server_ChangeSprint(true);
    }
}
```

**多播RPC - 视觉效果同步**

```cpp
// 重新装弹动画同步到所有客户端
UFUNCTION(NetMulticast, Reliable)
void NetMulti_PlayReloadAnim();

void AWeaponActor::NetMulti_PlayReloadAnim_Implementation()
{
    MyMaster->PlayAnimMontage(ReloadMontage);
}
```

**网络变量同步**

```cpp
// 关键数据网络同步
void AWeaponActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWeaponActor, MyMaster);
    DOREPLIFETIME(AWeaponActor, CurrentClipVolume);
    DOREPLIFETIME(AWeaponActor, CurrentState);
}

// 弹药同步回调
void AWeaponActor::OnRep_CurrentClipVolume()
{
    if(CurrentClipVolume != 0 || CurrentClipVolume != MaxClipVolume)
    {
        SpawnEffect(); // 播放射击特效
    }
}
```

### 5. 阵营系统设计

**三方阵营关系定义**

```cpp
// 全局阵营ID常量
const FGenericTeamId TeamID_Red(1);
const FGenericTeamId TeamID_Blue(2);
const FGenericTeamId TeamID_Yellow(3);

// 阵营枚举
UENUM()
enum class ETeamColor
{
    ETC_Red,     // 红色阵营
    ETC_Blue,    // 蓝色阵营
    ETC_Yellow   // 黄色阵营（中立）
};
```

**阵营关系求解器**

```cpp
// 在GameMode中设置阵营规则
void ALGGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Lambda函数定义阵营关系
    FGenericTeamId::SetAttitudeSolver([](FGenericTeamId A, FGenericTeamId B)
    {
        // 黄色阵营对所有人中立
        if(A == TeamID_Yellow || B == TeamID_Yellow)
        {
            return ETeamAttitude::Neutral;
        }
        // 其他阵营：相同友好，不同敌对
        return A != B ? ETeamAttitude::Hostile : ETeamAttitude::Friendly;
    });
}
```

**AI阵营判断逻辑**

```cpp
// AI控制器实现阵营接口
ETeamAttitude::Type ALGAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    if(const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other))
    {
        if(IGenericTeamAgentInterface* MyTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn()))
        {
            // 使用全局阵营规则求解器
            return FGenericTeamId::GetAttitude(MyTeamAgent->GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId());
        }
    }
    return ETeamAttitude::Neutral;
}

// 获取AI阵营ID
FGenericTeamId ALGAIController::GetGenericTeamId() const
{
    switch(TeamColor)
    {
        case ETeamColor::ETC_Blue: return TeamID_Blue;
        case ETeamColor::ETC_Yellow: return TeamID_Yellow;
        default: return TeamID_Red;
    }
}
```

### 6. 精确射击系统

**玩家精确瞄准**

```cpp
void AWeaponActor::GetFirePostAndDirection(FVector& Position, FVector& Direction)
{
    Position = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleSocket"));
    
    if(Cast<ALGPlayerCharacter>(MyMaster))
    {
        // 玩家使用相机中心射线检测
        FVector WorldPos = MyMaster->GetCameraComponent()->GetComponentLocation();
        FVector WorldDir = MyMaster->GetCameraComponent()->GetComponentRotation().Vector();
        
        FHitResult Hit;
        if(GetWorld()->LineTraceSingleByChannel(Hit, WorldPos, WorldPos + WorldDir * 50000, ECC_WeaponTrace))
        {
            // 从枪口指向玩家瞄准点的精确方向
            Direction = (Hit.Location - Position).GetSafeNormal();
        }
    }
    else
    {
        // AI使用控制器朝向
        Direction = MyMaster->GetControlRotation().Vector();
        
        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(MyMaster);
        
        if(GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() + Direction * 50000, ECC_WeaponTrace, Params))
        {
            Direction = (Hit.Location - Position).GetSafeNormal();
        }
    }
}
```

### 7. 死亡与观战系统

**死亡处理与观战模式**

```cpp
float ALGCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    CurrentHP--;
    if(CurrentHP <= 0)
    {
        // 玩家死亡生成观战Pawn
        if(Cast<APlayerController>(GetController()))
        {
            TSubclassOf<ASpectatorPawn> SpectatorClass = LoadClass<ASpectatorPawn>(nullptr, 
                TEXT("Blueprint'/Game/Lego/BP/Player/BP_Spectator.BP_Spectator_C'"));
            ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, GetActorLocation(), GetActorRotation());
            
            if(SpectatorPawn)
            {
                GetController()->Possess(SpectatorPawn);
            }
        }
        else
        {
            GetController()->UnPossess(); // AI死亡解除控制
        }
        
        // 布娃娃物理模拟
        const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
        FVector ForceDir = (PointDamageEvent->HitInfo.Location - DamageCauser->GetActorLocation()).GetSafeNormal();
        Multi_Dead(ForceDir, PointDamageEvent->HitInfo.BoneName);
    }
    return 0;
}

// 网络同步死亡效果
UFUNCTION(NetMulticast, Reliable)
void Multi_Dead(FVector Impulse, FName BoneName);

void ALGCharacterBase::Multi_Dead_Implementation(FVector Impulse, FName BoneName)
{
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->AddImpulse(Impulse * 2000, BoneName, true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
```

### 8. 事件广播系统

**装备系统事件通知**

```cpp
// 装备武器时触发事件
bool UPackageComponent::EquipWeaponByPropsID(int32 ID)
{
    HoldWeapon = GetOwner()->GetWorld()->SpawnActor<AWeaponActor>(WeaponInfo->WeaponClass);
    if(HoldWeapon)
    {
        HoldWeapon->SetMaster(Cast<ALGCharacterBase>(GetOwner()));
        CurrentWeaponID = ID;
        OnRep_HoldWeapon(); // 触发网络同步回调
    }
    return true;
}

// 武器装备事件广播
void UPackageComponent::OnRep_HoldWeapon()
{
    if(HoldWeapon)
    {
        if(OnEquipWeapon.IsBound())
        {
            OnEquipWeapon.Broadcast(HoldWeapon); // 通知所有监听者
        }
        
        if(ALGCharacterBase* CharacterBase = Cast<ALGCharacterBase>(GetOwner()))
        {
            CharacterBase->SetLockView(true);
        }
    }
}
```

## 技术特色总结

1. **模块化AI架构**：行为树 + EQS + PawnAction的完整AI解决方案
2. **健壮网络同步**：Server RPC权威验证 + Multicast视觉同步 + 关键变量Replication
3. **灵活阵营系统**：Lambda求解器 + 接口多态 + 可扩展设计
4. **精确射击机制**：玩家屏幕中心检测 + AI智能瞄准 + 网络延迟补偿
5. **事件驱动架构**：广播委托 + 监听器模式 + 松耦合组件通信

项目展示了现代多人游戏开发的核心技术栈，从底层网络同步到高层AI决策的完整实现。

