
#include "ranger_top.h"


void InsertTopCoord(Top* top, int x, int y)
{
	//dont insert duplicated coords if they user is holding stylus in place
	if (top->stack && (top->stack->x == x && top->stack->y == y))
	{
		//printf("skiped exsisting %d %d\n", x, y);
		return;
	}

	InsertCoord(&top->stack, x, y);
}

float ComputeAndUpdateDistance(Top* top)
{
	if (top->stack == nullptr || top->stack->nextCoord == nullptr) {
		//printf("Insufficient coordinates in stack.\n");
		return 0.0f;
	}

	float totalDistance = 0.0f;
	top->stack->distance_at_point = 0;
	for (Coord* aux = top->stack; aux->nextCoord != nullptr; aux = aux->nextCoord)
	{
		float dist = Dist(aux, aux->nextCoord);
		aux->nextCoord->distance_at_point = dist + aux->distance_at_point;
		totalDistance += dist;
		//printf(" %f ", aux->distance_at_point);
	}

	top->distance = totalDistance;
	//printf("total distance %f\n", totalDistance);
	return totalDistance;
}

void ForceTopDistanceLimit(Top* top)
{
	while (top->distance > kMaxPolyLength)
	{
		if (top->stack == nullptr || top->stack->nextCoord == nullptr) {
			return;
		}

		Coord* lastMinusOneCoord = BotStack(top->stack, -1);
		Coord* lastCoord = BotStack(top->stack, 0);

		if (lastCoord == nullptr || lastMinusOneCoord == nullptr) {
			return;
		}

		float distBetweenLastAndLastMinusOne = lastCoord->distance_at_point - lastMinusOneCoord->distance_at_point;

		// This block handles the case where the *entire* last segment is beyond kMaxDistance
		if (lastMinusOneCoord->distance_at_point >= kMaxPolyLength - KSegmentLengthEpsilon) // Use an epsilon for float comparison
		{
			// Remove the last point if the previous point is already at or beyond the limit.
			// This prevents adding zero-length segments or pushing points beyond the limit.
			lastMinusOneCoord->nextCoord = nullptr;
			free(lastCoord);
			top->distance = lastMinusOneCoord->distance_at_point; // Update total distance correctly
			continue; // Re-evaluate with the new last point
		}

		// Now, we are in the case where lastMinusOneCoord->distance_at_point < kMaxDistance
		// and top->distance (which is lastCoord->distance_at_point) > kMaxDistance.
		// We need to move lastCoord to the kMaxDistance point.

		Vector2 lastCoordVec = { lastCoord->x, lastCoord->y };
		Vector2 lastMinusOneCoordVec = { lastMinusOneCoord->x, lastMinusOneCoord->y };
		Vector2 lastMinusOneToLastVec = Vector2Subtract(lastCoordVec, lastMinusOneCoordVec);

		float distLeftToCover = kMaxPolyLength - lastMinusOneCoord->distance_at_point;

		// Avoid division by zero or normalizing a zero vector
		float segmentLengthSq = lastMinusOneToLastVec.x * lastMinusOneToLastVec.x +
			lastMinusOneToLastVec.y * lastMinusOneToLastVec.y;

		if (segmentLengthSq < KSegmentLengthEpsilon * KSegmentLengthEpsilon) // If the segment is practically a point
		{
			// This means lastMinusOneCoord and lastCoord are already very close or identical.
			// In this scenario, we should likely just make lastCoord identical to lastMinusOneCoord
			// and set the distance to kMaxDistance, effectively collapsing the segment.
			lastCoord->x = (int)std::round(lastMinusOneCoordVec.x);
			lastCoord->y = (int)std::round(lastMinusOneCoordVec.y);
			lastCoord->distance_at_point = kMaxPolyLength;
			top->distance = kMaxPolyLength;
			return; // Done
		}

		Vector2 normalizedDir = Vector2Normalize(lastMinusOneToLastVec);
		Vector2 vecToMaxDist = Vector2Scale(normalizedDir, distLeftToCover);

		Vector2 newLastPosVec = Vector2Add(lastMinusOneCoordVec, vecToMaxDist);

		lastCoord->x = (int)std::round(newLastPosVec.x);
		lastCoord->y = (int)std::round(newLastPosVec.y);
		lastCoord->distance_at_point = kMaxPolyLength;
		top->distance = kMaxPolyLength;
		return; // Done
	}
}

void ResetTop(Top* top)
{
	DestroyStack(&top->stack); top->stack = nullptr;
	top->distance = 0;
}

//======= intersection stuff  =============================================

// Check if point r is on segment pq
bool onSegment(Coord* p, Coord* q, Coord* r) {
	return r->x <= std::max(p->x, q->x) && r->x >= std::min(p->x, q->x) &&
		r->y <= std::max(p->y, q->y) && r->y >= std::min(p->y, q->y);
}

// Determine the orientation of the triplet (p, q, r)
// 0 -> collinear, 1 -> clockwise, 2 -> counterclockwise
int orientation(Coord* p, Coord* q, Coord* r) {
	int val = (q->y - p->y) * (r->x - q->x) -
		(q->x - p->x) * (r->y - q->y);
	if (val == 0) return 0;           // Collinear
	return (val > 0) ? 1 : 2;         // Clockwise or Counterclockwise
}

