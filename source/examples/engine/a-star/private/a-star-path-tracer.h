#pragma once

namespace ppp
{
    #pragma once

    #include "CoreMinimal.h"
    #include "UObject/Object.h"

    #include "PathTracer.generated.h"

    USTRUCT(BlueprintType, Category = "PowerpuffWorld/Data Structure", DisplayName = "Path Trace")
        struct POWERPUFFWORLD_API FPathTraceSnapshot
    {
        GENERATED_BODY()

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        int32 GCost;
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        int32 HCost;
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        int32 FCost;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        int32 IndexX;
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        int32 IndexY;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        FColor BackgroundColor;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        bool IsIncluded;
    };

    USTRUCT(BlueprintType, Category = "PowerpuffWorld/Data Structure", DisplayName = "Path Trace Snapshot")
        struct POWERPUFFWORLD_API FPathTrace
    {
        GENERATED_BODY()

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        TArray<FPathTraceSnapshot> Snapshots;
    };

    UCLASS(BlueprintType, Category = "PowerpuffWorld/Objects", DisplayName = "Path Tracer")
        class POWERPUFFWORLD_API UPathTracer : public UObject
    {
        GENERATED_BODY()

    public:
        const TArray<FPathTrace>& GetPathTraces() const;

        void ClearSnapshots();

        void TakeSnapshot(const class UGrid* grid, class UPathNode* current, const TArray<class UPathNode*>& openList, const TSet<UPathNode*>& closedSet);
        void TakeSnapshotFinal(const class UGrid* grid, const TArray<class UPathNode*>& retracedPath);

    protected:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        TArray<FPathTrace> PathTraces;
    };
}