#define SHADOWS @shadows_enabled

#if SHADOWS
    uniform float maximumShadowMapDistance;
    uniform float shadowFadeStart;
    @foreach shadow_texture_unit_index @shadow_texture_unit_list
        uniform sampler2D shadowTexture@shadow_texture_unit_index;
        varying vec4 shadowSpaceCoords@shadow_texture_unit_index;

#if @perspectiveShadowMaps
        varying vec4 shadowRegionCoords@shadow_texture_unit_index;
#endif
    @endforeach
#endif // SHADOWS

#if SHADOWS
const float lightSizeFactor = 0.05;
const float nearPlane = 0.4;
const float maxSearchDistance = 0.01;
const float maxFilterRadius = 0.01;
const int shadowSampleCount = 25;

const vec2 poissonDisk[shadowSampleCount] = vec2[](
    vec2(0.513658, -0.361747),
    vec2(0.7941055, -0.5156633),
    vec2(0.4015315, 0.00368995),
    vec2(0.2348373, -0.5646414),
    vec2(0.08148332, -0.1521704),
    vec2(0.9071004, -0.04301345),
    vec2(0.5765684, -0.7820904),
    vec2(0.02575831, -0.9248317),
    vec2(-0.4247141, -0.7601865),
    vec2(-0.1650984, -0.5609509),
    vec2(-0.00553054, 0.4322597),
    vec2(0.374622, 0.536724),
    vec2(0.629549, 0.3171313),
    vec2(-0.1453903, 0.8028749),
    vec2(0.1050673, 0.9782572),
    vec2(0.7705216, 0.6211317),
    vec2(-0.5209134, -0.2073833),
    vec2(-0.2157063, -0.05150497),
    vec2(-0.7314271, -0.4448851),
    vec2(-0.9658175, -0.08176702),
    vec2(-0.6456084, 0.08727718),
    vec2(0.4758473, 0.8578165),
    vec2(-0.4970213, 0.3748735),
    vec2(-0.79823, 0.4584326),
    vec2(-0.4652869, 0.7911729)
);

vec2 findShadowOccluders(sampler2D shadowMap, vec3 coords, float receiver)
{
    float searchDistance = min(maxSearchDistance, lightSizeFactor / receiver * (receiver - nearPlane));
    float scaledDistance = searchDistance * coords.z;
    float depthSum = 0;
    int occluderCount = 0;
    for (int i = 0; i < shadowSampleCount; ++i)
    {
        vec3 offset = vec3(poissonDisk[i] * scaledDistance, 0);
        float depth = texture2DProj(shadowMap, coords + offset).r;
        if (depth < receiver)
        {
            ++occluderCount;
            depthSum += depth;
        }
    }
    return vec2(depthSum / occluderCount, occluderCount);
}

float percentageCloserFilter(sampler2D shadowMap, vec3 coords, float receiver, float filterRadius)
{
    float scaledRadius = filterRadius * coords.z;
    float sum = 0.0;
    for (int i = 0; i < shadowSampleCount; ++i)
    {
        vec3 offset = vec3(poissonDisk[i] * scaledRadius, 0);
        sum += float(receiver <= texture2DProj(shadowMap, coords + offset).r);
    }
    return sum / shadowSampleCount;
}

float sampleShadow(sampler2D shadowMap, vec4 coords)
{
    float receiverDepth = min(coords.z / coords.w, 1);
    vec3 coordsProj = coords.xyw;
    vec2 occluders = findShadowOccluders(shadowMap, coordsProj, receiverDepth);
    if (occluders.y == 0)
    {
        return 1.0;
    }

    float meanDepth = occluders.x;
    float penumbra = (receiverDepth - meanDepth) / meanDepth;
    float filterRadius = penumbra * lightSizeFactor * nearPlane / receiverDepth;
    filterRadius = min(filterRadius, maxFilterRadius);
    return percentageCloserFilter(shadowMap, coordsProj, receiverDepth, filterRadius);
}
#endif