// Check if two segments p1q1 and p2q2 intersect
bool doIntersect(Coord* p1, Coord* q1, Coord* p2, Coord* q2) {
	// Find the orientations
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4) return true;

	// Special Cases
	if (o1 == 0 && onSegment(p1, q1, p2)) return true;
	if (o2 == 0 && onSegment(p1, q1, q2)) return true;
	if (o3 == 0 && onSegment(p2, q2, p1)) return true;
	if (o4 == 0 && onSegment(p2, q2, q1)) return true;

	return false; // Doesn't fall in any of the cases
}

bool getIntersectionPoint(Coord* p1, Coord* q1, Coord* p2, Coord* q2, Coord& intersection) {
	// Line equations: p1 + t1 * (q1 - p1) = p2 + t2 * (q2 - p2)
	int a1 = q1->y - p1->y;
	int b1 = p1->x - q1->x;
	int c1 = a1 * p1->x + b1 * p1->y;

	int a2 = q2->y - p2->y;
	int b2 = p2->x - q2->x;
	int c2 = a2 * p2->x + b2 * p2->y;

	int determinant = a1 * b2 - a2 * b1;

	if (determinant == 0) return false; // Parallel lines

	// Calculate intersection point
	intersection.x = (b2 * c1 - b1 * c2) / determinant;
	intersection.y = (a1 * c2 - a2 * c1) / determinant;
	return true;
}

// Shoelace formula to calculate the area of a polygon
double calculatePolygonArea(Coord* start, Coord* end) {
	if (!start || !end) return 0.0;

	double area = 0.0;
	Coord* current = start;
	Coord* next = nullptr;

	// Traverse from start to end (inclusive) and apply the Shoelace formula
	while (current != end) {
		next = current->nextCoord;
		if (!next) break;

		area += current->x * next->y - current->y * next->x;
		current = next;
	}

	// Add the last segment (end to start) to close the polygon
	area += current->x * start->y - current->y * start->x;

	return fabs(area) / 2.0;
}

Coord* DeepcopyPolyStartEnd(Coord* startNode, Coord *endNode)
{
	Coord* newPoly;
	Coord* aux = startNode;
	Coord* newPolyAux = nullptr;

	newPoly = (Coord*)malloc(sizeof(Coord));
	newPoly->x = startNode->x;
	newPoly->y = startNode->y;

	while (aux != endNode)
	{ 
	  aux = aux->nextCoord;

	  if (newPolyAux == nullptr) newPolyAux = newPoly;
	  newPolyAux->nextCoord = (Coord*)malloc(sizeof(Coord));
	  newPolyAux = newPolyAux->nextCoord;

	  newPolyAux->x = aux->x;
	  newPolyAux->y = aux->y;
	}

	newPolyAux->nextCoord = nullptr;

	return newPoly;
}

Coord* checkTopIntersection(Top* top, const std::vector<EnclosableObject*>& enclosableObjs) {
	Coord* head = top->stack;
	if (head == nullptr) return false;
	if (head->nextCoord == nullptr) return false;

	Coord* headNext = head->nextCoord;

	// starts at the kMinDepth point cause you can intersect with your adjacent segment
	Coord* current = headNext;
	while (current && current->nextCoord) {
		if (doIntersect(head, headNext, current, current->nextCoord)) {

			//printf("tried intersect %d %d %d %d\n", head->depth, headNext->depth, current->depth, current->nextCoord->depth);
			int depth = headNext->depth - current->depth;

			if ((depth < kMinDepth) /*|| (distance < kMinDistance)*/)
			{
				current = current->nextCoord;
				continue;
			}

			//check if area is big enough
			Coord intersection;
			if (!getIntersectionPoint(head, headNext, current, current->nextCoord, intersection))
			{
				current = current->nextCoord;
				continue;
			}
			int oldHeadX = head->x;
			int oldHeadY = head->y;
			int oldcurrentX = current->x;
			int oldcurrentY = current->y;

			head->x = intersection.x;
			head->y = intersection.y;
			current->x = intersection.x;
			current->y = intersection.y;

			double area = calculatePolygonArea(head, current);

			head->x = oldHeadX;
			head->y = oldHeadY;
			current->x = oldcurrentX;
			current->y = oldcurrentY;

			if (area < kMinArea)
			{
				current = current->nextCoord;
				continue;
			}

			head->intersected = true;
			headNext->intersected = true;
			current->intersected = true;
			current->nextCoord->intersected = true;
			printf("intersected %d %d %d %d  point %d %d area %f\n", head->depth, headNext->depth,
				current->depth, current->nextCoord->depth, intersection.x, intersection.y, area);

			Coord* enclosedPolygon = DeepcopyPolyStartEnd(head,current);
			

			//run enclosed logic
			for (EnclosableObject* enclosableObj : enclosableObjs)
			{
				float centerX = enclosableObj->boundingBox.x + enclosableObj->boundingBox.width / 2;
				float centerY = enclosableObj->boundingBox.y + enclosableObj->boundingBox.height / 2;

				//if (PointInPolygon(head, current, centerX, centerY)) {
				//	enclosableObj->OnEnclosed(); // This will call Pokemon::onEnclosed()
				//}

				int polyNodeCount = head->depth - current->depth;

				if (pnpoly({ centerX,centerY }, CoordListToPointList(head, current), polyNodeCount) != 0)
				{
					enclosableObj->OnEnclosed();
				}

			}

			//printf("depth headnext %d current %d \n", headNext->depth, current->depth);
			freeCoordsBackward(headNext, current);

			current->x = intersection.x;
			current->y = intersection.y;

			head->nextCoord = current;

			//TODO: distance hard to implement bc you are deleting nodes buddy
			//UpdateStackDepths(head);

			return enclosedPolygon;
		}
		current = current->nextCoord;
	}
	return nullptr;
}
