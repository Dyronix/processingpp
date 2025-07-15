#pragma once

namespace ppp
{
    #pragma once

    #include "CoreMinimal.h"
    #include "UObject/Object.h"
    #include "Objects/PathTracer.h"

    #include "PathResolver.generated.h"

    USTRUCT(BlueprintType, Category = "PowerpuffWorld/Data Structures", DisplayName = "Path Resolver Result")
        struct POWERPUFFWORLD_API FPathResolverResult
    {
        GENERATED_BODY()

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        TArray<class UPathNode*> PathNodes;
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerpuffWorld")
        TArray<FPathTrace> PathTraces;
    };

    UCLASS(BlueprintType, Category = "PowerpuffWorld/Objects", DisplayName = "Path Resolver")
        class POWERPUFFWORLD_API UPathResolver : public UObject
    {
        GENERATED_BODY()

    public:
        FPathResolverResult Resolve(const class UGrid* grid, const TArray<FIntPoint>& locations, class UPathTracer* tracer = nullptr);
    };
}