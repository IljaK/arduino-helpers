#pragma once
#include "StackArray.h"
#include "../common/Util.h"

class StringStackArray: public StackArray<char *>
{
protected:
    void FreeItem(char * item) override {
        free(item);
    }
public:
    StringStackArray(const size_t maxSize):StackArray(maxSize) {}

    bool IsElementEqual(char * item1, char * item2) override
    {
        if (item1 != NULL && item2 != NULL) {
            return strcmp(item1, item2) == 0;
        }
        return StackArray::IsElementEqual(item1, item2);
    }

    char *AppendCopy(char *item)
    {
        if (item == NULL) return NULL;
        if (item[0] == 0) return NULL;
        if (IsFull()) return NULL;
        if (Contains(item)) return NULL;

        char *pCopy = makeNewCopy(item);
        if (pCopy != NULL && !Append(pCopy)) {
            free(pCopy);
            return NULL;
        }

        return pCopy;
    }
    char *InsertCopy(char *item, size_t index)
    {
        if (item == NULL) return NULL;
        if (item[0] == 0) return NULL;
        if (IsFull()) return NULL;
        if (Contains(item)) return NULL;

        char *pCopy = makeNewCopy(item);
        if (pCopy != NULL && !Insert(pCopy, index)) {
            free(pCopy);
            return NULL;
        }

        return pCopy;
    }

    void DeleteFirst() {
        char *item = UnshiftFirst();
        if (item == NULL) return;
        FreeItem(item);
    }
};