float unshadowedLightRatio(float distance)
{
    float shadowing = 1.0;
#if SHADOWS
#if @limitShadowMapDistance
    float fade = clamp((distance - shadowFadeStart) / (maximumShadowMapDistance - shadowFadeStart), 0.0, 1.0);
    if (fade == 1.0)
        return shadowing;
#endif
    #if @shadowMapsOverlap
        bool doneShadows = false;
        @foreach shadow_texture_unit_index @shadow_texture_unit_list
            if (!doneShadows)
            {
                vec3 shadowXYZ = shadowSpaceCoords@shadow_texture_unit_index.xyz / shadowSpaceCoords@shadow_texture_unit_index.w;
#if @perspectiveShadowMaps
                vec3 shadowRegionXYZ = shadowRegionCoords@shadow_texture_unit_index.xyz / shadowRegionCoords@shadow_texture_unit_index.w;
#endif
                if (all(lessThan(shadowXYZ.xy, vec2(1.0, 1.0))) && all(greaterThan(shadowXYZ.xy, vec2(0.0, 0.0))))
                {
                    shadowing = min(sampleShadow(shadowTexture@shadow_texture_unit_index, shadowSpaceCoords@shadow_texture_unit_index), shadowing);

                    
                    doneShadows = all(lessThan(shadowXYZ, vec3(0.95, 0.95, 1.0))) && all(greaterThan(shadowXYZ, vec3(0.05, 0.05, 0.0)));
#if @perspectiveShadowMaps
                    doneShadows = doneShadows && all(lessThan(shadowRegionXYZ, vec3(1.0, 1.0, 1.0))) && all(greaterThan(shadowRegionXYZ.xy, vec2(-1.0, -1.0)));
#endif
                }
            }
        @endforeach
    #else
        @foreach shadow_texture_unit_index @shadow_texture_unit_list
            shadowing = min(sampleShadow(shadowTexture@shadow_texture_unit_index, shadowSpaceCoords@shadow_texture_unit_index), shadowing);
        @endforeach
    #endif
#if @limitShadowMapDistance
    shadowing = mix(shadowing, 1.0, fade);
#endif
#endif // SHADOWS
    return shadowing;
}

void applyShadowDebugOverlay()
{
#if SHADOWS && @useShadowDebugOverlay
    bool doneOverlay = false;
    float colourIndex = 0.0;
    @foreach shadow_texture_unit_index @shadow_texture_unit_list
        if (!doneOverlay)
        {
            vec3 shadowXYZ = shadowSpaceCoords@shadow_texture_unit_index.xyz / shadowSpaceCoords@shadow_texture_unit_index.w;
#if @perspectiveShadowMaps
            vec3 shadowRegionXYZ = shadowRegionCoords@shadow_texture_unit_index.xyz / shadowRegionCoords@shadow_texture_unit_index.w;
#endif
            if (all(lessThan(shadowXYZ.xy, vec2(1.0, 1.0))) && all(greaterThan(shadowXYZ.xy, vec2(0.0, 0.0))))
            {
                colourIndex = mod(@shadow_texture_unit_index.0, 3.0);
		        if (colourIndex < 1.0)
			        gl_FragData[0].x += 0.1;
		        else if (colourIndex < 2.0)
			        gl_FragData[0].y += 0.1;
		        else
			        gl_FragData[0].z += 0.1;

                doneOverlay = all(lessThan(shadowXYZ, vec3(0.95, 0.95, 1.0))) && all(greaterThan(shadowXYZ, vec3(0.05, 0.05, 0.0)));
#if @perspectiveShadowMaps
                doneOverlay = doneOverlay && all(lessThan(shadowRegionXYZ.xyz, vec3(1.0, 1.0, 1.0))) && all(greaterThan(shadowRegionXYZ.xy, vec2(-1.0, -1.0)));
#endif
            }
        }
    @endforeach
#endif // SHADOWS
}
