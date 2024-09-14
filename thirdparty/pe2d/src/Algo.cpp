#include "Algo.hpp"

namespace pe2d
{
    namespace algo
    {
        CollisionPoints FindBoxBoxCollision(
            Vector2 sizeA, Transform transformA,
            Vector2 sizeB, Transform transformB)
        {
            const std::array<Vector2, 4> verticesA = GetBoxVertices(sizeA, transformA);
            const std::array<Vector2, 4> verticesB = GetBoxVertices(sizeB, transformB);
            const std::array<Vector2, 2> axesA = GetBoxAxes(verticesA);
            const std::array<Vector2, 2> axesB = GetBoxAxes(verticesB);
            const Vector2 *smallestAxis = nullptr;
            float overlap = pe2dMath::INF;

            for(int i = 0; i < axesA.size(); i++)
            {
                const Vector2 pA1 = Project(verticesA, axesA[i]);
                const Vector2 pA2 = Project(verticesB, axesA[i]);  

                float overlapA = 0.0f;
                if(!Overlap(pA1, pA2, overlapA))
                {
                    return CollisionPoints();
                }

                const Vector2 pB1 = Project(verticesA, axesB[i]);
                const Vector2 pB2 = Project(verticesB, axesB[i]);

                float overlapB = 0.0f;
                if(!Overlap(pB1, pB2, overlapB))
                {
                    return CollisionPoints();
                }
                
                if(overlapA < overlap)
                {
                    overlap = overlapA;
                    smallestAxis = &axesA[i];
                }

                if(overlapB < overlap)
                {
                    overlap = overlapB;
                    smallestAxis = &axesB[i];
                }
            }
            
            std::pair<Vector2, Vector2> contactPoints = FindBoxBoxContactPoint(verticesA, verticesB);
            if(contactPoints.second == Vector2(-1.0f, -1.0f))
            {
                return CollisionPoints(*smallestAxis, overlap, contactPoints.first, true);
            }
            return CollisionPoints(*smallestAxis, overlap, contactPoints.first, contactPoints.second, true);
        }
        
        std::pair<Vector2, Vector2> FindBoxBoxContactPoint(const std::array<Vector2, 4> &boxVerticesA, const std::array<Vector2, 4> &boxVerticesB)
        {
            constexpr float error = 0.00005f;
            float minDistanceSquared = pe2dMath::INF;
            Vector2 contactPoint1 = Vector2(0.0f, 0.0f);
            Vector2 contactPoint2 = Vector2(0.0f, 0.0f);
            int contactCount = 1;

            for(int i = 0; i < boxVerticesA.size(); i++)
            {
                for(int j = 0; j < boxVerticesB.size(); j++)
                {
                    float distanceFromVertexAToEdgeBSquared = 0.0f;
                    Vector2 cp = Vector2(0.0f, 0.0f);
                    const Vector2 p1 = boxVerticesB[j];
                    const Vector2 p2 = boxVerticesB[(j + 1) % boxVerticesB.size()];

                    PointSegmentDistance(boxVerticesA[i], p1, p2, distanceFromVertexAToEdgeBSquared, cp);
                    if(pe2dMath::NearlyEquel(distanceFromVertexAToEdgeBSquared, minDistanceSquared, error))
                    {
                        if(!pe2dMath::NearlyEquel(cp, contactPoint1, error))
                        {
                            contactPoint2 = cp;
                            contactCount = 2;
                        }
                    }
                    else if(distanceFromVertexAToEdgeBSquared < minDistanceSquared)
                    {
                        minDistanceSquared = distanceFromVertexAToEdgeBSquared;
                        contactPoint1 = cp;
                        contactCount = 1;
                    }
                }
            }

            for(int i = 0; i < boxVerticesB.size(); i++)
            {
                for(int j = 0; j < boxVerticesA.size(); j++)
                {
                    float distanceFromVertexBToEdgeASquared = 0.0f;
                    Vector2 cp = Vector2(0.0f, 0.0f);
                    const Vector2 p1 = boxVerticesA[j];
                    const Vector2 p2 = boxVerticesA[(j + 1) % boxVerticesA.size()];

                    PointSegmentDistance(boxVerticesB[i], p1, p2, distanceFromVertexBToEdgeASquared, cp);
                    if(pe2dMath::NearlyEquel(distanceFromVertexBToEdgeASquared, minDistanceSquared, error))
                    {
                        if(!pe2dMath::NearlyEquel(cp, contactPoint1, error))
                        {
                            contactPoint2 = cp;
                            contactCount = 2;
                        }
                    }
                    else if(distanceFromVertexBToEdgeASquared < minDistanceSquared)
                    {
                        minDistanceSquared = distanceFromVertexBToEdgeASquared;
                        contactPoint1 = cp;
                        contactCount = 1;
                    }
                }
            }

            if(contactCount == 2)
            {
                return std::make_pair(contactPoint1, contactPoint2);
            }
            return std::make_pair(contactPoint1, Vector2(-1.0f, -1.0f));
        }
        
        void PointSegmentDistance(Vector2 point, Vector2 vertexA, Vector2 vertexB, float &distanceSquared, Vector2 &contactPoint)
        {
            const Vector2 ab = vertexB - vertexA;
            const Vector2 ap = point - vertexA;
            const float proj = pe2dMath::Dot(ap, ab);
            const float abLengthSquared = pe2dMath::SquaredLength(ab);
            // relative position of the projection of the point onto the line segment
            const float d = proj / abLengthSquared;

            if(d <= 0.0f)
            {
                contactPoint = vertexA;
            }
            else if(d >= 1.0f)
            {
                contactPoint = vertexB;
            }
            else
            {
                contactPoint = vertexA + ab * d;
            }
            distanceSquared = pe2dMath::SquaredDistance(point, contactPoint);
        }

        std::array<Vector2, 4> GetBoxVertices(Vector2 boxSize, Transform transform)
        {
            const Vector2 center = transform.position;
            const Vector2 scale = transform.scale;
            const float scaledHalfSizeX = (boxSize.x * scale.x) / 2.0f;
            const float scaledHalfSizeY = (boxSize.y * scale.y) / 2.0f;
            std::array<Vector2, 4> vertices = 
            {
                Vector2( center.x - scaledHalfSizeX, center.y - scaledHalfSizeY ),
                Vector2( center.x + scaledHalfSizeX, center.y - scaledHalfSizeY ),
                Vector2( center.x + scaledHalfSizeX, center.y + scaledHalfSizeY ),
                Vector2( center.x - scaledHalfSizeX, center.y + scaledHalfSizeY )
            };
            RotateVertices(vertices, center, transform.rotation);
            return vertices;
        }

        std::array<Vector2, 2> GetBoxAxes(const std::array<Vector2, 4> &vertices)
        {
            std::array<Vector2, 2> axes;
            // is has two parrarel edges so I don't have to check other two
            for(int i = 0; i < 2; i++)
            {
                const Vector2 p1 = vertices[i];
                const Vector2 p2 = vertices[(i + 1) % vertices.size()];
                const Vector2 edge = p1 - p2;
                const Vector2 normal = pe2dMath::Normalize(pe2dMath::Perp(edge));
                axes[i] = normal;
            }
            return axes;
        }
    }
}  