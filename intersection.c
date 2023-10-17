// Checks whether the first rectangle contains one of the two vertices of the second rectangle.
int PointIntersection(long x1, long x2, long y1, long y2, long x3, long x4, long y3, long y4){
    long greaterX = (x1 > x2) ? x1 : x2;
    long greaterY = (y1 > y2) ? y1 : y2;
    long smallerX = (x1 < x2) ? x1 : x2;
    long smallerY = (y1 < y2) ? y1 : y2;

    // Checks for first vertex of the second rectangle...
    // Checks if x3 is in the interval [smallerX, greaterX]
    // and if y3 is in the interval [smallerY, greaterY]
    if (smallerX <= x3 && x3 <= greaterX && smallerY <= y3 && y3 <= greaterY) {
        return 1;
    }

    // Checks for second vertex of the second rectangle...
    // Checks if x4 is in the interval [smallerX, greaterX]
    // and if y4 is in the interval [smallerY, greaterY]
    if (smallerX <= x4 && x4 <= greaterX && smallerY <= y4 && y4 <= greaterY) {
        return 1;
    }

    return 0;
}

// Checks whether any of the two vertices of one of the rectangles is inside of the other.
int Intersection(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4) {
    // Checks if first contains vertex of second or if second contains vertex of first.
    if (PointIntersection(x1,x2,y1,y2,x3,x4,y3,y4) == 1 || PointIntersection(x3,x4,y3,y4,x1,x2,y1,y2) == 1){
        return 1;
    }
    else {
        return 0;
    }
}