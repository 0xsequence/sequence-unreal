#pragma once

class FBatchTestBuilder
{
public:
    FBatchTestBuilder(int32 InBatchSize, int32 InTotalItems)
        : BatchSize(InBatchSize), TotalItems(InTotalItems), StartIndex(0), EndIndex(FMath::Min(BatchSize - 1, TotalItems - 1))
    {
    }

    bool CanBuildBatch() const
    {
        return StartIndex < TotalItems;
    }

    void BuildNextBatch()
    {
        StartIndex += BatchSize;
        EndIndex = FMath::Min((StartIndex + BatchSize - 1), TotalItems - 1);
    }

    int32 GetStartIndex() const
    {
        return StartIndex;
    }

    int32 GetEndIndex() const
    {
        return EndIndex;
    }

private:
    int32 BatchSize;
    int32 TotalItems;
    int32 StartIndex;
    int32 EndIndex;
};