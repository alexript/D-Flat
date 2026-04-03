/* rect_test.c - Minimal rect.c for testing */

#include "test_dflat.h"

/* --- Produce the vector end points produced by the overlap
        of two other vectors --- */
static void subVector(int *v1, int *v2,
                        int t1, int t2, int o1, int o2)
{
    *v1 = *v2 = -1;
    if (within(o1, t1, t2))    {
        *v1 = o1;
        if (within(o2, t1, t2))
            *v2 = o2;
        else
            *v2 = t2;
    }
    else if (within(o2, t1, t2))    {
        *v2 = o2;
        if (within(o1, t1, t2))
            *v1 = o1;
        else
            *v1 = t1;
    }
    else if (within(t1, o1, o2))    {
        *v1 = t1;
        if (within(t2, o1, o2))
            *v2 = t2;
        else
            *v2 = o2;
    }
    else if (within(t2, o1, o2))    {
        *v2 = t2;
        if (within(t1, o1, o2))
            *v1 = t1;
        else
            *v1 = o1;
    }
}

/* --- Return the rectangle produced by the overlap
        of two other rectangles ---- */
RECT subRectangle(RECT r1, RECT r2)
{
    RECT r = {0,0,0,0};
    subVector((int *) &RectLeft(r), (int *) &RectRight(r),
        RectLeft(r1), RectRight(r1),
        RectLeft(r2), RectRight(r2));
    subVector((int *) &RectTop(r), (int *) &RectBottom(r),
        RectTop(r1), RectBottom(r1),
        RectTop(r2), RectBottom(r2));
    if (RectRight(r) == -1 || RectTop(r) == -1)
        RectRight(r) =
        RectLeft(r) =
        RectTop(r) =
        RectBottom(r) = 0;
    return r;
}